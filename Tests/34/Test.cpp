﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::XML::SaxParser
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/DataExchange/StructuredStreamEvents/ObjectReader.h"
#include "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/iostream/InputStreamFromStdIStream.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "../TestHarness/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;
using namespace Stroika::Foundation::DataExchange::XML;

using Common::KeyValuePair;
using Containers::Sequence;
using Containers::Set;
using Containers::SortedMapping;
using Debug::TraceContextBumper;
using Streams::InputStream;
using Streams::iostream::InputStreamFromStdIStream;

namespace {
    //
    // PUT THIS OR SOMETHING LIKE IT TO STROIKA EVENTUALLY
    //  void    StreamUtils::WriteTextStream (const wstring& w, ostream& out)
    void WriteTextStream_ (const wstring& w, ostream& out)
    {
        CodeCvt<wchar_t> codeCvt{UnicodeExternalEncodings::eUTF8};
        size_t           sz = codeCvt.ComputeTargetByteBufferSize (w.length ());
        // const auto       bom = GetByteOrderMark (UnicodeExternalEncodings::eUTF8);
        // out.write (reinterpret_cast<const char*> (bom.data ()), bom.size ());         // https://stroika.atlassian.net/browse/STK-982 - this should work (and I think is) - but causes later failure (reader) - so debug why
        Memory::StackBuffer<byte> buf{Memory::UninitializedConstructorFlag::eUninitialized, sz};
        span<byte>                outSpan = codeCvt.Characters2Bytes (span{w}, span{buf});
        out.write (reinterpret_cast<const char*> (outSpan.data ()), outSpan.size ());
    }
}

#if qStroika_Foundation_DataExchange_XML_SupportParsing
namespace {
    void Test_1_SAXParser_ ()
    {
        TraceContextBumper ctx{"Test_1_SAXParser_"};
        const wstring      kNSTest = L"Test-NAMESPACE";
        //NYI
        //Schema    gSchema     =   Schema (kNSTest);
        wstring newDocXML = L"<PHRModel xmlns=\"" + wstring{kNSTest} +
                            L"\">\n"
                            L"      <BasicInformation id=\"id=101\">\n"
                            L"              <ContactInfo>\n"
                            L"                      <PersonName/>\n"
                            L"                      <Locations>\n";
        newDocXML += L"                          <Location id=\"id=102\">\n"
                     L"                                  <Name>Primary Residence</Name>\n"
                     L"                                  <Address/>\n"
                     L"                          </Location>\n";
        newDocXML += L"                          <Location id=\"id=103\">\n"
                     L"                                  <Name>Residence2</Name>\n"
                     L"                                  <Address/>\n"
                     L"                          </Location>\n";
        newDocXML += L"                  </Locations>\n"
                     L"          </ContactInfo>\n";
        newDocXML += L"          <AdvanceDirectives id=\"id=104\"/>\n"
                     L"          <BirthInfo id=\"id=105\"/>\n";
        newDocXML += L"  </BasicInformation>\n"
                     L"  <Calendar/>\n"
                     L"  <FamilyMembers/>\n"
                     L"  <ProviderOrganizations/>\n"
                     L"  <Providers/>\n"
                     L"  <Activities/>\n"
                     L"  <Allergies/>\n"
                     L"  <Attachments/>\n"
                     L"  <Communications/>\n"
                     L"  <Conditions/>\n"
                     L"  <Devices/>\n"
                     L"  <Expenses/>\n"
                     L"  <InsurancePolicies/>\n"
                     L"  <Journals/>\n"
                     L"  <JournalEntries/>\n"
                     L"  <Links/>\n"
                     L"  <Medications/>\n"
                     L"  <Tests/>\n"
                     L"  <Treatments/>\n"
                     L"  <Immunizations/>\n"
                     L"  <Visits/>\n"
                     L"  <PageCustomizations/>\n"
                     L"</PHRModel>\n";

        class MyCallback : public StructuredStreamEvents::IConsumer {
        public:
            virtual void StartDocument () override
            {
                fEltDepthCount = 0;
            }
            virtual void EndDocument () override
            {
                VerifyTestResult (fEltDepthCount == 0);
            }
            virtual void StartElement (const StructuredStreamEvents::Name&                                   name,
                                       [[maybe_unused]] const Mapping<StructuredStreamEvents::Name, String>& attributes) override
            {
                fEltDepthCount++;
                fEltStack.push_back (Memory::NullCoalesce (name.fNamespaceURI) + "/" + name.fLocalName);
            }
            virtual void EndElement (const StructuredStreamEvents::Name& name) override
            {
                VerifyTestResult (fEltStack.back () == Memory::NullCoalesce (name.fNamespaceURI) + "/" + name.fLocalName);
                fEltStack.pop_back ();
                fEltDepthCount--;
            }
            unsigned int   fEltDepthCount;
            vector<String> fEltStack;
        };
        stringstream tmpStrm;
        WriteTextStream_ (newDocXML, tmpStrm);
        MyCallback myCallback;
        XML::SAXParse (InputStreamFromStdIStream<byte>::New (tmpStrm), myCallback);
    }
}

namespace {
    namespace SAX_ObjectReader_EXAMPLE_1_ {
        const wstring kNSTest = L"Test-NAMESPACE";
        struct Person_ {
            String           firstName;
            String           lastName;
            optional<String> middleName;
        };
        struct Appointment_ {
            optional<DateTime> when;
            Person_            withWhom;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML = L"<Calendar xmlns=\"" + wstring (kNSTest) +
                                L"\">\n"
                                L"  <Appointment>\n"
                                L"    <When>2005-06-01T13:00:00-05:00</When>"
                                L"    <WithWhom>\n"
                                L"            <FirstName>Jim</FirstName>"
                                L"            <LastName>Smith</LastName>"
                                L"            <MiddleName>Up</MiddleName>"
                                L"    </WithWhom>\n"
                                L"  </Appointment>\n"
                                L"  <Appointment>\n"
                                L"    <When>2005-08-01T13:00:00-05:00</When>"
                                L"    <WithWhom>\n"
                                L"            <FirstName>Fred</FirstName>"
                                L"            <LastName>Down</LastName>"
                                L"    </WithWhom>\n"
                                L"  </Appointment>\n"
                                L"</Calendar>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }
        void Test_2a_ObjectReader_viaRegistry_ ()
        {
            TraceContextBumper traceCtx{"Test_2a_ObjectReader_viaRegistry_"};

            ObjectReader::Registry registry;

            registry.AddCommonType<DateTime> ();
            registry.AddCommonType<optional<DateTime>> ();
            registry.AddCommonType<String> ();
            registry.AddCommonType<optional<String>> ();

            registry.AddCommonReader_Class<Person_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"FirstName"}, StructFieldMetaInfo{&Person_::firstName}},
                {Name{"LastName"}, StructFieldMetaInfo{&Person_::lastName}},
                {Name{"MiddleName"}, StructFieldMetaInfo{&Person_::middleName}},
            });
            registry.AddCommonReader_Class<Appointment_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"When"}, StructFieldMetaInfo{&Appointment_::when}},
                {Name{"WithWhom"}, StructFieldMetaInfo{&Appointment_::withWhom}},
            });
            registry.AddCommonType<vector<Appointment_>> (Name{"Appointment"});

            {
                vector<Appointment_>                     calendar;
                ObjectReader::IConsumerDelegateToContext ctx{
                    registry, make_shared<ObjectReader::ReadDownToReader> (
                                  make_shared<ObjectReader::RepeatedElementReader<vector<Appointment_>>> (&calendar), Name{"Appointment"})};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (calendar.size () == 2);
                VerifyTestResult (calendar[0].withWhom.firstName == "Jim");
                VerifyTestResult (calendar[0].withWhom.lastName == "Smith");
                VerifyTestResult (*calendar[0].withWhom.middleName == "Up");
                VerifyTestResult (
                    (calendar[0].when and calendar[0].when->GetDate () == Time::Date{Time::Year (2005), Time::June, Time::DayOfMonth (1)}));
                VerifyTestResult (calendar[1].withWhom.firstName == "Fred");
                VerifyTestResult (calendar[1].withWhom.lastName == "Down");
            }
            // must figure out how to get below working
            {
                vector<Appointment_>                     calendar;
                ObjectReader::IConsumerDelegateToContext ctx{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&calendar))};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (calendar.size () == 2);
                VerifyTestResult (calendar[0].withWhom.firstName == "Jim");
                VerifyTestResult (calendar[0].withWhom.lastName == "Smith");
                VerifyTestResult (*calendar[0].withWhom.middleName == "Up");
                VerifyTestResult (
                    (calendar[0].when and calendar[0].when->GetDate () == Time::Date{Time::Year (2005), Time::June, Time::DayOfMonth (1)}));
                VerifyTestResult (calendar[1].withWhom.firstName == "Fred");
                VerifyTestResult (calendar[1].withWhom.lastName == "Down");
            }
        }
    }
    void Test_SAX_ObjectReader_EXAMPLE_1_ ()
    {
        using namespace SAX_ObjectReader_EXAMPLE_1_;
        Test_2a_ObjectReader_viaRegistry_ ();
    }
}

