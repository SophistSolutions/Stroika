/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "ConnectionPool.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

namespace {
    using Ptr     = Connection::Ptr;
    using Options = Connection::Options;

    struct Rep_ : Connection::IRep {

        Ptr fDelegateTo;

        Rep_ (const Ptr& delegateTo)
            : fDelegateTo (delegateTo)
        {
        }
        Rep_ (const Rep_&) = delete;
        virtual ~Rep_ ()
        {
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    public:
        virtual Options GetOptions () const override
        {
            return fDelegateTo.GetOptions ();
        }
        virtual DurationSecondsType GetTimeout () const override
        {
            return fDelegateTo.GetTimeout ();
        }
        virtual void SetTimeout (DurationSecondsType timeout) override
        {
            fDelegateTo.SetTimeout (timeout);
        }
        virtual URI DeprecatedGetAuthorityRelativeURL () const override
        {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_START (4996);
            return fDelegateTo.GetURL ().GetAuthorityRelativeResource ();
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_END (4996);
        }
        virtual void DeprecatedSetAuthorityRelativeURL (const URI& url) override
        {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_START (4996);
            fDelegateTo.SetURL (fDelegateTo.GetURL ().Combine (url));
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
            DISABLE_COMPILER_MSC_WARNING_END (4996);
        }
        virtual URI GetSchemeAndAuthority () const override
        {
            return fDelegateTo.GetSchemeAndAuthority ();
        }
        virtual void SetSchemeAndAuthority (const URI& schemeAndAuthority) override
        {
            fDelegateTo.SetSchemeAndAuthority (schemeAndAuthority);
        }
        virtual void Close () override
        {
            fDelegateTo.Close ();
        }
        virtual Response Send (const Request& request) override
        {
            return fDelegateTo.Send (request);
        }
    };
}

/*
 ********************************************************************************
 ************************ Transfer::ConnectionPool::Rep_ ************************
 ********************************************************************************
 */
class ConnectionPool::Rep_ {
public:
    Rep_ (size_t maxConnections, const Connection::Options& optionsForEachConnection)
    {
    }
    Connection::Ptr New (const optional<Time::Duration>& timeout, optional<URI> hint, optional<AllocateGloballyIfTimeout> allocateGloballyFlag)
    {
        // @todo - real caching impl... including throwing away old items
        // MAYBE add ConnectionPool::Options class - probably - cuz many options
        // maxConnections, optionsForEachConnection, more???

        // how todo?

        // Keep conventional LRU cache
        // REMOVE item from LRU Cache when 'allocated' (in use)
        // Put back (and recently used) when done with use
        // need a condition variable when cache changed. When added and then
        // if you have a timeout, and cannot find answer on first try wait on that condition variable and recheck on each wakeup.
        // then if you finally wakeup and time expired look at expiry policy and throw or return global new connection.

        // maybe make this AllocateGloballyIfTimeout policy part of CONNECTION POOL OPTIONS - instead of parameter? Or document
        // why not. I THINK it may depend on CALL point what you want to do and you may want to re-use the smae connection pool
        // for differnet purposes, and for some using a global makes sense (critical) and others not (advisory/unimportant).
        return Connection::New (); //tmphack
    }
};

/*
 ********************************************************************************
 ************************** Transfer::ConnectionPool ****************************
 ********************************************************************************
 */
ConnectionPool::ConnectionPool (size_t maxConnections, const Connection::Options& optionsForEachConnection)
    : fRep_ (make_unique<Rep_> (maxConnections, optionsForEachConnection))
{
}

Connection::Ptr ConnectionPool::New (URI hint)
{
    return fRep_->New (nullopt, hint, nullopt);
}

Connection::Ptr ConnectionPool::New (const Time::Duration& timeout, URI hint)
{
    return fRep_->New (timeout, hint, nullopt);
}

Connection::Ptr ConnectionPool::New (AllocateGloballyIfTimeout, const Time::Duration& timeout, URI hint)
{
    return fRep_->New (timeout, hint, AllocateGloballyIfTimeout::eAllocateGloballyIfTimeout);
}
