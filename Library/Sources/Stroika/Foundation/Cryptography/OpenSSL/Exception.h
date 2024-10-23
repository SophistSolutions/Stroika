/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_Exception_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_Exception_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/Common.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

#if qStroika_HasComponent_OpenSSL
    /**
     */
    class Exception : public Execution::RuntimeErrorException<> {
    private:
        using inherited = Execution::RuntimeErrorException<>;

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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_Exception_h_*/