namespace {
    namespace T3_SAXObjectReader_ReadDown2Sample_ {
        struct Person_ {
            String firstName;
            String lastName;
            bool   operator== (const Person_& rhs) const
            {
                return firstName == rhs.firstName and lastName == rhs.lastName;
            }
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML = L"<envelope1>\n"
                                L"  <envelope2>\n"
                                L"        <When>2005-06-01T13:00:00-05:00</When>"
                                L"        <WithWhom>\n"
                                L"                <FirstName>Jim</FirstName>"
                                L"                <LastName>Smith</LastName>"
                                L"        </WithWhom>\n"
                                L"  </envelope2>\n"
                                L"  <envelope2>\n"
                                L"        <When>2005-08-01T13:00:00-05:00</When>"
                                L"        <WithWhom>\n"
                                L"                <FirstName>Fred</FirstName>"
                                L"                <LastName>Down</LastName>"
                                L"        </WithWhom>\n"
                                L"  </envelope2>\n"
                                L"</envelope1>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }
        void DoTest ()
        {
            ObjectReader::Registry registry;
            registry.AddCommonType<String> ();

            registry.AddCommonReader_Class<Person_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"FirstName"}, StructFieldMetaInfo{&Person_::firstName}},
                {Name{"LastName"}, StructFieldMetaInfo{&Person_::lastName}},
            });

            vector<Person_> people;
            {
                ObjectReader::IConsumerDelegateToContext ctx{
                    registry, make_shared<ObjectReader::ReadDownToReader> (make_shared<ObjectReader::RepeatedElementReader<vector<Person_>>> (&people),
                                                                           Name{"envelope2"}, Name{"WithWhom"})};
                XML::SAXParse (mkdata_ (), ctx);

                VerifyTestResult (people.size () == 2);
                VerifyTestResult (people[0].firstName == "Jim");
                VerifyTestResult (people[0].lastName == "Smith");
                VerifyTestResult (people[1].firstName == "Fred");
                VerifyTestResult (people[1].lastName == "Down");
            }

            vector<Person_> people2; // add the vector type to the registry instead of explicitly constructing the right reader
            {
                ObjectReader::Registry newRegistry = registry;
                newRegistry.AddCommonType<vector<Person_>> (Name{"WithWhom"});
                ObjectReader::IConsumerDelegateToContext ctx{
                    newRegistry, make_shared<ObjectReader::ReadDownToReader> (newRegistry.MakeContextReader (&people2), Name{"envelope2"})};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (people2 == people);
            }

            Sequence<Person_> people3; // use sequence instead of vector
            {
                ObjectReader::Registry newRegistry = registry;
                newRegistry.AddCommonType<Sequence<Person_>> (Name{"WithWhom"});
                ObjectReader::IConsumerDelegateToContext ctx{
                    newRegistry, make_shared<ObjectReader::ReadDownToReader> (newRegistry.MakeContextReader (&people3), Name{"envelope2"})};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (people3.As<vector<Person_>> () == people);
            }
        }
    }
}

namespace {
    namespace T4_SAXObjectReader_ReadDown2Sample_MixedContent_ {
        struct ManagedObjectReference {
            String type;
            String value;
        };
        struct ObjectContent {
            ManagedObjectReference obj;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML =
                L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                L"<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                L"xmlns:soapenc=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "
                L"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\r\n"
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
                L"</soapenv:Envelope>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }

        void DoTest ()
        {
            ObjectReader::Registry mapper;

            mapper.AddCommonType<String> ();

            mapper.AddCommonReader_Class<ManagedObjectReference> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"type", Name::eAttribute}, StructFieldMetaInfo{&ManagedObjectReference::type}},
                {Name{Name::eValue}, StructFieldMetaInfo{&ManagedObjectReference::value}}});
            mapper.AddCommonReader_Class<ObjectContent> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"obj"}, StructFieldMetaInfo{&ObjectContent::obj}}
                /// wrong - must be mapping of this --metaInfo.Add ("propSet", pair<type_index, size_t> {typeid(decltype (ObjectContent::value)), offsetof(ObjectContent, propSet)});
            });

            vector<ObjectContent>                    objsContent;
            ObjectReader::IConsumerDelegateToContext ctx{
                mapper, make_shared<ObjectReader::ReadDownToReader> (make_shared<ObjectReader::RepeatedElementReader<vector<ObjectContent>>> (&objsContent),
                                                                     Name{"RetrievePropertiesResponse"}, Name{"returnval"})};
            XML::SAXParse (mkdata_ (), ctx);

            VerifyTestResult (objsContent.size () == 2);
            VerifyTestResult (objsContent[0].obj.type == "VirtualMachine");
            VerifyTestResult (objsContent[0].obj.value == "8");
            VerifyTestResult (objsContent[1].obj.type == "VirtualMachine");
            VerifyTestResult (objsContent[1].obj.value == "9");
        }
    }
}

namespace {
    namespace T5_SAXObjectReader_DocSamples_ {
        struct Person_ {
            String firstName;
            String lastName;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML = L"        <PERSON>\n"
                                L"                <FirstName>Jim</FirstName>"
                                L"                <LastName>Smith</LastName>"
                                L"        </PERSON>\n";
            ;
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStream<byte>::Ptr{InputStreamFromStdIStream<byte>::New (tmpStrm)}.ReadAll ();
        }
        void DoTest1 ()
        {
            ObjectReader::Registry mapper;
            mapper.AddCommonType<String> ();
            mapper.AddCommonReader_Class<Person_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"FirstName"}, StructFieldMetaInfo{&Person_::firstName}},
                {Name{"LastName"}, StructFieldMetaInfo{&Person_::lastName}},
            });
            Person_ p;
            ObjectReader::IConsumerDelegateToContext tmp{mapper, make_shared<ObjectReader::ReadDownToReader> (mapper.MakeContextReader (&p))};
            XML::SAXParse (mkdata_ (), tmp);
            VerifyTestResult (p.firstName == "Jim");
            VerifyTestResult (p.lastName == "Smith");
        }
        void DoTests ()
        {
            DoTest1 ();
        }
    }
}

