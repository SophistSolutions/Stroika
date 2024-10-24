﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::XML
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/SortedMapping.h"
#include "Stroika/Foundation/DataExchange/StructuredStreamEvents/ObjectReader.h"
#include "Stroika/Foundation/DataExchange/XML/DOM.h"
#include "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include "Stroika/Foundation/DataExchange/XML/Schema.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/iostream/InputStreamFromStdIStream.h"
#include "Stroika/Foundation/Time/Realtime.h"

#if qHasFeature_libxml2
#include "Stroika/Foundation/DataExchange/XML/Providers/LibXML2.h"
#endif
#if qHasFeature_Xerces
#include "Stroika/Foundation/DataExchange/XML/Providers/Xerces.h"
#endif

#include "Stroika/Frameworks/Test/TestHarness.h"

namespace Resources_ {
    constexpr
#include "Tests/34/SampleCCR.ccr.embed"
        constexpr
#include "Tests/34/ASTM_CCR_V1.xsd.embed"
        constexpr
#include "Tests/34/personal.xml.embed"
        constexpr
#include "Tests/34/personal.xsd.embed"
        constexpr
#include "Tests/34/HealthFrameWorks_v3.xml.embed"
        constexpr
#include "Tests/34/ReferenceContent-2012-03.xsd.embed"
}

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Streams;
using namespace Streams::iostream;

using namespace Stroika::Frameworks;

using Common::KeyValuePair;
using Containers::Sequence;
using Containers::Set;
using Containers::SortedMapping;
using Debug::TraceContextBumper;
using Memory::BLOB;

#if qHasFeature_GoogleTest
namespace {
    //
    // PUT THIS OR SOMETHING LIKE IT TO STROIKA EVENTUALLY
    //  void    StreamUtils::WriteTextStream (const wstring& w, ostream& out)
    void WriteTextStream_ (const wstring& w, ostream& out)
    {
        CodeCvt<wchar_t> codeCvt{UnicodeExternalEncodings::eUTF8};
        size_t           sz = codeCvt.ComputeTargetByteBufferSize (w.length ());
        // const auto       bom = GetByteOrderMark (UnicodeExternalEncodings::eUTF8);
        // out.write (reinterpret_cast<const char*> (bom.data ()), bom.size ());         // http://stroika-bugs.sophists.com/browse/STK-982 - this should work (and I think is) - but causes later failure (reader) - so debug why
        Memory::StackBuffer<byte> buf{Memory::UninitializedConstructorFlag::eUninitialized, sz};
        span<byte>                outSpan = codeCvt.Characters2Bytes (span{w}, span{buf});
        out.write (reinterpret_cast<const char*> (outSpan.data ()), outSpan.size ());
    }
}

#if qStroika_Foundation_DataExchange_XML_SupportDOM and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportParsing
namespace {
    template <typename REPEAT_TEST>
    void DoWithEachXMLProvider_ (REPEAT_TEST&& test)
    {
        using ResolverPtr = XML::Resource::ResolverPtr;
        test ([] (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                  const Schema::Ptr& schema) { XML::SAXParse (in, callback, schema); },
              [] (const BLOB& schemaData, const ResolverPtr& resolver) { return XML::Schema::New (schemaData, resolver); },
              [] (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReading) {
                  return XML::DOM::Document::New (in, schemaToValidateAgainstWhileReading);
              });
#if qHasFeature_libxml2
        test ([] (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                  const Schema::Ptr& schema) { XML::SAXParse (XML::Providers::LibXML2::kDefaultProvider, in, callback, schema); },
              [] (const BLOB& schemaData, const ResolverPtr& resolver) {
                  return XML::Schema::New (XML::Providers::LibXML2::kDefaultProvider, schemaData, resolver);
              },
              [] (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReading) {
                  return XML::DOM::Document::New (XML::Providers::LibXML2::kDefaultProvider, in, schemaToValidateAgainstWhileReading);
              });
#endif
#if qHasFeature_Xerces
        test ([] (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback,
                  const Schema::Ptr& schema) { XML::SAXParse (XML::Providers::Xerces::kDefaultProvider, in, callback, schema); },
              [] (const BLOB& schemaData, const ResolverPtr& resolver) {
                  return XML::Schema::New (XML::Providers::Xerces::kDefaultProvider, schemaData, resolver);
              },
              [] (const Streams::InputStream::Ptr<byte>& in, const Schema::Ptr& schemaToValidateAgainstWhileReading) {
                  return XML::DOM::Document::New (XML::Providers::Xerces::kDefaultProvider, in, schemaToValidateAgainstWhileReading);
              });
#endif
    }
}
#endif

