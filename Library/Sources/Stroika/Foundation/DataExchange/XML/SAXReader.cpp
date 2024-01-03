/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <atomic>

#include "../../Debug/Trace.h"
#include "../../Execution/Common.h"
#include "../../Execution/Finally.h"
#include "../../Execution/ProgressMonitor.h"
#include "../../Memory/Common.h"
#include "../../Memory/MemoryAllocator.h"
#include "../../Streams/InputStreamDelegationHelper.h"

#include "../BadFormatException.h"

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
void XML::SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer& callback, const Schema::Ptr& schema,
                    Execution::ProgressMonitor::Updater progress)
{
    static const XML::Providers::ISAXProvider* kDefaultProvider_ = XML::Providers::kDefaultProvider ();
    return SAXParse (*kDefaultProvider_, in, callback, schema, progress);
}
#endif

void XML::SAXParse (const Providers::ISAXProvider& saxProvider, const Streams::InputStream::Ptr<byte>& in,
                    StructuredStreamEvents::IConsumer& callback, const Schema::Ptr& schema, Execution::ProgressMonitor::Updater progress)
{
    if (progress == nullptr) {
        saxProvider.SAXParse (in, callback, schema);
    }
    else {
        // IF progress provided, wrap 'in' in magic to handle progress
        saxProvider.SAXParse (Execution::MakeInputStreamWithProgress (in, progress), callback, schema);
    }
}
