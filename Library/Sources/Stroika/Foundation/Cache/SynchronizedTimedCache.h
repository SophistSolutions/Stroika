/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedTimedCache_h_
#define _Stroika_Foundation_Cache_SynchronizedTimedCache_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#if !qCompilerAndStdLib_shared_mutex_module_Buggy
#include <shared_mutex>
#endif

#include "TimedCache.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo   HIGHLY EXPERIEMNTAL FIRST DRAFT OF IDEA
 */

namespace Stroika {
    namespace Foundation {
        namespace Cache {

            /**
             */
            template <typename KEY, typename VALUE, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
            class SynchronizedTimedCache : public TimedCache<KEY, VALUE, TRAITS> {
                using inherited = TimedCache<KEY, VALUE, TRAITS>;

            public:
                SynchronizedTimedCache (Time::DurationSecondsType timeoutInSeconds)
                    : inherited (timeoutInSeconds)
                {
                }
                // support eventually, but not trivial
                SynchronizedTimedCache (const SynchronizedTimedCache&) = delete;

                // @todo need much more but this is atart
            public:
                nonvirtual VALUE Lookup (typename Configuration::ArgByValueType<KEY> key, const std::function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller)
                {
#if qCompilerAndStdLib_shared_mutex_module_Buggy
                    lock_guard<mutex> lock (this->fMutex_);
#else
                    shared_lock<shared_timed_mutex> lock (this->fMutex_);
#endif
                    if (Memory::Optional<VALUE> o = inherited::Lookup (key)) {
                        return *o;
                    }
                    else {
                        VALUE v = cacheFiller (key);
                        {
#if !qCompilerAndStdLib_shared_mutex_module_Buggy
                            lock.unlock ();
                            unique_lock<shared_timed_mutex> newRWLock (this->fMutex_);
#endif
                            this->Add (key, v);
                        }
                        return move (v);
                    }
                }

            public:
                // support eventually, but not trivial
                nonvirtual SynchronizedTimedCache& operator= (const SynchronizedTimedCache&) = delete;

            private:
#if qCompilerAndStdLib_shared_mutex_module_Buggy
                mutable mutex fMutex_;
#else
                mutable shared_timed_mutex          fMutex_;
#endif
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SynchronizedTimedCache.inl"

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_h_*/
