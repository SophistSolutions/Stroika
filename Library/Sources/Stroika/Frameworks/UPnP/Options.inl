/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::UPnP {

    /*
     ********************************************************************************
     ******************************** UPnP::Options *********************************
     ********************************************************************************
     */
#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
    inline const Foundation::DataExchange::XML::Providers::IXMLProvider& Options::GetXMLProvider () const
    {
        return Foundation::DataExchange::XML::Providers::ProviderOrDefault (fXMLProvider);
    }
#endif

}
