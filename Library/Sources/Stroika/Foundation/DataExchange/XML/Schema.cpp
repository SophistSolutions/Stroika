/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"

#include "SAXReader.h"
#include "Schema.h"

#include "Providers/IProvider.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::XML::Schema;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************************* Schema::New **********************************
 ********************************************************************************
 */

Ptr Schema::New (const Providers::ISchemaProvider& p, const optional<URI>& targetNamespace, const BLOB& targetNamespaceData,
                 const Sequence<SourceComponent>& sourceComponents, const NamespaceDefinitionsList& namespaceDefinitions)
{
    return Ptr{p.SchemaFactory (targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions)};
}
#if qStroika_Foundation_DataExchange_XML_SupportSchema
Ptr Schema::New (const optional<URI>& targetNamespace, const BLOB& targetNamespaceData, const Sequence<SourceComponent>& sourceComponents,
                 const NamespaceDefinitionsList& namespaceDefinitions)
{
    static const XML::Providers::ISchemaProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return New (*kDefaultProvider_, targetNamespace, targetNamespaceData, sourceComponents, namespaceDefinitions);
}
#endif

/*
 ********************************************************************************
 ************************************** ValidateFile ****************************
 ********************************************************************************
 */
#if qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportParsing
void DataExchange::XML::Schema::ValidateFile (const filesystem::path& externalFileName, const Ptr& schema)
{
    // not the most efficient impl but simple and generic, and good enuf for now
    struct noOpConsumer : StructuredStreamEvents::IConsumer {};
    noOpConsumer consumer;
    SAXParse (IO::FileSystem::FileInputStream::New (externalFileName), consumer, schema);
}
#endif
