/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_  1

#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {


                /*
                 ********************************************************************************
                 *IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR,PATCHABLE_CONTAINER_VALUE> *
                 ********************************************************************************
                 */
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                inline  IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::IteratorImplHelper_ (IteratorOwnerID owner, PATCHABLE_CONTAINER* data)
                    : inherited ()
                    , fIterator (owner, data)
                {
                    RequireNotNull (data);
                    fIterator.More (static_cast<DataStructureImplValueType_*> (nullptr), true);   //tmphack cuz current backend iterators require a first more() - fix that!
                }
#if 0
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                inline  IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::~IteratorImplHelper_ ()
                {
                    AssertNotNull (fIterator.fPatchableContainer);
                    std::lock_guard<std::mutex> critSec (fIterator.fPatchableContainer->fActiveIteratorsMutex_);
                    // must do PatchableContainerHelper<...>::PatchableIteratorMixIn::DTOR login inside lock
                    fIterator.fPatchableContainer->RemoveIterator (&fIterator);
                    Assert (fIterator.fPatchableContainer == nullptr);
                    Assert (fIterator.fNextActiveIterator == nullptr);
                    // could assert owner  - fPatchableContainer - doenst contian us in list
                }
#endif
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                typename Iterator<T>::SharedIRepPtr IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::Clone () const
                {
                    AssertNotNull (fIterator.fPatchableContainer);
                    return Iterator<T>::template MakeSharedPtr<IteratorImplHelper_> (*this);
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                IteratorOwnerID IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::GetOwner () const
                {
                    return fIterator.GetOwner ();
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                void    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More (Memory::Optional<T>* result, bool advance)
                {
                    RequireNotNull (result);
                    More_SFINAE_ (result, advance);
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                template    <typename CHECK_KEY>
                inline  void    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More_SFINAE_ (Memory::Optional<T>* result, bool advance, typename std::enable_if<is_same<T, CHECK_KEY>::value>::type*)
                {
                    RequireNotNull (result);
                    AssertNotNull (fIterator.fPatchableContainer);
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> lg (*fIterator.fPatchableContainer);
                    fIterator.More (result, advance);

                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                template    <typename CHECK_KEY>
                inline  void    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More_SFINAE_ (Memory::Optional<T>* result, bool advance, typename std::enable_if < !is_same<T, CHECK_KEY>::value >::type*)
                {
                    RequireNotNull (result);
                    AssertNotNull (fIterator.fPatchableContainer);
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> lg (*fIterator.fPatchableContainer);
                    Memory::Optional<DataStructureImplValueType_> tmp;
                    fIterator.More (&tmp, advance);
                    if (tmp.IsPresent ()) {
                        *result = *tmp;
                    }
                    else {
                        result->clear ();
                    }
                }
                template    <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                bool    IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::Equals (const typename Iterator<T>::IRep* rhs) const
                {
                    RequireNotNull (rhs);
                    using   ActualIterImplType_ =   IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>;
                    RequireMember (rhs, ActualIterImplType_);
                    const ActualIterImplType_* rrhs =   dynamic_cast<const ActualIterImplType_*> (rhs);
                    AssertNotNull (rrhs);
                    AssertNotNull (fIterator.fPatchableContainer);
                    // @todo - FIX to use lock-2-at-a-time lock stuff on LHS and RHS
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> lg (*fIterator.fPatchableContainer);
                    return fIterator.Equals (rrhs->fIterator);
                }

            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ */
