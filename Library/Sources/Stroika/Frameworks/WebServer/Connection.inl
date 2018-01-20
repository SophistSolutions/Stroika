/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Connection_inl_
#define _Stroika_Frameworks_WebServer_Connection_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Containers/Common.h"

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            /*
             ********************************************************************************
             ***************************** WebServer::Connection ****************************
             ********************************************************************************
             */
            inline ConnectionOrientedSocket::Ptr Connection::GetSocket () const
            {
                return fSocket_;
            }
            inline const Request& Connection::GetRequest () const
            {
                return *fMessage_.PeekRequest ();
            }
            inline Request& Connection::GetRequest ()
            {
                return *fMessage_.PeekRequest ();
            }
            inline Response& Connection::GetResponse ()
            {
                return *fMessage_.PeekResponse ();
            }
            inline const Response& Connection::GetResponse () const
            {
                return *fMessage_.PeekResponse ();
            }
        }
    }
}
#endif /*_Stroika_Frameworks_WebServer_Connection_inl_*/
