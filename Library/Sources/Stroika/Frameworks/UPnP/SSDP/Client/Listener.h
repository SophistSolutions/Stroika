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
 *      @todo   Consider adding OnError callback?
 *
 *      @todo   Fix Result object to return other interesting fields
 *
 *      @todo   Switch to new Stroika containers (e.g. for map<>) when ready.
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {
                namespace   Client {


                    /**
                     *  The SSDP Listener object will listen for SSDP 'muticast' messages, and call any designated callbacks
                     *  with the values in those SSDP muticast 'NOTIFY' messages.
                     */
                    class   Listener {
                    public:
                        class Result;
                    public:
                        Listener ();

                    public:
                        NO_COPY_CONSTRUCTOR (Listener);
                        NO_ASSIGNMENT_OPERATOR (Listener);

                    public:
                        /**
                         *  Its OK to destroy a listener while running. It will silently stop the running listner thread.
                         */
                        ~Listener ();

                    public:
                        /**
                         *  Using std::function, no way to compare for operator==, so no way to remove.
                         *  @todo    RETHINK!
                         *  Note - the callback will be called on an arbitrary thread, so the callback must be threadsafe.
                         *  This can be done after the listening has started.
                         */
                        void    AddOnFoundCallback (const std::function<void(const Result& d)>& callOnFinds);

                    public:
                        /**
                         *  Starts listener (probably starts a thread).
                         *  \req not already started.
                         */
                        nonvirtual  void    Start ();

                    public:
                        /**
                         *  Stop an already running listener. Not an error to call if not already started (just does nothing).
                         *  This will block until the listner is stopped.
                         */
                        nonvirtual  void    Stop ();

                    private:
                        class Rep_;
                        shared_ptr<Rep_>    fRep_;
                    };


                    /**
                     */
                    class Listener::Result : public Device {
                    public:

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
