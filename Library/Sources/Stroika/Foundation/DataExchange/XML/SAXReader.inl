/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_SAXReader_inl_
#define _Stroika_Foundation_DataExchange_XML_SAXReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::DataExchange::XML {

#if qStroika_Foundation_DataExchange_XML_SupportParsing
    inline void SAXParse (const Streams::InputStream::Ptr<byte>& in, StructuredStreamEvents::IConsumer& callback, const Schema::Ptr& schema,
                          Execution::ProgressMonitor::Updater progress)
    {
#if qHasFeature_Xerces
        constexpr Provider kDefaultProvider_ = Provider::eXerces;
#elif qHasFeature_libxml2
        constexpr Provider kDefaultProvider_ = Provider::eLibXml2;
#endif
        return SAXParse (kDefaultProvider_, in, callback, schema, progress);
    }
#endif

}

#endif /*_Stroika_Foundation_DataExchange_XML_SAXReader_inl_*/
