/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DelegatedIterator_inl_
#define _Stroika_Foundation_Traversal_DelegatedIterator_inl_

#include "../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Traversal {

            /*
             ********************************************************************************
             *********************** DelegatedIterator<T, EXTRA_DATA>::Rep ******************
             ********************************************************************************
             */
            template <typename T, typename EXTRA_DATA>
            DelegatedIterator<T, EXTRA_DATA>::Rep::Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
                : fDelegateTo (delegateTo)
                , fExtraData (extraData)
            {
            }
#if !qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy
            template <typename T, typename EXTRA_DATA>
            typename Iterator<T>::IteratorRepSharedPtr DelegatedIterator<T, EXTRA_DATA>::Rep::Clone () const
            {
                return IteratorRepSharedPtr (Iterator<T>::template MakeSharedPtr<Rep> (*this));
            }
#endif
            template <typename T, typename EXTRA_DATA>
            IteratorOwnerID DelegatedIterator<T, EXTRA_DATA>::Rep::GetOwner () const
            {
                return fDelegateTo.GetOwner ();
            }
            template <typename T, typename EXTRA_DATA>
            void DelegatedIterator<T, EXTRA_DATA>::Rep::More (optional<T>* result, bool advance)
            {
                fDelegateTo.GetRep ().More (result, advance);
            }
            template <typename T, typename EXTRA_DATA>
            bool DelegatedIterator<T, EXTRA_DATA>::Rep::Equals (const IRep* rhs) const
            {
                return fDelegateTo.GetRep ().Equals (rhs);
            }
            template <typename T>
            DelegatedIterator<T, void>::Rep::Rep (const Iterator<T>& delegateTo)
                : fDelegateTo (delegateTo)
            {
            }

            /*
             ********************************************************************************
             *********************** DelegatedIterator<T, EXTRA_DATA> ***********************
             ********************************************************************************
             */
            template <typename T, typename EXTRA_DATA>
            DelegatedIterator<T, EXTRA_DATA>::DelegatedIterator (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
                : Iterator<T> (Iterator<T>::template MakeSharedPtr<Rep> (delegateTo, extraData))
            {
            }
        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DelegatedIterator_inl_ */
