/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************ ModuleGetterSetter<T, IMPL> ***************************
     ********************************************************************************
     */
    template <typename T, IModuleGetterSetterImpl<T> IMPL>
    inline void ModuleGetterSetter<T, IMPL>::AssureLoaded () const
    {
        typename RWSynchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
        if (not l->has_value ()) {
            DoInitOutOfLine_ (&l);
        }
    }
    template <typename T, IModuleGetterSetterImpl<T> IMPL>
    inline T ModuleGetterSetter<T, IMPL>::Get () const
    {
        {
            // most of the time, the value will have already been initialized, so use a readlock
            typename RWSynchronized<optional<IMPL>>::ReadableReference l = fIndirect_.cget ();
            if (l->has_value ()) {
                return l.cref ()->Get (); // IMPL::Get () must be const method
            }
        }
        AssureLoaded ();
        return fIndirect_.load ()->Get ();
    }
    template <typename T, IModuleGetterSetterImpl<T> IMPL>
    inline void ModuleGetterSetter<T, IMPL>::Set (const T& v)
    {
        typename RWSynchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
        if (not l->has_value ()) {
            DoInitOutOfLine_ (&l);
        }
        l.rwref ()->Set (v);
    }
    template <typename T, IModuleGetterSetterImpl<T> IMPL>
    inline shared_ptr<const T> ModuleGetterSetter<T, IMPL>::operator->() const
    {
        return Memory::MakeSharedPtr<const T> (Get ());
    }
    template <typename T, IModuleGetterSetterImpl<T> IMPL>
    optional<T> ModuleGetterSetter<T, IMPL>::Update (const function<optional<T> (const T&)>& updaterFunction)
    {
        /*
         *  Could consider rewriting this to optimisticly use read/shared lock, and upgrade lock if
         *  its found the update caused a change. In fact, using this->Get () and this->Set () would do that,
         *  except for not making update atomic.
         */
        typename RWSynchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
        if (not l->has_value ()) {
            DoInitOutOfLine_ (&l);
        }
        if (auto o = updaterFunction (l.cref ()->Get ())) {
            l.rwref ()->Set (*o);
            return o;
        }
        return {};
    }
    template <typename T, IModuleGetterSetterImpl<T> IMPL>
    dont_inline void ModuleGetterSetter<T, IMPL>::DoInitOutOfLine_ (typename RWSynchronized<optional<IMPL>>::WritableReference* ref)
    {
        RequireNotNull (ref);
        Require (not ref->cref ().has_value ());
        *ref = IMPL{}; // @todo redo with emplace()
        Ensure (ref->cref ().has_value ());
    }

}
