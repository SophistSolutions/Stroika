/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL {

#if qStroika_HasComponent_OpenSSL
    /*
     ********************************************************************************
     *************************** OpenSSL::Exception *********************************
     ********************************************************************************
     */
    inline void Exception::ThrowLastErrorIfFailed (int status)
    {
        if (status == 0) [[unlikely]] {
            ThrowLastError ();
        }
    }
#endif

}
