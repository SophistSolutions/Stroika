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

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ************************ ModuleGetterSetter<T, IMPL> ***************************
             ********************************************************************************
             */
            template <typename T, typename IMPL>
            inline T ModuleGetterSetter<T, IMPL>::Get ()
            {
                typename Synchronized<Memory::Optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
                if (l->IsMissing ()) {
                    DoInitOutOfLine_ (&l);
                }
                return l.cref ()->Get (); // IMPL::Get () must be const method
            }
            template <typename T, typename IMPL>
            inline void ModuleGetterSetter<T, IMPL>::Set (const T& v)
            {
                typename Synchronized<Memory::Optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
                if (l->IsMissing ()) {
                    DoInitOutOfLine_ (&l);
                }
                l.rwref ()->Set (v);
            }
            template <typename T, typename IMPL>
            void ModuleGetterSetter<T, IMPL>::SynchonizedUpdate (const function<T (T)>& updaterFunction)
            {
                typename Synchronized<Memory::Optional<IMPL>>::WritableReference l = fIndirect_.rwget ();
                if (l->IsMissing ()) {
                    DoInitOutOfLine_ (&l);
                }
                l.rwref ()->Set (updaterFunction (l.cref ()->Get ()));
            }
            template <typename T, typename IMPL>
            dont_inline void ModuleGetterSetter<T, IMPL>::DoInitOutOfLine_ (typename Synchronized<Memory::Optional<IMPL>>::WritableReference* ref)
            {
                RequireNotNull (ref);
                Require (ref->load ().IsMissing ());
                *ref = IMPL{};
                Ensure (ref->load ().IsPresent ());
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_ModuleGetterSetter_inl_*/
