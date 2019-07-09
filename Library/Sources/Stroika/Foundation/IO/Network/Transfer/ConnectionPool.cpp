/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Containers/Collection.h"
#include "../../../Execution/ConditionVariable.h"
#include "../../../Execution/TimeOutException.h"

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
    struct DelegatingConnectionRepWithDeleteHook_ : Connection::IRep, enable_shared_from_this<DelegatingConnectionRepWithDeleteHook_> {

        Ptr                                                                 fDelegateTo;
        function<void (shared_ptr<DelegatingConnectionRepWithDeleteHook_>)> fDeleter; // call on delete

        DelegatingConnectionRepWithDeleteHook_ (const Ptr& delegateTo, function<void (shared_ptr<DelegatingConnectionRepWithDeleteHook_>)> deleter)
            : fDelegateTo (delegateTo)
            , fDeleter (deleter)
        {
        }
        DelegatingConnectionRepWithDeleteHook_ (const DelegatingConnectionRepWithDeleteHook_&) = delete;
        virtual ~DelegatingConnectionRepWithDeleteHook_ ()
        {
            fDeleter (shared_from_this ());
        }

    public:
        nonvirtual DelegatingConnectionRepWithDeleteHook_& operator= (const DelegatingConnectionRepWithDeleteHook_&) = delete;

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
    ~Rep_ ()
    {
        Require (fOutstandingConnections == 0); // otherwise when they are destroyed, they will try to add to a list that
                                                // no longer exists...
    }
    Connection::Ptr New (const optional<Time::Duration>& timeout, optional<URI> hint, optional<AllocateGloballyIfTimeout> allocateGloballyOnTimeoutFlag)
    {
        /*
         *  Maintain an LRU(like) list. Not strictly LRU, because we want to walk/pick not arbitrarily, but by URI matching.
         *  This is why we cannot use the Stroika LRUCache class.
         *
         *  Add items dynamically to the list, but never more than the max.
         *
         *  If failed to find a match, wait (if argument given).
         *
         *  If still failed, either throw or allocate new connection (again depending on function argument).
         */
        Time::DurationSecondsType timeoutAt = Time::GetTickCount () + timeout.value_or (0s).As<Time::DurationSecondsType> ();
        optional<Connection::Ptr> poolEntryResult;
    again:
        if (hint) {
            poolEntryResult = FindAndAllocateFromAvailableByURIMatch_ (hint->GetSchemeAndAuthority ());
        }
        if (not poolEntryResult) {
            poolEntryResult = FindAndAllocateFromAvailable_ ();
        }
        if (not poolEntryResult) {
            lock_guard   critSec{fAvailableConnectionsChanged.fMutex};
            unsigned int totalAllocated = fAvailableConnections.size () + fOutstandingConnections;
            if (totalAllocated < fOptions.fMaxConnections) {
                fAvailableConnections += Connection::New (fOptions.fConnectionOptions);
                goto again; // multithreaded, someone else could allocate, or return a better match
                            // no need to notify_all () since we will try again anyhow
            }
        }
        if (not poolEntryResult and Time::GetTickCount () > timeoutAt) {
            // Let's see if we can wait a little
            unique_lock lock (fAvailableConnectionsChanged.fMutex);
            if (fAvailableConnectionsChanged.wait_until (lock, timeoutAt) == cv_status::no_timeout) {
                goto again; // a new one maybe available
            }
        }
        if (not poolEntryResult) {
            // Here the rubber hits the road. We didn't find a free entry so we allocate, or throw
            if (allocateGloballyOnTimeoutFlag) {
                return Connection::New (fOptions.fConnectionOptions);
            }
            else {
                Execution::Throw (TimeOutException::kThe);
            }
        }
        // wrap the connection-ptr in an envelope that will restore the connection to the pool
        return Connection::Ptr{
            make_shared<DelegatingConnectionRepWithDeleteHook_> (
                *poolEntryResult,
                [this] (const shared_ptr<DelegatingConnectionRepWithDeleteHook_>& p) {
                    this->AddConnection_ (p->fDelegateTo);
                })};
    }

    optional<Connection::Ptr> FindAndAllocateFromAvailableByURIMatch_ (const URI& matchScemeAndAuthority)
    {
        lock_guard critSec{fAvailableConnectionsChanged.fMutex};
        for (auto i = fAvailableConnections.begin (); i != fAvailableConnections.end (); ++i) {
            if (i->GetSchemeAndAuthority () == matchScemeAndAuthority) {
                fAvailableConnections.Remove (i);
                ++fOutstandingConnections;
                return *i;
            }
        }
        return nullopt;
    }
    optional<Connection::Ptr> FindAndAllocateFromAvailable_ ()
    {
        lock_guard critSec{fAvailableConnectionsChanged.fMutex};
        for (auto i = fAvailableConnections.begin (); i != fAvailableConnections.end (); ++i) {
            fAvailableConnections.Remove (i);
            ++fOutstandingConnections;
            return *i;
        }
        return nullopt;
    }
    void AddConnection_ (const Connection::Ptr p)
    {
        lock_guard critSec{fAvailableConnectionsChanged.fMutex};
        fAvailableConnections.Add (p);
        --fOutstandingConnections;
        fAvailableConnectionsChanged.notify_all ();
    }

    // Use a collection instead of an LRUCache, because the 'key' can change as the connection is used, and this
    // isn't handled well by LRUCache code (moving buckets etc automatically). And it doesn't handle the case
    // where there is no URL/hint

    Collection<Connection::Ptr> fAvailableConnections;
    unsigned int                fOutstandingConnections{}; // # connections handed out : this number + fAvailableConnections.size () must be less_or_equal to fOptions.GetMaxConnections - but
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
