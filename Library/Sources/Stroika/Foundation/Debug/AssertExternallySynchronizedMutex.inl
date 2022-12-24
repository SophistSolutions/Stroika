/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_inl_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <algorithm> // for std::count/std::distance

#include "Sanitizer.h"

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     *********** Debug::AssertExternallySynchronizedMutex::SharedContext ************
     ********************************************************************************
     */
#if qDebug
    inline AssertExternallySynchronizedMutex::SharedContext ::~SharedContext ()
    {
        Assert (fFullLocks_ == 0);
        Assert (fSharedLockThreads_.empty ());
    }
    inline bool AssertExternallySynchronizedMutex ::SharedContext ::GetSharedLockEmpty_ () const
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        return fSharedLockThreads_.empty ();
    }
    inline pair<size_t, size_t> AssertExternallySynchronizedMutex ::SharedContext ::CountSharedLockThreads_ () const
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        size_t            thisThreadCnt  = std::count (fSharedLockThreads_.begin (), fSharedLockThreads_.end (), this_thread::get_id ());
        size_t            otherThreadCnt = std::distance (fSharedLockThreads_.begin (), fSharedLockThreads_.end ()) - thisThreadCnt;
        return make_pair (thisThreadCnt, otherThreadCnt);
    }
    inline size_t AssertExternallySynchronizedMutex ::SharedContext ::GetSharedLockThreadsCount_ () const
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        return std::distance (fSharedLockThreads_.begin (), fSharedLockThreads_.end ());
    }
    inline size_t AssertExternallySynchronizedMutex ::SharedContext ::CountOfIInSharedLockThreads_ (thread::id i) const
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        return std::count (fSharedLockThreads_.begin (), fSharedLockThreads_.end (), i);
    }
    inline void AssertExternallySynchronizedMutex ::SharedContext ::AddSharedLock_ (thread::id i)
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        fSharedLockThreads_.push_front (i);
    }
    inline void AssertExternallySynchronizedMutex ::SharedContext ::RemoveSharedLock_ (thread::id i)
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        fSharedLockThreads_.remove (i);
    }
#endif

    /*
     ********************************************************************************
     **************** Debug::AssertExternallySynchronizedMutex **********************
     ********************************************************************************
     */
#if qDebug
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext) noexcept
        // https://stroika.atlassian.net/browse/STK-500
        // NOTE - this will generate a throw and std::unexpected violation if there is no memory and multiset CTOR
        // throws. There is no good answer in this case. We declare the constructors noexcept so the footprint of
        // AssertExternallySynchronizedMutex is as light as possible and the same (API/constraints) between debug and release
        // builds. And if we run out of memory here, there isn't much we can do to continue -- LGP 2018-10-02
        : fSharedContext_{sharedContext ? sharedContext : make_shared<SharedContext> ()}
    {
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext, const AssertExternallySynchronizedMutex& src) noexcept
        : AssertExternallySynchronizedMutex{sharedContext}
    {
        ReadContext readLockSrc{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const AssertExternallySynchronizedMutex& src) noexcept
        : AssertExternallySynchronizedMutex{}
    {
        ReadContext readLockSrc{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext, [[maybe_unused]] AssertExternallySynchronizedMutex&& src) noexcept
        : AssertExternallySynchronizedMutex{sharedContext}
    {
        WriteContext declareWriteContext4Src{src}; // move we must be able to modify source
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex ([[maybe_unused]] AssertExternallySynchronizedMutex&& src) noexcept
        : AssertExternallySynchronizedMutex{}
    {
        WriteContext writeLockRHS{src}; // move we must be able to modify source
    }
#endif
    inline AssertExternallySynchronizedMutex& AssertExternallySynchronizedMutex::operator= ([[maybe_unused]] const AssertExternallySynchronizedMutex& rhs) noexcept
    {
        ReadContext  readLockRHS{rhs};     // we must be able to read RHS
        WriteContext declareWriteContext4This{*this}; // we must be able modify this
        return *this;
    }
    inline AssertExternallySynchronizedMutex& AssertExternallySynchronizedMutex::operator= ([[maybe_unused]] AssertExternallySynchronizedMutex&& rhs) noexcept
    {
        WriteContext writeLockRHS{rhs};    // move we must be able to modify rhs to move it
        WriteContext writeLockThis{*this}; // we must be able modify this
        return *this;
    }
#if qDebug
    inline auto AssertExternallySynchronizedMutex::GetSharedContext () const -> shared_ptr<SharedContext>
    {
        return fSharedContext_;
    }
    inline void AssertExternallySynchronizedMutex::SetAssertExternallySynchronizedMutexContext (const shared_ptr<SharedContext>& sharedContext)
    {
        Require (sharedContext != nullptr);
        fSharedContext_ = sharedContext;
    }
#endif
    inline void AssertExternallySynchronizedMutex::lock () noexcept
    {
#if qDebug
        lock_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::unlock () noexcept
    {
#if qDebug
        unlock_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::lock_shared () const noexcept
    {
#if qDebug
        lock_shared_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::unlock_shared () const noexcept
    {
#if qDebug
        unlock_shared_ ();
#endif
    }

}

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_inl_*/
