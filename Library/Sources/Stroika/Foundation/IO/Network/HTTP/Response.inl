/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Response_inl_
#define _Stroika_Foundation_IO_Network_HTTP_Response_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ********************* Framework::WebServer::Response ***************************
     ********************************************************************************
     */
    inline IO::Network::HTTP::Status Response::GetStatus () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fStatus_;
    }

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Response_inl_*/
