/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Interceptor_h_
#define _Stroika_Framework_WebServer_Interceptor_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Debug/AssertExternallySynchronizedMutex.h"

#include "Message.h"

/*
 */

/*
 * TODO:
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    /**
     *  \note   Inspired by, but fairly different from
     *          @see https://cxf.apache.org/javadoc/latest/org/apache/cxf/interceptor/Interceptor.html
     * 
     *  The basic idea of an interceptor, is that its a handler for web-server requests/messages. These get arranged
     *  in a chain, and each gets a crack at doing something to an incoming message, preparing its
     *  response.
     * 
     *  Router, fault handlers, and logging are all achieved via interceptors (i.e. the Router IS an Interceptor).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     *              But note that HandleMessage() is a const method, so it can safely be called from any number of threads
     *              simultaneously.
     */
    class Interceptor {
    protected:
        class _IRep;

    public:
        /**
         *  \nb: avoid noexcept in funciton<> declaration due to https://stackoverflow.com/questions/41293025/stdfunction-with-noexcept-in-c17
         */
        Interceptor ()                   = delete;
        Interceptor (const Interceptor&) = default;
        Interceptor (Interceptor&&)      = default;
        Interceptor (const function<void (Message*)>& handleMessage, const function<void (Message*, const exception_ptr& e)>& handleFault = nullptr);

    protected:
        Interceptor (const shared_ptr<_IRep>& rep);

    public:
        nonvirtual Interceptor& operator= (const Interceptor&) = default;

    public:
        /**
         *  Called any interceptor which HandleMessage was invoked on, when a fault prevented completion of
         *  the interceptor chain.
         *
         *  This function should NOT throw an exception - just do what it can to cleanup.
         */
        nonvirtual void HandleFault (Message* m, const exception_ptr& e) const noexcept;

    public:
        /**
         *  Intercepts and handles a message. Typically this will read stuff from the Request and
         *  add stuff to the Response.
         */
        nonvirtual void HandleMessage (Message* m) const;

    public:
        /**
         */
        nonvirtual void CompleteNormally (Message* m) const;

    public:
        /**
         *  Two interceptors are equal if they have the same address, or are copies of one another by copy constructor or assignment.
         */
        nonvirtual bool operator== (const Interceptor& rhs) const;

    protected:
        /**
         */
        template <typename T = _IRep>
        nonvirtual const T& _GetRep () const;

    private:
        shared_ptr<_IRep>                                              fRep_;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

    private:
        class MyRep_;
    };

    /**
     *  \note   Each Interceptor::_IRep must be internally synchronized, as it may be called concurrently from different threads,
     *          for different messages.
     */
    class Interceptor::_IRep {
    public:
        virtual ~_IRep () = default;

        /**
         *  Called any interceptor which HandleMessage was invoked on, when a fault prevented completion of
         *  the interceptor chain.
         *
         *  This function should NOT throw an exception - just do what it can to cleanup.
         */
        virtual void HandleFault ([[maybe_unused]] Message* m, [[maybe_unused]] const exception_ptr& e) const noexcept;

        /**
         *  Intercepts and handles a message. Typically this will read stuff from the Request and
         *  add write to the Response.
         */
        virtual void HandleMessage (Message* m) const = 0;

        /**
         * Rarely overriden, but can be to get a notification
         * about message a second time. Used when you do something in Handle message going one way though the interceptor
         * chain and need todo a follow up on the way back towards the start of the interceptor chain.
         *
         * EG. for logging.
         */
        virtual void CompleteNormally (Message* m) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Interceptor.inl"

#endif /*_Stroika_Framework_WebServer_Interceptor_h_*/
