/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleGetterSetter_inl_
#define _Stroika_Foundation_Execution_ModuleGetterSetter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation {
    namespace Execution {

        /*
            ********************************************************************************
            ************************ ModuleGetterSetter<T, IMPL> ***************************
            ********************************************************************************
            */
        template <typename T, typename IMPL>
        inline T ModuleGetterSetter<T, IMPL>::Get ()
        {
            typename Synchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
            if (not l->has_value ()) {
                DoInitOutOfLine_ (&l);
            }
            return l.cref ()->Get (); // IMPL::Get () must be const method
        }
        template <typename T, typename IMPL>
        inline void ModuleGetterSetter<T, IMPL>::Set (const T& v)
        {
            typename Synchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
            if (not l->has_value ()) {
                DoInitOutOfLine_ (&l);
            }
            l.rwref ()->Set (v);
        }
        template <typename T, typename IMPL>
        optional<T> ModuleGetterSetter<T, IMPL>::Update (const function<optional<T> (const T&)>& updaterFunction)
        {
            typename Synchronized<optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
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
        dont_inline void ModuleGetterSetter<T, IMPL>::DoInitOutOfLine_ (typename Synchronized<optional<IMPL>>::WritableReference* ref)
        {
            RequireNotNull (ref);
            Require (not ref->load ().has_value ());
            *ref = IMPL{};
            Ensure (ref->load ().has_value ());
        }
    }
}
#endif /*_Stroika_Foundation_Execution_ModuleGetterSetter_inl_*/
