/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <atomic>

#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/ProgressMonitor.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Memory/MemoryAllocator.h"
#include "Stroika/Foundation/Streams/InputStreamDelegationHelper.h"

#include "SAXReader.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Debug;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ************************************* SAXParse *********************************
 ********************************************************************************
 */
#if qStroika_Foundation_DataExchange_XML_SupportParsing
void XML::SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback, const Schema::Ptr& schema,
                    Execution::ProgressMonitor::Updater progress)
{
    static const XML::Providers::ISAXProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return SAXParse (*kDefaultProvider_, in, callback, schema, progress);
}
#endif

void XML::SAXParse (const Providers::ISAXProvider& saxProvider, const Streams::InputStream::Ptr<byte>& in,
                    StructuredStreamEvents::IConsumer* callback, const Schema::Ptr& schema, Execution::ProgressMonitor::Updater progress)
{
    Schema::Ptr useSchema = schema;
    if (useSchema != nullptr) {
        if (static_cast<const Providers::IProvider*> (schema.GetRep ()->GetProvider ()) != &saxProvider) {
            WeakAssertNotReached (); // not necessarily a bug, but if this is done regularly, fix so they match
            useSchema = useSchema.As<Schema::Ptr> (
                *dynamic_cast<const Providers::ISchemaProvider*> (static_cast<const Providers::IProvider*> (&saxProvider)));
        }
    }
    if (progress == nullptr) {
        saxProvider.SAXParse (in, callback, useSchema);
    }
    else {
        // IF progress provided, wrap 'in' in magic to handle progress
        saxProvider.SAXParse (Execution::MakeInputStreamWithProgress (in, progress), callback, useSchema);
    }
}