#if qStroika_Foundation_DataExchange_XML_SupportParsing
namespace {
    GTEST_TEST (Foundation_DataExchange_XML, SAX_PARSER1)
    {
        TraceContextBumper ctx{"Test_1_SAXParser_"};
        const wstring      kNSTest   = L"Test-NAMESPACE";
        wstring            newDocXML = L"<PHRModel xmlns=\"" + wstring{kNSTest} +
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
                fStartCount    = 0;
            }
            virtual void EndDocument () override
            {
                EXPECT_EQ (fEltDepthCount, 0u);
            }
            virtual void StartElement (const StructuredStreamEvents::Name&                                   name,
                                       [[maybe_unused]] const Mapping<StructuredStreamEvents::Name, String>& attributes) override
            {
                //DbgTrace (L"attr=%s", Characters::ToString (attributes).c_str ());
                fEltDepthCount++;
                fStartCount++;
                fEltStack.push_back (Memory::NullCoalesce (name.fNamespaceURI) + "/" + name.fLocalName);
            }
            virtual void EndElement (const StructuredStreamEvents::Name& name) override
            {
                EXPECT_EQ (fEltStack.back (), Memory::NullCoalesce (name.fNamespaceURI) + "/" + name.fLocalName);
                fEltStack.pop_back ();
                fEltDepthCount--;
            }
            unsigned int   fEltDepthCount;
            unsigned int   fStartCount{0};
            vector<String> fEltStack;
        };
        stringstream tmpStrm;
        WriteTextStream_ (newDocXML, tmpStrm);
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            tmpStrm.clear ();
            tmpStrm.seekg (0);
            MyCallback myCallback;
            saxParser (InputStreamFromStdIStream::New<byte> (tmpStrm), &myCallback, nullptr);
            EXPECT_EQ (myCallback.fStartCount, 34u);
        });
    }
    GTEST_TEST (Foundation_DataExchange_XML, SAX_PARSER2)
    {
        TraceContextBumper ctx{"SAX_PARSER2"};
        const Memory::BLOB kHealthFrameWorks_v3_xml = Memory::BLOB::Attach (Resources_::TestFiles_HealthFrameWorks_v3_xml);
        class MyCallback : public StructuredStreamEvents::IConsumer {
        public:
            virtual void StartDocument () override
            {
                fEltDepthCount = 0;
            }
            virtual void EndDocument () override
            {
                EXPECT_EQ (fEltDepthCount, 0u);
            }
            virtual void StartElement (const StructuredStreamEvents::Name&                                   name,
                                       [[maybe_unused]] const Mapping<StructuredStreamEvents::Name, String>& attributes) override
            {
                fEltDepthCount++;
                fEltStack.push_back (Memory::NullCoalesce (name.fNamespaceURI) + "/" + name.fLocalName);
            }
            virtual void EndElement (const StructuredStreamEvents::Name& name) override
            {
                EXPECT_EQ (fEltStack.back (), Memory::NullCoalesce (name.fNamespaceURI) + "/" + name.fLocalName);
                fEltStack.pop_back ();
                fEltDepthCount--;
            }
            unsigned int   fEltDepthCount;
            vector<String> fEltStack;
        };
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            MyCallback myCallback;
            saxParser (kHealthFrameWorks_v3_xml, &myCallback, nullptr);
        });
    }
    GTEST_TEST (Foundation_DataExchange_XML, SAX_PARSER_SchemaValidate)
    {
        TraceContextBumper ctx{"SAX_PARSER2"};
        const Memory::BLOB kPersonalXML_ = Memory::BLOB::Attach (Resources_::TestFiles_personal_xml);
        const Memory::BLOB kPersonalXSD_ = Memory::BLOB::Attach (Resources_::TestFiles_personal_xsd);
        const Memory::BLOB kCCR_XSD_     = Memory::BLOB::Attach (Resources_::TestFiles_ASTM_CCR_V1_xsd);
        const Memory::BLOB kSampleCCR_   = Memory::BLOB::Attach (Resources_::TestFiles_SampleCCR_ccr);

        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Schema::Ptr personalSchema = schemaFactory (kPersonalXSD_, nullptr);
            saxParser (kPersonalXML_.As<InputStream::Ptr<byte>> (), nullptr, personalSchema);
        });
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Schema::Ptr ccrSchema = schemaFactory (kCCR_XSD_, nullptr);
            saxParser (kSampleCCR_.As<InputStream::Ptr<byte>> (), nullptr, ccrSchema);
        });
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Schema::Ptr                       personalSchema = schemaFactory (kPersonalXSD_, nullptr);
            Schema::Ptr                       ccrSchema      = schemaFactory (kCCR_XSD_, nullptr);
            StructuredStreamEvents::IConsumer ignoreData;
            EXPECT_THROW (saxParser (kSampleCCR_.As<InputStream::Ptr<byte>> (), nullptr, personalSchema), BadFormatException);
            EXPECT_THROW (saxParser (kPersonalXML_.As<InputStream::Ptr<byte>> (), nullptr, ccrSchema), BadFormatException);
        });
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, Test_2a_ObjectReader_viaRegistry_)
    {
        TraceContextBumper traceCtx{"Test_2a_ObjectReader_viaRegistry_"};
        const wstring      kNSTest = L"Test-NAMESPACE";
        struct Person_ {
            String           firstName;
            String           lastName;
            optional<String> middleName;
        };
        struct Appointment_ {
            optional<DateTime> when;
            Person_            withWhom;
        };
        auto mkdata_ = [=] () {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };

        ObjectReader::Registry registry;

        registry.AddCommonType<DateTime> ();
        registry.AddCommonType<optional<DateTime>> ();
        registry.AddCommonType<String> ();
        registry.AddCommonType<optional<String>> ();

        registry.AddCommonReader_Class<Person_> ({
            {Name{"FirstName"}, &Person_::firstName},
            {Name{"LastName"}, &Person_::lastName},
            {Name{"MiddleName"}, &Person_::middleName},
        });
        registry.AddCommonReader_Class<Appointment_> ({
            {Name{"When"}, &Appointment_::when},
            {Name{"WithWhom"}, &Appointment_::withWhom},
        });
        registry.AddCommonType<vector<Appointment_>> (Name{"Appointment"});

        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            vector<Appointment_>                     calendar;
            ObjectReader::IConsumerDelegateToContext ctx{
                registry, make_shared<ObjectReader::ReadDownToReader> (
                              make_shared<ObjectReader::RepeatedElementReader<vector<Appointment_>>> (&calendar), Name{"Appointment"})};
            saxParser (mkdata_ (), &ctx, nullptr);
            EXPECT_EQ (calendar.size (), 2u);
            EXPECT_EQ (calendar[0].withWhom.firstName, "Jim");
            EXPECT_EQ (calendar[0].withWhom.lastName, "Smith");
            EXPECT_EQ (*calendar[0].withWhom.middleName, "Up");
            EXPECT_TRUE (calendar[0].when and calendar[0].when->GetDate () == (Time::Date{Time::Year{2005}, Time::June, Time::DayOfMonth{1}}));
            EXPECT_EQ (calendar[1].withWhom.firstName, "Fred");
            EXPECT_EQ (calendar[1].withWhom.lastName, "Down");
        });
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            vector<Appointment_> calendar;
            ObjectReader::IConsumerDelegateToContext ctx{registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&calendar))};
            saxParser (mkdata_ (), &ctx, nullptr);
            EXPECT_EQ (calendar.size (), 2u);
            EXPECT_EQ (calendar[0].withWhom.firstName, "Jim");
            EXPECT_EQ (calendar[0].withWhom.lastName, "Smith");
            EXPECT_EQ (*calendar[0].withWhom.middleName, "Up");
            EXPECT_TRUE ((calendar[0].when and calendar[0].when->GetDate () == Time::Date{Time::Year (2005), Time::June, Time::DayOfMonth (1)}));
            EXPECT_EQ (calendar[1].withWhom.firstName, "Fred");
            EXPECT_EQ (calendar[1].withWhom.lastName, "Down");
        });
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T3_SAXObjectReader_ReadDown2Sample_)
    {
        struct Person_ {
            String firstName;
            String lastName;
            bool   operator== (const Person_& rhs) const
            {
                return firstName == rhs.firstName and lastName == rhs.lastName;
            }
        };
        auto mkdata_ = [] () {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };

        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();

        registry.AddCommonReader_Class<Person_> ({
            {Name{"FirstName"}, &Person_::firstName},
            {Name{"LastName"}, &Person_::lastName},
        });

        vector<Person_> people;
        {
            ObjectReader::IConsumerDelegateToContext ctx{
                registry, make_shared<ObjectReader::ReadDownToReader> (make_shared<ObjectReader::RepeatedElementReader<vector<Person_>>> (&people),
                                                                       Name{"envelope2"}, Name{"WithWhom"})};
            DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
                people.clear (); // cuz run multiple times
                saxParser (mkdata_ (), &ctx, nullptr);
                EXPECT_EQ (people.size (), 2u);
                EXPECT_EQ (people[0].firstName, "Jim");
                EXPECT_EQ (people[0].lastName, "Smith");
                EXPECT_EQ (people[1].firstName, "Fred");
                EXPECT_EQ (people[1].lastName, "Down");
            });
        }

        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            vector<Person_>        people2; // add the vector type to the registry instead of explicitly constructing the right reader
            ObjectReader::Registry newRegistry = registry;
            newRegistry.AddCommonType<vector<Person_>> (Name{"WithWhom"});
            ObjectReader::IConsumerDelegateToContext ctx{
                newRegistry, make_shared<ObjectReader::ReadDownToReader> (newRegistry.MakeContextReader (&people2), Name{"envelope2"})};
            saxParser (mkdata_ (), &ctx, nullptr);
            EXPECT_EQ (people2, people);
        });
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Sequence<Person_>      people3; // use sequence instead of vector
            ObjectReader::Registry newRegistry = registry;
            newRegistry.AddCommonType<Sequence<Person_>> (Name{"WithWhom"});
            ObjectReader::IConsumerDelegateToContext ctx{
                newRegistry, make_shared<ObjectReader::ReadDownToReader> (newRegistry.MakeContextReader (&people3), Name{"envelope2"})};
            saxParser (mkdata_ (), &ctx, nullptr);
            EXPECT_EQ (people3.As<vector<Person_>> (), people);
        });
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T4_SAXObjectReader_ReadDown2Sample_MixedContent_)
    {
        struct ManagedObjectReference {
            String type;
            String value;
        };
        struct ObjectContent {
            ManagedObjectReference obj;
        };
        auto mkdata_ = [] () -> Memory::BLOB {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };

        ObjectReader::Registry mapper;

        mapper.AddCommonType<String> ();

        mapper.AddCommonReader_Class<ManagedObjectReference> (
            {{Name{"type", Name::eAttribute}, &ManagedObjectReference::type}, {Name{Name::eValue}, &ManagedObjectReference::value}});
        mapper.AddCommonReader_Class<ObjectContent> ({
            {Name{"obj"}, &ObjectContent::obj}
            /// wrong - must be mapping of this --metaInfo.Add ("propSet", pair<type_index, size_t> {typeid(decltype (ObjectContent::value)), offsetof(ObjectContent, propSet)});
        });

        vector<ObjectContent>                    objsContent;
        ObjectReader::IConsumerDelegateToContext ctx{
            mapper, make_shared<ObjectReader::ReadDownToReader> (make_shared<ObjectReader::RepeatedElementReader<vector<ObjectContent>>> (&objsContent),
                                                                 Name{"RetrievePropertiesResponse"}, Name{"returnval"})};
        XML::SAXParse (mkdata_ (), &ctx);

        EXPECT_EQ (objsContent.size (), 2u);
        EXPECT_EQ (objsContent[0].obj.type, "VirtualMachine");
        EXPECT_EQ (objsContent[0].obj.value, "8");
        EXPECT_EQ (objsContent[1].obj.type, "VirtualMachine");
        EXPECT_EQ (objsContent[1].obj.value, "9");
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T5_SAXObjectReader_DocSamples_)
    {
        struct Person_ {
            String firstName;
            String lastName;
        };
        auto mkdata_ = [] () {
            wstring newDocXML = L"        <PERSON>\n"
                                L"                <FirstName>Jim</FirstName>"
                                L"                <LastName>Smith</LastName>"
                                L"        </PERSON>\n";
            ;
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStream::Ptr<byte>{InputStreamFromStdIStream::New<byte> (tmpStrm)}.ReadAll ();
        };

        ObjectReader::Registry mapper;
        mapper.AddCommonType<String> ();
        mapper.AddCommonReader_Class<Person_> ({
            {Name{"FirstName"}, &Person_::firstName},
            {Name{"LastName"}, &Person_::lastName},
        });
        Person_                                  p;
        ObjectReader::IConsumerDelegateToContext tmp{mapper, make_shared<ObjectReader::ReadDownToReader> (mapper.MakeContextReader (&p))};
        XML::SAXParse (mkdata_ (), &tmp);
        EXPECT_EQ (p.firstName, "Jim");
        EXPECT_EQ (p.lastName, "Smith");
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T6_SAXObjectReader_RepeatedElementReader_Sample_)
    {
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
        auto mkdata_ = [] () -> Memory::BLOB {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };

        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();
        registry.AddCommonReader_NamedEnumerations<GenderType_> (Containers::Bijection<GenderType_, String>{
            pair<GenderType_, String>{GenderType_::Male, "Male"},
            pair<GenderType_, String>{GenderType_::Female, "Female"},
        });
        registry.AddCommonType<optional<GenderType_>> ();
        registry.AddCommonReader_Class<Person_> ({
            {Name{"FirstName"}, &Person_::firstName},
            {Name{"LastName"}, &Person_::lastName},
            {Name{"Gender"}, &Person_::gender},
        });
        registry.AddCommonType<vector<Person_>> ();
        registry.Add<vector<Person_>> (ObjectReader::RepeatedElementReader<vector<Person_>>::AsFactory ());
        registry.AddCommonReader_Class<Address_> ({
            {Name{"city"}, &Address_::city},
            {Name{"state"}, &Address_::state},
        });
        registry.Add<vector<Address_>> (ObjectReader::RepeatedElementReader<vector<Address_>>::AsFactory ());
        registry.AddCommonReader_Class<Data_> ({
            {Name{"person"}, &Data_::people},
            {Name{"address"}, &Data_::addresses},
        });

        Data_ data;
        {
            ObjectReader::IConsumerDelegateToContext ctx{registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data))};
            XML::SAXParse (mkdata_ (), &ctx);
            EXPECT_EQ (data.people.size (), 2u);
            EXPECT_EQ (data.people[0].firstName, "Jim");
            EXPECT_EQ (data.people[0].lastName, "Smith");
            EXPECT_EQ (data.people[0].gender, GenderType_::Male);
            EXPECT_EQ (data.people[1].firstName, "Fred");
            EXPECT_EQ (data.people[1].lastName, "Down");
            EXPECT_TRUE (not data.people[1].gender.has_value ());
            EXPECT_EQ (data.addresses.size (), 3u);
            EXPECT_EQ (data.addresses[0].city, "Boston");
            EXPECT_EQ (data.addresses[0].state, "MA");
            EXPECT_EQ (data.addresses[1].city, "New York");
            EXPECT_EQ (data.addresses[1].state, "NY");
            EXPECT_EQ (data.addresses[2].city, "Albany");
            EXPECT_EQ (data.addresses[2].state, "NY");
        }
    }
}

