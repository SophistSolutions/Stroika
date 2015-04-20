/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_Exception_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_Exception_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Execution/StringException.h"
#include    "../../Memory/Common.h"



/**
 *  \file
 *
 *  TODO:
 *
 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace OpenSSL {


#if     qHasFeature_OpenSSL
                /**
                 */
                class   Exception : public Execution::StringException {
                public:
                    using   InternalErrorCodeType = unsigned long;

                public:
                    /**
                     */
                    Exception (InternalErrorCodeType errorCode);

                public:
                    /**
                     */
                    nonvirtual  InternalErrorCodeType   GetErrorCode () const;

                public:
                    /**
                     */
                    static  Characters::String  GetMessage (InternalErrorCodeType errorCode);

                public:
                    /**
                     * DoThrowLastErrorIfFailed throws if status is not = 1
                     */
                    static  void    DoThrowLastErrorIfFailed (int status);

                public:
                    /**
                     * DoThrowLastError () throws error in ERR_get_error
                     */
                    static  void    DoThrowLastError ();

                public:
                    /**
                     */
                    static  bool    GetNamesSupported ();

                public:
                    /**
                     */
                    static  void    SetNamesSupported (bool openSSLStringsSupported);

                private:
                    InternalErrorCodeType   fErrorCode_;
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
#include    "Exception.inl"

#endif  /*_Stroika_Foundation_Cryptography_OpenSSL_Exception_h_*/
