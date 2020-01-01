/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_inl_
#define _Stroika_Foundation_Containers_Bag_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"

#include "Factory/Bag_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************************ Bag<T> ************************************
     ********************************************************************************
     */
    template <typename T>
    inline Bag<T>::Bag ()
        : inherited (Factory::Bag_Factory<T> () ())
    {
        EnsureMember (&inherited::_GetRep (), _IRep);
    }
    template <typename T>
    inline Bag<T>::Bag (const _SharedPtrIRep& rep) noexcept
        : inherited (typename inherited::_SharedPtrIRep (rep))
    {
        RequireNotNull (rep);
        EnsureMember (&inherited::_GetRep (), _IRep);
    }
    template <typename T>
    inline Bag<T>::Bag (const initializer_list<T>& src)
        : Bag ()
    {
        AddAll (src);
        EnsureMember (&inherited::_GetRep (), _IRep);
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE>
    inline Bag<T>::Bag (const CONTAINER_OF_ADDABLE& src)
        : Bag ()
    {
        AssertMember (&inherited::_GetRep (), _IRep);
        AddAll (src);
        EnsureMember (&inherited::_GetRep (), _IRep);
    }
#if qDebug
    template <typename T>
    Bag<T>::~Bag ()
    {
        if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
            // SharingState can be NULL because of MOVE semantics
            _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
        }
    }
#endif
    template <typename T>
    inline const typename Bag<T>::_IRep& Bag<T>::_ConstGetRep () const
    {
        EnsureMember (&inherited::_ConstGetRep (), _IRep); // use static_cast cuz more efficient, but validate with assertion
        return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
    }
    template <typename T>
    inline const typename Bag<T>::_IRep& Bag<T>::_GetRep () const
    {
        EnsureMember (&inherited::_GetRep (), _IRep); // static_cast<> should perform better, but assert to verify safe
        return *static_cast<const _IRep*> (&inherited::_GetRep ());
    }
    template <typename T>
    inline typename Bag<T>::_IRep& Bag<T>::_GetRep ()
    {
        EnsureMember (&inherited::_GetRep (), _IRep); // static_cast<> should perform better, but assert to verify safe
        return *static_cast<_IRep*> (&inherited::_GetRep ());
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Bag<T>::Contains (T item) const
    {
        for (auto i : *this) {
            if (EQUALS_COMPARER{}(i, item)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void Bag<T>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        for (auto i = start; i != end; ++i) {
            Add (*i);
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE>
    inline void Bag<T>::AddAll (const CONTAINER_OF_ADDABLE& c)
    {
        /*
            * Because adding items to a Bag COULD result in those items appearing in a running iterator,
            * for the corner case of s.AddAll(s) - we want to assure we don't infinite loop.
            */
        if (static_cast<const void*> (this) == static_cast<const void*> (addressof (c))) {
            CONTAINER_OF_ADDABLE tmp = c;
            AddAll (std::begin (tmp), std::end (tmp));
        }
        else {
            AddAll (std::begin (c), std::end (c));
        }
    }
    template <typename T>
    inline void Bag<T>::Add (T item)
    {
        _GetRep ().Add (item);
        Ensure (not this->IsEmpty ());
    }
    template <typename T>
    inline void Bag<T>::Update (const Iterator<T>& i, T newValue)
    {
        _GetRep ().Update (i, newValue);
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline void Bag<T>::Remove (ArgByValueType<T> item)
    {
        for (Iterator<T> i = this->begin (); i != this->end (); ++i) {
            if (EQUALS_COMPARER{}(*i, item)) {
                _GetRep ().Remove (i);
                return;
            }
        }
    }
    template <typename T>
    inline void Bag<T>::RemoveAll ()
    {
        _GetRep ().RemoveAll ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename EQUALS_COMPARER>
    void Bag<T>::RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        for (auto i = start; i != end; ++i) {
            Remove<EQUALS_COMPARER> (*i);
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, typename EQUALS_COMPARER>
    inline void Bag<T>::RemoveAll (const CONTAINER_OF_ADDABLE& c)
    {
        if (static_cast<const void*> (this) == static_cast<const void*> (addressof (c))) {
            RemoveAll ();
        }
        else {
            RemoveAll (std::begin (c), std::end (c));
        }
    }
    template <typename T>
    inline void Bag<T>::Remove (const Iterator<T>& i)
    {
        _GetRep ().Remove (i);
    }
    template <typename T>
    inline void Bag<T>::clear ()
    {
        RemoveAll ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline void Bag<T>::erase (T item)
    {
        Remove<EQUALS_COMPARER> (item);
    }
    template <typename T>
    inline void Bag<T>::erase (const Iterator<T>& i)
    {
        Remove (i);
    }
    template <typename T>
    inline Bag<T> Bag<T>::Where (const function<bool (ArgByValueType<T>)>& doToElement) const
    {
        return Iterable<T>::Where (doToElement, Bag<T>{});
    }
    template <typename T>
    inline Bag<T>& Bag<T>::operator+= (T item)
    {
        Add (item);
        return *this;
    }
    template <typename T>
    inline Bag<T>& Bag<T>::operator+= (const Iterable<T>& items)
    {
        AddAll (items);
        return *this;
    }

    /*
     ********************************************************************************
     ************************************ Bag<T>::_IRep *****************************
     ********************************************************************************
     */
    template <typename T>
    inline Bag<T>::_IRep::_IRep ()
    {
    }
    template <typename T>
    inline Bag<T>::_IRep::~_IRep ()
    {
    }

}

#endif /* _Stroika_Foundation_Containers_Bag_inl_ */
