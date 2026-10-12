/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Options_h_
#define _Stroika_Frameworks_UPnP_Options_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/XML/Providers/IProvider.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::UPnP {

    /**
     *  \brief What UPnP must be told that has a sensible default - taken by Serialize and DeSerialize, defaulted, so a caller
     *         says only what it wants different
     *
     *  One block rather than an argument apiece, so that what comes later - whether to validate a description against its
     *  schema while reading, say - costs no caller anything.
     *
     *  \par Example Usage
     *      \code
     *          // this device reads and writes with Xerces, whatever the rest of the process does
     *          const UPnP::Options kOptions_{.fXMLProvider = &XML::Providers::Xerces::kDefaultProvider};
     *          response.write (Serialize (kSwitchPower, kOptions_));
     *      \endcode
     */
    struct Options {
        /**
         *  \brief Read and write XML with this provider - nullptr, the default, meaning DataExchange::XML::Providers::kDefaultProvider
         *
         *  Which is libxml2 where it is configured, else Xerces. A provider is a static singleton, so this is a plain
         *  pointer and nothing owns it.
         *
         *  \note This is a UPnP option, not a process-wide one, so two libraries that each speak UPnP need not agree.
         */
        const Foundation::DataExchange::XML::Providers::IXMLProvider* fXMLProvider{nullptr};

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
        /**
         *  \brief fXMLProvider, or Stroika's default where that is nullptr - the one Serialize and DeSerialize actually use
         */
        nonvirtual const Foundation::DataExchange::XML::Providers::IXMLProvider& GetXMLProvider () const
        {
            return fXMLProvider == nullptr ? *Foundation::DataExchange::XML::Providers::kDefaultProvider () : *fXMLProvider;
        }
#endif
    };

}

#endif /*_Stroika_Frameworks_UPnP_Options_h_*/
