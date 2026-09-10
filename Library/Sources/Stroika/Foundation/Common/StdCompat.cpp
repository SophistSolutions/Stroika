/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>
#include <shared_mutex>

#include "StdCompat.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common::StdCompat;

/*
 ********************************************************************************
 ******************** Checks on the 'named requirement' concepts ****************
 ********************************************************************************
 *
 *  Each concept in StdCompat.h documents which standard types it must, and must not, accept. Those are
 *  asserted here rather than in the header so that the <mutex>/<shared_mutex> needed to name the types
 *  costs ONE translation unit, rather than every one that includes StdCompat.h - which is most of Stroika.
 *
 *  The NEGATIVE cases are the ones with teeth. A concept that accepts everything constrains nothing, and
 *  that failure is invisible until someone instantiates the constrained template with the wrong type.
 */

namespace {
    // Every std lockable type offers lock ()/unlock ()/try_lock () together, so nothing in the standard
    // library separates BasicLockable from Lockable. This does.
    struct OnlyBasicLockable_ {
        void lock ();
        void unlock ();
    };
    // ... and this separates Lockable from TimedLockable without relying on std::mutex staying as it is.
    struct LockableButNotTimed_ {
        void lock ();
        void unlock ();
        bool try_lock ();
    };
}

// The types each concept must accept
static_assert (BasicLockable<mutex> and BasicLockable<recursive_mutex> and BasicLockable<timed_mutex> and BasicLockable<shared_mutex> and
               BasicLockable<unique_lock<mutex>>);
static_assert (Lockable<mutex> and Lockable<recursive_mutex> and Lockable<timed_mutex> and Lockable<shared_mutex>);
static_assert (TimedLockable<timed_mutex> and TimedLockable<recursive_timed_mutex>);
static_assert (TimedMutex<timed_mutex> and TimedMutex<recursive_timed_mutex>);

// ... and that each is STRICTLY STRONGER than the one it refines. Nothing in the standard library can
// check this - every std lockable type offers lock ()/unlock ()/try_lock ()/try_lock_for ()/try_lock_until ()
// as a set - so it takes purpose-built types. Without these, a concept that had collapsed into its base
// would still pass every assertion above.
static_assert (BasicLockable<OnlyBasicLockable_> and not Lockable<OnlyBasicLockable_>);
static_assert (Lockable<LockableButNotTimed_> and not TimedLockable<LockableButNotTimed_>);

// ... and that they are not vacuously true, which a botched requires-clause silently would be
static_assert (not BasicLockable<int> and not TimedMutex<int>);
