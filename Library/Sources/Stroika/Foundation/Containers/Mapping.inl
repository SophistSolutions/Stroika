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
                : inherited (Concrete::mkMapping_Default<KEY_TYPE, VALUE_TYPE, TRAITS> ())
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& m)
                : inherited (static_cast<const inherited&> (m))
            {
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const CONTAINER_OF_PAIR_KEY_T& cp)
                : inherited (Concrete::mkMapping_Default<KEY_TYPE, VALUE_TYPE, TRAITS> ())
            {
                AddAll (cp);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (Concrete::mkMapping_Default<KEY_TYPE, VALUE_TYPE, TRAITS> ())
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
                return _GetRep ().Lookup (key, item);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  Memory::Optional<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key) const
            {
                // @todo change virutal API so this trick not needed
                ValueType   r;  // find way so we dont require a default CTOR - probably change virtual AP
                if (_GetRep ().Lookup (key, &r)) {
                    return r;
                }
                return Memory::Optional<VALUE_TYPE> ();
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
                for (pair<KEY_TYPE, VALUE_TYPE> t : *this) {
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
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (i->first, i->second);
                }
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_PAIR_KEY_T& items)
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
                _GetRep ().Remove (key);
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Remove (const Iterator<pair<KEY_TYPE, VALUE_TYPE>>& i)
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
                return CONTAINER_OF_Key_T (this->begin (), this->end ());
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
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    ValueType   tmp;    // @todo THIS IS BAD - because of this we must hange the REP::Lookup () API to return OPTIONAL
                    if (not rhs.Lookup (i->first, &tmp) or not (tmp == i->second)) {
                        return false;
                    }
                }
                return true;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Mapping_inl_ */