namespace {
    namespace T6_SAXObjectReader_RepeatedElementReader_Sample_ {
        enum class GenderType_ {
            Male,
            Female,
            Stroika_Define_Enum_Bounds (Male, Female)
        };
        struct Person_ {
            String                firstName;
            String                lastName;
            optional<GenderType_> gender;
        };
        struct Address_ {
            String city;
            String state;
        };
        struct Data_ {
            vector<Person_>  people;
            vector<Address_> addresses;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML = L"<envelope1>\n"
                                L"        <person>\n"
                                L"                <FirstName>Jim</FirstName>"
                                L"                <LastName>Smith</LastName>"
                                L"                <Gender>Male</Gender>"
                                L"        </person>\n"
                                L"        <person>\n"
                                L"                <FirstName>Fred</FirstName>"
                                L"                <LastName>Down</LastName>"
                                L"        </person>\n"
                                L"        <address>\n"
                                L"                <city>Boston</city>"
                                L"                <state>MA</state>"
                                L"        </address>\n"
                                L"        <address>\n"
                                L"                <city>New York</city>"
                                L"                <state>NY</state>"
                                L"        </address>\n"
                                L"        <address>\n"
                                L"                <city>Albany</city>"
                                L"                <state>NY</state>"
                                L"        </address>\n"
                                L"</envelope1>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }

        void DoTest ()
        {
            ObjectReader::Registry registry;
            registry.AddCommonType<String> ();
            registry.AddCommonReader_NamedEnumerations<GenderType_> (Containers::Bijection<GenderType_, String>{
                pair<GenderType_, String>{GenderType_::Male, "Male"},
                pair<GenderType_, String>{GenderType_::Female, "Female"},
            });
            registry.AddCommonType<optional<GenderType_>> ();
            registry.AddCommonReader_Class<Person_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"FirstName"}, StructFieldMetaInfo{&Person_::firstName}},
                {Name{"LastName"}, StructFieldMetaInfo{&Person_::lastName}},
                {Name{"Gender"}, StructFieldMetaInfo{&Person_::gender}},
            });
            registry.AddCommonType<vector<Person_>> ();
            registry.Add<vector<Person_>> (ObjectReader::RepeatedElementReader<vector<Person_>>::AsFactory ());
            registry.AddCommonReader_Class<Address_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"city"}, StructFieldMetaInfo{&Address_::city}},
                {Name{"state"}, StructFieldMetaInfo{&Address_::state}},
            });
            registry.Add<vector<Address_>> (ObjectReader::RepeatedElementReader<vector<Address_>>::AsFactory ());
            registry.AddCommonReader_Class<Data_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"person"}, StructFieldMetaInfo{&Data_::people}},
                {Name{"address"}, StructFieldMetaInfo{&Data_::addresses}},
            });

            Data_ data;
            {
                ObjectReader::IConsumerDelegateToContext ctx{registry,
                                                             make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data))};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (data.people.size () == 2);
                VerifyTestResult (data.people[0].firstName == "Jim");
                VerifyTestResult (data.people[0].lastName == "Smith");
                VerifyTestResult (data.people[0].gender == GenderType_::Male);
                VerifyTestResult (data.people[1].firstName == "Fred");
                VerifyTestResult (data.people[1].lastName == "Down");
                VerifyTestResult (not data.people[1].gender.has_value ());
                VerifyTestResult (data.addresses.size () == 3);
                VerifyTestResult (data.addresses[0].city == "Boston");
                VerifyTestResult (data.addresses[0].state == "MA");
                VerifyTestResult (data.addresses[1].city == "New York");
                VerifyTestResult (data.addresses[1].state == "NY");
                VerifyTestResult (data.addresses[2].city == "Albany");
                VerifyTestResult (data.addresses[2].state == "NY");
            }
        }
    }
}

