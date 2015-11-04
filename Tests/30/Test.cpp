/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::XML::SaxParser
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Common.h"
#include    "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include    "Stroika/Foundation/DataExchange/StructuredStreamEvents/ObjectReader.h"
#include    "Stroika/Foundation/DataExchange/StructuredStreamEvents/ObjectReaderRegistry.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"
#include    "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#include    "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include    "Stroika/Foundation/Streams/iostream/InputStreamFromStdIStream.h"
#include    "Stroika/Foundation/Time/Realtime.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::StructuredStreamEvents;
using   namespace   Stroika::Foundation::DataExchange::XML;

using   Stroika::Foundation::Debug::TraceContextBumper;
using   Streams::iostream::InputStreamFromStdIStream;


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

        class   MyCallback : public StructuredStreamEvents::IConsumer {
        public:
            virtual void    StartDocument () override
            {
                fEltDepthCount = 0;
            }
            virtual void    EndDocument () override
            {
                VerifyTestResult (fEltDepthCount == 0);
            }
            virtual void    StartElement (const StructuredStreamEvents::Name& name) override
            {
                fEltDepthCount++;
                fEltStack.push_back (name.fNamespaceURI.Value () + L"/" + name.fLocalName);
            }
            virtual void    EndElement (const StructuredStreamEvents::Name& name) override
            {
                VerifyTestResult (fEltStack.back () == name.fNamespaceURI.Value () + L"/" + name.fLocalName);
                fEltStack.pop_back ();
                fEltDepthCount--;
            }
            unsigned int    fEltDepthCount;
            vector<String>  fEltStack;
        };
        stringstream tmpStrm;
        WriteTextStream_ (newDocXML, tmpStrm);
        MyCallback  myCallback;
        XML::SAXParse (InputStreamFromStdIStream<Memory::Byte> (tmpStrm), myCallback);
        //SAX::Parse (tmpStrm, gSchema, myCallback);
    }
}






namespace   {
    namespace SAX_ObjectReader_EXAMPLE_1_ {
        const wstring   kNSTest =   L"Test-NAMESPACE";
        struct  Person_ {
            String firstName;
            String lastName;
            Memory::Optional<String> middleName;
        };
        struct  Appointment_ {
            Time::DateTime  when;
            Person_         withWhom;
        };
        Memory::BLOB    mkdata_ ()
        {
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
            return InputStreamFromStdIStream<Memory::Byte> (tmpStrm).ReadAll ();
        }
        void    Test_2a_ObjectReader_viaRegistry_ ()
        {
            TraceContextBumper ctx ("Test_2a_ObjectReader_viaRegistry_");

            ObjectReaderRegistry registry;

            // @todo replace with addbuilt type sna ddd common types like we do for ObjectVariantMapper
            registry.Add<Time::DateTime> ([] (Time::DateTime * d) { return make_shared<SimpleReader<Time::DateTime>> (d); });
            registry.Add<String> ([] (String * d) { return make_shared<SimpleReader<String>> (d); });
            registry.Add<Optional<String>> ([] (Optional<String>* d) { return make_shared<OptionalTypesReader<String>> (d); });

            // not sure if this is clearer or macro version
            {
                Mapping<String, pair<type_index, size_t>>   metaInfo;
                metaInfo.Add (L"FirstName", pair<type_index, size_t> {typeid(decltype (Person_::firstName)), offsetof(Person_, firstName)});
                metaInfo.Add (L"LastName", pair<type_index, size_t> {typeid(decltype (Person_::lastName)), offsetof(Person_, lastName)});
                metaInfo.Add (L"MiddleName", pair<type_index, size_t> {typeid(decltype (Person_::middleName)), offsetof(Person_, middleName)});
                registry.Add<Person_> (mkClassReaderFactory<Person_> (metaInfo));
            }
            {
                Mapping<String, pair<type_index, size_t>>   metaInfo;
                metaInfo.Add (L"When", pair<type_index, size_t> {typeid(decltype (Appointment_::when)), offsetof(Appointment_, when)});
                metaInfo.Add (L"WithWhom", pair<type_index, size_t> {typeid(decltype (Appointment_::withWhom)), offsetof(Appointment_, withWhom)});
                registry.Add<Appointment_> (mkClassReaderFactory<Appointment_> (metaInfo));
            }

            vector<Appointment_>       calendar;
            {
                Run (registry, make_shared<ListOfObjectReader<Appointment_>> (L"Appointment", &calendar), mkdata_ ().As<Streams::InputStream<Byte>> ());
            }
            VerifyTestResult (calendar.size () == 2);
            VerifyTestResult (calendar[0].withWhom.firstName == L"Jim");
            VerifyTestResult (calendar[0].withWhom.lastName == L"Smith");
            VerifyTestResult (*calendar[0].withWhom.middleName == L"Up");
            VerifyTestResult (calendar[0].when.GetDate () == Time::Date (Time::Year (2005), Time::MonthOfYear::eJune, Time::DayOfMonth (1)));
            VerifyTestResult (calendar[1].withWhom.firstName == L"Fred");
            VerifyTestResult (calendar[1].withWhom.lastName == L"Down");
        }
    }
    void    Test_SAX_ObjectReader_EXAMPLE_1_ ()
    {
        using namespace SAX_ObjectReader_EXAMPLE_1_;
        Test_2a_ObjectReader_viaRegistry_ ();
    }
}



namespace   {

    void    DoRegressionTests_ ()
    {
        try {
            Test_1_SAXParser_ ();
            Test_SAX_ObjectReader_EXAMPLE_1_ ();
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

