/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleGetterSetter_inl_
#define _Stroika_Foundation_Execution_ModuleGetterSetter_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ************************ ModuleGetterSetter<T, IMPL> ***************************
             ********************************************************************************
             */
            template    <typename T, typename IMPL>
            inline  T       ModuleGetterSetter<T, IMPL>::Get ()
            {
                typename Execution::Synchronized<Memory::Optional<IMPL>>::WritableReference l = fIndirect_.get ();
                if (l->IsMissing ()) {
                    DoInitOutOfLine_ (&l);
                }
                return l.load ()->Get ();
            }
            template    <typename T, typename IMPL>
            inline  void    ModuleGetterSetter<T, IMPL>::Set (const T& v)
            {
                typename Execution::Synchronized<Memory::Optional<IMPL>>::WritableReference l = fIndirect_.get ();
                if (l->IsMissing ()) {
                    DoInitOutOfLine_ (&l);
                }
#if 1
                // @todo - understand why this .operator-> nonsense is needed???
                l->operator-> ()->Set (v);
#else
                l->Set (v);
#endif
            }
            template    <typename T, typename IMPL>
            dont_inline void    ModuleGetterSetter<T, IMPL>::DoInitOutOfLine_ (typename Execution::Synchronized<Memory::Optional<IMPL>>::WritableReference* ref)
            {
                RequireNotNull (ref);
                Require (ref->load ().IsMissing ());
                *ref = IMPL {};
                Ensure (ref->load ().IsPresent ());
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ModuleGetterSetter_inl_*/
