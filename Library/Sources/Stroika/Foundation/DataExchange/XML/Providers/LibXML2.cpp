/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../DataExchange/BadFormatException.h"
#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"
#include "../../../Memory/Common.h"
#include "../../../Memory/MemoryAllocator.h"

#include "LibXML2.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::Schema;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

static_assert (qHasFeature_libxml2, "Don't compile this file if qHasFeature_libxml2 not set");

using namespace Stroika::Foundation::DataExchange::XML;
using namespace Providers::LibXML2;

using namespace XML::Providers::LibXML2;

namespace {
    struct SchemaRep_ : Schema::IRep, ILibXML2SchemaRep {
        SchemaRep_ (const optional<URI>& targetNamespace, const Memory::BLOB& targetNamespaceData,
                    const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions)
            : fTargetNamespace{targetNamespace}
            , fSourceComponents{sourceComponents}
            , fNamespaceDefinitions{namespaceDefinitions}
        {
            if (targetNamespace) {
                fSourceComponents.push_back (SourceComponent{.fBLOB = targetNamespaceData, .fNamespace = *targetNamespace});
            }
            xmlSchemaParserCtxt* schemaParseContext = xmlSchemaNewMemParserCtxt (reinterpret_cast<const char*> (targetNamespaceData.data ()),
                                                                                 static_cast<int> (targetNamespaceData.size ()));
            fCompiledSchema = xmlSchemaParse (schemaParseContext);
            xmlSchemaFreeParserCtxt (schemaParseContext);
            Execution::ThrowIfNull (fCompiledSchema);
        }
        SchemaRep_ (const SchemaRep_&) = delete;
        virtual ~SchemaRep_ ()
        {
            xmlSchemaFree (fCompiledSchema);
        }
        optional<URI>             fTargetNamespace;
        Sequence<SourceComponent> fSourceComponents;
        NamespaceDefinitionsList  fNamespaceDefinitions;
        xmlSchema*                fCompiledSchema{nullptr};

        virtual optional<URI> GetTargetNamespace () const override
        {
            return fTargetNamespace; // should get from READING the schema itself! I THINK --LGP 2023-12-18
        }
        virtual NamespaceDefinitionsList GetNamespaceDefinitions () const override
        {
            return fNamespaceDefinitions;
        }
        virtual Sequence<SourceComponent> GetSourceComponents () override
        {
            return fSourceComponents;
        }
        virtual xmlSchema* GetSchemaLibRep () override
        {
            AssertNotNull (fCompiledSchema);
            return fCompiledSchema;
        }
    };
}

/*
 ********************************************************************************
 *************** Provider::Xerces::Map2StroikaExceptionsErrorReporter ***********
 ********************************************************************************
 */

String Providers::LibXML2::libXMLString2String (const xmlChar* s, int len)
{
    return String{span{reinterpret_cast<const char*> (s), static_cast<size_t> (len)}};
}
String Providers::LibXML2::libXMLString2String (const xmlChar* t)
{
    return String::FromUTF8 (reinterpret_cast<const char*> (t));
}

/*
 ********************************************************************************
 ***************** Provider::Xerces::Providers::LibXML2::Provider ***************
 ********************************************************************************
 */
Providers::LibXML2::Provider::Provider ()
{
    TraceContextBumper ctx{"LibXML2::Provider::CTOR"};
    LIBXML_TEST_VERSION;
}

Providers::LibXML2::Provider::~Provider ()
{
    TraceContextBumper ctx{"LibXML2::Provider::DTOR"};
    xmlCleanupParser ();
#if qDebug
    xmlMemoryDump ();
#endif
}

shared_ptr<Schema::IRep> Providers::LibXML2::Provider::SchemaFactory (const optional<URI>& targetNamespace, const BLOB& targetNamespaceData,
                                                                      const Sequence<Schema::SourceComponent>& sourceComponents,
                                                                      const NamespaceDefinitionsList&          namespaceDefinitions) const
{
    return make_shared<SchemaRep_> (targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions);
}
shared_ptr<DOM::Document::IRep> Providers::LibXML2::Provider::DocumentFactory (const String& documentElementName, const optional<URI>& ns) const
{
    AssertNotImplemented ();
    return nullptr;
}

shared_ptr<DOM::Document::IRep> Providers::LibXML2::Provider::DocumentFactory (const Streams::InputStream::Ptr<byte>& in,
                                                                               const Schema::Ptr& schemaToValidateAgainstWhileReading) const
{
    AssertNotImplemented ();
    return nullptr;
}
void Providers::LibXML2::Provider::SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer& callback,
                                             const Schema::Ptr& schema) const
{
    AssertNotImplemented ();
}
