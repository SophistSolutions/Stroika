/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_CachingListener_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_CachingListener_h_ 1

#include    "../../../StroikaPreComp.h"

#include    "Listener.h"

/*
 * TODO:
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {
                namespace   Client {


                    /**
                     *  Add list of found devices, paying attention to aging rules (max-age)
                     *
                     *  Unclear how to integrate with search stuff (if we find with search, dont lose it, and if we have other reason to know
                     *  device still aroudn dont lose it - like open tcp connection to it).
                     */
                    class   CachingListener : public Listener {
                        //lambda onChange callback;
                        //pays attention to max-age falgs;
                        //maintains list;
                        //public method to getMatchingEntriesFound()
                    };


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
#include    "CachingListener.inl"

#endif  /*_Stroika_Frameworks_UPnP_SSDP_Client_CachingListener_h_*/
