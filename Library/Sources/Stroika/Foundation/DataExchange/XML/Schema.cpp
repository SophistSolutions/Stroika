/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"

#include "DOM.h"
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
 ********************************* Schema::Ptr **********************************
 ********************************************************************************
 */
#if qStroika_Foundation_DataExchange_XML_SupportDOM
template <>
XML::DOM::Document::Ptr Ptr::As ()
{
    return DOM::Document::New (fRep_->GetData ());
}
#endif
template <>
XML::DOM::Document::Ptr Schema::Ptr::As (const Providers::IDOMProvider& p)
{
    return DOM::Document::New (p, fRep_->GetData ());
}

/*
 ********************************************************************************
 ********************************* Schema::New **********************************
 ********************************************************************************
 */
Ptr Schema::New (const Providers::ISchemaProvider& p, const Streams::InputStream::Ptr<byte>& schemaData, const Resource::ResolverPtr& resolver)
{
    return Ptr{p.SchemaFactory (schemaData, resolver)};
}

#if qStroika_Foundation_DataExchange_XML_SupportSchema
Ptr Schema::New (const Streams::InputStream::Ptr<byte>& schemaData, const Resource::ResolverPtr& resolver)
{
    static const XML::Providers::ISchemaProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return New (*kDefaultProvider_, schemaData, resolver);
}
#endif

/*
 ********************************************************************************
 ************************************ ValidateFile ******************************
 ********************************************************************************
 */
#if qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportParsing
void DataExchange::XML::Schema::ValidateFile (const filesystem::path& externalFileName, const Ptr& schema)
{
    AssertNotNull (dynamic_cast<const Providers::ISAXProvider*> (schema.GetRep ()->GetProvider ()));
    SAXParse (*dynamic_cast<const Providers::ISAXProvider*> (schema.GetRep ()->GetProvider ()),
              IO::FileSystem::FileInputStream::New (externalFileName), nullptr, schema);
}
#endif
