/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#include    "Concrete/Mapping_Factory.h"

#ifndef _Stroika_Foundation_Containers_Mapping_inl_
#define _Stroika_Foundation_Containers_Mapping_inl_

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
                : inherited (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ())
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& m)
                : inherited (static_cast<const inherited&> (m))
            {
            }
#if     qCompilerAndStdLib_Supports_initializer_lists
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const std::initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& m)
                : inherited (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (m);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const std::initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& m)
                : inherited (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll_pair (m);
            }
#endif
#if     !qBROKEN_MAPPING_CTOR_OF_STDMAP
#if 1
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const map<KEY_TYPE, VALUE_TYPE>& m)
                : inherited (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ())
            {
                AddAll_pair (m);
            }
#else
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename PR, typename ALLOC>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const map<KEY_TYPE, VALUE_TYPE, PR, ALLOC>& m)
                : inherited (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ())
            {
                AddAll_pair (m);
            }
#endif
#endif
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const CONTAINER_OF_PAIR_KEY_T& cp)
                : inherited (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ())
            {
                AddAll (cp);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ())
            {
                AddAll (start, end);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
            }
#if     !qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
            {
                inherited::operator= (src);
                return *this;
            }
#endif
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  const typename  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep&    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_GetRep () const
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  typename    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep&     Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_GetRep ()
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Iterable<KEY_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Keys () const
            {
                return _GetRep ().Keys ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, ValueType* item) const
            {
                if (item == nullptr) {
                    return _GetRep ().Lookup (key, nullptr);
                }
                else {
                    Memory::Optional<ValueType> tmp;
                    if (_GetRep ().Lookup (key, &tmp)) {
                        *item = *tmp;
                        return true;
                    }
                    return false;
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, Memory::Optional<ValueType>* item) const
            {
                return _GetRep ().Lookup (key, item);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Memory::Optional<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key) const
            {
                Memory::Optional<VALUE_TYPE>   r;
                bool    result = _GetRep ().Lookup (key, &r);
                Ensure (result == r.IsPresent ());
                return r;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, nullptr_t) const
            {
                return _GetRep ().Lookup (key, nullptr);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  VALUE_TYPE   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::LookupValue (KeyType key, ValueType defaultValue) const
            {
                Memory::Optional<VALUE_TYPE>   r    =   Lookup (key);
                return r.IsPresent () ? *r : defaultValue;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::ContainsKey (KeyType key) const
            {
                return _GetRep ().Lookup (key, nullptr);
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
                _GetRep ().Add (key, newElt);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEYVALUE>
            void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (i->fKey, i->fValue);
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
            template    <typename COPY_FROM_ITERATOR_PAIR_KEY_T>
            void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll_pair (COPY_FROM_ITERATOR_PAIR_KEY_T start, COPY_FROM_ITERATOR_PAIR_KEY_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (i->first, i->second);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll_pair (const CONTAINER_OF_PAIR_KEY_T& items)
            {
                /*
                 *  Note - unlike Bag<T> - we dont need to check for this != &s because if we
                 *  attempt to add items that already exist, it would do nothing to our iteration
                 *  and therefore not lead to an infinite loop.
                 */
                AddAll_pair (std::begin (items), std::end (items));
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (KeyType key)
            {
                _GetRep ().Remove (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
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
            template    <typename   CONTAINER_OF_Key_T>
            inline  CONTAINER_OF_Key_T  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::As () const
            {
                // for now this is (documented) to only support std::map<KEY_TYPE, VALUE_TYPE> ()
                // but we should extend to do better - somehow using typetraits?...
#if 1
                CONTAINER_OF_Key_T  result;
                for (auto i : *this) {
                    result.insert (typename CONTAINER_OF_Key_T::value_type (i.fKey, i.fValue));
                }
                return result;
#else
                return CONTAINER_OF_Key_T (this->begin (), this->end ());
#endif
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename   CONTAINER_OF_Key_T>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::As (CONTAINER_OF_Key_T* into) const
            {
                RequireNotNull (into);
                *into = CONTAINER_OF_Key_T (this->begin (), this->end ());
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  bool  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Equals (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const
            {
                return (_GetRep ().Equals (rhs._GetRep ()));
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::clear ()
            {
                _GetRep ().RemoveAll ();
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


            /*
             ********************************************************************************
             ***************** Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep *****************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::~_IRep ()
            {
            }
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
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    Memory::Optional<ValueType>   tmp;
                    if (not rhs.Lookup (i->fKey, &tmp) or not ValueEqualsCompareFunctionType::Equals (*tmp, i->fValue)) {
                        return false;
                    }
                }
                return true;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Mapping_inl_ */
