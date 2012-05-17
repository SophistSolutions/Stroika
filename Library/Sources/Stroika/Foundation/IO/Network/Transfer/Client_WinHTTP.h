/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_
#define _Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_   1

#include    "../../../StroikaPreComp.h"

#include    "Client.h"






/*
 * TODO:
 *      (o)     Very rough draft. This code is completely untested, and really only about 1/2 written. Even the high level
 *              usage (container) pattern is just a draft. Its based on HTTPSupport.cpp code in R4LLib (2012-05-08)
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {
                namespace   Transfer {

#if     qHasFeature_WinHTTP
                    // Just object-slice the smart pointer to get a regular connection object - this is just a factory for
                    // LibCurl connection rep objects
                    class   Connection_WinHTTP : public Connection {
                    public:
                        Connection_WinHTTP ();

                    private:
                        class   Rep_;
                    };
#endif
                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_Transfer_Client_WinHTTP_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Client_WinHTTP.inl"
