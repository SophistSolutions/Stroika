/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_h_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_h_  1

#include    "../../../StroikaPreComp.h"

#include    <map>
#include    <string>

#include    "../../../Configuration/Common.h"



/*
 * TODO:
 *      @todo		When we have a good C++ 'static string' class - maybe use that here.
 *					Maybe ONLY can do once we have compiler constexpr support?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   HTTP {


                    // standard HTTP headers one might want to access/retrieve
                    namespace   HeaderName {

#if     qCompilerAndStdLib_constexpr_Buggy
                        extern  const   wchar_t kContentType[13];
                        extern  const   wchar_t kContentLength[15];
                        extern  const   wchar_t kServer[7];
                        extern  const   wchar_t kDate[5];
                        extern  const   wchar_t kLastModified[14];
                        extern  const   wchar_t kUserAgent[11];
                        extern  const   wchar_t kSOAPAction[11];
                        extern  const   wchar_t kAcceptEncoding[16];
                        extern  const   wchar_t kExpect[7];
                        extern  const   wchar_t kTransferEncoding[18];
#else
                        constexpr   wchar_t kContentType[]      =   L"Content-Type";
                        constexpr   wchar_t kContentLength[]    =   L"Content-Length";
                        constexpr   wchar_t kServer[]           =   L"Server";
                        constexpr   wchar_t kDate[]             =   L"Date";
                        constexpr   wchar_t kLastModified[]     =   L"Last-Modified";
                        constexpr   wchar_t kUserAgent[]        =   L"User-Agent";
                        constexpr   wchar_t kSOAPAction[]       =   L"SOAPAction";
                        constexpr   wchar_t kAcceptEncoding[]   =   L"Accept-Encoding";
                        constexpr   wchar_t kExpect[]           =   L"Expect";
                        constexpr   wchar_t kTransferEncoding[] =   L"Transfer-Encoding";
#endif



                    }


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Headers.inl"

#endif  /*_Stroika_Foundation_IO_Network_HTTP_Headers_h_*/
