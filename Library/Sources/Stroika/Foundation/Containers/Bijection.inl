/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Bijection_inl_
#define _Stroika_Foundation_Containers_Bijection_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Bijection_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             *************** Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> *********************
             ********************************************************************************
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection ()
                : inherited (move (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& src)
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&& src)
            // dont udnrestand why move doesnt work
                : inherited (move (src))
                //: inherited (move<inherited> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const initializer_list<pair<DOMAIN_TYPE, RANGE_TYPE>>& src)
                : inherited (move (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const CONTAINER_OF_PAIR_KEY_T& src)
                : inherited (move (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (move (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const _SharedPtrIRep& src)
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (_SharedPtrIRep&& src)
                : inherited (move (src))
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::~Bijection ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  const typename  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep&    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Iterable<DOMAIN_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Preimage () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Preimage ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Iterable<RANGE_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Image () const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Image ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key, RangeType* item) const
            {
                if (item == nullptr) {
                    return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
                }
                else {
                    Memory::Optional<RangeType> tmp;
                    if (_SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key, Memory::Optional<RangeType>* item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, item);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Memory::Optional<RANGE_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key) const
            {
                Memory::Optional<RANGE_TYPE>   r;
                bool    result = _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (ArgByValueType<DomainType> key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  RANGE_TYPE   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::LookupValue (ArgByValueType<DomainType> key, ArgByValueType<RangeType> defaultValue) const
            {
                Memory::Optional<RANGE_TYPE>   r    =   Lookup (key);
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key, DomainType* item) const
            {
                if (item == nullptr) {
                    return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().InverseLookup (key, nullptr);
                }
                else {
                    Memory::Optional<DomainType> tmp;
                    if (_SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().InverseLookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key, Memory::Optional<DomainType>* item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().InverseLookup (key, item);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Memory::Optional<DOMAIN_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key) const
            {
                Memory::Optional<DOMAIN_TYPE>   r;
                bool    result = _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().InverseLookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (ArgByValueType<RangeType> key, nullptr_t) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().InverseLookup (key, nullptr);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  DOMAIN_TYPE   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookupValue (ArgByValueType<RangeType> key, ArgByValueType<DomainType> defaultValue) const
            {
                Memory::Optional<DOMAIN_TYPE>   r    =   InverseLookup (key);
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::ContainsDomainElement (ArgByValueType<DomainType> key) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::ContainsRangeElement (ArgByValueType<RangeType> v) const
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
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (ArgByValueType<DomainType> key, ArgByValueType<RangeType> newElt)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (key, newElt);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (const pair<DomainType, RangeType>& p)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (p.first, p.second);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename KEYVALUEPAIR, typename ENABLE_IF_TEST>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (KEYVALUEPAIR p)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (p.fKey, p.fValue);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEYVALUE>
            void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_KEYVALUE, typename ENABLE_IF>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_KEYVALUE& items)
            {
                /*
                 *  Note - unlike some other containers - we dont need to check for this != &s because if we
                 *  attempt to add items that already exist, it would do nothing to our iteration
                 *  and therefore not lead to an infinite loop.
                 */
                AddAll (std::begin (items), std::end (items));
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveDomainElement (ArgByValueType<DomainType> d)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveDomainElement (d);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveRangeElement (ArgByValueType<RangeType> r)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().RemoveRangeElement (r);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (i);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename   TARGET_CONTAINER>
            TARGET_CONTAINER    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Inverse () const
            {
                TARGET_CONTAINER    r;
                for (pair<DOMAIN_TYPE, RANGE_TYPE> i : *this) {
                    r.Add (i.second, i.first);
                }
                return r;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename CONTAINER_PAIR_RANGE_DOMAIN>
            inline  CONTAINER_PAIR_RANGE_DOMAIN Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::As() const
            {
                return CONTAINER_PAIR_RANGE_DOMAIN (this->begin (), this->end ());
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Equals (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep> { &rhs } ._ConstGetRep ());
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::clear ()
            {
                RemoveAll ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator+= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                AddAll (items);
                return *this;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator-= (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                RemoveAll (items);
                return *this;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


            /*
             ********************************************************************************
             ************ Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep *****************
             ********************************************************************************
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
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
                    Memory::Optional<RangeType>   tmp;
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
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool  operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Bijection_inl_ */