namespace {
    namespace T7_SAXObjectReader_BLKQCL_ReadSensors_Support_ {
        enum class TunerNumberType_ {
            eT1 = 1,
            eT2 = 2,
            eT3 = 3,
            eT4 = 4,
            Stroika_Define_Enum_Bounds (eT1, eT4)
        };
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
                static const ReaderFromVoidStarFactory sEltReader_ = [] () -> ReaderFromVoidStarFactory {
                    using KVPType_ = KeyValuePair<TunerNumberType_, TARGET_TYPE>;
                    return Registry::MakeClassReader<KVPType_> ({
                        {Name{"Tuner", Name::eAttribute}, &KVPType_::fKey},
                        {Name{Name::eValue}, &KVPType_::fValue},
                    });
                }();
                return make_shared<RepeatedElementReader<Mapping<TunerNumberType_, TARGET_TYPE>>> (fValuePtr_, sEltReader_);
            }
            static ObjectReader::ReaderFromVoidStarFactory AsFactory ()
            {
                return IElementConsumer::AsFactory<Mapping<TunerNumberType_, TARGET_TYPE>, TunerMappingReader_> ();
            }
        };
    }
}
namespace Stroika::Foundation::Configuration {
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunused-const-variable\"")
    template <>
    constexpr EnumNames<T7_SAXObjectReader_BLKQCL_ReadSensors_Support_::TunerNumberType_>
        DefaultNames<T7_SAXObjectReader_BLKQCL_ReadSensors_Support_::TunerNumberType_>::k{{{
            {T7_SAXObjectReader_BLKQCL_ReadSensors_Support_::TunerNumberType_::eT1, L"eT1"},
            {T7_SAXObjectReader_BLKQCL_ReadSensors_Support_::TunerNumberType_::eT2, L"eT2"},
            {T7_SAXObjectReader_BLKQCL_ReadSensors_Support_::TunerNumberType_::eT3, L"eT3"},
            {T7_SAXObjectReader_BLKQCL_ReadSensors_Support_::TunerNumberType_::eT4, L"eT4"},
        }}};
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunused-const-variable\"")
}
namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T7_SAXObjectReader_BLKQCL_ReadSensors_)
    {
        using namespace T7_SAXObjectReader_BLKQCL_ReadSensors_Support_;
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
        auto mkdata_ = [] () -> Memory::BLOB {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };

        ObjectReader::Registry registry;
        registry.AddCommonReader_NamedEnumerations<TunerNumberType_> (Containers::Bijection<TunerNumberType_, String>{
            {TunerNumberType_::eT1, "1"},
            {TunerNumberType_::eT2, "2"},
            {TunerNumberType_::eT3, "3"},
            {TunerNumberType_::eT4, "4"},
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
        registry.AddCommonReader_Class<SensorDataType_> ({
            {Name{"ActiveLaser"}, &SensorDataType_::ActiveLaser},
            {Name{"DetectorTemperature"}, &SensorDataType_::DetectorTemperature},
            {Name{"OpticsTemperature"}, &SensorDataType_::OpticsTemperature},
            {Name{"ExternalTemperature1"}, &SensorDataType_::ExternalTemperature1},
            {Name{"LaserTemperature"}, &SensorDataType_::LaserTemperatures},
            {Name{"LaserCurrent"}, &SensorDataType_::LaserCurrents},
            {Name{"MirrorTemperature"}, &SensorDataType_::MirrorTemperatures},
            {Name{"TECPowerConsumptionStats"}, &SensorDataType_::TECPowerConsumptionStats},
        });

        SensorDataType_ data;
        {
            ObjectReader::IConsumerDelegateToContext consumerCallback{
                registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"Sensors"})};
            //consumerCallback.fContext.fTraceThisReader = true;
            XML::SAXParse (mkdata_ (), &consumerCallback);
            DbgTrace ("LaserTemperatures={}"_f, Characters::ToString (data.LaserTemperatures));
            DbgTrace ("MirrorTemperature={}"_f, Characters::ToString (data.MirrorTemperatures));
            DbgTrace ("LaserCurrents={}"_f, Characters::ToString (data.LaserCurrents));
            DbgTrace ("TECPowerConsumptionStats={}"_f, Characters::ToString (data.TECPowerConsumptionStats->TunerTECCurrent));
            EXPECT_TRUE (not data.ActiveLaser.has_value ());
            EXPECT_TRUE (Math::NearlyEquals (*data.DetectorTemperature, 13.1));
            EXPECT_TRUE (Math::NearlyEquals (*data.OpticsTemperature, 0.86115019791435543));
            EXPECT_TRUE ((data.LaserTemperatures.Keys () == Set<TunerNumberType_>{TunerNumberType_::eT1}));
            EXPECT_TRUE (Math::NearlyEquals (*data.LaserTemperatures.Lookup (TunerNumberType_::eT1), 20.899877489241646));
            EXPECT_EQ (data.LaserCurrents.Keys (), (Set<TunerNumberType_>{TunerNumberType_::eT1}));
            EXPECT_TRUE (Math::NearlyEquals (*data.LaserCurrents.Lookup (TunerNumberType_::eT1), 0.86871794871794872));
            EXPECT_TRUE ((data.MirrorTemperatures.Keys () == Set<TunerNumberType_>{TunerNumberType_::eT2}));
            EXPECT_TRUE (Math::NearlyEquals (*data.MirrorTemperatures.Lookup (TunerNumberType_::eT2), 0.86115019791435543));
            EXPECT_TRUE ((data.TECPowerConsumptionStats->TunerTECCurrent.Keys () ==
                          Set<TunerNumberType_>{TunerNumberType_::eT1, TunerNumberType_::eT2, TunerNumberType_::eT3, TunerNumberType_::eT4}));
            EXPECT_TRUE (Math::NearlyEquals (*data.ExternalTemperature1, 0.0));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T8_SAXObjectReader_BLKQCL_ReadAlarms_)
    {
        using AlarmType_ = String;
        auto mkdata_     = [] () -> Memory::BLOB {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };
        ObjectReader::Registry registry;
        registry.AddCommonType<AlarmType_> ();
        registry.Add<Set<AlarmType_>> (ObjectReader::RepeatedElementReader<Set<AlarmType_>>::AsFactory ());
        {
            // Example matching ANY sub-element
            Set<AlarmType_> data;
            {
                ObjectReader::IConsumerDelegateToContext consumerCallback{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"GetAlarmsResponse"}, Name{"Alarm"})};
                XML::SAXParse (mkdata_ (), &consumerCallback);
                DbgTrace ("Alarms={}"_f, Characters::ToString (data));
            }
            EXPECT_EQ (data, (Set<AlarmType_>{"Fred", "Critical_LaserOverheating"}));
        }
        const Name kAlarmName_ = Name{"Alarm"};
        registry.Add<Set<AlarmType_>> (ObjectReader::RepeatedElementReader<Set<AlarmType_>>::AsFactory (kAlarmName_));
        {
            // Example matching THE RIGHT sub-element
            Set<AlarmType_> data;
            {
                ObjectReader::IConsumerDelegateToContext consumerCallback{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"GetAlarmsResponse"}, kAlarmName_)};
                XML::SAXParse (mkdata_ (), &consumerCallback);
                DbgTrace ("Alarms={}"_f, Characters::ToString (data));
            }
            EXPECT_EQ (data, (Set<AlarmType_>{"Fred", "Critical_LaserOverheating"}));
        }
        const Name kWrongAlarmName_ = Name{"xxxAlarm"};
        registry.Add<Set<AlarmType_>> (ObjectReader::RepeatedElementReader<Set<AlarmType_>>::AsFactory (kWrongAlarmName_));
        {
            // Example matching THE WRONG sub-element
            Set<AlarmType_> data;
            {
                ObjectReader::IConsumerDelegateToContext consumerCallback{
                    registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"GetAlarmsResponse"}, kWrongAlarmName_)};
                XML::SAXParse (mkdata_ (), &consumerCallback);
                DbgTrace ("Alarms={}"_f, Characters::ToString (data));
            }
            EXPECT_EQ (data, (Set<AlarmType_>{}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T9_SAXObjectReader_BLKQCL_ReadScanDetails_)
    {
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
        auto mkdata_ = [] () -> Memory::BLOB {
            wstring newDocXML =
                L"<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                L"xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" "
                L"xmlns:blk201505=\"http://tempuri.org/blk201505.xsd\" "
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };
        struct SpectrumReader_ : public ObjectReader::IElementConsumer {
            SpectrumType_* fValuePtr_;
            SpectrumReader_ (SpectrumType_* v)
                : fValuePtr_{v}
            {
            }
            virtual shared_ptr<IElementConsumer> HandleChildStart ([[maybe_unused]] const Name& name) override
            {
                using namespace ObjectReader;
                static const ReaderFromVoidStarFactory sEltReader_ = [] () -> ReaderFromVoidStarFactory {
                    using KVPType_ = SpectrumType_::value_type;
                    return Registry::MakeClassReader<KVPType_> ({
                        {Name{"waveNumber", Name::eAttribute}, &KVPType_::fKey},
                        {Name{"intensity", Name::eAttribute}, &KVPType_::fValue},
                    });
                }();
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
                    return Registry::MakeClassReader<KVPType_> ({
                        {Name{"Key", Name::eAttribute}, &KVPType_::fKey},
                        {Name{"Value", Name::eAttribute}, &KVPType_::fValue},
                    });
                }();
                return make_shared<RepeatedElementReader<Mapping<String, String>>> (fValuePtr_, sEltReader_);
            }
            static ObjectReader::ReaderFromVoidStarFactory AsFactory ()
            {
                return IElementConsumer::AsFactory<Mapping<String, String>, StringKVStringReader> ();
            }
        };
        ObjectReader::Registry registry;
        registry.AddCommonType<ScanIDType_> ();
        registry.AddCommonType<WaveNumberType_> ();
        registry.AddCommonType<IntensityType_> ();
        registry.AddCommonType<DateTime> ();
        registry.AddCommonType<optional<DateTime>> ();
        registry.AddCommonType<String> ();
        registry.AddCommonType<optional<String>> ();
        registry.Add<SpectrumType_> (SpectrumReader_::AsFactory ());
        registry.AddCommonType<optional<SpectrumType_>> ();
        registry.Add<PersistenceScanAuxDataType_> (StringKVStringReader::AsFactory ());
        registry.AddCommonReader_Class<PersistentScanDetailsType_> ({
            {Name{"ScanID"}, &PersistentScanDetailsType_::ScanID},
            {Name{"ScanStart"}, &PersistentScanDetailsType_::ScanStart},
            {Name{"ScanEnd"}, &PersistentScanDetailsType_::ScanEnd},
            {Name{"ScanLabel"}, &PersistentScanDetailsType_::ScanLabel},
            {Name{"RawSpectrum"}, &PersistentScanDetailsType_::RawSpectrum},
            {Name{"AuxData"}, &PersistentScanDetailsType_::AuxData},
        });
        PersistentScanDetailsType_ data;
        {
            ObjectReader::IConsumerDelegateToContext consumerCallback{
                registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data),
                                                                       Name{"ScanPersistenceGetScanDetailsResponse"}, Name{"Scan"})};
            //consumerCallback.fContext.fTraceThisReader = true;
            XML::SAXParse (mkdata_ (), &consumerCallback);
            DbgTrace ("ScanID={}"_f, data.ScanID);
            DbgTrace ("ScanStart={}"_f, data.ScanStart);
            DbgTrace ("ScanEnd={}"_f, data.ScanEnd);
            if (data.ScanLabel) {
                DbgTrace (L"ScanLabel={}"_f, *data.ScanLabel);
            }
            if (data.RawSpectrum) {
                DbgTrace ("RawSpectrum={}"_f, Characters::ToString (*data.RawSpectrum));
            }
            DbgTrace ("AuxData={}"_f, Characters::ToString (data.AuxData));
            EXPECT_EQ (data.ScanID, 8320u);
            EXPECT_EQ (data.ScanStart, DateTime::Parse ("2016-07-28T20:14:30Z", DateTime::kISO8601Format));
            EXPECT_EQ (data.ScanEnd, DateTime::Parse ("2016-07-28T20:14:44Z", DateTime::kISO8601Format));
            EXPECT_TRUE (not data.ScanLabel.has_value ());
            EXPECT_TRUE ((data.RawSpectrum == Mapping<WaveNumberType_, IntensityType_>{pair<WaveNumberType_, IntensityType_>{901.5, 0},
                                                                                       pair<WaveNumberType_, IntensityType_>{902.5, 1}}));
            EXPECT_TRUE ((data.AuxData ==
                          Mapping<String, String>{pair<String, String>{"Cell-Pressure", "1000"}, pair<String, String>{"Cell-Temperature", "0"},
                                                  pair<String, String>{"EngineId", "B1E56F82-B217-40D3-A24D-FAC491EDCDE8"}}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T10_SAXObjectReader_NANValues_)
    {
        struct Values_ {
            double valueMissing;
            double valueExplicitGood;
            double valueExplicitNAN1;
            double valueExplicitNAN2;
        };
        auto mkdata_ = [] () {
            wstring newDocXML = L"<Values>\n"
                                //L"          <valueMissing></valueMissing>"
                                L"            <valueExplicitGood>3.0</valueExplicitGood>"
                                L"            <valueExplicitNAN1>NAN</valueExplicitNAN1>"
                                L"            <valueExplicitNAN2>NAN</valueExplicitNAN2>"
                                L"</Values>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };
        TraceContextBumper     traceCtx{"T10_SAXObjectReader_NANValues_"};
        ObjectReader::Registry registry;
        registry.AddCommonType<double> ();
        registry.AddCommonReader_Class<Values_> ({
            {Name{"valueMissing"}, &Values_::valueMissing},
            {Name{"valueExplicitGood"}, &Values_::valueExplicitGood},
            {Name{"valueExplicitNAN1"}, &Values_::valueExplicitNAN1},
            {Name{"valueExplicitNAN2"}, &Values_::valueExplicitNAN2},
        });
        {
            Values_ values{};
            values.valueMissing = 999;
            ObjectReader::IConsumerDelegateToContext ctx{
                registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&values), Name{"Values"})};
            XML::SAXParse (mkdata_ (), &ctx);
            EXPECT_EQ (values.valueMissing, 999);
            EXPECT_TRUE (Math::NearlyEquals (values.valueExplicitGood, 3.0));
            EXPECT_TRUE (isnan (values.valueExplicitNAN1));
            EXPECT_TRUE (isnan (values.valueExplicitNAN2));
        }
    }
}