namespace {
    namespace T7_SAXObjectReader_BLKQCL_ReadSensors_ {
        enum class TunerNumberType_ {
            eT1 = 1,
            eT2 = 2,
            eT3 = 3,
            eT4 = 4,
            Stroika_Define_Enum_Bounds (eT1, eT4)
        };
        using WaveNumberType_  = double;
        using TemperatureType_ = double;
        using CurrentType_     = double;
        struct TECPowerConsumptionStatsType_ {
            Mapping<TunerNumberType_, CurrentType_> TunerTECCurrent;
        };
        struct SensorDataType_ {
            optional<TunerNumberType_>                  ActiveLaser;
            optional<WaveNumberType_>                   ActiveLaserWaveNumber;
            optional<TemperatureType_>                  DetectorTemperature;
            optional<TemperatureType_>                  OpticsTemperature;
            optional<TemperatureType_>                  ExternalTemperature1;
            Mapping<TunerNumberType_, TemperatureType_> LaserTemperatures;
            Mapping<TunerNumberType_, CurrentType_>     LaserCurrents;
            Mapping<TunerNumberType_, TemperatureType_> MirrorTemperatures;
            optional<TECPowerConsumptionStatsType_>     TECPowerConsumptionStats;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML =
                L"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                L"xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:blk201505=\"http://tempuri.org/blk201505.xsd\" "
                L"xmlns:blk201605=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/\" "
                L"xmlns:blk2016052=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IConfiguration\" "
                L"xmlns:blk2016053=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILaserOperation\" "
                L"xmlns:blk2016054=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IDeviceManagement\" "
                L"xmlns:blk2016055=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IManufacturing\" "
                L"xmlns:blk2016056=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILowLevelHardwareAccess\" "
                L"xmlns:blk2016057=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IBasicPersistence\" "
                L"xmlns:blk2016058=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IScanPersistence\" "
                L"xmlns:ns1=\"http://www.blockeng.com/Schemas/2015-05/BLKQCL-Common/\" "
                L"xmlns:ns2=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL-App/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "
                L"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
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
                L"               <blk201605:Temperature Tuner=\"2\">0.86115019791435543</blk201605:Temperature>\n"
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
                L"</SOAP-ENV:Envelope>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }
        /*
         *   <blk201605:LaserTemperature>\n"
         *      <blk201605:Temperature Tuner=\"1\">20.899877489241646</blk201605:Temperature>\n"
         *   </blk201605:LaserTemperature>\n"
         *
         *   Mapping<TunerNumberType,Temperature>    LaserTemperature;
         */
        template <typename TARGET_TYPE>
        struct TunerMappingReader_ : public ObjectReader::IElementConsumer {
            Mapping<TunerNumberType_, TARGET_TYPE>* fValuePtr_;
            TunerMappingReader_ (Mapping<TunerNumberType_, TARGET_TYPE>* v)
                : fValuePtr_ (v)
            {
            }
            virtual shared_ptr<IElementConsumer> HandleChildStart ([[maybe_unused]] const Name& name) override
            {
                using namespace ObjectReader;
                DISABLE_COMPILER_MSC_WARNING_START (4573)
                static const ReaderFromVoidStarFactory sEltReader_ = [] () -> ReaderFromVoidStarFactory {
                    using KVPType_ = KeyValuePair<TunerNumberType_, TARGET_TYPE>;
                    return Registry::MakeClassReader<KVPType_> (initializer_list<StructFieldInfo>{
                        {Name{"Tuner", Name::eAttribute}, StructFieldMetaInfo{&KVPType_::fKey}},
                        {Name{Name::eValue}, StructFieldMetaInfo{&KVPType_::fValue}},
                    });
                }();
                DISABLE_COMPILER_MSC_WARNING_END (4573)
                return make_shared<RepeatedElementReader<Mapping<TunerNumberType_, TARGET_TYPE>>> (fValuePtr_, sEltReader_);
            }
            static ObjectReader::ReaderFromVoidStarFactory AsFactory ()
            {
                return IElementConsumer::AsFactory<Mapping<TunerNumberType_, TARGET_TYPE>, TunerMappingReader_> ();
            }
        };
        void DoTest ()
        {
            ObjectReader::Registry registry;
            registry.AddCommonReader_NamedEnumerations<TunerNumberType_> (Containers::Bijection<TunerNumberType_, String>{
                pair<TunerNumberType_, String>{TunerNumberType_::eT1, "1"},
                pair<TunerNumberType_, String>{TunerNumberType_::eT2, "2"},
                pair<TunerNumberType_, String>{TunerNumberType_::eT3, "3"},
                pair<TunerNumberType_, String>{TunerNumberType_::eT4, "4"},
            });
            registry.AddCommonType<optional<TunerNumberType_>> ();
            registry.AddCommonType<WaveNumberType_> ();
            registry.AddCommonType<optional<WaveNumberType_>> ();
            registry.AddCommonType<TemperatureType_> ();
            registry.AddCommonType<optional<TemperatureType_>> ();
            registry.AddCommonType<CurrentType_> ();
            registry.Add<Mapping<TunerNumberType_, TemperatureType_>> (TunerMappingReader_<TemperatureType_>::AsFactory ());
            registry.Add<Mapping<TunerNumberType_, CurrentType_>> (TunerMappingReader_<CurrentType_>::AsFactory ());
            registry.Add<TECPowerConsumptionStatsType_> (TunerMappingReader_<CurrentType_>::AsFactory ());
            registry.AddCommonType<optional<TECPowerConsumptionStatsType_>> ();
            registry.AddCommonReader_Class<SensorDataType_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"ActiveLaser"}, StructFieldMetaInfo{&SensorDataType_::ActiveLaser}},
                {Name{"DetectorTemperature"}, StructFieldMetaInfo{&SensorDataType_::DetectorTemperature}},
                {Name{"OpticsTemperature"}, StructFieldMetaInfo{&SensorDataType_::OpticsTemperature}},
                {Name{"ExternalTemperature1"}, StructFieldMetaInfo{&SensorDataType_::ExternalTemperature1}},
                {Name{"LaserTemperature"}, StructFieldMetaInfo{&SensorDataType_::LaserTemperatures}},
                {Name{"LaserCurrent"}, StructFieldMetaInfo{&SensorDataType_::LaserCurrents}},
                {Name{"MirrorTemperature"}, StructFieldMetaInfo{&SensorDataType_::MirrorTemperatures}},
                {Name{"TECPowerConsumptionStats"}, StructFieldMetaInfo{&SensorDataType_::TECPowerConsumptionStats}},
            });

            SensorDataType_ data;
            {
                ObjectReader::IConsumerDelegateToContext consumerCallback{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"Sensors"})};
                //consumerCallback.fContext.fTraceThisReader = true;
                XML::SAXParse (mkdata_ (), consumerCallback);
                DbgTrace (L"LaserTemperatures=%s", Characters::ToString (data.LaserTemperatures).c_str ());
                DbgTrace (L"MirrorTemperature=%s", Characters::ToString (data.MirrorTemperatures).c_str ());
                DbgTrace (L"LaserCurrents=%s", Characters::ToString (data.LaserCurrents).c_str ());
                DbgTrace (L"TECPowerConsumptionStats=%s", Characters::ToString (data.TECPowerConsumptionStats->TunerTECCurrent).c_str ());
                VerifyTestResult (not data.ActiveLaser.has_value ());
                VerifyTestResult (Math::NearlyEquals (*data.DetectorTemperature, 13.1));
                VerifyTestResult (Math::NearlyEquals (*data.OpticsTemperature, 0.86115019791435543));
                VerifyTestResult ((data.LaserTemperatures.Keys () == Set<TunerNumberType_>{TunerNumberType_::eT1}));
                VerifyTestResult (Math::NearlyEquals (*data.LaserTemperatures.Lookup (TunerNumberType_::eT1), 20.899877489241646));
                VerifyTestResult ((data.LaserCurrents.Keys () == Set<TunerNumberType_>{TunerNumberType_::eT1}));
                VerifyTestResult (Math::NearlyEquals (*data.LaserCurrents.Lookup (TunerNumberType_::eT1), 0.86871794871794872));
                VerifyTestResult ((data.MirrorTemperatures.Keys () == Set<TunerNumberType_>{TunerNumberType_::eT2}));
                VerifyTestResult (Math::NearlyEquals (*data.MirrorTemperatures.Lookup (TunerNumberType_::eT2), 0.86115019791435543));
                VerifyTestResult ((data.TECPowerConsumptionStats->TunerTECCurrent.Keys () ==
                                   Set<TunerNumberType_>{TunerNumberType_::eT1, TunerNumberType_::eT2, TunerNumberType_::eT3, TunerNumberType_::eT4}));
                VerifyTestResult (Math::NearlyEquals (*data.ExternalTemperature1, 0.0));
            }
        }
    }
}
namespace Stroika::Foundation::Configuration {
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunused-const-variable\"")
    template <>
    constexpr EnumNames<T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerNumberType_> DefaultNames<T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerNumberType_>::k{
        EnumNames<T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerNumberType_>::BasicArrayInitializer{{
            {T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerNumberType_::eT1, L"eT1"},
            {T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerNumberType_::eT2, L"eT2"},
            {T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerNumberType_::eT3, L"eT3"},
            {T7_SAXObjectReader_BLKQCL_ReadSensors_::TunerNumberType_::eT4, L"eT4"},
        }}};
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunused-const-variable\"")
}

namespace {
    namespace T8_SAXObjectReader_BLKQCL_ReadAlarms_ {
        using AlarmType_ = String;
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML =
                L"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                L"xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:blk201505=\"http://tempuri.org/blk201505.xsd\" "
                L"xmlns:blk201605=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/\" "
                L"xmlns:blk2016052=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IConfiguration\" "
                L"xmlns:blk2016053=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILaserOperation\" "
                L"xmlns:blk2016054=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IDeviceManagement\" "
                L"xmlns:blk2016055=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IManufacturing\" "
                L"xmlns:blk2016056=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILowLevelHardwareAccess\" "
                L"xmlns:blk2016057=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IBasicPersistence\" "
                L"xmlns:blk2016058=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IScanPersistence\" "
                L"xmlns:ns1=\"http://www.blockeng.com/Schemas/2015-05/BLKQCL-Common/\" "
                L"xmlns:ns2=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL-App/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "
                L"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
                L"   <SOAP-ENV:Header>\n"
                L"      <blk201505:timestamp>8557.8791092709998</blk201505:timestamp>\n"
                L"   </SOAP-ENV:Header>\n"
                L"   <SOAP-ENV:Body>\n"
                L"                      <blk201605:GetAlarmsResponse>\n"
                L"                              <blk201605:Alarm>Fred</blk201605:Alarm>\n"
                L"                              <blk201605:Alarm>Critical_LaserOverheating</blk201605:Alarm>\n"
                L"                      </blk201605:GetAlarmsResponse>>\n"
                L"   </SOAP-ENV:Body>\n"
                L"</SOAP-ENV:Envelope>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }
        void DoTest ()
        {
            ObjectReader::Registry registry;
            registry.AddCommonType<AlarmType_> ();
            registry.Add<Set<AlarmType_>> (ObjectReader::RepeatedElementReader<Set<AlarmType_>>::AsFactory ());
            {
                // Example matching ANY sub-element
                Set<AlarmType_> data;
                {
                    ObjectReader::IConsumerDelegateToContext consumerCallback{
                        registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data),
                                                                               Name{"GetAlarmsResponse"}, Name{"Alarm"})};
                    XML::SAXParse (mkdata_ (), consumerCallback);
                    DbgTrace (L"Alarms=%s", Characters::ToString (data).c_str ());
                }
                VerifyTestResult ((data == Set<AlarmType_>{"Fred", "Critical_LaserOverheating"}));
            }
            const Name kAlarmName_ = Name{"Alarm"};
            registry.Add<Set<AlarmType_>> (ObjectReader::RepeatedElementReader<Set<AlarmType_>>::AsFactory (kAlarmName_));
            {
                // Example matching THE RIGHT sub-element
                Set<AlarmType_> data;
                {
                    ObjectReader::IConsumerDelegateToContext consumerCallback{
                        registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"GetAlarmsResponse"}, kAlarmName_)};
                    XML::SAXParse (mkdata_ (), consumerCallback);
                    DbgTrace (L"Alarms=%s", Characters::ToString (data).c_str ());
                }
                VerifyTestResult ((data == Set<AlarmType_>{"Fred", "Critical_LaserOverheating"}));
            }
            const Name kWrongAlarmName_ = Name{"xxxAlarm"};
            registry.Add<Set<AlarmType_>> (ObjectReader::RepeatedElementReader<Set<AlarmType_>>::AsFactory (kWrongAlarmName_));
            {
                // Example matching THE WRONG sub-element
                Set<AlarmType_> data;
                {
                    ObjectReader::IConsumerDelegateToContext consumerCallback{
                        registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data),
                                                                               Name{"GetAlarmsResponse"}, kWrongAlarmName_)};
                    XML::SAXParse (mkdata_ (), consumerCallback);
                    DbgTrace (L"Alarms=%s", Characters::ToString (data).c_str ());
                }
                VerifyTestResult ((data == Set<AlarmType_>{}));
            }
        }
    }
}

