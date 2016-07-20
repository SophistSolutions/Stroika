﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::XML::SaxParser
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Containers/Common.h"
#include    "Stroika/Foundation/DataExchange/XML/SAXReader.h"
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
            String                      firstName;
            String                      lastName;
            Memory::Optional<String>    middleName;
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

            registry.AddCommonType<Time::DateTime> ();
            registry.AddCommonType<String> ();
            registry.AddCommonType<Optional<String>> ();

            // not sure if this is clearer or macro version
            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            registry.AddClass<Person_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"FirstName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, firstName) },
                { Name { L"LastName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, lastName) },
                { Name { L"MiddleName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, middleName) },
            });
            registry.AddClass<Appointment_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"When" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Appointment_, when) },
                { Name { L"WithWhom" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Appointment_, withWhom) },
            });
            registry.AddCommonType<vector<Appointment_>> (Name { L"Appointment" });
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

            {
                vector<Appointment_>       calendar;
                ObjectReaderRegistry::IConsumerDelegateToContext ctx { registry, registry.mkReadDownToReader (make_shared<ObjectReaderRegistry::ListOfObjectReader<vector<Appointment_>>> (&calendar, Name { L"Appointment" })) };
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (calendar.size () == 2);
                VerifyTestResult (calendar[0].withWhom.firstName == L"Jim");
                VerifyTestResult (calendar[0].withWhom.lastName == L"Smith");
                VerifyTestResult (*calendar[0].withWhom.middleName == L"Up");
                VerifyTestResult (calendar[0].when.GetDate () == Time::Date (Time::Year (2005), Time::MonthOfYear::eJune, Time::DayOfMonth (1)));
                VerifyTestResult (calendar[1].withWhom.firstName == L"Fred");
                VerifyTestResult (calendar[1].withWhom.lastName == L"Down");
            }
            // must figure out how to get below working
            {
                vector<Appointment_>       calendar;
                ObjectReaderRegistry::IConsumerDelegateToContext ctx { registry, registry.mkReadDownToReader (registry.MakeContextReader (&calendar)) };
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (calendar.size () == 2);
                VerifyTestResult (calendar[0].withWhom.firstName == L"Jim");
                VerifyTestResult (calendar[0].withWhom.lastName == L"Smith");
                VerifyTestResult (*calendar[0].withWhom.middleName == L"Up");
                VerifyTestResult (calendar[0].when.GetDate () == Time::Date (Time::Year (2005), Time::MonthOfYear::eJune, Time::DayOfMonth (1)));
                VerifyTestResult (calendar[1].withWhom.firstName == L"Fred");
                VerifyTestResult (calendar[1].withWhom.lastName == L"Down");
            }
        }
    }
    void    Test_SAX_ObjectReader_EXAMPLE_1_ ()
    {
        using namespace SAX_ObjectReader_EXAMPLE_1_;
        Test_2a_ObjectReader_viaRegistry_ ();
    }
}






namespace {
    namespace  T3_SAXObjectReader_ReadDown2Sample_ {

        struct  Person_ {
            String firstName;
            String lastName;
            bool operator == (const Person_& rhs) const { return firstName == rhs.firstName and lastName == rhs.lastName; }
        };
        Memory::BLOB    mkdata_ ()
        {
            wstring newDocXML   =
                L"<envelope1>\n"
                L"  <envelope2>\n"
                L"	  <When>2005-06-01T13:00:00-05:00</When>"
                L"	  <WithWhom>\n"
                L"		  <FirstName>Jim</FirstName>"
                L"		  <LastName>Smith</LastName>"
                L"	  </WithWhom>\n"
                L"  </envelope2>\n"
                L"  <envelope2>\n"
                L"	  <When>2005-08-01T13:00:00-05:00</When>"
                L"	  <WithWhom>\n"
                L"		  <FirstName>Fred</FirstName>"
                L"		  <LastName>Down</LastName>"
                L"	  </WithWhom>\n"
                L"  </envelope2>\n"
                L"</envelope1>\n"
                ;
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<Memory::Byte> (tmpStrm).ReadAll ();
        }

        void    DoTest ()
        {
            ObjectReaderRegistry registry;
            registry.AddCommonType<String> ();

            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            registry.AddClass<Person_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"FirstName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, firstName) },
                { Name { L"LastName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, lastName) },
            });
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

            vector<Person_> people;
            {
                ObjectReaderRegistry::IConsumerDelegateToContext ctx { registry, registry.mkReadDownToReader (make_shared<ObjectReaderRegistry::ListOfObjectReader<vector<Person_>>> (&people, Name (L"WithWhom")), Name (L"envelope2")) };
                XML::SAXParse (mkdata_ (), ctx);

                VerifyTestResult (people.size () == 2);
                VerifyTestResult (people[0].firstName == L"Jim");
                VerifyTestResult (people[0].lastName == L"Smith");
                VerifyTestResult (people[1].firstName == L"Fred");
                VerifyTestResult (people[1].lastName == L"Down");
            }

            vector<Person_> people2;    // add the vector type to the registry instead of explicitly constructing the right reader
            {
                ObjectReaderRegistry newRegistry = registry;
                newRegistry.AddCommonType<vector<Person_>> (Name (L"WithWhom"));
                ObjectReaderRegistry::IConsumerDelegateToContext ctx { newRegistry, newRegistry.mkReadDownToReader (newRegistry.MakeContextReader (&people2), Name (L"envelope2")) };
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (people2 == people);
            }

            Sequence<Person_> people3;  // use sequence instead of vector
            {
                ObjectReaderRegistry newRegistry = registry;
                newRegistry.AddCommonType<Sequence<Person_>> (Name (L"WithWhom"));
                ObjectReaderRegistry::IConsumerDelegateToContext ctx { newRegistry, newRegistry.mkReadDownToReader (newRegistry.MakeContextReader (&people3), Name (L"envelope2")) };
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (people3.As<vector<Person_>> () == people);
            }
        }
    }

}







