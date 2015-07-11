/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::XML::SaxParser
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Common.h"
#include    "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include    "Stroika/Foundation/DataExchange/XML/SAXObjectReader.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include    "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include    "Stroika/Foundation/Streams/iostream/InputStreamAdapter.h"
#include    "Stroika/Foundation/Time/Realtime.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::XML;

using   Stroika::Foundation::Debug::TraceContextBumper;
using   Streams::iostream::InputStreamAdapter;


namespace   {


    //
    // PUT THIS OR SOMETHING LIKE IT TO STROIKA EVENTUALLY
    //  void    StreamUtils::WriteTextStream (const wstring& w, ostream& out)
    void    WriteTextStream_ (const wstring& w, ostream& out)
    {
        CodePageConverter   cpc (kCodePage_UTF8, CodePageConverter::eHandleBOM);
        size_t              sz  =       cpc.MapFromUNICODE_QuickComputeOutBufSize (w.c_str (), w.length ());
        Memory::SmallStackBuffer<char>  buf (sz + 1);
        size_t  charCnt =   sz;
        cpc.MapFromUNICODE (w.c_str (), w.length (), buf, &charCnt);
        Assert (charCnt <= sz);
        out.write (buf, charCnt);
    }


}


namespace   {
    void    Test_1_SAXParser_ ()
    {
        TraceContextBumper ctx ("Test_1_SAXParser_");
        const wstring   kNSTest =   L"Test-NAMESPACE";
        //NYI
        //Schema    gSchema     =   Schema (kNSTest);
        wstring newDocXML   =
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

        class   MyCallback : public SAXCallbackInterface {
        public:
            virtual void    StartDocument () override
            {
                fEltDepthCount = 0;
            }
            virtual void    EndDocument () override
            {
                Assert (fEltDepthCount == 0);
            }
            virtual void    StartElement (const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs) override
            {
                fEltDepthCount++;
                fEltStack.push_back (uri + L"/" + localName + L"/" + qname);
            }
            virtual void    EndElement (const String& uri, const String& localName, const String& qname) override
            {
                Assert (fEltStack.back () == uri + L"/" + localName + L"/" + qname);
                fEltStack.pop_back ();
                fEltDepthCount--;
            }
            virtual void    CharactersInsideElement (const String& text) override
            {
            }
            unsigned int    fEltDepthCount;
            vector<String>  fEltStack;
        };
        stringstream tmpStrm;
        WriteTextStream_ (newDocXML, tmpStrm);
        MyCallback  myCallback;
        XML::SAXParse (InputStreamAdapter<Memory::Byte> (tmpStrm), myCallback);
        //SAX::Parse (tmpStrm, gSchema, myCallback);
    }
}