namespace {
    namespace T9_SAXObjectReader_BLKQCL_ReadScanDetails_ {

        using ScanIDType_                 = uint32_t;
        using WaveNumberType_             = double;
        using IntensityType_              = double;
        using SpectrumType_               = SortedMapping<double, double>;
        using PersistenceScanAuxDataType_ = Mapping<String, String>;
        struct PersistentScanDetailsType_ {
            ScanIDType_                 ScanID{};
            optional<DateTime>          ScanStart;
            optional<DateTime>          ScanEnd;
            optional<String>            ScanLabel;
            optional<SpectrumType_>     RawSpectrum;
            PersistenceScanAuxDataType_ AuxData;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML =
                L"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                L"xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:blk201505=\"http://tempuri.org/blk201505.xsd\" "
                L"xmlns:blk201605=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/\" "
                L"xmlns:blk2016052=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IConfiguration\" "
                L"xmlns:blk2016053=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILaserOperation\" "
                L"xmlns:blk2016054=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IDeviceManagement\" "
                L"xmlns:blk2016055=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IManufacturing\" "
                L"xmlns:blk2016056=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-ILowLevelHardwareAccess\" "
                L"xmlns:blk2016057=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IBasicPersistence\" "
                L"xmlns:blk2016058=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL/SOAP-IScanPersistence\" "
                L"xmlns:ns1=\"http://www.blockeng.com/Schemas/2015-05/BLKQCL-Common/\" "
                L"xmlns:ns2=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL-App/\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "
                L"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
                L"   <SOAP-ENV:Header>\n"
                L"      <blk201505:timestamp>8557.8791092709998</blk201505:timestamp>\n"
                L"   </SOAP-ENV:Header>\n"
                L"   <SOAP-ENV:Body>\n"
                L"              <blk201605:ScanPersistenceGetScanDetailsResponse>\n"
                L"                      <blk201605:Scan>\n"
                L"                              <blk201605:ScanID>8320</blk201605:ScanID>\n"
                L"                              <blk201605:ScanStart>2016-07-28T20:14:30Z</blk201605:ScanStart>\n"
                L"                              <blk201605:ScanEnd>2016-07-28T20:14:44Z</blk201605:ScanEnd>\n"
                L"                              <blk201605:ScanKind>Reference</blk201605:ScanKind>\n"
                L"                              <blk201605:RawSpectrum>\n"
                L"                                      <ns1:Measurement intensity=\"0\" waveNumber=\"901.5\"/>\n"
                L"                                      <ns1:Measurement intensity=\"1\" waveNumber=\"902.5\"/>\n"
                //...
                L"                              </blk201605:RawSpectrum>\n"
                L"                              <blk201605:AuxData>\n"
                L"                                      <blk201605:KeyValuePair Value=\"1000\" Key=\"Cell-Pressure\"/>\n"
                L"                                      <blk201605:KeyValuePair Value=\"0\" Key=\"Cell-Temperature\"/>\n"
                L"                                      <blk201605:KeyValuePair Value=\"B1E56F82-B217-40D3-A24D-FAC491EDCDE8\" "
                L"Key=\"EngineId\"/>\n"
                L"                              </blk201605:AuxData>\n"
                L"                      </blk201605:Scan>\n"
                L"              </blk201605:ScanPersistenceGetScanDetailsResponse>\n"
                L"   </SOAP-ENV:Body>\n"
                L"</SOAP-ENV:Envelope>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }
        namespace PRIVATE_ {

