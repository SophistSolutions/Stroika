/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_Server_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_Server_h_ 1

#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/String.h"
#include    "../../../../Foundation/Configuration/Common.h"


/*
 * TODO:
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace SSDP {
                namespace Server {


                    class   Server {
                        //...
                        //Get/Set supported DeviceEntries ();

                        //Get/Set Refresh/MaxAge (default is autocompute refresh pace based on maxage)

                        // smart ptr to one of these - caller keeps it around, it runs in its own
                        // thread as needed, does responses etc.
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
#include    "Server.inl"

#endif  /*_Stroika_Frameworks_UPnP_SSDP_Server_Server_h_*/
