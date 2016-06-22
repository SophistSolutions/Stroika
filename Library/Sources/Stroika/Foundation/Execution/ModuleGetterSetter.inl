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
            T       ModuleGetterSetter<T, IMPL>::Get ()
            {
                auto l = fIndirect_.get ();
                if (l->IsMissing ()) {
                    l = IMPL {};
                }
                return l.load ().Value ().Get ();
            }
            template    <typename T, typename IMPL>
            void    ModuleGetterSetter<T, IMPL>::Set (const T& v)
            {
                typename Execution::Synchronized<Memory::Optional<IMPL>>::WritableReference l = fIndirect_.get ();
                if (l->IsMissing ()) {
                    l = IMPL {};
                }
#if 1
                // @todo - understand why this .operator-> nonsense is needed???
                l->operator-> ()->Set (v);
#else
                l->Set (v);
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ModuleGetterSetter_inl_*/
