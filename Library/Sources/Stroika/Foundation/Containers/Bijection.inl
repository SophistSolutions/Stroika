/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#include    "Concrete/Bijection_Factory.h"

#ifndef _Stroika_Foundation_Containers_Bijection_inl_
#define _Stroika_Foundation_Containers_Bijection_inl_

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
                : inherited (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                RequireMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const std::initializer_list<pair<DOMAIN_TYPE, RANGE_TYPE>>& src)
                : inherited (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (src);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const CONTAINER_OF_PAIR_KEY_T& src)
                : inherited (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (src);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (Concrete::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (start, end);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Bijection (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                RequireMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  const typename  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep&    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_GetRep () const
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  typename    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep&     Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_GetRep ()
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Iterable<DOMAIN_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Preimage () const
            {
                return _GetRep ().Preimage ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Iterable<RANGE_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Image () const
            {
                return _GetRep ().Image ();
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (DomainType key, RangeType* item) const
            {
                if (item == nullptr) {
                    return _GetRep ().Lookup (key, nullptr);
                }
                else {
                    Memory::Optional<RangeType> tmp;
                    if (_GetRep ().Lookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (DomainType key, Memory::Optional<RangeType>* item) const
            {
                return _GetRep ().Lookup (key, item);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Memory::Optional<RANGE_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (DomainType key) const
            {
                Memory::Optional<RANGE_TYPE>   r;
                bool    result = _GetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Lookup (DomainType key, nullptr_t) const
            {
                return _GetRep ().Lookup (key, nullptr);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  RANGE_TYPE   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::LookupValue (DomainType key, RangeType defaultValue) const
            {
                Memory::Optional<RANGE_TYPE>   r    =   Lookup (key);
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (RangeType key, DomainType* item) const
            {
                if (item == nullptr) {
                    return _GetRep ().InverseLookup (key, nullptr);
                }
                else {
                    Memory::Optional<DomainType> tmp;
                    if (_GetRep ().InverseLookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (RangeType key, Memory::Optional<DomainType>* item) const
            {
                return _GetRep ().InverseLookup (key, item);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Memory::Optional<DOMAIN_TYPE>    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (RangeType key) const
            {
                Memory::Optional<DOMAIN_TYPE>   r;
                bool    result = _GetRep ().InverseLookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookup (RangeType key, nullptr_t) const
            {
                return _GetRep ().InverseLookup (key, nullptr);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  DOMAIN_TYPE   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::InverseLookupValue (RangeType key, DomainType defaultValue) const
            {
                Memory::Optional<DOMAIN_TYPE>   r    =   InverseLookup (key);
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::ContainsDomainElement (DomainType key) const
            {
                return _GetRep ().Lookup (key, nullptr);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::ContainsRangeElement (RangeType v) const
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
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (DomainType key, RangeType newElt)
            {
                _GetRep ().Add (key, newElt);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (pair<DomainType, RangeType> p)
            {
                _GetRep ().Add (p.first, p.second);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            template    <typename KEYVALUEPAIR, typename ENABLE_IF_TEST>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Add (KEYVALUEPAIR p)
            {
                _GetRep ().Add (p.fKey, p.fValue);
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
            template    <typename CONTAINER_OF_KEYVALUE>
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
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveDomainElement (DomainType d)
            {
                _GetRep ().RemoveDomainElement (d);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveRangeElement (RangeType r)
            {
                _GetRep ().RemoveRangeElement (r);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
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
                return (_GetRep ().Equals (rhs._GetRep ()));
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  void    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::clear ()
            {
                _GetRep ().RemoveAll ();
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
            inline  bool  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  bool    Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }


            /*
             ********************************************************************************
             ************ Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep *****************
             ********************************************************************************
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            inline  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep::~_IRep ()
            {
            }
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
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    Memory::Optional<RangeType>   tmp;
                    if (not rhs.Lookup (i->first, &tmp) or not RangeEqualsCompareFunctionType::Equals (*tmp, i->second)) {
                        return false;
                    }
                }
                return true;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Bijection_inl_ */