namespace {
    namespace  T4_SAXObjectReader_ReadDown2Sample_MixedContent_ {

        struct  ManagedObjectReference {
            String  type;
            String  value;
        };
        struct  ObjectContent {
            ManagedObjectReference  obj;
        };
        Memory::BLOB    mkdata_ ()
        {
            wstring newDocXML   =
                L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                L"<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\r\n"
                L"   <soapenv:Body>\n"
                L"      <RetrievePropertiesResponse xmlns=\"urn:vim25\">\n"
                L"         <returnval>\n"
                L"            <obj type=\"VirtualMachine\">8</obj>"
                L"            <propSet>"
                L"               <name>availableField</name>\n"
                L"               <val xsi:type=\"ArrayOfCustomFieldDef\" />\n"
                L"            </propSet>\n"
                L"            <propSet>"
                L"               <name>capability</name>\n"
                L"               <val xsi:type=\"VirtualMachineCapability\">\n"
                L"                  <snapshotOperationsSupported>true</snapshotOperationsSupported>\n"
                L"               </val>\n"
                L"            </propSet>\n"
                L"         </returnval>\n"
                L"         <returnval>\n"
                L"            <obj type=\"VirtualMachine\">9</obj>"
                L"            <propSet>"
                L"               <name>capability</name>\n"
                L"               <val xsi:type=\"VirtualMachineCapability\">\n"
                L"                  <snapshotOperationsSupported>true</snapshotOperationsSupported>\n"
                L"               </val>\n"
                L"            </propSet>\n"
                L"         </returnval>\n"
                L"      </RetrievePropertiesResponse>\n"
                L"   </soapenv:Body>\n"
                L"</soapenv:Envelope>\n"
                ;
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<Memory::Byte> (tmpStrm).ReadAll ();
        }

        void    DoTest ()
        {
            ObjectReaderRegistry mapper;

            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04

            using   Memory::Optional;
            mapper.AddCommonType<String> ();

            mapper.AddClass<ManagedObjectReference> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"type", Name::eAttribute }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (ManagedObjectReference, type) },
                { Name { Name::eValue }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (ManagedObjectReference, value) }
            });
            mapper.AddClass<ObjectContent> (initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"obj" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (ObjectContent, obj) }
                /// wrong - must be mapping of this --metaInfo.Add (L"propSet", pair<type_index, size_t> {typeid(decltype (ObjectContent::value)), offsetof(ObjectContent, propSet)});
            });

            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

            vector<ObjectContent> objsContent;
            ObjectReaderRegistry::IConsumerDelegateToContext ctx { mapper, make_shared<ObjectReaderRegistry::ReadDownToReader> ( make_shared<ObjectReaderRegistry::ReadDownToReader> (make_shared<ObjectReaderRegistry::ListOfObjectReader<vector<ObjectContent>>> (&objsContent, Name (L"returnval")), Name (L"RetrievePropertiesResponse"))) };
            XML::SAXParse (mkdata_ (), ctx);

            VerifyTestResult (objsContent.size () == 2);
            VerifyTestResult (objsContent[0].obj.type == L"VirtualMachine");
            VerifyTestResult (objsContent[0].obj.value == L"8");
            VerifyTestResult (objsContent[1].obj.type == L"VirtualMachine");
            VerifyTestResult (objsContent[1].obj.value == L"9");
        }
    }

}





namespace {
    namespace T5_SAXObjectReader_DocSamples_ {
        struct  Person_ {
            String firstName;
            String lastName;
        };
        Memory::BLOB    mkdata_ ()
        {
            wstring newDocXML   =
                L"	  <PERSON>\n"
                L"		  <FirstName>Jim</FirstName>"
                L"		  <LastName>Smith</LastName>"
                L"	  </PERSON>\n"
                ;
            ;
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<Memory::Byte> (tmpStrm).ReadAll ();
        }

        void    DoTest1 ()
        {
            ObjectReaderRegistry mapper;
            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            mapper.AddCommonType<String> ();
            mapper.AddClass<Person_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"FirstName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, firstName) },
                { Name { L"LastName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, lastName) },
            });
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            Person_ p;
            ObjectReaderRegistry::IConsumerDelegateToContext tmp (mapper, mapper.mkReadDownToReader (mapper.MakeContextReader (&p)));
            XML::SAXParse (mkdata_ (), tmp);
            VerifyTestResult (p.firstName == L"Jim");
            VerifyTestResult (p.lastName == L"Smith");
        }
        void    DoTests ()
        {
            DoTest1 ();
        }

    }
}





