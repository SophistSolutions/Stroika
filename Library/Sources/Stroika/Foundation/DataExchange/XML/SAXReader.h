/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_SAXReader_h_
#define _Stroika_Foundation_DataExchange_XML_SAXReader_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Mapping.h"
#include "../../Execution/ProgressMonitor.h"
#include "../../Streams/InputStream.h"

#include "../StructuredStreamEvents/IConsumer.h"
#include "../VariantValue.h"
#include "Common.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   Redo SAXParse() using factory style - like we have for IO::Networking::Transfer (curl or winhttp)
 *              then have ability to check for registered impl (and select impl) for SAX backend)
 *
 *      @todo   Add schema support, and overload of SAXParse taking schema, such as
 *              void  SAXParse (istream& in, const Schema& schema, CallbackInterface& callback, Execution::ProgressMontior* progress = nullptr);
 *
 */

namespace Stroika::Foundation::DataExchange::XML {

    using Characters::String;
    using Containers::Mapping;

    /**
     * Parse will throw an exception if it encounters any errors parsing.
     *
     *  @see ObjectReaderRegistry for examples of one way to use this
     */
    void SAXParse (const Streams::InputStream<std::byte>::Ptr& in, StructuredStreamEvents::IConsumer& callback, Execution::ProgressMonitor::Updater progress = nullptr);
    void SAXParse (const Memory::BLOB& in, StructuredStreamEvents::IConsumer& callback, Execution::ProgressMonitor::Updater progress = nullptr);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SAXReader.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_SAXReader_h_*/
