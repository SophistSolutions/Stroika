/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_inl_
#define _Stroika_Foundation_Containers_Collection_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#include    "Concrete/Collection_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ******************************** Collection<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Collection<T>::Collection ()
                : inherited (Concrete::Collection_Factory<T>::mk ())
            {
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Collection<T>::Collection (const Collection<T>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Collection<T>::Collection (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Collection<T>::Collection (const std::initializer_list<T>& src)
                : inherited (Concrete::Collection_Factory<T>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (src);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Collection<T>::Collection (const CONTAINER_OF_T& src)
                : inherited (Concrete::Collection_Factory<T>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (src);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
#if     qDebug
            template    <typename T>
            Collection<T>::~Collection ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _GetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename T>
            inline  const typename  Collection<T>::_IRep&    Collection<T>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);   // static_cast<> should perform better, but assert to verify safe
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename    Collection<T>::_IRep&  Collection<T>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);   // static_cast<> should perform better, but assert to verify safe
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            bool    Collection<T>::Contains (T item) const
            {
                for (auto i : *this) {
                    if (EQUALS_COMPARER::Equals (i, item)) {
                        return true;
                    }
                }
                return false;
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Collection<T>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Collection<T>::AddAll (const CONTAINER_OF_T& c)
            {
                /*
                 * Because adding items to a Collection COULD result in those items appearing in a running iterator,
                 * for the corner case of s.AddAll(s) - we want to assure we don't infinite loop.
                 */
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    CONTAINER_OF_T  tmp =   c;
                    AddAll (std::begin (tmp), std::end (tmp));
                }
                else {
                    AddAll (std::begin (c), std::end (c));
                }
            }
            template    <typename T>
            inline  void    Collection<T>::Add (T item)
            {
                _GetRep ().Add (item);
                Ensure (not this->IsEmpty ());
            }
            template    <typename T>
            inline  void    Collection<T>::Update (const Iterator<T>& i, T newValue)
            {
                _GetRep ().Update (i, newValue);
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  void  Collection<T>::Remove (T item)
            {
                for (Iterator<T> i = this->begin (); i != this->end (); ++i) {
                    if (EQUALS_COMPARER::Equals (*i, item)) {
                        _GetRep ().Remove (i);
                        return;
                    }
                }
            }
            template    <typename T>
            inline  void    Collection<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T, typename EQUALS_COMPARER>
            void    Collection<T>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove<EQUALS_COMPARER> (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T, typename EQUALS_COMPARER>
            inline  void    Collection<T>::RemoveAll (const CONTAINER_OF_T& c)
            {
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    RemoveAll ();
                }
                else {
                    RemoveAll (std::begin (c), std::end (c));
                }
            }
            template    <typename T>
            inline  void    Collection<T>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T>
            inline  void    Collection<T>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            inline  void    Collection<T>::erase (T item)
            {
                Remove<EQUALS_COMPARER> (item);
            }
            template    <typename T>
            inline  void    Collection<T>::erase (const Iterator<T>& i)
            {
                Remove (i);
            }
            template    <typename T>
            Collection<T>    Collection<T>::EachWith (const function<bool(const T& item)>& doToElement) const
            {
                Collection<T>   result;
                for (T i : *this) {
                    if (doToElement (i)) {
                        result.Add (i);
                    }
                }
                return result;
            }
            template    <typename T>
            inline  Collection<T>& Collection<T>::operator+= (T item)
            {
                Add (item);
                return *this;
            }
            template    <typename T>
            inline  Collection<T>& Collection<T>::operator+= (const Iterable<T>& items)
            {
                AddAll (items);
                return *this;
            }


            /*
             ********************************************************************************
             ****************************** Collection<T>::_IRep ****************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Collection<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Collection<T>::_IRep::~_IRep ()
            {
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_Collection_inl_ */
