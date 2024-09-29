/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_WSImpl_h_
#define _StroikaSample_WebServices_WSImpl_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "IWSAPI.h"

/**
 */

namespace Stroika::Samples::HTMLUI {

    /**
     *  \brief: WSImpl is the module providing the concrete C++ implementation (fully using C++ objects, exceptions etc) for the web service API.
     *
     *  This is straight C++ application logic, with (hopefully) zero logic relating to marshalling, or HTTP etc.
     */
    class WSImpl : public IWSAPI {
    public:
        WSImpl (function<About::APIServerInfo::WebServer ()> webServerStatsFetcher);

    public:
        virtual Stroika::Frameworks::WebService::OpenAPI::Specification GetOpenAPISpecification () const override;

    public:
        virtual About about_GET () const override;

    public:
        virtual tuple<BLOB, InternetMediaType> resource_GET (const String& name) const override;

    private:
        struct Rep_;

    private:
        shared_ptr<Rep_> fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WSImpl.inl"

#endif /*_StroikaSample_WebServices_WSImpl_h_*/