            struct SpectrumReader_ : public ObjectReader::IElementConsumer {
                SpectrumType_* fValuePtr_;
                SpectrumReader_ (SpectrumType_* v)
                    : fValuePtr_{v}
                {
                }
                virtual shared_ptr<IElementConsumer> HandleChildStart ([[maybe_unused]] const Name& name) override
                {
                    using namespace ObjectReader;
                    DISABLE_COMPILER_MSC_WARNING_START (4573)
                    static const ReaderFromVoidStarFactory sEltReader_ = [] () -> ReaderFromVoidStarFactory {
                        using KVPType_ = SpectrumType_::value_type;
                        return Registry::MakeClassReader<KVPType_> (initializer_list<StructFieldInfo>{
                            {Name{"waveNumber", Name::eAttribute}, StructFieldMetaInfo{&KVPType_::fKey}},
                            {Name{"intensity", Name::eAttribute}, StructFieldMetaInfo{&KVPType_::fValue}},
                        });
                    }();
                    DISABLE_COMPILER_MSC_WARNING_END (4573)
                    return make_shared<RepeatedElementReader<SpectrumType_>> (fValuePtr_, sEltReader_);
                }
                static ObjectReader::ReaderFromVoidStarFactory AsFactory ()
                {
                    return IElementConsumer::AsFactory<SpectrumType_, SpectrumReader_> ();
                }
            };
            struct StringKVStringReader : public ObjectReader::IElementConsumer {
                Mapping<String, String>* fValuePtr_;
                StringKVStringReader (Mapping<String, String>* v)
                    : fValuePtr_{v}
                {
                }
                virtual shared_ptr<IElementConsumer> HandleChildStart ([[maybe_unused]] const Name& name) override
                {
                    using namespace ObjectReader;
                    static const ReaderFromVoidStarFactory sEltReader_ = [] () -> ReaderFromVoidStarFactory {
                        using KVPType_ = KeyValuePair<String, String>;
                        return Registry::MakeClassReader<KVPType_> (initializer_list<StructFieldInfo>{
                            {Name{"Key", Name::eAttribute}, StructFieldMetaInfo{&KVPType_::fKey}},
                            {Name{"Value", Name::eAttribute}, StructFieldMetaInfo{&KVPType_::fValue}},
                        });
                    }();
                    return make_shared<RepeatedElementReader<Mapping<String, String>>> (fValuePtr_, sEltReader_);
                }
                static ObjectReader::ReaderFromVoidStarFactory AsFactory ()
                {
                    return IElementConsumer::AsFactory<Mapping<String, String>, StringKVStringReader> ();
                }
            };
        }
        void DoTest ()
        {
            ObjectReader::Registry registry;
            registry.AddCommonType<ScanIDType_> ();
            registry.AddCommonType<WaveNumberType_> ();
            registry.AddCommonType<IntensityType_> ();
            registry.AddCommonType<DateTime> ();
            registry.AddCommonType<optional<DateTime>> ();
            registry.AddCommonType<String> ();
            registry.AddCommonType<optional<String>> ();
            registry.Add<SpectrumType_> (PRIVATE_::SpectrumReader_::AsFactory ());
            registry.AddCommonType<optional<SpectrumType_>> ();
            registry.Add<PersistenceScanAuxDataType_> (PRIVATE_::StringKVStringReader::AsFactory ());
            registry.AddCommonReader_Class<PersistentScanDetailsType_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"ScanID"}, StructFieldMetaInfo{&PersistentScanDetailsType_::ScanID}},
                {Name{"ScanStart"}, StructFieldMetaInfo{&PersistentScanDetailsType_::ScanStart}},
                {Name{"ScanEnd"}, StructFieldMetaInfo{&PersistentScanDetailsType_::ScanEnd}},
                {Name{"ScanLabel"}, StructFieldMetaInfo{&PersistentScanDetailsType_::ScanLabel}},
                {Name{"RawSpectrum"}, StructFieldMetaInfo{&PersistentScanDetailsType_::RawSpectrum}},
                {Name{"AuxData"}, StructFieldMetaInfo{&PersistentScanDetailsType_::AuxData}},
            });
            PersistentScanDetailsType_ data;
            {
                ObjectReader::IConsumerDelegateToContext consumerCallback{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data),
                                                                           Name{"ScanPersistenceGetScanDetailsResponse"}, Name{"Scan"})};
                //consumerCallback.fContext.fTraceThisReader = true;
                XML::SAXParse (mkdata_ (), consumerCallback);
                DbgTrace (L"ScanID=%s", Characters::ToString (data.ScanID).c_str ());
                DbgTrace (L"ScanStart=%s", Characters::ToString (data.ScanStart).c_str ());
                DbgTrace (L"ScanEnd=%s", Characters::ToString (data.ScanEnd).c_str ());
                if (data.ScanLabel) {
                    DbgTrace (L"ScanLabel=%s", Characters::ToString (*data.ScanLabel).c_str ());
                }
                if (data.RawSpectrum) {
                    DbgTrace (L"RawSpectrum=%s", Characters::ToString (*data.RawSpectrum).c_str ());
                }
                DbgTrace (L"AuxData=%s", Characters::ToString (data.AuxData).c_str ());
                VerifyTestResult (data.ScanID == 8320);
                VerifyTestResult (data.ScanStart == DateTime::Parse ("2016-07-28T20:14:30Z", DateTime::kISO8601Format));
                VerifyTestResult (data.ScanEnd == DateTime::Parse ("2016-07-28T20:14:44Z", DateTime::kISO8601Format));
                VerifyTestResult (not data.ScanLabel.has_value ());
                VerifyTestResult ((data.RawSpectrum == Mapping<WaveNumberType_, IntensityType_>{pair<WaveNumberType_, IntensityType_>{901.5, 0},
                                                                                                pair<WaveNumberType_, IntensityType_>{902.5, 1}}));
                VerifyTestResult (
                    (data.AuxData == Mapping<String, String>{pair<String, String>{"Cell-Pressure", "1000"}, pair<String, String>{"Cell-Temperature", "0"},
                                                             pair<String, String>{"EngineId", "B1E56F82-B217-40D3-A24D-FAC491EDCDE8"}}));
            }
        }
    }
}

namespace {
    namespace T10_SAXObjectReader_NANValues_ {
        namespace PRIVATE_ {
            struct Values_ {
                double valueMissing;
                double valueExplicitGood;
                double valueExplicitNAN1;
                double valueExplicitNAN2;
            };
            Memory::BLOB mkdata_ ()
            {
                wstring newDocXML = L"<Values>\n"
                                    //L"          <valueMissing></valueMissing>"
                                    L"            <valueExplicitGood>3.0</valueExplicitGood>"
                                    L"            <valueExplicitNAN1>NAN</valueExplicitNAN1>"
                                    L"            <valueExplicitNAN2>NAN</valueExplicitNAN2>"
                                    L"</Values>\n";
                stringstream tmpStrm;
                WriteTextStream_ (newDocXML, tmpStrm);
                return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
            }
        }
        void DoTest ()
        {
            using namespace PRIVATE_;
            TraceContextBumper     traceCtx{"T10_SAXObjectReader_NANValues_"};
            ObjectReader::Registry registry;
            registry.AddCommonType<double> ();
            registry.AddCommonReader_Class<Values_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"valueMissing"}, StructFieldMetaInfo{&Values_::valueMissing}},
                {Name{"valueExplicitGood"}, StructFieldMetaInfo{&Values_::valueExplicitGood}},
                {Name{"valueExplicitNAN1"}, StructFieldMetaInfo{&Values_::valueExplicitNAN1}},
                {Name{"valueExplicitNAN2"}, StructFieldMetaInfo{&Values_::valueExplicitNAN2}},
            });
            {
                Values_ values{};
                values.valueMissing = 999;
                ObjectReader::IConsumerDelegateToContext ctx{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&values), Name{"Values"})};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (values.valueMissing == 999);
                VerifyTestResult (Math::NearlyEquals (values.valueExplicitGood, 3.0));
                VerifyTestResult (isnan (values.valueExplicitNAN1));
                VerifyTestResult (isnan (values.valueExplicitNAN2));
            }
        }
    }
}

namespace {
    namespace T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_ {
        enum class TunerNumberType_ {
            eT1 = 1,
            eT2 = 2,
            eT3 = 3,
            eT4 = 4,
            Stroika_Define_Enum_Bounds (eT1, eT4)
        };
        using FrequencyType_ = double;
        struct PerTunerFactorySettingsType_ {
            optional<FrequencyType_> MirrorOperationFrequency;
            optional<FrequencyType_> MirrorResonantFrequency;
            nonvirtual String        ToString () const
            {
                StringBuilder out;
                out += "{";
                if (MirrorOperationFrequency) {
                    out += "MirrorOperationFrequency: '" + Characters::ToString (*MirrorOperationFrequency) + "',";
                }
                if (MirrorResonantFrequency) {
                    out += "MirrorResonantFrequency: '" + Characters::ToString (*MirrorResonantFrequency) + "',";
                }
                out += "}";
                return out.str ();
            }
        };
        struct FactorySettingsType_ {
            Mapping<TunerNumberType_, PerTunerFactorySettingsType_> Tuners;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML =
                L"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                L"xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
                L"xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:ns1=\"http://www.blockeng.com/Schemas/2015-05/BLKQCL-Common/\" "
                L"xmlns:ns2=\"http://www.blockeng.com/Schemas/2016-05/BLKQCL-App/\" "
                L"xmlns:blk2017042=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/SOAP-IConfiguration\" "
                L"xmlns:blk2017043=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/SOAP-ILaserOperation\" "
                L"xmlns:blk2017044=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/SOAP-IDeviceManagement\" "
                L"xmlns:blk2017045=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/SOAP-IManufacturing\" "
                L"xmlns:blk2017046=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/SOAP-ILowLevelHardwareAccess\" "
                L"xmlns:blk2017047=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/SOAP-IBasicPersistence\" "
                L"xmlns:blk2017048=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/SOAP-IScanPersistence\" "
                L"xmlns:blk201704=\"http://www.blockeng.com/Schemas/2017-04x/BLKQCL/\" "
                L"xmlns:blk201505=\"http://tempuri.org/blk201505.xsd\">\n"
                L"   <SOAP-ENV:Header>\n"
                L"      <blk201505:timestamp>465.104564455</blk201505:timestamp>\n"
                L"   </SOAP-ENV:Header>\n"
                L"   <SOAP-ENV:Body>\n"
                L"      <blk201704:GetFactorySettingsResponse>\n"
                L"         <blk201704:Tuners>\n"
                L"            <blk201704:Tuner Tuner=\"1\">\n"
                L"               <blk201704:MirrorResonantFrequency>150</blk201704:MirrorResonantFrequency>\n"
                L"               <blk201704:MirrorOperationFrequency>40</blk201704:MirrorOperationFrequency>\n"
                L"            </blk201704:Tuner>\n"
                L"            <blk201704:Tuner Tuner=\"2\">\n"
                L"               <blk201704:MirrorResonantFrequency>151</blk201704:MirrorResonantFrequency>\n"
                L"               <blk201704:MirrorOperationFrequency>41</blk201704:MirrorOperationFrequency>\n"
                L"            </blk201704:Tuner>\n"
                L"         </blk201704:Tuners>\n"
                L"      </blk201704:GetFactorySettingsResponse>\n"
                L"   </SOAP-ENV:Body>\n"
                L"</SOAP-ENV:Envelope>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }

