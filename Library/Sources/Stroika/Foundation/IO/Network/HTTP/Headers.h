/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Headers_h_
#define _Stroika_Foundation_IO_Network_HTTP_Headers_h_  1

#include    "../../../StroikaPreComp.h"

#include    <map>
#include    <string>

#include    "../../../Configuration/Common.h"


/*
 * TODO:
 *      (o)     When we have a good C++ 'static string' class - maybe use that here. Maybe ONLY can do once we have compiler constexpr support?
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   HTTP {

                    // standard HTTP headers one might want to access/retrieve
                    namespace   HeaderName {

                        extern  const   wchar_t kContentType[];
                        extern  const   wchar_t kContentLength[];
                        extern  const   wchar_t kServer[];
                        extern  const   wchar_t kDate[];
                        extern  const   wchar_t kLastModified[];
                        extern  const   wchar_t kUserAgent[];
                        extern  const   wchar_t kSOAPAction[];
                        extern  const   wchar_t kAcceptEncoding[];
                    }

                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_HTTP_Headers_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Headers.inl"