namespace   {
    struct  Person_ {
        String firstName;
        String lastName;
        Memory::Optional<String> middleName;
    };
    struct PersonReader_ : public ComplexObjectReader<Person_> {
#if     !qCompilerAndStdLib_templated_constructionInTemplateConstructors_Buggy
        PersonReader_ (Person_* v, const Mapping<String, VariantValue>& attrs = Mapping<String, VariantValue> ())
#else
        PersonReader_ (Person_* v, const Mapping<String, VariantValue>& attrs = kEmptyMapString2VariantVal_)
#endif
            : ComplexObjectReader<Person_> (v)
        {
        }
        virtual void    HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs) override
        {
            RequireNotNull (fValuePtr);
            if (localName == L"FirstName") {
                _PushNewObjPtr (r, new BuiltinReader<String> (&fValuePtr->firstName));
            }
            else if (localName == L"LastName") {
                _PushNewObjPtr (r, new BuiltinReader<String> (&fValuePtr->lastName));
            }
            else if (localName == L"MiddleName") {
                _PushNewObjPtr (r, new OptionalTypesReader<String> (&fValuePtr->middleName));
            }
            else {
                ThrowUnRecognizedStartElt (uri, localName);
            }
        }
    };
    struct  Appointment_ {
        Time::DateTime  when;
        Person_         withWhom;
    };
    struct AppointmentReader_ : public ComplexObjectReader<Appointment_> {
#if     !qCompilerAndStdLib_templated_constructionInTemplateConstructors_Buggy
        AppointmentReader_ (Appointment_* v, const Mapping<String, VariantValue>& attrs = Mapping<String, VariantValue> ()):
#else
        AppointmentReader_ (Appointment_* v, const Mapping<String, VariantValue>& attrs = kEmptyMapString2VariantVal_):
#endif
            ComplexObjectReader<Appointment_> (v, attrs)
        {
        }
        virtual void    HandleChildStart (SAXObjectReader& r, const String& uri, const String& localName, const String& qname, const Mapping<String, VariantValue>& attrs) override
        {
            if (localName == L"When") {
                _PushNewObjPtr (r, new BuiltinReader<Time::DateTime> (&fValuePtr->when));
            }
            else if (localName == L"WithWhom") {
                _PushNewObjPtr (r, new PersonReader_ (&fValuePtr->withWhom));
            }
            else {
                ThrowUnRecognizedStartElt (uri, localName);
            }
        }
    };
    using       CalendarType_       =   vector<Appointment_>;
    struct  CalendarReaderTraits_ {
        using   ElementType =   Appointment_;
        using   ReaderType  =   AppointmentReader_;
        static  const wchar_t       ElementName[];
    };
    const wchar_t   CalendarReaderTraits_::ElementName[]        =   L"Appointment";
    using       CalendarReader_ =   ListOfObjectReader<CalendarReaderTraits_>;

    void    Test_2_SAXObjectReader_ ()
    {
        TraceContextBumper ctx ("Test_2_SAXObjectReader_");
        const wstring   kNSTest =   L"Test-NAMESPACE";
        wstring newDocXML   =
            L"<Calendar xmlns=\"" + wstring (kNSTest) + L"\">\n"
            L"  <Appointment>\n"
            L"	  <When>2005-06-01T13:00:00-05:00</When>"
            L"	  <WithWhom>\n"
            L"		  <FirstName>Jim</FirstName>"
            L"		  <LastName>Smith</LastName>"
            L"		  <MiddleName>Up</MiddleName>"
            L"	  </WithWhom>\n"
            L"  </Appointment>\n"
            L"  <Appointment>\n"
            L"	  <When>2005-08-01T13:00:00-05:00</When>"
            L"	  <WithWhom>\n"
            L"		  <FirstName>Fred</FirstName>"
            L"		  <LastName>Down</LastName>"
            L"	  </WithWhom>\n"
            L"  </Appointment>\n"
            L"</Calendar>\n"
            ;
        stringstream tmpStrm;
        WriteTextStream_ (newDocXML, tmpStrm);

        SAXObjectReader reader;
#if     qDefaultTracingOn
        reader.fTraceThisReader = true; // handy to debug these SAX-object trees...
#endif
        CalendarType_       calendar;
        reader.Run (shared_ptr<SAXObjectReader::ObjectBase> (new CalendarReader_ (&calendar)), InputStreamAdapter<Memory::Byte> (tmpStrm));
        VerifyTestResult (calendar.size () == 2);
        VerifyTestResult (calendar[0].withWhom.firstName == L"Jim");
        VerifyTestResult (calendar[0].withWhom.lastName == L"Smith");
        VerifyTestResult (*calendar[0].withWhom.middleName == L"Up");
        VerifyTestResult (calendar[0].when.GetDate () == Time::Date (Time::Year (2005), Time::MonthOfYear::eJune, Time::DayOfMonth (1)));
        VerifyTestResult (calendar[1].withWhom.firstName == L"Fred");
        VerifyTestResult (calendar[1].withWhom.lastName == L"Down");
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        try {
            Test_1_SAXParser_ ();
            Test_2_SAXObjectReader_ ();
        }
        catch (const Execution::RequiredComponentMissingException&) {
#if     !qHasLibrary_Xerces
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
#else
            Execution::DoReThrow ();
#endif
        }

    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

