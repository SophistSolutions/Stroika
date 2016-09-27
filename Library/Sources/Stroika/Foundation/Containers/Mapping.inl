/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_inl_
#define _Stroika_Foundation_Containers_Mapping_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <set>

#include    "../Debug/Assertions.h"
#include    "../Traversal/Generator.h"
#include    "Factory/Mapping_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ***************** Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> ************************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const CONTAINER_OF_PAIR_KEY_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const _SharedPtrIRep& rep)
                : inherited (rep)
            {
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (_SharedPtrIRep&& rep)
                : inherited (move (rep))
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::~Mapping ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    // SharingState can be NULL because of MOVE semantics
                    _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Iterable<KEY_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Keys () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Keys ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Iterable<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Values () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Values ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (ArgByValueType<key_type> key, mapped_type* item) const
            {
                if (item == nullptr) {
                    return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
                }
                else {
                    Memory::Optional<mapped_type> tmp;
                    if (_SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (ArgByValueType<key_type> key, Memory::Optional<mapped_type>* item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, item);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Memory::Optional<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (ArgByValueType<key_type> key) const
            {
                Memory::Optional<VALUE_TYPE>   r;
                bool    result = _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (ArgByValueType<key_type> key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  VALUE_TYPE   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::LookupValue (ArgByValueType<key_type> key, ArgByValueType<mapped_type> defaultValue) const
            {
                Memory::Optional<VALUE_TYPE>   r    { Lookup (key) };
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  VALUE_TYPE   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator[] (ArgByValueType<key_type> key) const
            {
                return *Lookup (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsKey (ArgByValueType<key_type> key) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsValue (ArgByValueType<mapped_type> v) const
            {
                //WRONG - need something similar...@todo - use new traits - RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                for (value_type t : *this) {
                    if (t.fValue == v) {
                        return true;
                    }
                }
                return false;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newElt)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (key, newElt);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (ArgByValueType<KeyValuePair<key_type, mapped_type>> p)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (p.fKey, p.fValue);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEYVALUE>
            void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_KEYVALUE, typename ENABLE_IF>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_KEYVALUE& items)
            {
                /*
                 *  Note - unlike other containers - we dont need to check for this != &s because if we
                 *  attempt to add items that already exist, it would do nothing to our iteration
                 *  and therefore not lead to an infinite loop.
                 */
                AddAll (std::begin (items), std::end (items));
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (ArgByValueType<key_type> key)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (const Iterator<value_type>& i)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (i);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                for (auto i : items) {
                    Remove (i.first);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (i->first);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_KEY_TYPE>
            void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::RetainAll (const CONTAINER_OF_KEY_TYPE& items)
            {
                set<KEY_TYPE>   tmp (items.begin (), items.end ());   // @todo - weak implementation because of 'comparison' function, and performance (if items already a set)
                for (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i = this->begin (); i != this->end (); ++i) {
                    if (tmp.find (i->fKey) == tmp.end ()) {
                        Remove (i);
                    }
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_Key_T>
            inline  CONTAINER_OF_Key_T Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::As() const
            {
                return As_<CONTAINER_OF_Key_T> ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_Key_T>
            CONTAINER_OF_Key_T  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (typename enable_if <is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int>::type usesInsertPair) const
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
            inline  CONTAINER_OF_Key_T  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (typename enable_if < !is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int >::type) const
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
            bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Equals (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const
            {
                /*
                 *    @todo   THIS CODE IS TOO COMPLICATED, and COULD USE CLEANUP/CODE REVIEW - LGP 2014-06-11
                 */
                _SafeReadRepAccessor<_IRep> lhs { this };
                _SafeReadRepAccessor<_IRep> rhsR { &rhs };
                if (&lhs._ConstGetRep () == &rhsR._ConstGetRep ()) {
                    // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
                    return true;
                }
                // Check length, so we dont need to check both iterators for end/done
                if (lhs._ConstGetRep ().GetLength () != rhsR._ConstGetRep ().GetLength ()) {
                    return false;
                }
                /*
                 *  Two Mappings compare equal, if they have the same domain, and map each element of that domain to the same range.
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
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Accumulate (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newValue, const function<mapped_type(ArgByValueType<mapped_type>, ArgByValueType<mapped_type>)>& f, mapped_type initialValue)
            {
                Add (key, f (LookupValue (key, initialValue), newValue));
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::erase (ArgByValueType<key_type> key)
            {
                Remove (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::erase (const Iterator<value_type>& i)
            {
                Remove (i);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::clear ()
            {
                RemoveAll ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>       Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator+ (const CONTAINER_OF_PAIR_KEY_T& items) const
            {
                Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>   result = *this;
                result.AddAll (items);
                return result;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>&    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator+= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                AddAll (items);
                return *this;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>&    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator-= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                RemoveAll (items);
                return *this;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                _SafeReadRepAccessor<_IRep> { this };
#endif
            }


            /*
             ********************************************************************************
             ***************** Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep *****************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Iterable<KEY_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::_Keys_Reference_Implementation () const
            {
                struct  MyIterable_ : Iterable<KEY_TYPE> {
                    using   MyMapping_      =   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>;
                    struct  MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep {
                        using   inherited       = typename Traversal::IterableFromIterator<KEY_TYPE>::_Rep;
                        using   _SharedPtrIRep  = typename Iterable<KEY_TYPE>::_SharedPtrIRep;
                        MyMapping_  fMapping_;
                        DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                        MyIterableRep_ (const MyMapping_& map)
                            : inherited ()
                            , fMapping_ (map)
                        {
                        }
                        virtual Iterator<KEY_TYPE>     MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
                            auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>> (fMapping_.MakeIterator ());
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
                            return Iterable<KEY_TYPE>::template MakeSharedPtr<MyIterableRep_> (*this);
                        }
                    };
                    MyIterable_ (const MyMapping_& m)
                        : Iterable<KEY_TYPE> (Iterable<KEY_TYPE>::template MakeSharedPtr<MyIterableRep_> (m))
                    {
                    }
                };
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                auto rep = dynamic_pointer_cast<typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep> (const_cast<typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
                auto rep = const_cast<typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
                return MyIterable_ (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> (rep));
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Iterable<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::_Values_Reference_Implementation () const
            {
                struct  MyIterable_ : Iterable<VALUE_TYPE> {
                    using   MyMapping_      =   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>;
                    struct  MyIterableRep_ : Traversal::IterableFromIterator<VALUE_TYPE>::_Rep {
                        using   inherited       = typename Traversal::IterableFromIterator<VALUE_TYPE>::_Rep;
                        using   _SharedPtrIRep  = typename Iterable<VALUE_TYPE>::_SharedPtrIRep;
                        MyMapping_  fMapping_;
                        DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                        MyIterableRep_ (const MyMapping_& map)
                            : inherited ()
                            , fMapping_ (map)
                        {
                        }
                        virtual Iterator<VALUE_TYPE>     MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
                            auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>> (fMapping_.MakeIterator ());
                            auto getNext = [myContext] () -> Memory::Optional<VALUE_TYPE> {
                                if (myContext->Done ())
                                {
                                    return Memory::Optional<VALUE_TYPE> ();
                                }
                                else {
                                    auto result = (*myContext)->fValue;
                                    (*myContext)++;
                                    return result;
                                }
                            };
                            return Traversal::CreateGeneratorIterator<VALUE_TYPE> (getNext);
                        }
                        virtual _SharedPtrIRep Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                        {
                            // For now - ignore forIterableEnvelope
                            return Iterable<VALUE_TYPE>::template MakeSharedPtr<MyIterableRep_> (*this);
                        }
                    };
                    MyIterable_ (const MyMapping_& m)
                        : Iterable<VALUE_TYPE> (Iterable<VALUE_TYPE>::template MakeSharedPtr<MyIterableRep_> (m))
                    {
                    }
                };
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                auto rep = dynamic_pointer_cast<typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep> (const_cast<typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
                auto rep = const_cast<typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
                return MyIterable_ (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> (rep));
            }


            /*
             ********************************************************************************
             ************************** Mapping operators ***********************************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    operator== (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    operator!= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Mapping_inl_ */
