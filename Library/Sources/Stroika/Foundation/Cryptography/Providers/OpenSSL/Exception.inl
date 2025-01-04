/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL {

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

}
