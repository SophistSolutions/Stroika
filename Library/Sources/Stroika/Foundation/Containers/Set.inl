/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_inl_
#define _Stroika_Foundation_Containers_Set_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Set_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ***************************** Set<T, TRAITS> ***********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set ()
                : inherited (move<inherited> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (const Set<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (Set<T, TRAITS>&& src)
                : inherited (static_cast < const inherited&& > (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (const initializer_list<T>& src)
                : inherited (move<inherited> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (const set<T>& src)
                : inherited (move<inherited> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Set<T, TRAITS>::Set (const CONTAINER_OF_T& src)
                : inherited (move<inherited> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (const _SharedPtrIRep& src)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (_SharedPtrIRep&& src)
                : inherited (move<typename inherited::_SharedPtrIRep> (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Set<T, TRAITS>::Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (move<inherited> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (start, end);
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>::~Set ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename T, typename TRAITS>
            inline  const typename  Set<T, TRAITS>::_IRep&    Set<T, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  bool    Set<T, TRAITS>::Contains (T item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Contains (item);
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>    Set<T, TRAITS>::Lookup (T item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::Add (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (item);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Set<T, TRAITS>::AddIf (T item)
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
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Set<T, TRAITS>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T, TRAITS>::AddAll (const CONTAINER_OF_T& s)
            {
                // Note - unlike Bag<T, TRAITS> - we dont need to check for this != &s because if we
                // attempt to add items that already exist, it would do nothing, and not lead to
                // an infinite loop
                AddAll (std::begin (s), std::end (s));
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::Remove (T item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::Remove (const Iterator<T>& i)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (i);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Set<T, TRAITS>::RemoveIf (T item)
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
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Set<T, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T, TRAITS>::RemoveAll (const CONTAINER_OF_T& s)
            {
                RemoveAll (std::begin (s), std::end (s));
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename T, typename TRAITS>
            bool  Set<T, TRAITS>::Equals (const Set<T, TRAITS>& rhs) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep> { &rhs } ._ConstGetRep ());
            }
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>    Set<T, TRAITS>::EachWith (const function<bool(const T& item)>& doToElement) const
            {
                Set<T, TRAITS>   result;
                for (T i : *this) {
                    if (doToElement (i)) {
                        result.Add (i);
                    }
                }
                return result;
            }
            template    <typename T, typename TRAITS>
            bool    Set<T, TRAITS>::Intersects (const Iterable<T>& rhs) const
            {
                for (T i : rhs) {
                    if (Contains (i)) {
                        return true;
                    }
                }
                return false;
            }
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>   Set<T, TRAITS>::Intersection (const Iterable<T>& rhs) const
            {
                Set<T, TRAITS>  result;
                for (T i : rhs) {
                    if (Contains (i)) {
                        result.Add (i);
                    }
                }
                return result;
            }
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>   Set<T, TRAITS>::Union (const Iterable<T>& rhs) const
            {
                Set<T, TRAITS>   r   =   *this;
                r.AddAll (rhs);
                return r;
            }
            template    <typename T, typename TRAITS>
            Set<T, TRAITS>   Set<T, TRAITS>::Difference (const Set<T, TRAITS>& rhs) const
            {
                Set<T, TRAITS>  result;
                for (T i : *this) {
                    if (not rhs.Contains (i)) {
                        result.Add (i);
                    }
                }
                return result;
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>   Set<T, TRAITS>::operator+ (const Iterable<T>& rhs) const
            {
                return Union (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>   Set<T, TRAITS>::operator- (const Set<T, TRAITS>& rhs) const
            {
                return Difference (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>   Set<T, TRAITS>::operator^ (const Iterable<T>& rhs) const
            {
                return Intersection (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool  Set<T, TRAITS>::operator== (const Set<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Set<T, TRAITS>::operator!= (const Set<T, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator+= (T item)
            {
                Add (item);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator+= (const Iterable<T>& items)
            {
                AddAll (items);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator-= (T item)
            {
                Remove (item);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator-= (const Iterable<T>& items)
            {
                RemoveAll (items);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::insert (T item)
            {
                Add (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::erase (T item)
            {
                Remove (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


            /*
             ********************************************************************************
             ************************** Set<T, TRAITS>::_IRep *******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            bool    Set<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
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


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Set_inl_ */
