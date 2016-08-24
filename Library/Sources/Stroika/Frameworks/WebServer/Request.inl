/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Request_inl_
#define _Stroika_Frameworks_WebServer_Request_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Containers/Common.h"

namespace   Stroika {
    namespace   Frameworks  {
        namespace   WebServer {


            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            inline  Memory::Optional<IO::Network::SocketAddress> Request::GetPeerAddress () const
            {
                return fPeerAddress_;
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_Request_inl_*/
