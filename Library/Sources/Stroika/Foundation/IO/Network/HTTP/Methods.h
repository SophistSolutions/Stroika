/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Methods_h_
#define _Stroika_Foundation_IO_Network_HTTP_Methods_h_  1

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


                    // standard HTTP METHODS
                    namespace   Methods {


                        extern  const   wchar_t kGet[];
                        extern  const   wchar_t kPut[];
                        extern  const   wchar_t kPost[];
                        extern  const   wchar_t kDelete[];
                        extern  const   wchar_t kOptions[];


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
#include    "Methods.inl"

#endif  /*_Stroika_Foundation_IO_Network_HTTP_Methods_h_*/
