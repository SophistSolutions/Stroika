/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/DataExchangeFormat/XML/SAXReader.h"
#include	"Stroika/Foundation/DataExchangeFormat/XML/SAXObjectReader.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"
#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Time/Realtime.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::DataExchangeFormat;
using	namespace	Stroika::Foundation::DataExchangeFormat::XML;

using	Stroika::Foundation::Debug::TraceContextBumper;




namespace	{


	//
	// PUT THIS OR SOMETHING LIKE IT TO STROIKA EVENTUALLY
	//	void	StreamUtils::WriteTextStream (const wstring& w, ostream& out)
	void	WriteTextStream_ (const wstring& w, ostream& out)
		{
			CodePageConverter	cpc (kCodePage_UTF8, CodePageConverter::eHandleBOM);
			size_t				sz	=		cpc.MapFromUNICODE_QuickComputeOutBufSize (w.c_str (), w.length ());
			Memory::SmallStackBuffer<char>	buf (sz + 1);
			size_t	charCnt	=	sz;
			cpc.MapFromUNICODE (w.c_str (), w.length (), buf, &charCnt);
			Assert (charCnt <= sz);
			out.write (buf, charCnt);
		}


}


namespace	{
	void	Test_1_SAXParser_ ()
		{
			TraceContextBumper ctx (TSTR ("Test_1_SAXParser_"));
			const wstring	kNSTest	=	L"Test-NAMESPACE";
			//NYI
			//Schema	gSchema		=	Schema (kNSTest);
			wstring	newDocXML	=
					L"<PHRModel xmlns=\"" + wstring (kNSTest) + L"\">\n"
					L"	<BasicInformation id=\"id=101\">\n"
					L"		<ContactInfo>\n"
					L"			<PersonName/>\n"
					L"			<Locations>\n"
				;
			newDocXML += 
				L"				<Location id=\"id=102\">\n"
				L"					<Name>Primary Residence</Name>\n"
				L"					<Address/>\n"
				L"				</Location>\n"
			;
			newDocXML += 
				L"				<Location id=\"id=103\">\n"
				L"					<Name>Residence2</Name>\n"
				L"					<Address/>\n"
				L"				</Location>\n"
			;
			newDocXML += 
					L"			</Locations>\n"
					L"		</ContactInfo>\n";
			newDocXML += 
				L"		<AdvanceDirectives id=\"id=104\"/>\n"
				L"		<BirthInfo id=\"id=105\"/>\n"
			;
			newDocXML += 
					L"	</BasicInformation>\n"
					L"	<Calendar/>\n"
					L"	<FamilyMembers/>\n"
					L"	<ProviderOrganizations/>\n"
					L"	<Providers/>\n"
					L"	<Activities/>\n"
					L"	<Allergies/>\n"
					L"	<Attachments/>\n"
					L"	<Communications/>\n"
					L"	<Conditions/>\n"
					L"	<Devices/>\n"
					L"	<Expenses/>\n"
					L"	<InsurancePolicies/>\n"
					L"	<Journals/>\n"
					L"	<JournalEntries/>\n"
					L"	<Links/>\n"
					L"	<Medications/>\n"
					L"	<Tests/>\n"
					L"	<Treatments/>\n"
					L"	<Immunizations/>\n"
					L"	<Visits/>\n"
					L"	<PageCustomizations/>\n"
					L"</PHRModel>\n"
				;
			
			class	MyCallback : public SAXCallbackInterface {
				public:
					virtual	void	StartDocument () override
						{
							fEltDepthCount = 0;
						}
					virtual	void	EndDocument () override
						{
							Assert (fEltDepthCount == 0);
						}
					virtual	void	StartElement (const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
						{
							fEltDepthCount++;
							fEltStack.push_back (uri + L"/" + localName + L"/" + qname);
						}
					virtual	void	EndElement (const String& uri, const String& localName, const String& qname) override
						{
							Assert (fEltStack.back () == uri + L"/" + localName + L"/" + qname);
							fEltStack.pop_back ();
							fEltDepthCount--;
						}
					virtual	void	CharactersInsideElement (const String& text) override
						{
						}
					unsigned int	fEltDepthCount;
					vector<String>	fEltStack;
			};
			stringstream tmpStrm;
			WriteTextStream_ (newDocXML, tmpStrm);
			MyCallback	myCallback;
			XML::SAXParse (tmpStrm, myCallback);
			//SAX::Parse (tmpStrm, gSchema, myCallback);
		}
}






namespace	{
	struct	Person_ {
		String firstName;
		String lastName;
	};
	struct PersonReader_ : public ComplexObjectReader<Person_> {
		PersonReader_ (Person_* v):
			ComplexObjectReader<Person_> (v)
			{
			}
		virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
			{
				RequireNotNull (valuePtr);
				if (localName == L"FirstName") {
					_PushNewObjPtr (r, new BuiltinReader<String> (&valuePtr->firstName));
				}
				else if (localName == L"LastName") {
					_PushNewObjPtr (r, new BuiltinReader<String> (&valuePtr->lastName));
				}
				else {
					ThrowUnRecognizedStartElt (uri, localName);
				}
			}
	};
	struct	Appointment_ {
		Time::DateTime	when;
		Person_			withWhom;
	};
	struct AppointmentReader_ : public ComplexObjectReader<Appointment_> {
		AppointmentReader_ (Appointment_* v):
			ComplexObjectReader<Appointment_> (v)
			{
			}
		virtual	void	HandleChildStart (SAXObjectReader &r, const String& uri, const String& localName, const String& qname, const map<String,Memory::VariantValue>& attrs) override
			{
				if (localName == L"When") {
					_PushNewObjPtr (r, new BuiltinReader<Time::DateTime> (&valuePtr->when));
				}
				else if (localName == L"WithWhom") {
					_PushNewObjPtr (r, new PersonReader_ (&valuePtr->withWhom));
				}
				else {
					ThrowUnRecognizedStartElt (uri, localName);
				}
			}
	};

	void	Test_2_SAXObjectReader_ ()
		{
			TraceContextBumper ctx (TSTR ("Test_2_SAXObjectReader_"));
			const wstring	kNSTest	=	L"Test-NAMESPACE";
			wstring	newDocXML	=
					L"<Appointment xmlns=\"" + wstring (kNSTest) + L"\">\n"
					L"	<When>2005-06-01T13:00:00-05:00</When>"
					L"	<WithWhom>\n"
					L"		<FirstName>Jim</FirstName>"
					L"		<LastName>Smith</LastName>"
					L"	</WithWhom>\n"
					L"</Appointment>\n"
				;
			stringstream tmpStrm;
			WriteTextStream_ (newDocXML, tmpStrm);

			SAXObjectReader	reader;
			Appointment_		appointment;
			reader.Run (Memory::SharedPtr<SAXObjectReader::ObjectBase> (new AppointmentReader_ (&appointment)), tmpStrm);
			VerifyTestResult (appointment.withWhom.firstName == L"Jim");
			VerifyTestResult (appointment.withWhom.lastName == L"Smith");
			VerifyTestResult (appointment.when.GetDate () == Time::Date (Time::Date::Year (2005), Time::Date::eJune, Time::Date::DayOfMonth (1)));
		}
}



namespace	{

	void	DoRegressionTests_ ()
		{
			Test_1_SAXParser_ ();
			Test_2_SAXObjectReader_ ();
		}
}





int		main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

