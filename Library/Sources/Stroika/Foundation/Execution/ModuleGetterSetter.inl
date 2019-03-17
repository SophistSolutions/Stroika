/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleGetterSetter_inl_
#define _Stroika_Foundation_Execution_ModuleGetterSetter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************ ModuleGetterSetter<T, IMPL> ***************************
     ********************************************************************************
     */
    template <typename T, typename IMPL>
    inline T ModuleGetterSetter<T, IMPL>::Get ()
    {
        {
            // most of the time, the value will have already been initialized, so use a readlock
            typename RWSynchronized<optional<IMPL>>::ReadableReference l = fIndirect_.cget ();
            if (l->has_value ()) {
                return l.cref ()->Get (); // IMPL::Get () must be const method
            }
        }
        typename RWSynchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
        if (not l->has_value ()) {
            DoInitOutOfLine_ (&l);
        }
        return l.cref ()->Get (); // IMPL::Get () must be const method
    }
    template <typename T, typename IMPL>
    inline void ModuleGetterSetter<T, IMPL>::Set (const T& v)
    {
        typename RWSynchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
        if (not l->has_value ()) {
            DoInitOutOfLine_ (&l);
        }
        l.rwref ()->Set (v);
    }
    template <typename T, typename IMPL>
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
    template <typename T, typename IMPL>
    dont_inline void ModuleGetterSetter<T, IMPL>::DoInitOutOfLine_ (typename RWSynchronized<optional<IMPL>>::WritableReference* ref)
    {
        RequireNotNull (ref);
        Require (not ref->load ().has_value ());
        *ref = IMPL{};
        Ensure (ref->load ().has_value ());
    }

}

#endif /*_Stroika_Foundation_Execution_ModuleGetterSetter_inl_*/
