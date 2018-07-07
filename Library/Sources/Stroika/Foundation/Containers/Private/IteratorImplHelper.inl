/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_
#define _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Private {

                /*
                 ********************************************************************************
                 *IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR,PATCHABLE_CONTAINER_VALUE> *
                 ********************************************************************************
                 */
                template <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                inline IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::IteratorImplHelper_ (IteratorOwnerID owner, PATCHABLE_CONTAINER* data)
                    : inherited ()
                    , fIterator (owner, data)
                {
                    RequireNotNull (data);
                    fIterator.More (static_cast<DataStructureImplValueType_*> (nullptr), true); //tmphack cuz current backend iterators require a first more() - fix that!
                }
                template <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                typename Iterator<T>::IteratorRepSharedPtr IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::Clone () const
                {
                    return Iterator<T>::template MakeSharedPtr<IteratorImplHelper_> (*this);
                }
                template <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                IteratorOwnerID IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::GetOwner () const
                {
                    return fIterator.GetOwner ();
                }
                template <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                void IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More (optional<T>* result, bool advance)
                {
                    RequireNotNull (result);
                    // NOTE: the reason this is Debug::AssertExternallySynchronizedLock, is because we only modify data on the newly cloned (breakreferences)
                    // iterator, and that must be in the thread (so externally synchronized) of the modifier
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> lg (*fIterator.GetPatchableContainerHelper ());
                    More_SFINAE_ (result, advance);
                }
                template <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                template <typename CHECK_KEY>
                inline void IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More_SFINAE_ (optional<T>* result, bool advance, enable_if_t<is_same<T, CHECK_KEY>::value>*)
                {
                    RequireNotNull (result);
                    fIterator.More (result, advance);
                }
                template <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                template <typename CHECK_KEY>
                inline void IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::More_SFINAE_ (optional<T>* result, bool advance, enable_if_t<!is_same<T, CHECK_KEY>::value>*)
                {
                    RequireNotNull (result);
                    optional<DataStructureImplValueType_> tmp;
                    fIterator.More (&tmp, advance);
                    if (tmp.has_value ()) {
                        *result = *tmp;
                    }
                    else {
                        *result = nullopt;
                    }
                }
                template <typename T, typename PATCHABLE_CONTAINER, typename PATCHABLE_CONTAINER_ITERATOR, typename PATCHABLE_CONTAINER_VALUE>
                bool IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>::Equals (const typename Iterator<T>::IRep* rhs) const
                {
                    RequireNotNull (rhs);
                    using ActualIterImplType_ = IteratorImplHelper_<T, PATCHABLE_CONTAINER, PATCHABLE_CONTAINER_ITERATOR, PATCHABLE_CONTAINER_VALUE>;
                    RequireMember (rhs, ActualIterImplType_);
                    const ActualIterImplType_* rrhs = dynamic_cast<const ActualIterImplType_*> (rhs);
                    AssertNotNull (rrhs);
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec1 (*fIterator.GetPatchableContainerHelper ());
                    std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec2 (*rrhs->fIterator.GetPatchableContainerHelper ());
                    return fIterator.Equals (rrhs->fIterator);
                }
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Private_IteratorImplHelper_inl_ */
