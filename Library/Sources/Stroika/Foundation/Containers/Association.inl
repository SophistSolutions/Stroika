/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Association_inl_
#define _Stroika_Foundation_Containers_Association_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"
#include "../Traversal/Generator.h"
#include "Factory/Association_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ***************** Association<KEY_TYPE, VALUE_TYPE, TRAITS> ********************
             ********************************************************************************
             */
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association ()
                : inherited (move (Factory::Association_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> () ()))
            {
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
                : Association ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
                : Association ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const multimap<KEY_TYPE, VALUE_TYPE>& src)
                : Association ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename TRAITS2>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const Association<KEY_TYPE, VALUE_TYPE, TRAITS2>& src)
                : Association ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const CONTAINER_OF_PAIR_KEY_T& src)
                : Association ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_KEY_T>
            Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : Association ()
            {
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (const _SharedPtrIRep& rep)
                : inherited (rep)
            {
                RequireNotNull (rep);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Association (_SharedPtrIRep&& rep)
                : inherited (move (rep))
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
#if qDebug
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Association<KEY_TYPE, VALUE_TYPE, TRAITS>::~Association ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    // SharingState can be NULL because of MOVE semantics
                    _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline Iterable<KEY_TYPE> Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Keys () const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Keys ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline bool Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, ValueType* item) const
            {
                if (item == nullptr) {
                    return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
                }
                else {
                    Memory::Optional<ValueType> tmp;
                    if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline bool Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, Memory::Optional<ValueType>* item) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, item);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline Memory::Optional<VALUE_TYPE> Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key) const
            {
                Memory::Optional<VALUE_TYPE> r;
                bool                         result = _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline bool Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline VALUE_TYPE Association<KEY_TYPE, VALUE_TYPE, TRAITS>::LookupValue (KeyType key, ValueType defaultValue) const
            {
                Memory::Optional<VALUE_TYPE> r{Lookup (key)};
                return r.IsPresent () ? *r : defaultValue;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline bool Association<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsKey (KeyType key) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline bool Association<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsValue (ValueType v) const
            {
                for (KeyValuePair<KEY_TYPE, VALUE_TYPE> t : *this) {
                    if (t.second == v) {
                        return true;
                    }
                }
                return false;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (KeyType key, ValueType newElt)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (key, newElt);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Add (KeyValuePair<KeyType, ValueType> p)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.fKey, p.fValue);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_KEYVALUE>
            void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_KEYVALUE, typename ENABLE_IF>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_KEYVALUE& items)
            {
                /*
                 *  Note - unlike Bag<T> - we dont need to check for this != &s because if we
                 *  attempt to add items that already exist, it would do nothing to our iteration
                 *  and therefore not lead to an infinite loop.
                 */
                AddAll (std::begin (items), std::end (items));
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (KeyType key)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (key);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T>
            void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                for (auto i : items) {
                    Remove (i.first);
                }
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_KEY_T>
            void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (i->first);
                }
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_Key_T>
            inline CONTAINER_OF_Key_T Association<KEY_TYPE, VALUE_TYPE, TRAITS>::As () const
            {
                return As_<CONTAINER_OF_Key_T> ();
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_Key_T>
            CONTAINER_OF_Key_T Association<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (typename enable_if<is_convertible<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int>::type usesInsertPair) const
            {
                CONTAINER_OF_Key_T result;
                for (auto i : *this) {
                    // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
                    result.insert (result.end (), pair<KEY_TYPE, VALUE_TYPE> (i.fKey, i.fValue));
                }
                return result;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_Key_T>
            inline CONTAINER_OF_Key_T Association<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (typename enable_if<!is_convertible<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int>::type) const
            {
                CONTAINER_OF_Key_T result;
                for (auto i : *this) {
                    // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
                    result.insert (result.end (), KeyValuePair<KEY_TYPE, VALUE_TYPE> (i.fKey, i.fValue));
                }
                return result;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename VALUE_EQUALS_COMPARER>
            bool Association<KEY_TYPE, VALUE_TYPE, TRAITS>::Equals (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const
            {
                /*
                 *    @todo   THIS CODE IS TOO COMPLICATED, and COULD USE CLEANUP/CODE REVIEW - LGP 2014-06-11
                 */
                _SafeReadRepAccessor<_IRep> lhs{this};
                _SafeReadRepAccessor<_IRep> rhsR{&rhs};
                if (&lhs._ConstGetRep () == &rhsR._ConstGetRep ()) {
                    // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
                    return true;
                }
                // Check length, so we dont need to check both iterators for end/done
                if (lhs._ConstGetRep ().GetLength () != rhsR._ConstGetRep ().GetLength ()) {
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
                    bool keysEqual = GetKeyEqualsComparer () (li->fKey, ri->fKey);
                    if (keysEqual and GetValueEqualsComparer () (li->fValue, ri->fValue)) {
                        // then this element matches
                    }
                    else {
                        // check if li maps to right value in rhs
                        auto o = rhs.Lookup (li->fKey);
                        if (o.IsMissing () or not GetValueEqualsComparer () (*o, li->fValue)) {
                            return false;
                        }
                        // if the keys were differnt, we must check the reverse direction too
                        if (not keysEqual) {
                            if (not lhs._ConstGetRep ().Lookup (ri->fKey, &o) or not GetValueEqualsComparer () (*o, ri->fValue)) {
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
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::clear ()
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
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>& Association<KEY_TYPE, VALUE_TYPE, TRAITS>::operator+= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                AddAll (items);
                return *this;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T>
            inline Association<KEY_TYPE, VALUE_TYPE, TRAITS>& Association<KEY_TYPE, VALUE_TYPE, TRAITS>::operator-= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                RemoveAll (items);
                return *this;
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline void Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_AssertRepValidType () const
            {
                _SafeReadRepAccessor<_IRep>{this};
            }

            /*
             ********************************************************************************
             ************** Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep ****************
             ********************************************************************************
             */
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Iterable<KEY_TYPE> Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::_Keys_Reference_Implementation () const
            {
                struct MyIterable_ : Iterable<KEY_TYPE> {
                    using MyAssociation_ = Association<KEY_TYPE, VALUE_TYPE, TRAITS>;
                    struct MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep {
                        using inherited             = typename Traversal::IterableFromIterator<KEY_TYPE>::_Rep;
                        using _IterableRepSharedPtr = typename Iterable<KEY_TYPE>::_IterableRepSharedPtr;
                        MyAssociation_ fAssociation_;
                        DECLARE_USE_BLOCK_ALLOCATION (MyIterableRep_);
                        MyIterableRep_ (const MyAssociation_& map)
                            : inherited ()
                            , fAssociation_ (map)
                        {
                        }
                        virtual Iterator<KEY_TYPE> MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
                            auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>> (fAssociation_.MakeIterator ());
                            auto getNext   = [myContext]() -> Memory::Optional<KEY_TYPE> {
                                if (myContext->Done ()) {
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
                        virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                        {
                            // For now - ignore forIterableEnvelope
                            return Iterable<KEY_TYPE>::template MakeSharedPtr<MyIterableRep_> (*this);
                        }
                    };
                    MyIterable_ (const MyAssociation_& m)
                        : Iterable<KEY_TYPE> (Iterable<KEY_TYPE>::template MakeSharedPtr<MyIterableRep_> (m))
                    {
                    }
                };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
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
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline bool operator== (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline bool operator!= (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Association_inl_ */
