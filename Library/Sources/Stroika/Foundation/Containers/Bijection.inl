/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Bijection_inl_
#define _Stroika_Foundation_Containers_Bijection_inl_

#include "../Debug/Assertions.h"

#include "Factory/Bijection_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             *************** Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> *********************
             ********************************************************************************
             */
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection ()
                : inherited (move (Factory::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS> () ()))
            {
                _AssertRepValidType ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const initializer_list<pair<DOMAIN_TYPE, RANGE_TYPE>>& src)
                : Bijection ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const CONTAINER_OF_PAIR_KEY_T& src)
                : Bijection ()
            {
                AddAll (src);
                _AssertRepValidType ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_KEY_T>
            Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : Bijection ()
            {
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const _BijectionRepSharedPtr& src) noexcept
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (_BijectionRepSharedPtr&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
#if qDebug
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::~Bijection ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    // SharingState can be NULL because of MOVE semantics
                    _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline Iterable<DOMAIN_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Preimage () const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Preimage ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline Iterable<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Image () const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Image ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key, RangeType* item) const
            {
                if (item == nullptr) {
                    return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
                }
                else {
                    Memory::Optional<RangeType> tmp;
                    if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key, Memory::Optional<RangeType>* item) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, item);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline Memory::Optional<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key) const
            {
                Memory::Optional<RANGE_TYPE> r;
                bool                         result = _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline RANGE_TYPE Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::LookupValue (ArgByValueType<DomainType> key, ArgByValueType<RangeType> defaultValue) const
            {
                Memory::Optional<RANGE_TYPE> r = Lookup (key);
                return r.IsPresent () ? *r : defaultValue;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key, DomainType* item) const
            {
                if (item == nullptr) {
                    return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, nullptr);
                }
                else {
                    Memory::Optional<DomainType> tmp;
                    if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key, Memory::Optional<DomainType>* item) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, item);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline Memory::Optional<DOMAIN_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key) const
            {
                Memory::Optional<DOMAIN_TYPE> r;
                bool                          result = _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, nullptr);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline DOMAIN_TYPE Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookupValue (ArgByValueType<RangeType> key, ArgByValueType<DomainType> defaultValue) const
            {
                Memory::Optional<DOMAIN_TYPE> r = InverseLookup (key);
                return r.IsPresent () ? *r : defaultValue;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            auto Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Map (const Iterable<DomainType>& values) const -> Iterable<RangeType>
            {
                vector<RANGE_TYPE> tmp;
                tmp.reserve (values.size ());
                for (auto i : values) {
                    tmp.push_back (*Lookup (i));
                }
                return Iterable<RANGE_TYPE> (tmp);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            auto Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseMap (const Iterable<RangeType>& values) const -> Iterable<DomainType>
            {
                vector<DOMAIN_TYPE> tmp;
                tmp.reserve (values.size ());
                for (auto i : values) {
                    tmp.push_back (*InverseLookup (i));
                }
                return Iterable<DOMAIN_TYPE> (tmp);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::ContainsDomainElement (ArgByValueType<DomainType> key) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::ContainsRangeElement (ArgByValueType<RangeType> v) const
            {
                // REIMPLEMENT USING InverseLookup()!!! @todo
                //WRONG - need something similar...@todo - use new traits - RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                for (pair<DOMAIN_TYPE, RANGE_TYPE> t : *this) {
                    if (RangeEqualsCompareFunctionType::Equals (t.second, v)) {
                        return true;
                    }
                }
                return false;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (ArgByValueType<DomainType> key, ArgByValueType<RangeType> newElt)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (key, newElt);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (const pair<DomainType, RangeType>& p)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.first, p.second);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename KEYVALUEPAIR, typename ENABLE_IF_TEST>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (KEYVALUEPAIR p)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.fKey, p.fValue);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename COPY_FROM_ITERATOR_KEYVALUE>
            void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_KEYVALUE, typename ENABLE_IF>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_KEYVALUE& items)
            {
                /*
                 *  Note - unlike some other containers - we dont need to check for this != &s because if we
                 *  attempt to add items that already exist, it would do nothing to our iteration
                 *  and therefore not lead to an infinite loop.
                 */
                AddAll (std::begin (items), std::end (items));
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveDomainElement (ArgByValueType<DomainType> d)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveDomainElement (d);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveRangeElement (ArgByValueType<RangeType> r)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveRangeElement (r);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i)
            {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp{this};
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename TARGET_CONTAINER>
            TARGET_CONTAINER Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Inverse () const
            {
                TARGET_CONTAINER r;
                for (pair<DOMAIN_TYPE, RANGE_TYPE> i : *this) {
                    r.Add (i.second, i.first);
                }
                return r;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename CONTAINER_PAIR_RANGE_DOMAIN>
            inline CONTAINER_PAIR_RANGE_DOMAIN Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::As () const
            {
                return CONTAINER_PAIR_RANGE_DOMAIN (this->begin (), this->end ());
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Equals (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) const
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::clear ()
            {
                RemoveAll ();
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T>
            inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator+= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                AddAll (items);
                return *this;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template <typename CONTAINER_OF_PAIR_KEY_T>
            inline Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator-= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                RemoveAll (items);
                return *this;
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline void Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }

            /*
             ********************************************************************************
             ************ Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep *****************
             ********************************************************************************
             */
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            Iterable<DOMAIN_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::_PreImage_Reference_Implementation () const
            {
                struct MyIterable_ : Iterable<DOMAIN_TYPE> {
                    using MyBijection_ = Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>;
                    struct MyIterableRep_ : Traversal::IterableFromIterator<DOMAIN_TYPE>::_Rep {
                        using inherited             = typename Traversal::IterableFromIterator<DOMAIN_TYPE>::_Rep;
                        using _IterableRepSharedPtr = typename Iterable<DOMAIN_TYPE>::_IterableRepSharedPtr;
                        MyBijection_ fBijection_;
                        DECLARE_USE_BLOCK_ALLOCATION (MyIterableRep_);
                        MyIterableRep_ (const MyBijection_& b)
                            : inherited ()
                            , fBijection_ (b)
                        {
                        }
                        virtual Iterator<DOMAIN_TYPE> MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
                            auto myContext = make_shared<Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>> (fBijection_.MakeIterator ());
                            auto getNext   = [myContext]() -> Memory::Optional<DOMAIN_TYPE> {
                                if (myContext->Done ()) {
                                    return Memory::Optional<DOMAIN_TYPE> ();
                                }
                                else {
                                    auto result = (*myContext)->first;
                                    (*myContext)++;
                                    return result;
                                }
                            };
                            return Traversal::CreateGeneratorIterator<DOMAIN_TYPE> (getNext);
                        }
                        virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                        {
                            // For now - ignore forIterableEnvelope
                            return Iterable<DOMAIN_TYPE>::template MakeSharedPtr<MyIterableRep_> (*this);
                        }
                    };
                    MyIterable_ (const MyBijection_& b)
                        : Iterable<DOMAIN_TYPE> (Iterable<DOMAIN_TYPE>::template MakeSharedPtr<MyIterableRep_> (b))
                    {
                    }
                };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                auto rep = dynamic_pointer_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep> (const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
                auto rep = const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
                return MyIterable_ (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> (rep));
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            Iterable<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::_Image_Reference_Implementation () const
            {
                struct MyIterable_ : Iterable<RANGE_TYPE> {
                    using MyBijection_ = Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>;
                    struct MyIterableRep_ : Traversal::IterableFromIterator<RANGE_TYPE>::_Rep {
                        using inherited             = typename Traversal::IterableFromIterator<RANGE_TYPE>::_Rep;
                        using _IterableRepSharedPtr = typename Iterable<RANGE_TYPE>::_IterableRepSharedPtr;
                        MyBijection_ fBijection_;
                        DECLARE_USE_BLOCK_ALLOCATION (MyIterableRep_);
                        MyIterableRep_ (const MyBijection_& b)
                            : inherited ()
                            , fBijection_ (b)
                        {
                        }
                        virtual Iterator<RANGE_TYPE> MakeIterator (IteratorOwnerID suggestedOwner) const override
                        {
                            auto myContext = make_shared<Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>> (fBijection_.MakeIterator ());
                            auto getNext   = [myContext]() -> Memory::Optional<RANGE_TYPE> {
                                if (myContext->Done ()) {
                                    return Memory::Optional<RANGE_TYPE> ();
                                }
                                else {
                                    auto result = (*myContext)->second;
                                    (*myContext)++;
                                    return result;
                                }
                            };
                            return Traversal::CreateGeneratorIterator<RANGE_TYPE> (getNext);
                        }
                        virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                        {
                            // For now - ignore forIterableEnvelope
                            return Iterable<RANGE_TYPE>::template MakeSharedPtr<MyIterableRep_> (*this);
                        }
                    };
                    MyIterable_ (const MyBijection_& b)
                        : Iterable<RANGE_TYPE> (Iterable<RANGE_TYPE>::template MakeSharedPtr<MyIterableRep_> (b))
                    {
                    }
                };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                auto rep = dynamic_pointer_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep> (const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
                auto rep = const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
                return MyIterable_ (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> (rep));
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            bool Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
            {
                if (this == &rhs) {
                    return true;
                }
                // If sizes differ, the Bijections differ
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }
                // Since both sides are the same size, we can iterate over one, and make sure the key/values in the first
                // are present, and with the same Bijection in the second.
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    Memory::Optional<RangeType> tmp;
                    if (not rhs.Lookup (i->first, &tmp) or not RangeEqualsCompareFunctionType::Equals (*tmp, i->second)) {
                        return false;
                    }
                }
                return true;
            }

            /*
             ********************************************************************************
             **************************** Bijection operators *******************************
             ********************************************************************************
             */
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline bool operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Bijection_inl_ */