namespace {
    namespace T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_ {
        enum class TunerNumberType_ {
            eT1 = 1,
            eT2 = 2,
            eT3 = 3,
            eT4 = 4,
            Stroika_Define_Enum_Bounds (eT1, eT4)
        };
    }
}
namespace Stroika::Foundation::Configuration {
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunused-const-variable\"")
    // for ToString ()
    template <>
    constexpr EnumNames<T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_::TunerNumberType_>
        DefaultNames<T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_::TunerNumberType_>::k{{{
            {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_::TunerNumberType_::eT1, L"eT1"},
            {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_::TunerNumberType_::eT2, L"eT2"},
            {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_::TunerNumberType_::eT3, L"eT3"},
            {T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_::TunerNumberType_::eT4, L"eT4"},
        }}};
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunused-const-variable\"")
}
namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_)
    {
        using namespace T11_SAXObjectReader_BLKQCL_GetFactorySettings_Tuners_Support_;
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
        auto mkdata_ = [] () {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };

        static const ObjectReader::ReaderFromVoidStarFactory k_PerTunerFactorySettingsType_ReaderFactory_ =
            ObjectReader::Registry::MakeClassReader<PerTunerFactorySettingsType_> ({
                {Name{"MirrorOperationFrequency"}, &PerTunerFactorySettingsType_::MirrorOperationFrequency},
                {Name{"MirrorResonantFrequency"}, &PerTunerFactorySettingsType_::MirrorResonantFrequency},
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
                        using KVPType_                                       = KeyValuePair<TunerNumberType_, PerTunerFactorySettingsType_>;
                        static const ReaderFromVoidStarFactory kTunerReader_ = Registry::MakeClassReader<KVPType_> ({
                            {Name{"Tuner", Name::eAttribute}, &KVPType_::fKey},
                        });
                        Sequence<MixinEltTraits>               tmp;
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
        registry.AddCommonReader_Class<FactorySettingsType_> ({
            {Name{"Tuners"}, &FactorySettingsType_::Tuners},
        });
        FactorySettingsType_ data;
        {
            ObjectReader::IConsumerDelegateToContext consumerCallback{
                registry,
                make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data), Name{"GetFactorySettingsResponse"})};
            //consumerCallback.fContext.fTraceThisReader = true;
            XML::SAXParse (mkdata_ (), &consumerCallback);
            DbgTrace ("Tuners={}"_f, Characters::ToString (data.Tuners));
            EXPECT_EQ (data.Tuners.Keys (), (Set<TunerNumberType_>{TunerNumberType_::eT1, TunerNumberType_::eT2}));
            EXPECT_TRUE (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT1)->MirrorOperationFrequency, 40.0));
            EXPECT_TRUE (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT1)->MirrorResonantFrequency, 150.0));
            EXPECT_TRUE (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT2)->MirrorOperationFrequency, 41.0));
            EXPECT_TRUE (Math::NearlyEquals (*data.Tuners.Lookup (TunerNumberType_::eT2)->MirrorResonantFrequency, 151.0));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T12_RangeReader_)
    {
        using Traversal::Range;
        using MY_TEST_RANGE_ = Range<double>;
        struct Values_ {
            MY_TEST_RANGE_ r{};
        };
        auto mkdata_ = [] () {
            wstring newDocXML = L"<Values>\n"
                                L"            <r LowerBound=\"3.0\" UpperBound=\"6.0\"/>"
                                L"</Values>\n";
            stringstream tmpStrm;
            WriteTextStream_ (newDocXML, tmpStrm);
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };
        TraceContextBumper     traceCtx{"T12_RangeReader_"};
        ObjectReader::Registry registry;
        registry.AddCommonType<MY_TEST_RANGE_::value_type> ();
        registry.Add<MY_TEST_RANGE_> (ObjectReader::RangeReader<MY_TEST_RANGE_>::AsFactory ());
        registry.AddCommonReader_Class<Values_> ({
            {Name{"r"}, &Values_::r},
        });
        {
            Values_                                  values{};
            ObjectReader::IConsumerDelegateToContext ctx{
                registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&values), Name{"Values"})};
            XML::SAXParse (mkdata_ (), &ctx);
            EXPECT_TRUE (Math::NearlyEquals (values.r.GetLowerBound (), 3.0));
            EXPECT_TRUE (Math::NearlyEquals (values.r.GetUpperBound (), 6.0));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T13_SAXObjectReader_OverrideTypeInStructInfo_)
    {
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
        auto mkdata_ = [] () {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };
        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();
        static const auto kGenderType_Reader_ =
            ObjectReader::Registry::MakeCommonReader_NamedEnumerations<GenderType_> (Containers::Bijection<GenderType_, String>{
                pair<GenderType_, String>{GenderType_::Male, "Male"},
                pair<GenderType_, String>{GenderType_::Female, "Female"},
            });
        registry.AddCommonReader_Class<Person_> ({
            {Name{"FirstName"}, &Person_::firstName},
            {Name{"LastName"}, &Person_::lastName},
            {Name{"Gender"}, &Person_::gender, kGenderType_Reader_},
        });
        registry.AddCommonType<vector<Person_>> ();
        registry.Add<vector<Person_>> (ObjectReader::RepeatedElementReader<vector<Person_>>::AsFactory ());
        registry.AddCommonReader_Class<Data_> ({
            {Name{"person"}, &Data_::people},
        });

        Data_ data;
        {
            ObjectReader::IConsumerDelegateToContext ctx{registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data))};
            XML::SAXParse (mkdata_ (), &ctx);
            EXPECT_EQ (data.people.size (), 2u);
            EXPECT_EQ (data.people[0].firstName, "Jim");
            EXPECT_EQ (data.people[0].lastName, "Smith");
            EXPECT_EQ (data.people[0].gender, GenderType_::Male);
            EXPECT_EQ (data.people[1].firstName, "Fred");
            EXPECT_EQ (data.people[1].lastName, "Down");
            EXPECT_EQ (data.people[1].gender, GenderType_::Female);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T14_SAXObjectReader_CustomSimpleType_)
    {
        // Custom Type will require a custom reader, but that is easy to do for most simple types
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
        auto mkdata_ = [] () {
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
            return InputStreamFromStdIStream::New<byte> (tmpStrm).ReadAll ();
        };
        ObjectReader::Registry registry;
        registry.AddCommonType<String> ();
        registry.AddCommonReader_Simple<GenderType_> ([] (String s) -> GenderType_ {
            GenderType_ result;
            result.fRep = s;
            return result;
        });
        registry.AddCommonReader_Class<Person_> ({
            {Name{"FirstName"}, &Person_::firstName},
            {Name{"LastName"}, &Person_::lastName},
            {Name{"Gender"}, &Person_::gender},
        });
        registry.AddCommonType<vector<Person_>> ();
        registry.Add<vector<Person_>> (ObjectReader::RepeatedElementReader<vector<Person_>>::AsFactory ());
        registry.AddCommonReader_Class<Data_> ({
            {Name{"person"}, &Data_::people},
        });

        Data_ data;
        {
            ObjectReader::IConsumerDelegateToContext ctx{registry, make_shared<ObjectReader::ReadDownToReader> (registry.MakeContextReader (&data))};
            XML::SAXParse (mkdata_ (), &ctx);
            EXPECT_EQ (data.people.size (), 2u);
            EXPECT_EQ (data.people[0].firstName, "Jim");
            EXPECT_EQ (data.people[0].lastName, "Smith");
            EXPECT_EQ (data.people[0].gender.fRep, "Male");
            EXPECT_EQ (data.people[1].firstName, "Fred");
            EXPECT_EQ (data.people[1].lastName, "Down");
            EXPECT_EQ (data.people[1].gender.fRep, "Female");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, T15_DOMRead_)
    {
        const Memory::BLOB kPersonalXML_                 = Memory::BLOB::Attach (Resources_::TestFiles_personal_xml);
        const Memory::BLOB kPersonalXSD_                 = Memory::BLOB::Attach (Resources_::TestFiles_personal_xsd);
        const Memory::BLOB kHealthFrameWorks_v3_xml      = Memory::BLOB::Attach (Resources_::TestFiles_HealthFrameWorks_v3_xml);
        const Memory::BLOB kReferenceContent_2012_03_xsd = Memory::BLOB::Attach (Resources_::TestFiles_ReferenceContent_2012_03_xsd);
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            DOM::Document::Ptr d   = domFactory (kPersonalXML_.As<Streams::InputStream::Ptr<byte>> (), nullptr);
            String             tmp = d.Write ();
            DbgTrace ("tmp={}"_f, Characters::ToString (tmp));
        });
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Schema::Ptr        personalSchema = schemaFactory (kPersonalXSD_, nullptr);
            DOM::Document::Ptr d              = domFactory (kPersonalXML_.As<Streams::InputStream::Ptr<byte>> (), personalSchema);
            String             tmp            = d.Write ();
            DbgTrace (L"tmp={}"_f, Characters::ToString (tmp));
        });
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Schema::Ptr        schema = schemaFactory (kReferenceContent_2012_03_xsd, nullptr);
            DOM::Document::Ptr d      = domFactory (kHealthFrameWorks_v3_xml.As<Streams::InputStream::Ptr<byte>> (), schema);
            String             tmp    = d.Write ();
            DbgTrace (L"tmp={}"_f, Characters::ToString (tmp));
        });
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, DOM_Update)
    {
        const Memory::BLOB kPersonalXML_                 = Memory::BLOB::Attach (Resources_::TestFiles_personal_xml);
        const Memory::BLOB kPersonalXSD_                 = Memory::BLOB::Attach (Resources_::TestFiles_personal_xsd);
        const Memory::BLOB kHealthFrameWorks_v3_xml      = Memory::BLOB::Attach (Resources_::TestFiles_HealthFrameWorks_v3_xml);
        const Memory::BLOB kReferenceContent_2012_03_xsd = Memory::BLOB::Attach (Resources_::TestFiles_ReferenceContent_2012_03_xsd);

        {
            // DoWithEachXMLProvider_ factory trick doesn't work for this test cuz use alt CTOR for doc
            auto test1 = [] (DOM::Document::Ptr d) {
                EXPECT_EQ (d.GetRootElement ().GetName (), (NameWithNamespace{nullopt, "simpleElt"}));
                EXPECT_EQ (d.GetRootElement ().GetValue (), "");
            };
            {
                test1 (DOM::Document::New ({nullopt, "simpleElt"}));
#if qHasFeature_libxml2
                test1 (DOM::Document::New (XML::Providers::LibXML2::kDefaultProvider, NameWithNamespace{nullopt, "simpleElt"}));
#endif
#if qHasFeature_Xerces
                test1 (DOM::Document::New (XML::Providers::Xerces::kDefaultProvider, {nullopt, "simpleElt"}));
#endif
            }
        }
        // no namespace's content test
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Schema::Ptr        schema      = schemaFactory (kPersonalXSD_, nullptr);
            Schema::Ptr        wrongSchema = schemaFactory (kReferenceContent_2012_03_xsd, nullptr);
            DOM::Document::Ptr d           = domFactory (kPersonalXML_.As<Streams::InputStream::Ptr<byte>> (), nullptr);
            String             tmp         = d.Write ();
            DOM::Element::Ptr  personelElt = d.GetRootElement ();
            EXPECT_EQ (personelElt.GetName ().fNamespace, nullopt);
            EXPECT_EQ (personelElt.GetName ().fName, "personnel");
            EXPECT_EQ (personelElt.GetChildElements ().size (), 6u);
            personelElt.GetChildElements ().Apply ([] (DOM::Element::Ptr p) {
                EXPECT_EQ (p.GetName ().fNamespace, nullopt);
                EXPECT_EQ (p.GetName ().fName, "person");
                String id                   = Memory::ValueOf (p.GetAttribute ("id"));
                String testCanSerializeElts = Characters::ToString (p);
                //DbgTrace (L"o=%s", Characters::ToString (p).c_str ());
            });
            DOM::Element::Ptr{*personelElt.GetChildElements ().Find ([] (DOM::Element::Ptr p) {
                return p.HasAttribute ("id", "three.worker");
            })}.Delete ();
            EXPECT_EQ (personelElt.GetChildElements ().size (), 5u);
            {
                // This should throw, but doesn't on libxml - less quality validation --LGP 2024-01-08
                try {
                    d.Validate (schema); // dangling link we must remove from manager
                }
                catch (...) {
                }
                DOM::Element::Ptr bigBoss         = personelElt.GetChildByID ("Big.Boss");
                String            itsSubordinates = Memory::ValueOf (bigBoss.GetChild ("link").GetAttribute ("subordinates"));
                EXPECT_TRUE (itsSubordinates.Contains ("three.worker")); // guy we deleted
                bigBoss.GetChild ("link").SetAttribute ("subordinates", itsSubordinates.RemoveFirstIf ("three.worker"));
                EXPECT_NO_THROW (d.Validate (schema));
                DbgTrace ("bigBoss={}"_f, Characters::ToString (bigBoss));
            }
            personelElt.GetChildElements ().Apply ([] (DOM::Element::Ptr p) {
                EXPECT_EQ (p.GetName ().fNamespace, nullopt);
                EXPECT_EQ (p.GetName ().fName, "person");
            });
            // check schema validation
            EXPECT_NO_THROW (d.Validate (schema));
            EXPECT_THROW (d.Validate (wrongSchema), BadFormatException);
            // add an element
            {
                auto person2Add = personelElt.Append (NameWithNamespace{"person"});
                person2Add.SetAttribute ("id", "somenewguy");
                {
                    auto hisName = person2Add.Append (NameWithNamespace{"name"});
                    hisName.Append (NameWithNamespace{"family"}, "Jones");
                    hisName.Append ("given", "Phreddy");
                }
                person2Add.Append (NameWithNamespace{"email"}, "phred@jones.com");
                {
                    auto link = person2Add.Append (NameWithNamespace{"link"});
                    link.SetAttribute ("manager", "Big.Boss");
                }
                String testCanSerializeElts = Characters::ToString (person2Add);
                DbgTrace ("o={}"_f, Characters::ToString (person2Add));
            }
            EXPECT_NO_THROW (d.Validate (schema));
            EXPECT_EQ (personelElt.GetChildElements ().size (), 6u);
        });

        // Namespace's content test
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            Schema::Ptr        wrongSchema       = schemaFactory (kPersonalXSD_, nullptr);
            static const URI   kNS_              = "http://www.RecordsForLiving.com/Schemas/2012-03/ContentInformation/";
            Schema::Ptr        schema            = schemaFactory (kReferenceContent_2012_03_xsd, nullptr);
            DOM::Document::Ptr d                 = domFactory (kHealthFrameWorks_v3_xml.As<Streams::InputStream::Ptr<byte>> (), nullptr);
            String             tmp               = d.Write ();
            DOM::Element::Ptr  refContentDataElt = d.GetRootElement ();

            EXPECT_EQ (refContentDataElt.GetName ().fNamespace, kNS_);
            EXPECT_EQ (refContentDataElt.GetName ().fName, "ReferenceContentData");
            // check schema validation
            EXPECT_NO_THROW (d.Validate (schema));
            EXPECT_THROW (d.Validate (wrongSchema), BadFormatException);

            // grab 'concepts' Node, and try removing from it (and recheck validation) and then try adding to it and recheck validation
            DOM::Element::Ptr conceptsElt = refContentDataElt.GetChild (NameWithNamespace{kNS_, "Concepts"});
            EXPECT_EQ (conceptsElt.GetChildElements ().size (), 459u);

            {
                DOM::Element::Ptr c = conceptsElt.GetChildElements ().FirstValue (); // delete a fairly random concept (not in xml file order)
                //DbgTrace (L"deleting %s", Characters::ToString (c).c_str ());
                c.Delete ();
            }
            EXPECT_EQ (conceptsElt.GetChildElements ().size (), 458u);
            EXPECT_NO_THROW (d.Validate (schema));

            // add an element
            {
                /*
                    ConceptDetails>
                        <PrimaryKey Type="UMLS">C1547289</PrimaryKey>
                        <Priority>5</Priority>
                        <Categories>
                            <Category>Activity</Category>
                        </Categories>
                        <Terms>
                            <Term Priority="9">Tobacco - Chewed</Term>
                        </Terms>
                    </ConceptDetails>     
                *
                * NOTE:
                *       When appending, if NS not specified, its copied from parent node (elt appending to).
                * 
                *   TBD about attributes - http://stroika-bugs.sophists.com/browse/STK-999
                */
                auto concept2Add = conceptsElt.Append (NameWithNamespace{kNS_, "ConceptDetails"});
                {
                    auto primaryKey = concept2Add.Append (NameWithNamespace{"PrimaryKey"}, "C1547289");
                    // Using kNS_ in the SetAttribute call will produce schema validation failure with libxml2 - http://stroika-bugs.sophists.com/browse/STK-999
                    primaryKey.SetAttribute (NameWithNamespace{"Type"}, "UMLS");
                }
                concept2Add.Append (NameWithNamespace{"Priority"}, "5");
                concept2Add.Append (NameWithNamespace{"Categories"}).Append (NameWithNamespace{"Category"}, "Activity");
                {
                    auto terms = concept2Add.Append (NameWithNamespace{"Terms"});
                    {
                        auto term = terms.Append (NameWithNamespace{"Term"}, "Tobacco - Chewed");
                        term.SetAttribute (NameWithNamespace{"Priority"}, "9");
                    }
                }
                String testCanSerializeElts = Characters::ToString (concept2Add);
                //DbgTrace (L"o=%s", Characters::ToString (concept2Add).c_str ());
            }
            EXPECT_EQ (conceptsElt.GetChildElements ().size (), 459u);
            EXPECT_NO_THROW (d.Validate (schema));
            //d.Write (IO::FileSystem::FileOutputStream::New (IO::FileSystem::ToPath (Characters::Format (L"c:/temp/foo%d.xml", ++i))));
        });
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, DOM_WEIRD_LIBXML2_NAMESPACE_BUG)
    {
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            static const URI   kNS_     = "http://www.RecordsForLiving.com/doesntmatter";
            DOM::Document::Ptr origD    = domFactory (nullptr, nullptr);
            const auto         testName = XML::NameWithNamespace{kNS_, "phred"};
            origD.ReplaceRootElement (testName);
            EXPECT_EQ (origD.GetRootElement ().GetName (), testName);
            Streams::MemoryStream::Ptr<byte> ms = Streams::MemoryStream::New<byte> ();
            origD.Write (ms);
            DOM::Document::Ptr newD = domFactory (ms, nullptr);
            EXPECT_EQ (newD.GetRootElement ().GetName (), testName);
        });
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_XML, DOM_XPath)
    {
        const Memory::BLOB kPersonalXML_            = Memory::BLOB::Attach (Resources_::TestFiles_personal_xml);
        const Memory::BLOB kHealthFrameWorks_v3_xml = Memory::BLOB::Attach (Resources_::TestFiles_HealthFrameWorks_v3_xml);

        // no namespace's content test
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            using namespace XML::DOM;
            Document::Ptr d = domFactory (kPersonalXML_, nullptr);
            // Basic XPath
            {
                // read https://www.w3schools.com/xml/xpath_syntax.asp tutorial - basics
                auto n1 = d.GetRootElement ().LookupOneElement (XPath::Expression{"person"});
                EXPECT_EQ (n1.GetName (), "person");
                auto n2 = d.GetRootElement ().LookupOneElement (XPath::Expression{"person/name"});
                EXPECT_EQ (n2.GetName (), "name");
                auto n3 = d.GetRootElement ().LookupOneElement (XPath::Expression{"/personnel/person/name"});
                EXPECT_EQ (n3.GetName (), "name");
                auto n4 = d.GetRootElement ().LookupOneElement (XPath::Expression{"/person/name"}); // '/' means start at root of document, not context node, and root of document is personnel
                EXPECT_EQ (n4, nullptr);
                auto n5 = d.GetRootElement ().LookupOneElement (XPath::Expression{"//person/name"});
                EXPECT_EQ (n5.GetName (), "name");
            }
            // Iterator XPath
            {
                auto n1 = d.GetRootElement ().Lookup (XPath::Expression{"person"});
                EXPECT_EQ (n1.size (), 6u);
                auto n2 = d.GetRootElement ().Lookup (XPath::Expression{"person/name"});
                EXPECT_EQ (n2.size (), 6u);
                auto n3 = d.GetRootElement ().Lookup (XPath::Expression{"/personnel/person/name"});
                EXPECT_EQ (n3.size (), 6u);
                auto n4 = d.GetRootElement ().Lookup (XPath::Expression{"/person/name"}); // see similar case above
                EXPECT_EQ (n4.size (), 0u);
                auto n5 = d.GetRootElement ().Lookup (XPath::Expression{"//person/name"});
                EXPECT_EQ (n5.size (), 6u);
            }
            // Test Updating DOM using results from XPath lookups
            DISABLE_COMPILER_MSC_WARNING_START (4305) // apparently bad warning from compiler - about FirstValue second argument I think --LGP 2024-01-15
            {
                // Workaround lack of XPath predicates with Stroika Iterable filter functions
                Element::Ptr mrManager{d.GetRootElement ().Lookup (XPath::Expression{"person"}).FirstValue ([] (XPath::Result n) -> bool {
                    Element::Ptr e = n;
                    return e != nullptr and e.GetID () == "Big.Boss";
                })};
                EXPECT_EQ (mrManager.GetValue ("email"), "chief@foo.com"_k);
                mrManager.SetValue ("email", "alpha@beta.com");
                Element::Ptr mrManager2{d.GetRootElement ().Lookup (XPath::Expression{"person"}).FirstValue ([] (XPath::Result n) {
                    Element::Ptr e = n;
                    return e != nullptr and e.GetValue ("email") == "alpha@beta.com"_k;
                })};
                EXPECT_EQ (mrManager2.GetID (), "Big.Boss"_k);
            }
            // (DOM Update/XPath code)
            try {
                {
                    Element::Ptr mrManager = d.GetRootElement ().Lookup (XPath::Expression{"person"}).FirstValue ([] (XPath::Result n) {
                        Element::Ptr e = n;
                        return e != nullptr and e.GetValue ("email") == "alpha@beta.com"_k;
                    });
                    EXPECT_EQ (mrManager.GetValue ("@id"), "Big.Boss"_k);
                }
                auto mrManager1Subordinates = d.GetRootElement ().LookupOneNode (XPath::Expression{"person/link/@subordinates"});
                EXPECT_EQ (mrManager1Subordinates.GetValue (), "one.worker two.worker three.worker four.worker five.worker");
                Element::Ptr mrManager2Link = d.GetRootElement ().LookupOneElement (XPath::Expression{"person/link[@subordinates]"});
                EXPECT_EQ (mrManager2Link.GetValue ("@subordinates"), "one.worker two.worker three.worker four.worker five.worker");
                auto mrManager2a = d.GetRootElement ().LookupOneElement (XPath::Expression{"person[link/@subordinates]"});
                EXPECT_EQ (mrManager2a.GetValue ("email"), "alpha@beta.com"); // because changed in previous test
                EXPECT_EQ (mrManager2a.GetValue ("name/family"), "Boss");
                mrManager2a.SetValue ("name/family", "Bossy");
                auto mrManager3 = d.GetRootElement ().LookupOneElement (XPath::Expression{"person[@id='Big.Boss']"});
                EXPECT_EQ (mrManager3.GetValue ("email"), "alpha@beta.com");
                EXPECT_EQ (mrManager3.GetValue ("name/family"), "Bossy");
            }
            catch (const XML::DOM::XPath::XPathExpressionNotSupported&) {
#if qHasFeature_Xerces
                Assert (d.GetRep ()->GetProvider () == &Providers::Xerces::kDefaultProvider); // sadly Xerces 3.2 doesn't support [
#else
                AssertNotReached ();
#endif
            }
            DISABLE_COMPILER_MSC_WARNING_END (4305)
        });

        // namespace's content test
        // Basic XPath
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            using namespace XML::DOM;
            Document::Ptr                           d = domFactory (kHealthFrameWorks_v3_xml, nullptr);
            static const URI                        kNS_{"http://www.RecordsForLiving.com/Schemas/2012-03/ContentInformation/"};
            static const XPath::Expression::Options kXPathOptions_{.fNamespaces = Mapping<String, URI>{{"n", kNS_}}};

            auto n1 = d.GetRootElement ().LookupOneElement (XPath::Expression{"person"sv});
            EXPECT_EQ (n1, nullptr);
            auto badHeader1 = d.GetRootElement ().LookupOneElement (XPath::Expression{"Header", kXPathOptions_});
