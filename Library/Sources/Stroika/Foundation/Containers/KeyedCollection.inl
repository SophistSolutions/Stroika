/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_KeyedCollection_inl_
#define _Stroika_Foundation_Containers_KeyedCollection_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <set>

#include "../Debug/Assertions.h"
#include "../Traversal/Generator.h"
#include "Factory/Mapping_Factory.h"

namespace Stroika::Foundation::Containers {

#if 0
        /*
            ********************************************************************************
            ***************** Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> ************************
            ********************************************************************************
            */
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping ()
            : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS> () ()))
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
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
            : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::New ()))
        {
            _AssertRepValidType ();
            AddAll (src);
            _AssertRepValidType ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
            : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::New ()))
        {
            _AssertRepValidType ();
            AddAll (src);
            _AssertRepValidType ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const map<KEY_TYPE, VALUE_TYPE>& src)
            : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::New ()))
        {
            _AssertRepValidType ();
            AddAll (src);
            _AssertRepValidType ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        template    <typename TRAITS2>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS2>& src)
            : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::New ()))
        {
            _AssertRepValidType ();
            AddAll (src);
            _AssertRepValidType ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        template    <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const CONTAINER_OF_ADDABLE& src)
            : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::New ()))
        {
            _AssertRepValidType ();
            AddAll (src);
            _AssertRepValidType ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        template    <typename COPY_FROM_ITERATOR_KEY_T>
        Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
            : inherited (move (Concrete::Mapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::New ()))
        {
            _AssertRepValidType ();
            AddAll (start, end);
            _AssertRepValidType ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (const _KeyedCollectionRepSharedPtr& rep)
            : inherited (rep)
        {
            RequireNotNull (rep);
            _AssertRepValidType ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping (_KeyedCollectionRepSharedPtr&& rep)
            : inherited (move (rep))
        {
            //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
            _AssertRepValidType ();
        }
#if qDebug
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
        inline  Iterable<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Values () const
        {
            return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Values ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, ValueType* item) const
        {
            if (item == nullptr) {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, nullptr);
            }
            else {
                optional<ValueType> tmp;
                if (_SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &tmp)) {
                    *item = *tmp;
                    return true;
                }
                return false;
            }
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  bool    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key, optional<ValueType>* item) const
        {
            return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, item);
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  optional<VALUE_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::Lookup (KeyType key) const
        {
            optional<VALUE_TYPE>   r;
            bool    result = _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Lookup (key, &r);
            Ensure (result == r.has_value ());
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
            optional<VALUE_TYPE>   r    { Lookup (key) };
            return r.has_value () ? *r : defaultValue;
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
        template    <typename CONTAINER_OF_KEYVALUE, typename ENABLE_IF>
        inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::AddAll (const CONTAINER_OF_KEYVALUE& items)
        {
            /*
             *  Note - unlike other containers - we don't need to check for this != &s because if we
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
        template    <typename CONTAINER_OF_ADDABLE>
        void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::RemoveAll (const CONTAINER_OF_ADDABLE& items)
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
        CONTAINER_OF_Key_T  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (enable_if_t <is_convertible_v <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>, int> usesInsertPair) const
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
        inline  CONTAINER_OF_Key_T  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::As_ (enable_if_t < !is_convertible_v <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>, int >) const
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
            if (&lhs._ConstGetRep () == &rhs._ConstGetRep ()) {
                // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
                return true;
            }
            // Check length, so we don't need to check both iterators for end/done
            if (lhs._ConstGetRep ().GetLength () != rhs.GetLength ()) {
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
                    if (not o.has_value () or not VALUE_EQUALS_COMPARER::Equals (*o, li->fValue)) {
                        return false;
                    }
                    // if the keys were different, we must check the reverse direction too
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
        inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::erase (KeyType key)
        {
            Remove (key);
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::erase (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i)
        {
            Remove (i);
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        inline  void    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::clear ()
        {
            RemoveAll ();
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        template    <typename CONTAINER_OF_ADDABLE>
        Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>       Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator+ (const CONTAINER_OF_ADDABLE& items) const
        {
            Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>   result = *this;
            result.AddAll (items);
            return result;
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        template    <typename CONTAINER_OF_ADDABLE>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>&    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator+= (const CONTAINER_OF_ADDABLE& items)
        {
            AddAll (items);
            return *this;
        }
        template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
        template    <typename CONTAINER_OF_ADDABLE>
        inline  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>&    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator-= (const CONTAINER_OF_ADDABLE& items)
        {
            RemoveAll (items);
            return *this;
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
        Iterable<KEY_TYPE>    Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep::_Keys_Reference_Implementation () const
        {
            struct  MyIterable_ : Iterable<KEY_TYPE> {
                using   MyMapping_      =   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>;
                struct  MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_>  {
                    using   inherited       = typename Traversal::IterableFromIterator<KEY_TYPE>::_Rep;
                    using   _SharedPtrIRep  = typename Iterable<KEY_TYPE>::_SharedPtrIRep;
                    MyMapping_  fMapping_;
                    MyIterableRep_ (const MyMapping_& map)
                        : inherited ()
                        , fMapping_ (map)
                    {
                    }
                    virtual Iterator<KEY_TYPE>     MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>> (fMapping_.MakeIterator ());
                        auto getNext = [myContext] () -> optional<KEY_TYPE> {
                            if (myContext->Done ())
                            {
                                return optional<KEY_TYPE> ();
                            }
                            else
                            {
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
                        return Iterable<VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                    }
                };
                MyIterable_ (const MyMapping_& map)
                    : Iterable<KEY_TYPE> (Iterable<VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (map)))
                {
                }
            };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
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
                struct  MyIterableRep_ : Traversal::IterableFromIterator<VALUE_TYPE>::_Rep , public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                    using   inherited       = typename Traversal::IterableFromIterator<VALUE_TYPE>::_Rep;
                    using   _SharedPtrIRep  = typename Iterable<VALUE_TYPE>::_SharedPtrIRep;
                    MyMapping_  fMapping_;
                    MyIterableRep_ (const MyMapping_& map)
                        : inherited ()
                        , fMapping_ (map)
                    {
                    }
                    virtual Iterator<VALUE_TYPE>     MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>> (fMapping_.MakeIterator ());
                        auto getNext = [myContext] () -> optional<VALUE_TYPE> {
                            if (myContext->Done ())
                            {
                                return optional<VALUE_TYPE> ();
                            }
                            else
                            {
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
                        return Iterable<VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                    }
                };
                MyIterable_ (const MyMapping_& map)
                    : Iterable<VALUE_TYPE> (Iterable<VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (map)))
                {
                }
            };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
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
#endif

}

#endif /* _Stroika_Foundation_Containers_KeyedCollection_inl_ */
