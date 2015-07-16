/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Association_inl_
#define _Stroika_Foundation_Containers_Association_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

#include    "Concrete/Association_Factory.h"
#include    "../Traversal/Generator.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ***************** Association<KEY_TYPE, VALUE_TYPE, TRAITS> ********************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association ()
                : inherited (move<inherited> (Concrete::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                : inherited (static_cast <const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move<inherited> (Concrete::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move<inherited> (Concrete::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const multimap<KEY_TYPE, VALUE_TYPE>& src)
                : inherited (move<inherited> (Concrete::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename TRAITS2>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const Association<KEY_TYPE, VALUE_TYPE, TRAITS2>& src)
                : inherited (move<inherited> (Concrete::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const CONTAINER_OF_PAIR_KEY_T& src)
                : inherited (move<inherited> (Concrete::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (move<inherited> (Concrete::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const _SharedPtrIRep& rep)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (rep))
            {
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (_SharedPtrIRep&& rep)
                : inherited (move<typename inherited::_SharedPtrIRep> (rep))
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Association<KEY_TYPE, VALUE_TYPE, TRAITS>::~Association ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  const typename  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep&    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Iterable<KEY_TYPE>    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Keys () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Keys ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, ValueType* item) const
            {
                if (item == nullptr) {
                    return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
                }
                else {
                    Memory::Optional<ValueType> tmp;
                    if (_SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, Memory::Optional<ValueType>* item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, item);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Memory::Optional<VALUE_TYPE>    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key) const
            {
                Memory::Optional<VALUE_TYPE>   r;
                bool    result = _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  VALUE_TYPE   Association<KEY_TYPE, VALUE_TYPE, TRAITS>::LookupValue (KeyType key, ValueType defaultValue) const
            {
                Memory::Optional<VALUE_TYPE>   r    { Lookup (key) };
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsKey (KeyType key) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsValue (ValueType v) const
            {
                //WRONG - need something similar...@todo - use new traits - RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                for (KeyValuePair<KEY_TYPE, VALUE_TYPE> t : *this) {
                    if (t.second == v) {
                        return true;
                    }
                }
                return false;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (KeyType key, ValueType newElt)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (key, newElt);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (KeyValuePair<KeyType, ValueType> p)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (p.fKey, p.fValue);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEYVALUE>
            void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_KEYVALUE>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_KEYVALUE& items)
            {
                /*
                 *  Note - unlike Bag<T> - we dont need to check for this != &s because if we
                 *  attempt to add items that already exist, it would do nothing to our iteration
                 *  and therefore not lead to an infinite loop.
                 */
                AddAll (std::begin (items), std::end (items));
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (KeyType key)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (i);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                for (auto i : items) {
                    Remove (i.first);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (i->first);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_Key_T>
            inline  CONTAINER_OF_Key_T Association<KEY_TYPE, VALUE_TYPE, TRAITS>::As() const
            {
                return As_<CONTAINER_OF_Key_T> ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_Key_T>
            CONTAINER_OF_Key_T  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (typename enable_if <is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int>::type usesInsertPair) const
            {
                CONTAINER_OF_Key_T  result;
                for (auto i : *this) {
                    // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
                    result.insert (result.end (), pair<KEY_TYPE, VALUE_TYPE> (i.fKey, i.fValue));
                }
                return result;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename   CONTAINER_OF_Key_T>
            inline  CONTAINER_OF_Key_T  Association<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (typename enable_if < !is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int >::type) const
            {
                CONTAINER_OF_Key_T  result;
                for (auto i : *this) {
                    // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
                    result.insert (result.end (), KeyValuePair<KEY_TYPE, VALUE_TYPE> (i.fKey, i.fValue));
                }
                return result;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename VALUE_EQUALS_COMPARER>
            bool    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Equals (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const
            {
                /*
                 *    @todo   THIS CODE IS TOO COMPLICATED, and COULD USE CLEANUP/CODE REVIEW - LGP 2014-06-11
                 */
                _SafeReadRepAccessor<_IRep> lhs { this };
                if (&lhs._ConstGetRep () == &rhs._ConstGetRep ()) {
                    // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
                    return true;
                }
                // Check length, so we dont need to check both iterators for end/done
                if (lhs._ConstGetRep ().GetLength () != rhs.GetLength ()) {
                    return false;
                }
                /*
                 *  Two Associations compare equal, if they have the same domain, and map each element of that domain to the same range.
                 *  They need not be in the same order to compare equals. Still - they often are, and if they are, this algorithm is faster.
                 *  If they miss, we need to fall back to a slower strategy.
                 */
                auto li = lhs._ConstGetRep ().MakeIterator (this);
                auto ri = rhs.MakeIterator ();
                while (not li.Done ()) {
                    bool keysEqual = KeyEqualsCompareFunctionType::Equals (li->fKey, ri->fKey);
                    if (keysEqual and VALUE_EQUALS_COMPARER::Equals (li->fValue, ri->fValue)) {
                        // then this element matches
                    }
                    else {
                        // check if li maps to right value in rhs
                        auto o = rhs.Lookup (li->fKey);
                        if (o.IsMissing () or not VALUE_EQUALS_COMPARER::Equals (*o, li->fValue)) {
                            return false;
                        }
                        // if the keys were differnt, we must check the reverse direction too
                        if (not keysEqual) {
                            if (not lhs._ConstGetRep ().Lookup (ri->fKey, &o) or not VALUE_EQUALS_COMPARER::Equals (*o, ri->fValue)) {
                                return false;
                            }
                        }
                    }
                    // if we got this far, all compared OK so far, so keep going
                    ++li;
                    ++ri;
                }
                return li.Done ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::clear ()
            {
                RemoveAll ();
            }
#if 0
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            Association<KEY_TYPE, VALUE_TYPE, TRAITS>       Association<KEY_TYPE, VALUE_TYPE, TRAITS>::operator+ (const CONTAINER_OF_PAIR_KEY_T& items) const
            {
                Association<KEY_TYPE, VALUE_TYPE, TRAITS>   result = *this;
                result.AddAll (items);
                return result;
            }
#endif
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>&    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::operator+= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                AddAll (items);
                return *this;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Association<KEY_TYPE, VALUE_TYPE, TRAITS>&    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::operator-= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                RemoveAll (items);
                return *this;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_AssertRepValidType () const
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }


            /*
             ********************************************************************************
             ************** Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep ****************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Iterable<KEY_TYPE>    Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::_Keys_Reference_Implementation () const
            {
                struct  MyIterable_ : Iterable<KEY_TYPE> {
                    using   MyAssociation_      =   Association<KEY_TYPE, VALUE_TYPE, TRAITS>;
                    struct  MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep {
                        using   inherited       = typename Traversal::IterableFromIterator<KEY_TYPE>::_Rep;
                        using   _SharedPtrIRep  = typename Iterable<KEY_TYPE>::_SharedPtrIRep;
                        MyAssociation_  fAssociation_;
                        DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                        MyIterableRep_ (const MyAssociation_& map)
                            : inherited ()
                            , fAssociation_ (map)
                        {
                        }
                        virtual Iterator<KEY_TYPE>     MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
                            auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>> (fAssociation_.MakeIterator ());
                            auto getNext = [myContext] () -> Memory::Optional<KEY_TYPE> {
                                if (myContext->Done ())
                                {
                                    return Memory::Optional<KEY_TYPE> ();
                                }
                                else {
                                    auto result = (*myContext)->fKey;
                                    (*myContext)++;
                                    return result;
                                }
                            };
                            return Traversal::CreateGeneratorIterator<KEY_TYPE> (getNext);
                        }
                        virtual _SharedPtrIRep Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                        {
                            // For now - ignore forIterableEnvelope
                            return _SharedPtrIRep (new MyIterableRep_ (*this));
                        }
                    };
                    MyIterable_ (const MyAssociation_& map)
                        : Iterable<KEY_TYPE> (typename Iterable<KEY_TYPE>::_SharedPtrIRep (new MyIterableRep_ (map)))
                    {
                    }
                };
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                auto rep = dynamic_pointer_cast<typename Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep> (const_cast<typename Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
                auto rep = const_cast<typename Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
                return MyIterable_ (Association<KEY_TYPE, VALUE_TYPE, TRAITS> (rep));
            }


            /*
             ********************************************************************************
             **************************** Association operators *****************************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    operator== (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    operator!= (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Association_inl_ */
