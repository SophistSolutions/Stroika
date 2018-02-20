/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_inl_
#define _Stroika_Foundation_Containers_Set_inl_

#include "../Debug/Assertions.h"
#include "Factory/Set_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ******************************* Set<T, TRAITS> *********************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>::Set ()
                : inherited (move (Factory::Set_Factory<T, TRAITS>::New ()))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>::Set (const Set<T, TRAITS>& src) noexcept
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>::Set (Set<T, TRAITS>&& src) noexcept
                : inherited (move (src))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>::Set (const initializer_list<T>& src)
                : Set ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline Set<T, TRAITS>::Set (const CONTAINER_OF_T& src)
                : Set ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>::Set (const _SetRepSharedPtr& src) noexcept
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>::Set (_SetRepSharedPtr&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
            template <typename T, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_OF_T>
            inline Set<T, TRAITS>::Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : Set ()
            {
                AddAll (start, end);
                _AssertRepValidType ();
            }
#if qDebug
            template <typename T, typename TRAITS>
            Set<T, TRAITS>::~Set ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    // SharingState can be NULL because of MOVE semantics
                    _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template <typename T, typename TRAITS>
            inline bool Set<T, TRAITS>::Contains (ArgByValueType<T> item) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Contains (item);
            }
            template <typename T, typename TRAITS>
            bool Set<T, TRAITS>::IsSubsetOf (const Set<T>& superset) const
            {
                for (auto i : *this) {
                    if (not superset.Contains (i)) {
                        return false;
                    }
                }
                return true;
            }
            template <typename T, typename TRAITS>
            inline Memory::Optional<T> Set<T, TRAITS>::Lookup (ArgByValueType<T> item) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (item);
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::Add (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item);
            }
            template <typename T, typename TRAITS>
            inline bool Set<T, TRAITS>::AddIf (ArgByValueType<T> item)
            {
                /*
                 *  Note, this is an non-performance optimal implementation, but is not a race, because from the outside
                 *  if someone calls AddIf() - they dont know/care if this call or another at the same time is doing the
                 *  addition. Any 'race' would be in the logical of the calling code.
                 */
                if (Contains (item)) {
                    return false;
                }
                else {
                    Add (item);
                    return true;
                }
            }
            template <typename T, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_OF_T>
            void Set<T, TRAITS>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template <typename T, typename TRAITS>
            template <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline void Set<T, TRAITS>::AddAll (const CONTAINER_OF_T& s)
            {
                // Note - unlike Collection<T, TRAITS> - we dont need to check for this != &s because if we
                // attempt to add items that already exist, it would do nothing, and not lead to
                // an infinite loop
                AddAll (std::begin (s), std::end (s));
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::Remove (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (item);
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::Remove (const Iterator<T>& i)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
            }
            template <typename T, typename TRAITS>
            inline bool Set<T, TRAITS>::RemoveIf (ArgByValueType<T> item)
            {
                /*
                 *  Note, this is an non-performance optimal implementation, but is not a race, because from the outside
                 *  if someone calls RemoveIf() - they dont know/care if this call or another at the same time is doing the
                 *  addition. Any 'race' would be in the logical of the calling code.
                 */
                if (Contains (item)) {
                    Remove (item);
                    return true;
                }
                else {
                    return false;
                }
            }
            template <typename T, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_OF_T>
            void Set<T, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i);
                }
            }
            template <typename T, typename TRAITS>
            template <typename CONTAINER_OF_T>
            inline void Set<T, TRAITS>::RemoveAll (const CONTAINER_OF_T& s)
            {
                RemoveAll (std::begin (s), std::end (s));
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template <typename T, typename TRAITS>
            inline bool Set<T, TRAITS>::Equals (const Set<T, TRAITS>& rhs) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
            }
            template <typename T, typename TRAITS>
            inline bool Set<T, TRAITS>::Equals (const Iterable<T>& rhs) const
            {
                // KISS for now
                return Equals (Set<T, TRAITS>{rhs});
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> Set<T, TRAITS>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
            {
                return Iterable<T>::Where (includeIfTrue, Set<T, TRAITS>{});
            }
            template <typename T, typename TRAITS>
            bool Set<T, TRAITS>::Intersects (const Iterable<T>& rhs) const
            {
                for (T i : rhs) {
                    if (Contains (i)) {
                        return true;
                    }
                }
                return false;
            }
            template <typename T, typename TRAITS>
            Set<T, TRAITS> Set<T, TRAITS>::Intersection (const Iterable<T>& rhs) const
            {
                Set<T, TRAITS> result;
                for (T i : rhs) {
                    if (Contains (i)) {
                        result.Add (i);
                    }
                }
                return result;
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> Set<T, TRAITS>::Union (const Iterable<T>& rhs) const
            {
                Set<T, TRAITS> r = *this;
                r.AddAll (rhs);
                return r;
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> Set<T, TRAITS>::Union (const T& rhs) const
            {
                Set<T, TRAITS> r = *this;
                r.Add (rhs);
                return r;
            }
            template <typename T, typename TRAITS>
            Set<T, TRAITS> Set<T, TRAITS>::Difference (const Set<T, TRAITS>& rhs) const
            {
                Set<T, TRAITS> result;
                for (T i : *this) {
                    if (not rhs.Contains (i)) {
                        result.Add (i);
                    }
                }
                return result;
            }
            template <typename T, typename TRAITS>
            Set<T, TRAITS> Set<T, TRAITS>::Difference (const T& rhs) const
            {
                return Difference (Set<T, TRAITS>{rhs});
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>& Set<T, TRAITS>::operator+= (T item)
            {
                Add (item);
                return *this;
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>& Set<T, TRAITS>::operator+= (const Iterable<T>& items)
            {
                AddAll (items);
                return *this;
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>& Set<T, TRAITS>::operator-= (T item)
            {
                Remove (item);
                return *this;
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>& Set<T, TRAITS>::operator-= (const Iterable<T>& items)
            {
                RemoveAll (items);
                return *this;
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS>& Set<T, TRAITS>::operator^= (const Iterable<T>& items)
            {
                *this = Intersection (items);
                return *this;
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::clear ()
            {
                RemoveAll ();
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::insert (ArgByValueType<T> item)
            {
                Add (item);
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::erase (ArgByValueType<T> item)
            {
                Remove (item);
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::erase (const Iterator<T>& i)
            {
                Remove (i);
            }
            template <typename T, typename TRAITS>
            inline void Set<T, TRAITS>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }

            /*
             ********************************************************************************
             ************************** Set<T, TRAITS>::_IRep *******************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            bool Set<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
            {
                if (this == &rhs) {
                    return true;
                }
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }
                // Note - no need to iterate over rhs because we checked sizes the same
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    if (not rhs.Contains (*i)) {
                        return false;
                    }
                }
                return true;
            }

            /*
             ********************************************************************************
             ******************************* Set<T> operators *******************************
             ********************************************************************************
             */
            template <typename T, typename TRAITS>
            inline bool operator== (const Set<T, TRAITS>& lhs, const Set<T, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template <typename T, typename TRAITS>
            inline bool operator== (const Set<T, TRAITS>& lhs, const Iterable<T>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template <typename T, typename TRAITS>
            inline bool operator== (const Iterable<T>& lhs, const Set<T, TRAITS>& rhs)
            {
                return rhs.Equals (lhs);
            }
            template <typename T, typename TRAITS>
            inline bool operator!= (const Set<T, TRAITS>& lhs, const Set<T, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }
            template <typename T, typename TRAITS>
            inline bool operator!= (const Set<T, TRAITS>& lhs, const Iterable<T>& rhs)
            {
                return not lhs.Equals (rhs);
            }
            template <typename T, typename TRAITS>
            inline bool operator!= (const Iterable<T>& lhs, const Set<T, TRAITS>& rhs)
            {
                return not rhs.Equals (lhs);
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> operator+ (const Set<T, TRAITS>& lhs, const Iterable<T>& rhs)
            {
                return lhs.Union (rhs);
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> operator+ (const Set<T, TRAITS>& lhs, const T& rhs)
            {
                return lhs.Union (rhs);
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> operator- (const Set<T, TRAITS>& lhs, const Set<T, TRAITS>& rhs)
            {
                return lhs.Difference (rhs);
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> operator- (const Set<T, TRAITS>& lhs, const T& rhs)
            {
                return lhs.Difference (rhs);
            }
            template <typename T, typename TRAITS>
            inline Set<T, TRAITS> operator^ (const Set<T, TRAITS>& lhs, const Iterable<T>& rhs)
            {
                return lhs.Intersection (rhs);
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Set_inl_ */