#if qHasFeature_Xerces
            if (d.GetRep ()->GetProvider () != &Providers::Xerces::kDefaultProvider) { // Xerces 3.2.5 appears to match despite the wrong namespace reference
                EXPECT_EQ (badHeader1, nullptr);                                       // no namespace specified
            }
#else
            EXPECT_EQ (badHeader1, nullptr); // no namespace specified
#endif
            auto badHeader2 = d.GetRootElement ().LookupOneElement (XPath::Expression{"n:header", kXPathOptions_});
            EXPECT_EQ (badHeader2, nullptr); // case sensitive match
            EXPECT_ANY_THROW (d.GetRootElement ().LookupOneElement (XPath::Expression{"N:Header", kXPathOptions_}));
            auto header = d.GetRootElement ().LookupOneElement (XPath::Expression{"n:Header", kXPathOptions_});
            //DbgTrace (L"header=%s", Characters::ToString (header).c_str ());
            EXPECT_EQ (header.GetName (), (NameWithNamespace{kNS_, "Header"}));
            auto docRelHeader = d.GetRootElement ().LookupOneElement (XPath::Expression{"/n:ReferenceContentData/n:Header", kXPathOptions_});
            EXPECT_EQ (docRelHeader, header); // Check doc Rel Path same as relative path, and check == works for nodes
            auto docRelHeader2 = d.GetRootElement ().LookupOneElement (XPath::Expression{"//n:Header", kXPathOptions_});
            EXPECT_EQ (docRelHeader2, header); // verify // working (at least somewhat - limited for xerces)
            EXPECT_NE (header.LookupOneElement (XPath::Expression{"n:SourceDefinitions", kXPathOptions_}), nullptr);
            EXPECT_EQ (header.GetValue (XPath::Expression{"n:SourceDefinitions/n:SourceDefinition/n:Version", kXPathOptions_}), "v3"sv);