        static const ObjectReader::ReaderFromVoidStarFactory k_PerTunerFactorySettingsType_ReaderFactory_ =
            ObjectReader::Registry::MakeClassReader<PerTunerFactorySettingsType_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{L"MirrorOperationFrequency"}, StructFieldMetaInfo{&PerTunerFactorySettingsType_::MirrorOperationFrequency}},
                {Name{L"MirrorResonantFrequency"}, StructFieldMetaInfo{&PerTunerFactorySettingsType_::MirrorResonantFrequency}},
            });

        /*
         *  <blk201704:Tuners>\n"
         *      <blk201704:Tuner Tuner=\"3\">\n"
         *               <blk201704:MirrorResonantFrequency>150</blk201704:MirrorResonantFrequency>\n"
         *               <blk201704:MirrorOperationFrequency>40</blk201704:MirrorOperationFrequency>\n"
         *      </blk201605:Tuner>\n"
         *  </blk201704:Tuners>\n"
         *
         *   Mapping<TunerNumberType,Temperature>    LaserTemperature;
         */
        struct TunerMappingReader_ : public ObjectReader::IElementConsumer {
            Mapping<TunerNumberType_, PerTunerFactorySettingsType_>* fValuePtr_;
            TunerMappingReader_ (Mapping<TunerNumberType_, PerTunerFactorySettingsType_>* v)
                : fValuePtr_{v}
            {
            }
            virtual shared_ptr<IElementConsumer> HandleChildStart ([[maybe_unused]] const Name& name) override
            {
                using namespace ObjectReader;
                struct MyKVPReader_ : MixinReader<KeyValuePair<TunerNumberType_, PerTunerFactorySettingsType_>> {
                    static Sequence<MixinEltTraits> mkMixinHelpers_ ()
                    {
                        using KVPType_ = KeyValuePair<TunerNumberType_, PerTunerFactorySettingsType_>;
                        static const ReaderFromVoidStarFactory kTunerReader_ = Registry::MakeClassReader<KVPType_> (initializer_list<StructFieldInfo>{
                            {Name{"Tuner", Name::eAttribute}, StructFieldMetaInfo{&KVPType_::fKey}},
                        });
                        Sequence<MixinEltTraits> tmp;
                        tmp += MixinEltTraits{kTunerReader_,
                                              [] (const Name& name) {
                                                  return name == Name{"Tuner", Name::eAttribute};
                                              },
                                              [] (KVPType_* kvp) { return reinterpret_cast<byte*> (&kvp->fKey); }};
                        tmp += MixinEltTraits{k_PerTunerFactorySettingsType_ReaderFactory_,
                                              [] (const Name& name) {
                                                  return name != Name{"Tuner", Name::eAttribute};
                                              },
                                              [] (KVPType_* kvp) { return reinterpret_cast<byte*> (&kvp->fValue); }};
                        return tmp;
                    }
                    MyKVPReader_ (KeyValuePair<TunerNumberType_, PerTunerFactorySettingsType_>* v)
                        : MixinReader<KeyValuePair<TunerNumberType_, PerTunerFactorySettingsType_>> (v, mkMixinHelpers_ ())
                    {
                    }
                    static ReaderFromVoidStarFactory AsFactory ()
                    {
                        return IElementConsumer::AsFactory<KeyValuePair<TunerNumberType_, PerTunerFactorySettingsType_>, MyKVPReader_> ();
                    }
                };
                return make_shared<RepeatedElementReader<Mapping<TunerNumberType_, PerTunerFactorySettingsType_>>> (fValuePtr_,
                                                                                                                    MyKVPReader_::AsFactory ());
            }
            static ObjectReader::ReaderFromVoidStarFactory AsFactory ()
            {
                return IElementConsumer::AsFactory<Mapping<TunerNumberType_, PerTunerFactorySettingsType_>, TunerMappingReader_> ();
            }
        };
        void DoTest ()
        {
            ObjectReader::Registry registry;
            registry.AddCommonReader_NamedEnumerations<TunerNumberType_> (Containers::Bijection<TunerNumberType_, String>{
                pair<TunerNumberType_, String>{TunerNumberType_::eT1, "1"},
                pair<TunerNumberType_, String>{TunerNumberType_::eT2, "2"},
                pair<TunerNumberType_, String>{TunerNumberType_::eT3, "3"},
                pair<TunerNumberType_, String>{TunerNumberType_::eT4, "4"},
            });
            registry.AddCommonType<optional<TunerNumberType_>> ();
            registry.AddCommonType<FrequencyType_> ();
            registry.AddCommonType<optional<FrequencyType_>> ();
            registry.Add<PerTunerFactorySettingsType_> (k_PerTunerFactorySettingsType_ReaderFactory_);
            registry.Add<Mapping<TunerNumberType_, PerTunerFactorySettingsType_>> (TunerMappingReader_::AsFactory ());
            registry.AddCommonReader_Class<FactorySettingsType_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"Tuners"}, StructFieldMetaInfo{&FactorySettingsType_::Tuners}},
            });

            FactorySettingsType_ data;
            {
                ObjectReader::IConsumerDelegateToContext consumerCallback{
                    registry,
                    make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"GetFactorySettingsResponse"})};
                //consumerCallback.fContext.fTraceThisReader = true;
                XML::SAXParse (mkdata_ (), consumerCallback);
                DbgTrace (L"Tuners=%s", Characters::ToString (data.Tuners).c_str ());
                VerifyTestResult ((data.Tuners.Keys () == Set<TunerNumberType_>{TunerNumberType_::eT1, TunerNumberType_::eT2}));
                VerifyTestResult (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT1)->MirrorOperationFrequency, 40.0));
                VerifyTestResult (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT1)->MirrorResonantFrequency, 150.0));
                VerifyTestResult (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT2)->MirrorOperationFrequency, 41.0));
                VerifyTestResult (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT2)->MirrorResonantFrequency, 151.0));
            }
        }
    }
}
namespace Stroika::Foundation::Configuration {
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunused-const-variable\"")
    // for ToString ()
    template <>
    constexpr EnumNames<T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::TunerNumberType_>
        DefaultNames<T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::TunerNumberType_>::k{
            EnumNames<T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::TunerNumberType_>::BasicArrayInitializer{{
                {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::TunerNumberType_::eT1, L"eT1"},
                {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::TunerNumberType_::eT2, L"eT2"},
                {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::TunerNumberType_::eT3, L"eT3"},
                {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::TunerNumberType_::eT4, L"eT4"},
            }}};
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunused-const-variable\"")
}

