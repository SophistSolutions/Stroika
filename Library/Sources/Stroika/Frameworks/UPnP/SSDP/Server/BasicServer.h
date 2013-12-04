/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_
#define _Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_ 1

#include    "../../../StroikaPreComp.h"

#include    "../../../../Foundation/Characters/String.h"
#include    "../../../../Foundation/Configuration/Common.h"

#include    "../../Device.h"
#include    "PeriodicNotifier.h"
#include    "SearchResponder.h"


/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Look at http://brisa.garage.maemo.org/doc/html/upnp/ssdp.html for example server API
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace SSDP {
                namespace Server {


                    /*
                     *  When this object is instantiated, it fires off threads to notify and respond to
                     *  searches. When it is destroyed, it stops doing that.
                    */
                    class   BasicServer {
                    public:
                        typedef PeriodicNotifier::FrequencyInfo FrequencyInfo;

                    public:
                        BasicServer (const BasicServer&) = delete;
                        BasicServer (const Device& d, const FrequencyInfo& fi);
                        const BasicServer& operator= (const BasicServer&) = delete;

                    private:
                        class   Rep_;
                        shared_ptr<Rep_>    fRep_;
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
#include    "BasicServer.inl"

#endif  /*_Stroika_Frameworks_UPnP_SSDP_Server_BasicServer_h_*/
