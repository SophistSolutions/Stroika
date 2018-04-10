/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_inl_
#define _Stroika_Foundation_Containers_Collection_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"

#include "Factory/Collection_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ******************************** Collection<T> *********************************
             ********************************************************************************
             */
            template <typename T>
            inline Collection<T>::Collection ()
                : inherited (move (Factory::Collection_Factory<T> () ()))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            inline Collection<T>::Collection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
                : Collection ()
            {
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename T>
            inline Collection<T>::Collection (const _CollectionRepSharedPtr& src) noexcept
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template <typename T>
            inline Collection<T>::Collection (_CollectionRepSharedPtr&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
            template <typename T>
            inline Collection<T>::Collection (const initializer_list<T>& src)
                : Collection ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T>
            template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
            inline Collection<T>::Collection (const CONTAINER_OF_ADDABLE& src)
                : Collection ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
#if qDebug
            template <typename T>
            Collection<T>::~Collection ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    // SharingState can be NULL because of MOVE semantics
                    _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template <typename T>
            template <typename EQUALS_COMPARER>
            bool Collection<T>::Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer) const
            {
                for (auto i : *this) {
                    if (equalsComparer (i, item)) {
                        return true;
                    }
                }
                return false;
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
            void Collection<T>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                for (auto i = start; i != end; ++i) {
                    tmp._GetWriteableRep ().Add (*i);
                }
            }
            template <typename T>
            template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
            inline void Collection<T>::AddAll (const CONTAINER_OF_ADDABLE& c)
            {
                /*
                 * Because adding items to a Collection COULD result in those items appearing in a running iterator,
                 * for the corner case of s.AddAll(s) - we want to assure we don't infinite loop.
                 */
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    CONTAINER_OF_ADDABLE tmp = c;
                    AddAll (std::begin (tmp), std::end (tmp));
                }
                else {
                    AddAll (std::begin (c), std::end (c));
                }
            }
            template <typename T>
            inline void Collection<T>::Add (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item);
                Ensure (not this->IsEmpty ());
            }
            template <typename T>
            inline void Collection<T>::Update (const Iterator<T>& i, ArgByValueType<T> newValue)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Update (i, newValue);
            }
            template <typename T>
            template <typename EQUALS_COMPARER>
            inline void Collection<T>::Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
            {
                for (Iterator<T> i = this->begin (); i != this->end (); ++i) {
                    if (equalsComparer (*i, item)) {
                        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
                        return;
                    }
                }
            }
            template <typename T>
            inline void Collection<T>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template <typename T>
            template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename EQUALS_COMPARER>
            void Collection<T>::RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end, const EQUALS_COMPARER& equalsComparer)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i, equalsComparer);
                }
            }
            template <typename T>
            template <typename CONTAINER_OF_ADDABLE, typename EQUALS_COMPARER>
            inline void Collection<T>::RemoveAll (const CONTAINER_OF_ADDABLE& c, const EQUALS_COMPARER& equalsComparer)
            {
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    RemoveAll (equalsComparer);
                }
                else {
                    RemoveAll (std::begin (c), std::end (c), equalsComparer);
                }
            }
            template <typename T>
            inline void Collection<T>::Remove (const Iterator<T>& i)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
            }
            template <typename T>
            inline void Collection<T>::clear ()
            {
                RemoveAll ();
            }
            template <typename T>
            template <typename EQUALS_COMPARER>
            inline void Collection<T>::erase (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
            {
                Remove (item, equalsComparer);
            }
            template <typename T>
            inline void Collection<T>::erase (const Iterator<T>& i)
            {
                Remove (i);
            }
            template <typename T>
            inline Collection<T> Collection<T>::Where (const function<bool(ArgByValueType<T>)>& doToElement) const
            {
                return Iterable<T>::Where (doToElement, Collection<T>{});
            }
            template <typename T>
            inline Collection<T>& Collection<T>::operator+= (ArgByValueType<T> item)
            {
                Add (item);
                return *this;
            }
            template <typename T>
            inline Collection<T>& Collection<T>::operator+= (const Iterable<T>& items)
            {
                AddAll (items);
                return *this;
            }
            template <typename T>
            inline void Collection<T>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Collection_inl_ */