#if qHasFeature_Xerces
            if (d.GetRep ()->GetProvider () != &Providers::Xerces::kDefaultProvider) { // fails on xerces 3.2.5
                auto docRelHeader3 = d.GetRootElement ().LookupOneElement (XPath::Expression{"/n:ReferenceContentData//n:Header", kXPathOptions_});
                EXPECT_EQ (docRelHeader3, header); // verify // working
            }
#else
            auto docRelHeader3 = d.GetRootElement ().LookupOneElement (XPath::Expression{"/n:ReferenceContentData//n:Header", kXPathOptions_});
            EXPECT_EQ (docRelHeader3, header); // verify // working
#endif
        });
        // Iterator XPath
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            using namespace XML::DOM;
            Document::Ptr                           d = domFactory (kHealthFrameWorks_v3_xml, nullptr);
            static const URI                        kNS_{"http://www.RecordsForLiving.com/Schemas/2012-03/ContentInformation/"};
            static const XPath::Expression::Options kXPathOptions_{.fNamespaces = Mapping<String, URI>{{"n", kNS_}}};

            auto n1 = d.GetRootElement ().Lookup (XPath::Expression{"person"});
            EXPECT_EQ (n1.size (), 0u); // none there, no namespace, and no elt...
            auto n2 = d.GetRootElement ().Lookup (XPath::Expression{"n:Header/n:SourceDefinitions/n:SourceDefinition", kXPathOptions_});
            EXPECT_EQ (n2.size (), 1u);
            Iterable<XPath::Result> n3 = d.GetRootElement ().Lookup (XPath::Expression{"n:Concepts/n:ConceptDetails", kXPathOptions_});
            EXPECT_GE (n3.size (), 459u); // could be more cuz could include other nodes (comments etc)
            Iterable<Element::Ptr> n3e = n3.Map<Iterable<Element::Ptr>> ([] (XPath::Result e) -> optional<Element::Ptr> {
                Element::Ptr ep{e};
                return ep != nullptr ? ep : optional<Element::Ptr>{};
            });
            EXPECT_EQ (n3e.size (), 459u);
            EXPECT_TRUE (n3e.SequentialEquals (d.GetRootElement ().LookupElements (XPath::Expression{"n:Concepts/n:ConceptDetails", kXPathOptions_})));
        });
        // Test Updating DOM using results from XPath lookups
        DoWithEachXMLProvider_ ([&] ([[maybe_unused]] auto saxParser, [[maybe_unused]] auto schemaFactory, [[maybe_unused]] auto domFactory) {
            using namespace XML::DOM;
            Document::Ptr                           d = domFactory (kHealthFrameWorks_v3_xml, nullptr);
            static const URI                        kNS_{"http://www.RecordsForLiving.com/Schemas/2012-03/ContentInformation/"};
            static const XPath::Expression::Options kXPathOptions_{.fNamespaces = Mapping<String, URI>{{"n", kNS_}}};

            Iterable<Element::Ptr> originalConceptElts =
                d.GetRootElement ().LookupElements (XPath::Expression{"n:Concepts/n:ConceptDetails", kXPathOptions_});
            EXPECT_EQ (originalConceptElts.size (), 459u);

            /*
                <ConceptDetails>
                    <PrimaryKey Type="HF">H0000124</PrimaryKey>
                    <Categories>
                        <Category>Activity</Category>
                    </Categories>
                    <Terms>
                        <Term Priority="10">Aerobic Excercise</Term>
                    </Terms>
                </ConceptDetails>
            */
#if qHasFeature_Xerces

            if (d.GetRep ()->GetProvider () != &Providers::Xerces::kDefaultProvider) { // Xerces 3.2.5 doesn't come close to supporting this
#endif
                Element::Ptr aerobicExcerciseElt = d.GetRootElement ().LookupOneElement (
                    XPath::Expression{"//n:ConceptDetails[n:PrimaryKey/@Type='HF' and n:PrimaryKey/text()='H0000124']", kXPathOptions_});
                EXPECT_NE (aerobicExcerciseElt, nullptr);
                EXPECT_EQ (aerobicExcerciseElt.GetValue (XPath::Expression{"n:Categories/n:Category", kXPathOptions_}), "Activity");
                EXPECT_EQ ((Set<String>{"Activity"}), aerobicExcerciseElt.GetValues (XPath::Expression{"n:Categories/n:Category", kXPathOptions_}));
                // Note as commented in DOM.h, "Default namespace does is inherited by enclosed elements, but **not** by enclosed attributes"
                EXPECT_EQ (aerobicExcerciseElt.LookupOneNode (XPath::Expression{"n:PrimaryKey/@Type", kXPathOptions_}).GetName ().fNamespace, nullopt);

                auto conceptsElt = d.GetRootElement ().LookupOneElement (XPath::Expression{"n:Concepts", kXPathOptions_});
                EXPECT_EQ (conceptsElt, aerobicExcerciseElt.GetParent ());

                String savedAerobicExcerciseSerialized = Characters::ToString (aerobicExcerciseElt);
                aerobicExcerciseElt.Delete ();
                EXPECT_EQ (d.GetRootElement ().LookupElements (XPath::Expression{"n:Concepts/n:ConceptDetails", kXPathOptions_}).size (), 459u - 1u);
                aerobicExcerciseElt = d.GetRootElement ().LookupOneElement (
                    XPath::Expression{"//n:ConceptDetails[n:PrimaryKey/@Type='HF' and n:PrimaryKey/text()='H0000124']", kXPathOptions_});
                EXPECT_EQ (aerobicExcerciseElt, nullptr);

                // OK - now lets try adding a concept (namespace inherited from parent node)
                auto concept2Add = conceptsElt.Append ("ConceptDetails");
                {
                    auto primaryKey = concept2Add.Append ("PrimaryKey", "H0000124");
                    primaryKey.SetAttribute (NameWithNamespace{"Type"}, "HF");
                }
                concept2Add.Append (NameWithNamespace{"Priority"}, "10");
                concept2Add.Append (NameWithNamespace{"Categories"}).Append (NameWithNamespace{"Category"}, "Activity");
                {
                    auto terms = concept2Add.Append (NameWithNamespace{"Terms"});
                    {
                        auto term = terms.Append (NameWithNamespace{"Term"}, "Aerobic Excercise");
                        term.SetAttribute (NameWithNamespace{"Priority"}, "10");
                    }
                }
                // Let's look it up again
                aerobicExcerciseElt = d.GetRootElement ().LookupOneElement (
                    XPath::Expression{"//n:ConceptDetails[n:PrimaryKey/@Type='HF' and n:PrimaryKey/text()='H0000124']", kXPathOptions_});
                // Not the same cuz of 'i'm surprised...' xml comment node
                //DbgTrace (L"savedAerobicExcerciseSerialized=%s", Characters::ToString (savedAerobicExcerciseSerialized).c_str ());
                // DbgTrace (L"NEW savedAerobicExcerciseSerialized=%s", Characters::ToString (aerobicExcerciseElt).c_str ());
                EXPECT_NE (savedAerobicExcerciseSerialized, Characters::ToString (aerobicExcerciseElt));
#if qHasFeature_Xerces
            }
#endif
        });
    }
}
#endif
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
