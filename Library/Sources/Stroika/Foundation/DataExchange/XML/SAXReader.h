/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_SAXReader_h_
#define _Stroika_Foundation_DataExchange_XML_SAXReader_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Execution/ProgressMonitor.h"
#include "../../Streams/InputStream.h"
#include "../StructuredStreamEvents/IConsumer.h"

#include "Common.h"
#include "Schema.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::XML::Providers {
    struct ISAXProvider;
};
namespace Stroika::Foundation::DataExchange::XML {

    /**
     * Parse will throw an exception if it encounters any errors parsing.
     *
     *  @see ObjectReaderRegistry for examples of one way to use this
     */
    void SAXParse (const Providers::ISAXProvider& saxProvider, const Streams::InputStream::Ptr<byte>& in,
                   StructuredStreamEvents::IConsumer* callback = nullptr, const Schema::Ptr& schema = nullptr,
                   Execution::ProgressMonitor::Updater progress = nullptr);
#if qStroika_Foundation_DataExchange_XML_SupportParsing
    void SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer* callback = nullptr,
                   const Schema::Ptr& schema = nullptr, Execution::ProgressMonitor::Updater progress = nullptr);
#endif

#if qStroika_Foundation_DataExchange_XML_SupportParsing
   [[deprecated ("Since Stroika v3.0d5 use address of callback")]]  inline void
    SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer& callback, const Schema::Ptr& schema = nullptr)
    {
        SAXParse (in, &callback, schema);
    }
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SAXReader.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_SAXReader_h_*/
