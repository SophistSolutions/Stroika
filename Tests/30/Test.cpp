/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::XML::SaxParser
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>
#include    <sstream>

#include    "Stroika/Foundation/Characters/ToString.h"
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

using   Common::KeyValuePair;
using   Debug::TraceContextBumper;
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









namespace {
    namespace  T7_SAXObjectReader_BLKQCL_ReadSensors_ {

        enum    class   TunerNumberType_ {
            eT1 = 1,
            eT2 = 2,
            eT3 = 3,
            eT4 = 4,
            Stroika_Define_Enum_Bounds(eT1, eT4)
        };
        using WaveNumberType_   = double;
        using TemperatureType_  = double;
        using CurrentType_  = double;
        struct  TECPowerConsumptionStatsType_ {
            Mapping<TunerNumberType_, CurrentType_>     TunerTECCurrent;
        };
        struct  SensorDataType_ {
            Optional<TunerNumberType_>                  ActiveLaser;
            Optional<WaveNumberType_>                   ActiveLaserWaveNumber;
            Optional<TemperatureType_>                  DetectorTemperature;
            Optional<TemperatureType_>                  OpticsTemperature;
            Optional<TemperatureType_>                  ExternalTemperature1;
            Mapping<TunerNumberType_, TemperatureType_> LaserTemperatures;
            Mapping<TunerNumberType_, CurrentType_>     LaserCurrents;
            Mapping<TunerNumberType_, TemperatureType_> MirrorTemperatures;
            Optional<TECPowerConsumptionStatsType_>     TECPowerConsumptionStats;
        };
        Memory::BLOB    mkdata_ ()
        {
            wstring newDocXML   =
                L"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:blk201505=\"http://tempuri.org/blk201505.xsd\" xmlns:blk201605=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/\" xmlns:blk2016052=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IConfiguration\" xmlns:blk2016053=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILaserOperation\" xmlns:blk2016054=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IDeviceManagement\" xmlns:blk2016055=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IManufacturing\" xmlns:blk2016056=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILowLevelHardwareAccess\" xmlns:blk2016057=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IBasicPersistence\" xmlns:blk2016058=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IScanPersistence\" xmlns:ns1=\"http://www.blockeng.com/Schemas/2015-05/BLKQCL-Common/\" xmlns:ns2=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL-App/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
                L"   <SOAP-ENV:Header>\n"
                L"      <blk201505:timestamp>8557.8791092709998</blk201505:timestamp>\n"
                L"   </SOAP-ENV:Header>\n"
                L"   <SOAP-ENV:Body>\n"
                L"      <blk201605:ReadSensorsResponse>\n"
                L"         <blk201605:Sensors>\n"
                L"            <blk201605:DetectorTemperature>13.1</blk201605:DetectorTemperature>\n"
                L"            <blk201605:OpticsTemperature>0.86115019791435543</blk201605:OpticsTemperature>\n"
                L"            <blk201605:LaserTemperature>\n"
                L"               <blk201605:Temperature Tuner=\"1\">20.899877489241646</blk201605:Temperature>\n"
                L"            </blk201605:LaserTemperature>\n"
                L"            <blk201605:LaserCurrent>\n"
                L"               <blk201605:Current Tuner=\"1\">0.86871794871794872</blk201605:Current>\n"
                L"            </blk201605:LaserCurrent>\n"
                L"             <blk201605:MirrorTemperature>\n"
                L"               <blk201605:Temperature Tuner=\"1\">0.86115019791435543</blk201605:Temperature>\n"
                L"            </blk201605:MirrorTemperature>\n"
                L"            <blk201605:TECPowerConsumptionStats>\n"
                L"               <blk201605:TunerTECCurrent Tuner=\"1\">-0.0015262515262515208</blk201605:TunerTECCurrent>\n"
                L"               <blk201605:TunerTECCurrent Tuner=\"2\">0</blk201605:TunerTECCurrent>\n"
                L"               <blk201605:TunerTECCurrent Tuner=\"3\">0</blk201605:TunerTECCurrent>\n"
                L"               <blk201605:TunerTECCurrent Tuner=\"4\">0</blk201605:TunerTECCurrent>\n"
                L"            </blk201605:TECPowerConsumptionStats>\n"
                L"            <blk201605:ExternalTemperature1>0</blk201605:ExternalTemperature1>\n"
                L"         </blk201605:Sensors>\n"
                L"      </blk201605:ReadSensorsResponse>\n"
                L"   </SOAP-ENV:Body>\n"
                L"</SOAP-ENV:Envelope>\n"
                ;
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<Memory::Byte> (tmpStrm).ReadAll ();
        }
        /*
         *   <blk201605:LaserTemperature>\n"
         *      <blk201605:Temperature Tuner=\"1\">20.899877489241646</blk201605:Temperature>\n"
         *   </blk201605:LaserTemperature>\n"
         *
         *   Mapping<TunerNumberType,Temperature>    LaserTemperature;
         */
        template    <typename TARGET_TYPE>
        struct   TunerMappingReader_TRAITS_ {
            using   ElementType = KeyValuePair<TunerNumberType_, TARGET_TYPE>;
            static  shared_ptr<ObjectReaderRegistry::IElementConsumer>   MakeActualReader (ObjectReaderRegistry::Context& r, ElementType* proxyValue)
            {
                RequireNotNull (proxyValue);
                return  sEltReader_ (proxyValue);
            }
            static  void   AppendToOutputContainer (Mapping<TunerNumberType_, TARGET_TYPE>* container, const ElementType& v)
            {
                RequireNotNull (container);
                container->Add (v.fKey, v.fValue);
            }
            static  const   ObjectReaderRegistry::ReaderFromVoidStarFactory sEltReader_;
        };
        template    <typename TARGET_TYPE>
        const   ObjectReaderRegistry::ReaderFromVoidStarFactory TunerMappingReader_TRAITS_<TARGET_TYPE>::sEltReader_ =
        [] () -> ObjectReaderRegistry::ReaderFromVoidStarFactory {
            using   KVPType_    =   KeyValuePair<TunerNumberType_, TARGET_TYPE>;
            return ObjectReaderRegistry::MakeClassReader<KVPType_> (
            initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"Tuner", Name::eAttribute }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (KVPType_, fKey) },
                { Name { Name::eValue }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (KVPType_, fValue) },
            }
            );
        } ();
        template    <typename TARGET_TYPE>
        class   TunerMappingReader_: public ObjectReaderRegistry::IElementConsumer {
        public:
            TunerMappingReader_ (Mapping<TunerNumberType_, TARGET_TYPE>* v)
                : fValuePtr_ (v)
            {
            }
            virtual void    Activated (ObjectReaderRegistry::Context& r) override
            {
            }
            virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name& name) override
            {
                //DbgTrace (L"name=%s", name.ToString ().c_str ());
                return  make_shared<ObjectReaderRegistry::RepeatedElementReader<Mapping<TunerNumberType_, TARGET_TYPE>, TunerMappingReader_TRAITS_<TARGET_TYPE>>> (fValuePtr_);
            }
            virtual void    Deactivating () override
            {
            }
        private:
            Mapping<TunerNumberType_, TARGET_TYPE>*     fValuePtr_;
        };

        /// @todo see if I can replace TunerMappingReader_ with ReadDownToReader


        void    DoTest ()
        {
            ObjectReaderRegistry registry;
            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
            registry.Add<TunerNumberType_> (ObjectReaderRegistry::MakeCommonReader_NamedEnumerations<TunerNumberType_> (Containers::Bijection<TunerNumberType_, String> {
                pair<TunerNumberType_, String> { TunerNumberType_::eT1, L"1" },
                pair<TunerNumberType_, String> { TunerNumberType_::eT2, L"2" },
                pair<TunerNumberType_, String> { TunerNumberType_::eT3, L"3" },
                pair<TunerNumberType_, String> { TunerNumberType_::eT4, L"4" },
            }));
            registry.AddCommonType<Optional<TunerNumberType_>> ();
            registry.AddCommonType<WaveNumberType_> ();
            registry.AddCommonType<Optional<WaveNumberType_>> ();
            registry.AddCommonType<TemperatureType_> ();
            registry.AddCommonType<Optional<TemperatureType_>> ();
            registry.AddCommonType<CurrentType_> ();
            registry.Add<Mapping<TunerNumberType_, TemperatureType_>> (ObjectReaderRegistry::ConvertReaderToFactory<Mapping<TunerNumberType_, TemperatureType_>, TunerMappingReader_<TemperatureType_> > ());
            registry.Add<TECPowerConsumptionStatsType_> (ObjectReaderRegistry::ConvertReaderToFactory<Mapping<TunerNumberType_, TemperatureType_>, TunerMappingReader_<TemperatureType_> > ());
            registry.AddCommonType<Optional<TECPowerConsumptionStatsType_>> ();
            registry.AddClass<SensorDataType_> ( initializer_list<pair<Name, StructFieldMetaInfo>> {
                { Name { L"ActiveLaser" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, ActiveLaser) },
                { Name { L"DetectorTemperature" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, DetectorTemperature) },
                { Name { L"OpticsTemperature" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, OpticsTemperature) },
                { Name { L"ExternalTemperature1" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, ExternalTemperature1) },
                { Name { L"LaserTemperature" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, LaserTemperatures) },
                { Name { L"LaserCurrent" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, LaserCurrents) },
                { Name { L"MirrorTemperature" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, MirrorTemperatures) },
                { Name { L"TECPowerConsumptionStats" }, Stroika_Foundation_DataExchange_StructFieldMetaInfo (SensorDataType_, TECPowerConsumptionStats) },
            });
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");

            SensorDataType_   data;
            {
                ObjectReaderRegistry::IConsumerDelegateToContext consumerCallback { registry, registry.mkReadDownToReader (registry.MakeContextReader (&data), Name { L"Sensors" }) };
                consumerCallback.fContext.fTraceThisReader = true;
                XML::SAXParse (mkdata_ (), consumerCallback);
                VerifyTestResult (data.ActiveLaser.IsMissing ());
                VerifyTestResult (Math::NearlyEquals (*data.DetectorTemperature, 13.1));
                VerifyTestResult (Math::NearlyEquals (*data.OpticsTemperature, 0.86115019791435543));
                VerifyTestResult ((data.LaserTemperatures.Keys () == Set<TunerNumberType_> { TunerNumberType_::eT1 }));
                VerifyTestResult (Math::NearlyEquals (*data.LaserTemperatures.Lookup (TunerNumberType_::eT1), 20.899877489241646));
                int a = data.LaserTemperatures.size ();
                //DbgTrace(L"la=%s", Characters::ToString (data.LaserTemperatures).c_str ());
                for (auto aaa : data.LaserTemperatures) {
                    DbgTrace (L"%d : %f", aaa.fKey, aaa.fValue);
                }
                for (auto aaa : data.TECPowerConsumptionStats->TunerTECCurrent) {
                    DbgTrace (L"%d : %f", aaa.fKey, aaa.fValue);
                }

                VerifyTestResult ((data.TECPowerConsumptionStats->TunerTECCurrent.Keys () == Set<TunerNumberType_> { TunerNumberType_::eT1, TunerNumberType_::eT2, TunerNumberType_::eT3, TunerNumberType_::eT4 }));

                VerifyTestResult (Math::NearlyEquals (*data.ExternalTemperature1, 0.0));
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
            T7_SAXObjectReader_BLKQCL_ReadSensors_::DoTest ();
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

