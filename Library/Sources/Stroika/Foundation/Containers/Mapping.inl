/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_inl_
#define _Stroika_Foundation_Containers_Mapping_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

#include    "Concrete/Mapping_Factory.h"
#include    "../Traversal/Generator.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ***************** Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> ************************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping ()
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                : inherited (static_cast < const inherited&& > (src))
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const map<KEY_TYPE, VALUE_TYPE>& src)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const CONTAINER_OF_PAIR_KEY_T& src)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::~Mapping ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  const typename  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep&    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Iterable<KEY_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Keys () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Keys ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, ValueType* item) const
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
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, Memory::Optional<ValueType>* item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, item);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Memory::Optional<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key) const
            {
                Memory::Optional<VALUE_TYPE>   r;
                bool    result = _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  VALUE_TYPE   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::LookupValue (KeyType key, ValueType defaultValue) const
            {
                Memory::Optional<VALUE_TYPE>   r    { Lookup (key) };
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  VALUE_TYPE   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator[] (KeyType key) const
            {
                return *Lookup (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsKey (KeyType key) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsValue (ValueType v) const
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
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (KeyType key, ValueType newElt)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (key, newElt);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (KeyValuePair<KeyType, ValueType> p)
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
            template    <typename CONTAINER_OF_KEYVALUE>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_KEYVALUE& items)
            {
                /*
                 *  Note - unlike Bag<T> - we dont need to check for this != &s because if we
                 *  attempt to add items that already exist, it would do nothing to our iteration
                 *  and therefore not lead to an infinite loop.
                 */
                AddAll (std::begin (items), std::end (items));
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (KeyType key)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i)
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
            inline  bool  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Equals (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep> { &rhs } ._ConstGetRep ());
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::clear ()
            {
                RemoveAll ();
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
            inline  bool  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator== (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator!= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_AssertRepValidType () const
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }


            /*
             ********************************************************************************
             ***************** Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep *****************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
            {
                if (this == &rhs) {
                    return true;
                }
                // If sizes differ, the Mappings differ
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }
                // Since both sides are the same size, we can iterate over one, and make sure the key/values in the first
                // are present, and with the same mapping in the second.
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    Memory::Optional<ValueType>   tmp;
                    if (not rhs.Lookup (i->fKey, &tmp) or not ValueEqualsCompareFunctionType::Equals (*tmp, i->fValue)) {
                        return false;
                    }
                }
                return true;
            }
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
                            return _SharedPtrIRep (new MyIterableRep_ (*this));
                        }
                    };
                    MyIterable_ (const MyMapping_& map)
                        : Iterable<KEY_TYPE> (typename Iterable<KEY_TYPE>::_SharedPtrIRep (new MyIterableRep_ (map)))
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


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Mapping_inl_ */
