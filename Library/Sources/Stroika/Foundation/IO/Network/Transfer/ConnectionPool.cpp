/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Containers/Collection.h"
#include "../../../Execution/ConditionVariable.h"

#include "ConnectionPool.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::Transfer;

namespace {
    using Ptr     = Connection::Ptr;
    using Options = Connection::Options;

    /**
     *  Dynamically wrap this around a connection pool entry, so that when its destroyed, it returns
     *  the underlying entry to the pool
     */
    struct Rep_ : Connection::IRep, enable_shared_from_this<Rep_> {

        Ptr                               fDelegateTo;
        function<void (shared_ptr<Rep_>)> fDeleter; // call on delete

        Rep_ (const Ptr& delegateTo, function<void (shared_ptr<Rep_>)> deleter)
            : fDelegateTo (delegateTo)
            , fDeleter (deleter)
        {
        }
        Rep_ (const Rep_&) = delete;
        virtual ~Rep_ ()
        {
            fDeleter (shared_from_this ());
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
    Options fOptions;

    Rep_ (const Options& options)
        : fOptions (options)
    {
    }
    Connection::Ptr New (const optional<Time::Duration>& timeout, optional<URI> hint, optional<AllocateGloballyIfTimeout> allocateGloballyOnTimeoutFlag)
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

    // Use a collection instead of an LRUCache, because the 'key' can change as the connection is used, and this
    // isn't handled well by LRUCache code (moving buckets etc automatically). And it doesn't handle the case
    // where there is no URL/hint
    Collection<Connection::Ptr> fAvailableConnections;
    unsigned int                fOutstandingConnections; // # connections handed out : this number + fAvailableConnections.size () must be less_or_equal to fOptions.GetMaxConnections - but
                                                         // then don't actually allocate extra connections until/unless needed
    ConditionVariable<> fAvailableConnectionsChanged;
};

/*
 ********************************************************************************
 ************************** Transfer::ConnectionPool ****************************
 ********************************************************************************
 */
ConnectionPool::ConnectionPool (const Options& options)
    : fRep_ (make_unique<Rep_> (options))
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
