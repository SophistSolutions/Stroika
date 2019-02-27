/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_ 1

#include "../../../StroikaPreComp.h"

#include <system_error>

#if qHasFeature_LibCurl
// For CURLCode define
#include <curl/curl.h>
#endif

#include "../../../Execution/Exceptions.h"

#include "Client.h"

/*
 * TODO:
 *      @todo   Must do basic METHOD support - setting CURLOPT_GET for gets, etc.
 *              Need extended MEHTOD support for DELETE (just specify string)
 *              And for PUT/POST special setopt(CURLOPT_POST/PUT, and do reader function for pUT ajnd direct data pass for Post -
 *              since I THINK thats what curl requires - use common API if I can find a way).
 *
 *      @todo   Handle pass in of headers. Treat special headers like content type proeprly. Same for content-length.
 *
 *      @todo   Consider if curl global init/global_free stuff safe (look at curl code). May run into issue if other
 *              librraries lusing curl (like CurlNetAccessor::initCurl in xerces) do the same thing.
 */

namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_LibCurl
    /**
     *  Return a reference the the LibCurl error category object. This object lives forever (like other error categories).
     */
    const std::error_category& LibCurl_error_category () noexcept;

    /**
     * throw Exception () type iff the status indicates a real exception code. This MAY not throw an exception of type LibCurlException,
     * but MAY map to any other exception type
     */
    void ThrowIfError (CURLcode status);

    /**
     *  Just object-slice the smart pointer to get a regular connection object - this is just a factory for
     *  LibCurl connection rep objects
     */
    class Connection_LibCurl : public Connection {
    public:
        Connection_LibCurl (const Options& options = Options ());

    private:
        class Rep_;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Client_libcurl.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Client_libcurl_h_*/
