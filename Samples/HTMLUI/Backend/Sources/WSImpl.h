/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_WSImpl_h_
#define _StroikaSample_WebServices_WSImpl_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/WebServer/ConnectionManager.h"

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
        /**
         * Function that can be called safely on a webserver connection-manager
         */
        using WithWebServerCallbackType = function<void (const Stroika::Frameworks::WebServer::ConnectionManager&)>;

    public:
        /**
         *  WSImpl may need access to webserver connection manager (const API access) occasionally, so provide in
         *  controlled way that can work with locking if needed; note effectively same as passing in ConnectionManager&,
         *  except that the caller might want to control when the ConnectionManager& is referenced (e.g. locking).
         */
        WSImpl (function<void (const WithWebServerCallbackType&)> passWS2Callback);

    public:
        virtual Stroika::Frameworks::WebService::OpenAPI::Specification GetOpenAPISpecification () const override;

    public:
        virtual About about_GET () const override;

    public:
        virtual HealthStatus healthcheck_GET () const override;

    public:
        virtual TypedBLOB resource_GET (const String& name) const override;

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
