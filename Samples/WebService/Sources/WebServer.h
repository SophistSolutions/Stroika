/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _StroikaSample_WebServices_WebServer_h_
#define _StroikaSample_WebServices_WebServer_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Containers/Collection.h"
#include "Stroika/Foundation/Containers/Sequence.h"

#include "IWSAPI.h"

namespace StroikaSample::WebServices {

    using namespace std;

    /**
     *  WebServer handles basic HTTP web server technology, and translates to ISAPI interface web-service implementation methods.
     *  This internally does all the marshalling (serialization/deserialization) of the web service parameters into a clean C++ API (IWSAPI).
     */
    class WebServer {
    public:
        WebServer (uint16_t portNumber, const shared_ptr<IWSAPI>& wsImpl);

    private:
        class Rep_;
        shared_ptr<Rep_> fRep_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WebServer.inl"

#endif /*_StroikaSample_WebServices_WebServer_h_*/
