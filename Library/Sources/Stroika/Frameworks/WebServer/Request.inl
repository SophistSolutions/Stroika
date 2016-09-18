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
            inline  String  Request::GetHTTPVersion () const
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                return fHTTPVersion;
            }
            inline  String  Request::GetHTTPMethod () const
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                return fMethod;
            }
            inline  IO::Network::URL    Request::GetURL () const
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                return fURL;
            }
            inline  Mapping<String, String> Request::GetHeaders () const
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec { *this };

                return fHeaders;
            }
            inline  Streams::InputStream<Memory::Byte>  Request::GetInputStream ()
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec { *this };

                return fInputStream;
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_Request_inl_*/