namespace {
    namespace  T6_SAXObjectReader_RepeatedElementReader_Sample_ {
        enum class GenderType_ {
            Male,
            Female,
            Stroika_Define_Enum_Bounds(Male, Female)
        };
        struct  Person_ {
            String                  firstName;
            String                  lastName;
            Optional<GenderType_>   gender;
        };
        struct  Address_ {
            String city;
            String state;
        };
        struct Data_ {
            vector<Person_>     people;
            vector<Address_>    addresses;
        };
        Memory::BLOB    mkdata_ ()
        {
            wstring newDocXML   =
                L"<envelope1>\n"
                L"	  <person>\n"
                L"		  <FirstName>Jim</FirstName>"
                L"		  <LastName>Smith</LastName>"
                L"		  <Gender>Male</Gender>"
                L"	  </person>\n"
                L"	  <person>\n"
                L"		  <FirstName>Fred</FirstName>"
                L"		  <LastName>Down</LastName>"
                L"	  </person>\n"
                L"	  <address>\n"
                L"		  <city>Boston</city>"
                L"		  <state>MA</state>"
                L"	  </address>\n"
                L"	  <address>\n"
                L"		  <city>New York</city>"
                L"		  <state>NY</state>"
                L"	  </address>\n"
                L"	  <address>\n"
                L"		  <city>Albany</city>"
                L"		  <state>NY</state>"
                L"	  </address>\n"
                L"</envelope1>\n"
                ;
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<Memory::Byte> (tmpStrm).ReadAll ();
        }

        void    DoTest ()
        {
            ObjectReaderRegistry registry;
            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            registry.AddCommonType<String> ();
            registry.Add<GenderType_> (ObjectReaderRegistry::MakeCommonReader_NamedEnumerations<GenderType_> (Containers::Bijection<GenderType_, String> {
                pair<GenderType_, String> { GenderType_::Male, L"Male" },
                pair<GenderType_, String> { GenderType_::Female, L"Female" },
            }));
            registry.AddCommonType<Optional<GenderType_>> ();
            registry.AddClass<Person_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"FirstName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, firstName) },
                { Name { L"LastName" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, lastName) },
                { Name { L"Gender" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Person_, gender) },
            });
            registry.AddCommonType<vector<Person_>> ();
            registry.Add<vector<Person_>> (ObjectReaderRegistry::ConvertReaderToFactory <vector<Person_>, ObjectReaderRegistry::RepeatedElementReader<vector<Person_>>> ());
            registry.AddClass<Address_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"city" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Address_, city) },
                { Name { L"state" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Address_, state) },
            });
            registry.Add<vector<Address_>> (ObjectReaderRegistry::ConvertReaderToFactory <vector<Address_>, ObjectReaderRegistry::RepeatedElementReader<vector<Address_>>> ());
            registry.AddClass<Data_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"person" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Data_, people) },
                { Name { L"address" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (Data_, addresses) },
            });
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

            Data_   data;
            {
                ObjectReaderRegistry::IConsumerDelegateToContext ctx { registry, registry.mkReadDownToReader (registry.MakeContextReader (&data)) };
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (data.people.size () == 2);
                VerifyTestResult (data.people[0].firstName == L"Jim");
                VerifyTestResult (data.people[0].lastName == L"Smith");
                VerifyTestResult (data.people[0].gender == GenderType_::Male);
                VerifyTestResult (data.people[1].firstName == L"Fred");
                VerifyTestResult (data.people[1].lastName == L"Down");
                VerifyTestResult (data.people[1].gender.IsMissing ());
                VerifyTestResult (data.addresses.size () == 3);
                VerifyTestResult (data.addresses[0].city == L"Boston");
                VerifyTestResult (data.addresses[0].state == L"MA");
                VerifyTestResult (data.addresses[1].city == L"New York");
                VerifyTestResult (data.addresses[1].state == L"NY");
                VerifyTestResult (data.addresses[2].city == L"Albany");
                VerifyTestResult (data.addresses[2].state == L"NY");
            }
        }
    }

}










namespace   {

    void    DoRegressionTests_ ()
    {
        try {
            Test_1_SAXParser_ ();
            Test_SAX_ObjectReader_EXAMPLE_1_ ();
            T3_SAXObjectReader_ReadDown2Sample_::DoTest ();
            T4_SAXObjectReader_ReadDown2Sample_MixedContent_::DoTest ();
            T5_SAXObjectReader_DocSamples_::DoTests ();
            T6_SAXObjectReader_RepeatedElementReader_Sample_::DoTest ();
        }
        catch (const Execution::RequiredComponentMissingException&) {
#if     !qHasLibrary_Xerces
            // OK to ignore. We don't wnat to call this failing a test, because there is nothing to fix.
            // This is more like the absence of a feature beacuse of the missing component.
#else
            Execution::ReThrow ();
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

