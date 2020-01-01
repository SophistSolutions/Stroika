/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Connection_libcurl_h_
#define _Stroika_Foundation_IO_Network_Transfer_Connection_libcurl_h_ 1

#include "../../../StroikaPreComp.h"

#include <system_error>

#if qHasFeature_LibCurl
// For CURLCode define
#include <curl/curl.h>
#endif

#include "../../../Execution/Exceptions.h"

#include "Connection.h"

/*
 * TODO:
 *      @todo   Consider if curl global init/global_free stuff safe (look at curl code). May run into issue if other
 *              libraries using curl (like CurlNetAccessor::initCurl in xerces) do the same thing.
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
     *  'Connection_LibCurl' is a quasi-namespace - just for the 'libcurl' based Connection factory
     */
    class Connection_LibCurl : public Connection {
    public:
        /**
         */
        static Connection::Ptr New (const Options& options = {});

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
#include "Connection_libcurl.inl"

#endif /*_Stroika_Foundation_IO_Network_Transfer_Connection_libcurl_h_*/
