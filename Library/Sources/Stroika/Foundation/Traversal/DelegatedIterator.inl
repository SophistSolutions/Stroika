/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DelegatedIterator_inl_
#define _Stroika_Foundation_Traversal_DelegatedIterator_inl_

#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             *********************** DelegatedIterator<T, EXTRA_DATA>::Rep ******************
             ********************************************************************************
             */
            template    <typename T, typename EXTRA_DATA>
            DelegatedIterator<T, EXTRA_DATA>::Rep::Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
                : fDelegateTo (delegateTo)
                , fExtraData (extraData)
            {
            }
#if     !qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy
            template    <typename T, typename EXTRA_DATA>
            typename    Iterator<T>::SharedIRepPtr   DelegatedIterator<T, EXTRA_DATA>::Rep::Clone () const
            {
                return SharedIRepPtr (new Rep (*this));
            }
#endif
            template    <typename T, typename EXTRA_DATA>
            IteratorOwnerID DelegatedIterator<T, EXTRA_DATA>::Rep::GetOwner () const
            {
                return fDelegateTo.GetOwner ();
            }
            template    <typename T, typename EXTRA_DATA>
            void    DelegatedIterator<T, EXTRA_DATA>::Rep::More (Memory::Optional<T>* result, bool advance)
            {
                fDelegateTo.GetRep ().More (result, advance);
            }
            template    <typename T, typename EXTRA_DATA>
            bool    DelegatedIterator<T, EXTRA_DATA>::Rep::Equals (const IRep* rhs) const
            {
                return fDelegateTo.GetRep ().Equals (rhs);
            }
            template    <typename T>
            DelegatedIterator<T, void>::Rep::Rep (const Iterator<T>& delegateTo)
                : fDelegateTo (delegateTo)
            {
            }


            /*
             ********************************************************************************
             *********************** DelegatedIterator<T, EXTRA_DATA> ***********************
             ********************************************************************************
             */
            template    <typename T, typename EXTRA_DATA>
            DelegatedIterator<T, EXTRA_DATA>::DelegatedIterator (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
                : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (delegateTo, extraData)))
            {
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DelegatedIterator_inl_ */
