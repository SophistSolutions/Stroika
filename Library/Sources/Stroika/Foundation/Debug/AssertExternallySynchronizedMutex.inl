/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <algorithm> // for std::count/std::distance

#include "Sanitizer.h"

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     *********** Debug::AssertExternallySynchronizedMutex::SharedContext ************
     ********************************************************************************
     */
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    inline AssertExternallySynchronizedMutex::SharedContext ::~SharedContext ()
    {
        Assert (fFullLocks_ == 0);
        Assert (fSharedLocks_.fOverflowThreads_.empty () and fSharedLocks_.fInitialThreadsSize_ == 0);
    }
    inline bool AssertExternallySynchronizedMutex ::SharedContext ::GetSharedLockEmpty_ () const
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        return fSharedLocks_.fInitialThreadsSize_ == 0 and fSharedLocks_.fOverflowThreads_.empty ();
    }
    inline pair<size_t, size_t> AssertExternallySynchronizedMutex ::SharedContext ::CountSharedLockThreads_ () const
    {
        auto              tid = this_thread::get_id ();
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        size_t            thisThreadCnt = std::count (fSharedLocks_.fInitialThreads_.begin (),
                                                      fSharedLocks_.fInitialThreads_.begin () + fSharedLocks_.fInitialThreadsSize_, tid) and
                               std::count (fSharedLocks_.fOverflowThreads_.begin (), fSharedLocks_.fOverflowThreads_.end (), tid);
        size_t otherThreadCnt = fSharedLocks_.fInitialThreadsSize_ +
                                std::distance (fSharedLocks_.fOverflowThreads_.begin (), fSharedLocks_.fOverflowThreads_.end ());
        otherThreadCnt -= thisThreadCnt;
        return make_pair (thisThreadCnt, otherThreadCnt);
    }
    inline size_t AssertExternallySynchronizedMutex ::SharedContext ::GetSharedLockThreadsCount_ () const
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        return fSharedLocks_.fInitialThreadsSize_ + std::distance (fSharedLocks_.fOverflowThreads_.begin (), fSharedLocks_.fOverflowThreads_.end ());
    }
    inline size_t AssertExternallySynchronizedMutex ::SharedContext ::CountOfIInSharedLockThreads_ (thread::id i) const
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        return std::count (fSharedLocks_.fInitialThreads_.begin (), fSharedLocks_.fInitialThreads_.begin () + fSharedLocks_.fInitialThreadsSize_, i) +
               std::count (fSharedLocks_.fOverflowThreads_.begin (), fSharedLocks_.fOverflowThreads_.end (), i);
    }
    inline void AssertExternallySynchronizedMutex ::SharedContext ::AddSharedLock_ (thread::id i)
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        if (fSharedLocks_.fInitialThreadsSize_ < kInlineSharedLockBufSize_) {
            fSharedLocks_.fInitialThreads_[fSharedLocks_.fInitialThreadsSize_++] = i;
        }
        else {
            fSharedLocks_.fOverflowThreads_.push_front (i);
        }
    }
    inline void AssertExternallySynchronizedMutex ::SharedContext ::RemoveSharedLock_ (thread::id i)
    {
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        if constexpr (kInlineSharedLockBufSize_ != 0) {
            auto re = fSharedLocks_.fInitialThreads_.begin () + fSharedLocks_.fInitialThreadsSize_;
            auto ri = find (fSharedLocks_.fInitialThreads_.begin (), re, i);
            if (ri != re) {
                if (ri + 1 != re) {
                    copy (ri + 1, re, ri); // if test not useful if optimized, but this code mainly used unoptimized and appears to help there
                }
                --fSharedLocks_.fInitialThreadsSize_;
                return;
            }
        }
        auto re = fSharedLocks_.fOverflowThreads_.end ();
        for (auto beforeI = fSharedLocks_.fOverflowThreads_.before_begin ();; ++beforeI) {
            Assert (beforeI != re);
            auto n = beforeI;
            n++;
            Assert (n != re);
            if (*n == i) {
                fSharedLocks_.fOverflowThreads_.erase_after (beforeI);
                return;
            }
        }
        RequireNotReached ();
    }
#endif

    /*
     ********************************************************************************
     **************** Debug::AssertExternallySynchronizedMutex **********************
     ********************************************************************************
     */
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext) noexcept
        // http://stroika-bugs.sophists.com/browse/STK-500
        // NOTE - this will generate a throw and std::unexpected violation if there is no memory and multiset CTOR
        // throws. There is no good answer in this case. We declare the constructors noexcept so the footprint of
        // AssertExternallySynchronizedMutex is as light as possible and the same (API/constraints) between debug and release
        // builds. And if we run out of memory here, there isn't much we can do to continue -- LGP 2018-10-02
        : fSharedContext_{sharedContext ? sharedContext : make_shared<SharedContext> ()}
    {
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>&         sharedContext,
                                                                                 const AssertExternallySynchronizedMutex& src) noexcept
        : AssertExternallySynchronizedMutex{sharedContext}
    {
        ReadContext readLockSrc{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const AssertExternallySynchronizedMutex& src) noexcept
        : AssertExternallySynchronizedMutex{}
    {
        ReadContext readLockSrc{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext,
                                                                                 [[maybe_unused]] AssertExternallySynchronizedMutex&& src) noexcept
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
        ReadContext  readLockRHS{rhs};                // we must be able to read RHS
        WriteContext declareWriteContext4This{*this}; // we must be able modify this
        return *this;
    }
    inline AssertExternallySynchronizedMutex& AssertExternallySynchronizedMutex::operator= ([[maybe_unused]] AssertExternallySynchronizedMutex&& rhs) noexcept
    {
        WriteContext writeLockRHS{rhs};    // move we must be able to modify rhs to move it
        WriteContext writeLockThis{*this}; // we must be able modify this
        return *this;
    }
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
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
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
        lock_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::unlock () noexcept
    {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
        unlock_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::lock_shared () const noexcept
    {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
        lock_shared_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::unlock_shared () const noexcept
    {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
        unlock_shared_ ();
#endif
    }

}
