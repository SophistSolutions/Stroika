/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_
#define _Stroika_Frameworks_UPnP_SSDP_Client_Search_h_ 1

#include    "../../../StroikaPreComp.h"

#include    <functional>

#include    "../../Device.h"
#include    "../Advertisement.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   must re-read ssdp spec for exact format of messages!
 *
 *      @todo   Must have some API for how often to send searches, and how long
 *              to listen for responses (maybe forever on responses?)
 *
 *      @todo   Consider adding OnError callback?
 *
 *      @todo   Fix Result object to return other interesting fields
 *
 *      @todo   Consider some synchonous API, so it sends a certian number of times, and
 *              then returns all the answers.
 *
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   UPnP {
            namespace   SSDP {
                namespace   Client {


                    /**
                     */
                    class   Search {
                    public:
                        Search ();
                        Search (const Search&) = delete;

                    public:
                        /**
                         *  Its OK to destroy a searcher while running. It will silently stop the running searcher thread.
                         */
                        ~Search ();

                    public:
                        nonvirtual  const Search& operator= (const Search&) = delete;

                    public:
                        /**
                         *  Using std::function, no way to compare for operator==, so no way to remove.
                         *  @todo    RETHINK!
                         *  Note - the callback will be called on an arbitrary thread, so the callback must be threadsafe.
                         *  This can be done after the listening has started.
                         */
                        void    AddOnFoundCallback (const function<void (const SSDP::Advertisement& d)>& callOnFinds);

                    public:
                        /**
                         *  Starts searcher (probably starts a thread).
                         *  args - ST, strings, uuid etc.
                         *
                         *  If already running, this automatically stops an existing search, and restarts it with
                         *  the given serviceType parameters.
                         */
                        nonvirtual  void    Start (const String& serviceType);

                    public:
                        /**
                         *  Stop an already running search. Not an error to call if not already started (just does nothing).
                         *  This will block until the listner is stopped.
                         */
                        nonvirtual  void    Stop ();

                    private:
                        class Rep_;
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
#include    "Search.inl"

#endif  /*_Stroika_Frameworks_UPnP_SSDP_Client_Search_h_*/
