/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Connection_inl_
#define _Stroika_Frameworks_WebServer_Connection_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Containers/Common.h"

namespace   Stroika {
    namespace   Frameworks  {
        namespace   WebServer {

            // class    Connection
            inline  Socket  Connection::GetSocket () const      { return fSocket_; }
            inline  const Request&  Connection::GetRequest () const { return fRequest_; }
            inline  Request&    Connection::GetRequest () { return fRequest_; }
            inline  Response&   Connection::GetResponse () { return fResponse_; }
            inline  const Response& Connection::GetResponse () const { return fResponse_; }

        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_Connection_inl_*/
