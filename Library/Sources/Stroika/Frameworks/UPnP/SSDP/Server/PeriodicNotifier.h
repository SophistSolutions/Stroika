/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_ 1

#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/String.h"
#include    "../../../../Foundation/Configuration/Common.h"

#include    "../../Device.h"

/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 *
 *      @todo   Did rough draft implementation. Works for some simple cases.
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace SSDP {
                namespace Server {


                    /**
                     *  A big part of SSDP server functinality is to send periodic notifications of the Device info
                     *
                     *
                     *  Could pass in thread or maybe just keep it hidden
                     *
                     */
                    class   PeriodicNotifier {
                    public:

                        PeriodicNotifier ();

                        struct FrequencyInfo {

                        };
                        void    Run (const Device& d, const FrequencyInfo& fi);



#if 0
                        //...
                        //Get/Set supported DeviceEntries ();

                        //Get/Set Refresh/MaxAge (default is autocompute refresh pace based on maxage)

                        // smart ptr to one of these - caller keeps it around, it runs in its own
                        // thread as needed, does responses etc.
#endif
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
#include    "PeriodicNotifier.inl"

#endif  /*_Stroika_Frameworks_UPnP_SSDP_Server_PeriodicNotifier_h_*/
