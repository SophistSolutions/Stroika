/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_ 1

#include    "../../../StroikaPreComp.h"

#include    <functional>

#include    "../../Device.h"

/**
 * TODO:
 *      @todo   Lots of rethinking needed for thread  management, and CTOR arg for search, or jsut Start()/Stop() methods?
 *              But in background, wehnw estart  - we want to use shared_ptr<> rep, and thread in private area todo search
 *              and document callback can be called back on any thread
 *
 *		@todo	Must do muticast group add here!!! to listen to multicast messages
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {
                namespace   Client {


                    /**
                     */
                    class   Listener {
                    public:
                        Listener (const std::function<void(const Device& d)>& callOnFinds);
                    public:
                        NO_DEFAULT_CONSTRUCTOR (Listener);
                        NO_COPY_CONSTRUCTOR (Listener);
                        NO_ASSIGNMENT_OPERATOR (Listener);

                    private:
                        class Rep_;
                        shared_ptr<Rep_>    fRep_;
                    };


                }
            }
        }
    }
}
#endif  /*_Stroika_Frameworks_UPnP_SSDP_Client_Listener_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Listener.inl"