namespace {
    namespace T12_RangeReader_ {
        namespace PRIVATE_ {
            using Traversal::Range;
            using MY_TEST_RANGE_ = Range<double>;
            struct Values_ {
                MY_TEST_RANGE_ r{};
            };
            Memory::BLOB mkdata_ ()
            {
                wstring newDocXML = L"<Values>\n"
                                    L"            <r LowerBound=\"3.0\" UpperBound=\"6.0\"/>"
                                    L"</Values>\n";
                stringstream tmpStrm;
                WriteTextStream_ (newDocXML, tmpStrm);
                return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
            }
        }
        void DoTest ()
        {
            using namespace PRIVATE_;
            TraceContextBumper     traceCtx{"T12_RangeReader_"};
            ObjectReader::Registry registry;
            registry.AddCommonType<MY_TEST_RANGE_::value_type> ();
            registry.Add<MY_TEST_RANGE_> (ObjectReader::RangeReader<MY_TEST_RANGE_>::AsFactory ());
            registry.AddCommonReader_Class<Values_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"r"}, StructFieldMetaInfo{&Values_::r}},
            });
            {
                Values_                                  values{};
                ObjectReader::IConsumerDelegateToContext ctx{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&values), Name{"Values"})};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (Math::NearlyEquals (values.r.GetLowerBound (), 3.0));
                VerifyTestResult (Math::NearlyEquals (values.r.GetUpperBound (), 6.0));
            }
        }
    }
}

namespace {

    namespace T13_SAXObjectReader_OverrideTypeInStructInfo_ {
        enum class GenderType_ {
            Male,
            Female,
            Stroika_Define_Enum_Bounds (Male, Female)
        };
        struct Person_ {
            String      firstName;
            String      lastName;
            GenderType_ gender;
        };
        struct Data_ {
            vector<Person_> people;
        };
        Memory::BLOB mkdata_ ()
        {
            wstring newDocXML = L"<envelope1>\n"
                                L"        <person>\n"
                                L"                <FirstName>Jim</FirstName>"
                                L"                <LastName>Smith</LastName>"
                                L"                <Gender>Male</Gender>"
                                L"        </person>\n"
                                L"        <person>\n"
                                L"                <FirstName>Fred</FirstName>"
                                L"                <LastName>Down</LastName>"
                                L"                <Gender>Female</Gender>"
                                L"        </person>\n"
                                L"</envelope1>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
        }
        void DoTest ()
        {
            ObjectReader::Registry registry;
            registry.AddCommonType<String> ();
            static const auto kGenderType_Reader_ =
                ObjectReader::Registry::MakeCommonReader_NamedEnumerations<GenderType_> (Containers::Bijection<GenderType_, String>{
                    pair<GenderType_, String>{GenderType_::Male, "Male"},
                    pair<GenderType_, String>{GenderType_::Female, "Female"},
                });
            registry.AddCommonReader_Class<Person_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"FirstName"}, StructFieldMetaInfo{&Person_::firstName}},
                {Name{"LastName"}, StructFieldMetaInfo{&Person_::lastName}},
                {Name{"Gender"}, StructFieldMetaInfo{&Person_::gender}, kGenderType_Reader_},
            });
            registry.AddCommonType<vector<Person_>> ();
            registry.Add<vector<Person_>> (ObjectReader::RepeatedElementReader<vector<Person_>>::AsFactory ());
            registry.AddCommonReader_Class<Data_> (initializer_list<ObjectReader::StructFieldInfo>{
                {Name{"person"}, StructFieldMetaInfo{&Data_::people}},
            });

            Data_ data;
            {
                ObjectReader::IConsumerDelegateToContext ctx{registry,
                                                             make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data))};
                XML::SAXParse (mkdata_ (), ctx);
                VerifyTestResult (data.people.size () == 2);
                VerifyTestResult (data.people[0].firstName == "Jim");
                VerifyTestResult (data.people[0].lastName == "Smith");
                VerifyTestResult (data.people[0].gender == GenderType_::Male);
                VerifyTestResult (data.people[1].firstName == "Fred");
                VerifyTestResult (data.people[1].lastName == "Down");
                VerifyTestResult (data.people[1].gender == GenderType_::Female);
            }
        }
    }
}

namespace T14_SAXObjectReader_CustomSimpleType_ {

    // Custom Type will require a custom reader, but thats easy to do for most simple types
    // Use MakeCommonReader_SimpleStringish for this type
    struct GenderType_ {
        String fRep;
    };

    struct Person_ {
        String      firstName;
        String      lastName;
        GenderType_ gender;
    };
    struct Data_ {
        vector<Person_> people;
    };
    Memory::BLOB mkdata_ ()
    {
        wstring newDocXML = L"<envelope1>\n"
                            L"        <person>\n"
                            L"                <FirstName>Jim</FirstName>"
                            L"                <LastName>Smith</LastName>"
                            L"                <Gender>Male</Gender>"
                            L"        </person>\n"
                            L"        <person>\n"
                            L"                <FirstName>Fred</FirstName>"
                            L"                <LastName>Down</LastName>"
                            L"                <Gender>Female</Gender>"
                            L"        </person>\n"
                            L"</envelope1>\n";
        stringstream tmpStrm;
        WriteTextStream_ (newDocXML, tmpStrm);
        return InputStreamFromStdIStream<byte>::New (tmpStrm).ReadAll ();
    }
    void DoTest ()
    {
        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();
        registry.AddCommonReader_Simple<GenderType_> ([] (String s) -> GenderType_ {
            GenderType_ result;
            result.fRep = s;
            return result;
        });
        registry.AddCommonReader_Class<Person_> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{"FirstName"}, StructFieldMetaInfo{&Person_::firstName}},
            {Name{"LastName"}, StructFieldMetaInfo{&Person_::lastName}},
            {Name{"Gender"}, StructFieldMetaInfo{&Person_::gender}},
        });
        registry.AddCommonType<vector<Person_>> ();
        registry.Add<vector<Person_>> (ObjectReader::RepeatedElementReader<vector<Person_>>::AsFactory ());
        registry.AddCommonReader_Class<Data_> (initializer_list<ObjectReader::StructFieldInfo>{
            {Name{"person"}, StructFieldMetaInfo{&Data_::people}},
        });

        Data_ data;
        {
            ObjectReader::IConsumerDelegateToContext ctx{registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data))};
            XML::SAXParse (mkdata_ (), ctx);
            VerifyTestResult (data.people.size () == 2);
            VerifyTestResult (data.people[0].firstName == "Jim");
            VerifyTestResult (data.people[0].lastName == "Smith");
            VerifyTestResult (data.people[0].gender.fRep == "Male");
            VerifyTestResult (data.people[1].firstName == "Fred");
            VerifyTestResult (data.people[1].lastName == "Down");
            VerifyTestResult (data.people[1].gender.fRep == "Female");
        }
    }
}
#endif

namespace {

    void DoRegressionTests_ ()
    {
#if qStroika_Foundation_DataExchange_XML_SupportParsing
        Test_1_SAXParser_ ();
        Test_SAX_ObjectReader_EXAMPLE_1_ ();
        T3_SAXObjectReader_ReadDown2Sample_::DoTest ();
        T4_SAXObjectReader_ReadDown2Sample_MixedContent_::DoTest ();
        T5_SAXObjectReader_DocSamples_::DoTests ();
        T6_SAXObjectReader_RepeatedElementReader_Sample_::DoTest ();
        T7_SAXObjectReader_BLKQCL_ReadSensors_::DoTest ();
        T8_SAXObjectReader_BLKQCL_ReadAlarms_::DoTest ();
        T9_SAXObjectReader_BLKQCL_ReadScanDetails_::DoTest ();
        T10_SAXObjectReader_NANValues_::DoTest ();
        T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_::DoTest ();
        T12_RangeReader_::DoTest ();
        T13_SAXObjectReader_OverrideTypeInStructInfo_::DoTest ();
        T14_SAXObjectReader_CustomSimpleType_::DoTest ();
#endif
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
