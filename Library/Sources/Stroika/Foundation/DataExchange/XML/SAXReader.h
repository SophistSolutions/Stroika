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
#include "../VariantValue.h"

#include "Common.h"
#include "Schema.h"

/**
 *  \file
 */

namespace Stroika::Foundation::DataExchange::XML {

    /**
     * Parse will throw an exception if it encounters any errors parsing.
     *
     *  @see ObjectReaderRegistry for examples of one way to use this
     */
#if qStroika_Foundation_DataExchange_XML_SupportParsing
    void SAXParse (const Streams::InputStream<byte>::Ptr& in, StructuredStreamEvents::IConsumer& callback,
                   const optional<Schema>& schema = nullopt, Execution::ProgressMonitor::Updater progress = nullptr);
    void SAXParse (const Memory::BLOB& in, StructuredStreamEvents::IConsumer& callback, const optional<Schema>& schema = nullopt,
                   Execution::ProgressMonitor::Updater progress = nullptr);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SAXReader.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_SAXReader_h_*/
