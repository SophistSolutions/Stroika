/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <algorithm>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/Throw.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************** CallbackRegistry<...>::ID ***************************
     ********************************************************************************
     */
    template <typename... ARGS>
    constexpr CallbackRegistry<void (ARGS...)>::ID::ID (uint64_t id)
        : fID_{id}
    {
    }
    template <typename... ARGS>
    inline Characters::String CallbackRegistry<void (ARGS...)>::ID::ToString () const
    {
        return Characters::ToString (fID_);
    }

    /*
     ********************************************************************************
     ************************** Execution::CallbackRegistry *************************
     ********************************************************************************
     */
    template <typename... ARGS>
    inline CallbackRegistry<void (ARGS...)>::Entry_::Entry_ (ID id, const Callback& callback)
        : fID{id}
        , fCallback{callback}
    {
    }
    template <typename... ARGS>
    inline CallbackRegistry<void (ARGS...)>::~CallbackRegistry ()
    {
        if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
            [[maybe_unused]] lock_guard critSec{fMutex_};
            for ([[maybe_unused]] const shared_ptr<Entry_>& e : fEntries_) {
                Require (e->fRunningOn.empty ()); // else a callback is running, on a registry going away
            }
        }
    }
    template <typename... ARGS>
    auto CallbackRegistry<void (ARGS...)>::Add (const Callback& callback) -> ID
    {
        Require (callback != nullptr);
        auto e = Memory::MakeSharedPtr<Entry_> (ID{++Private_::sCallbackRegistryLastID_}, callback);

        [[maybe_unused]] lock_guard critSec{fMutex_};
        fEntries_.push_back (e);
        return e->fID;
    }
    template <typename... ARGS>
    void CallbackRegistry<void (ARGS...)>::Remove (ID id) noexcept
    {
        unique_lock critSec{fMutex_};
        auto        i = find_if (fEntries_.begin (), fEntries_.end (), [&] (const shared_ptr<Entry_>& e) { return e->fID == id; });
        if (i == fEntries_.end ()) {
            return; // not added (or already removed)
        }
        shared_ptr<Entry_> e = *i;
        fEntries_.erase (i);
        e->fRemoved = true; // so a Call that copied the list before this calls it no more
        // and wait for a call of it running on another thread - not this one's: that is the callback removing itself
        const thread::id me = this_thread::get_id ();
        fCallEnded_.wait (critSec,
                          [&] () { return all_of (e->fRunningOn.begin (), e->fRunningOn.end (), [&] (thread::id t) { return t == me; }); });
    }
    template <typename... ARGS>
    void CallbackRegistry<void (ARGS...)>::RemoveAll () noexcept
    {
        unique_lock                critSec{fMutex_};
        vector<shared_ptr<Entry_>> removed;
        removed.swap (fEntries_);
        for (const shared_ptr<Entry_>& e : removed) {
            e->fRemoved = true;
        }
        // and wait for any of them running on another thread
        const thread::id me = this_thread::get_id ();
        fCallEnded_.wait (critSec, [&] () {
            return all_of (removed.begin (), removed.end (), [&] (const shared_ptr<Entry_>& e) {
                return all_of (e->fRunningOn.begin (), e->fRunningOn.end (), [&] (thread::id t) { return t == me; });
            });
        });
    }
    template <typename... ARGS>
    void CallbackRegistry<void (ARGS...)>::Call (ARGS... args)
    {
        using namespace Characters::Literals;
        vector<shared_ptr<Entry_>> entries;
        {
            [[maybe_unused]] lock_guard critSec{fMutex_};
            entries = fEntries_; // a copy, to call holding no lock: a callback may add or remove callbacks
        }
        const thread::id me = this_thread::get_id ();
        for (const shared_ptr<Entry_>& e : entries) {
            {
                [[maybe_unused]] lock_guard critSec{fMutex_};
                if (e->fRemoved) {
                    continue; // removed since the copy was made
                }
                e->fRunningOn.push_back (me);
            }
            [[maybe_unused]] auto&& cleanup = Finally ([&] () noexcept {
                {
                    [[maybe_unused]] lock_guard critSec{fMutex_};
                    e->fRunningOn.erase (find (e->fRunningOn.begin (), e->fRunningOn.end (), me));
                }
                fCallEnded_.notify_all (); // for a Remove waiting for it
            });
            try {
                e->fCallback (args...);
            }
            catch (const Thread::AbortException&) {
                ReThrow ();
            }
            catch (...) {
                DbgTrace ("CallbackRegistry: ignoring exception from a callback: {}"_f, current_exception ());
            }
        }
    }

}
