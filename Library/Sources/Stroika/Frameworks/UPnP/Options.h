/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_Options_h_
#define _Stroika_Frameworks_UPnP_Options_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/XML/Providers/IProvider.h"
#include "Stroika/Foundation/DataExchange/XML/Schema.h"

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

        /**
         *  \brief Check a description being READ against this schema, throwing where it does not hold - nullptr, the
         *         default, checking nothing
         *
         *      \code
         *          DeviceDescription dd = UPnP::DeSerialize (fetched); // no validation by default, because real devices in the field are loose and a control point that refused them would see fewer than one that did not
         *      \endcode
         *
         *      \code
         *          // told a description is malformed, rather than silently getting half of one
         *          const UPnP::Options kStrict_{.fValidateReadAgainst = UPnP::DeviceDescriptionSchema (*XML::Providers::kDefaultProvider ())};
         *          DeviceDescription dd = UPnP::DeSerialize (fetched, kStrict_);
         *      \endcode
         *
         *  Pass the schema that goes with the call - DeviceDescriptionSchema to DeSerialize a device description,
         *  ServiceDescriptionSchema to DeSerialize an SCPD. They are different documents in different namespaces, and
         *  nothing here can tell which you meant.
         *
         *  Nothing by default, because validating refuses descriptions that work today. UPnP devices in the field are
         *  famously loose - a vendor's own elements in no namespace, elements out of the order the schema fixes,
         *  required ones missing - and a control point that refused them would see fewer devices than one that did not,
         *  which is not a library's decision to make. Reading is deliberately lenient to match (@see
         *  ServiceDescription_Others_), and this is how a caller that would rather be told says so.
         *
         *  \note Naming the schema here, rather than this being a flag that reaches for one, is also what lets a program
         *        that never validates not carry either schema: nothing inside UPnP mentions them, so their ~11K each is
         *        linked only by a caller that asks (@see DeviceDescriptionSchema, ServiceDescriptionSchema).
         *
         *  \note What Stroika WRITES does validate - @see Tests/54's DescriptionsValidateAgainstPublishedSchemas_.
         */
        Foundation::DataExchange::XML::Schema::Ptr fValidateReadAgainst{nullptr};

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
        /**
         *  \brief fXMLProvider, or Stroika's default where that is nullptr - the one Serialize and DeSerialize actually use
         */
        nonvirtual const Foundation::DataExchange::XML::Providers::IXMLProvider& GetXMLProvider () const;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Options.inl"

#endif /*_Stroika_Frameworks_UPnP_Options_h_*/
