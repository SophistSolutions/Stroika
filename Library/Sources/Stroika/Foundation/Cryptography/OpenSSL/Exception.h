/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_Exception_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_Exception_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Execution/StringException.h"
#include "../../Memory/Common.h"

/**
 *  \file
 *
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Cryptography {
            namespace OpenSSL {

#if qHasFeature_OpenSSL
                /**
                 */
                class Exception : public Execution::StringException {
                public:
                    using InternalErrorCodeType = unsigned long;

                public:
                    /**
                     */
                    Exception (InternalErrorCodeType errorCode);

                public:
                    /**
                     */
                    nonvirtual InternalErrorCodeType GetErrorCode () const;

                public:
                    /**
                     */
                    static Characters::String GetMessage (InternalErrorCodeType errorCode);

                public:
                    /**
                     * ThrowLastErrorIfFailed throws if status is not = 1
                     */
                    static void ThrowLastErrorIfFailed (int status);

                public:
                    /**
                     * ThrowLastError () throws error in ERR_get_error
                     */
                    static void ThrowLastError ();

                public:
                    /**
                     */
                    static bool GetNamesSupported ();

                public:
                    /**
                     */
                    static void SetNamesSupported (bool openSSLStringsSupported);

                private:
                    InternalErrorCodeType fErrorCode_;
                };
#endif
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_Exception_h_*/
