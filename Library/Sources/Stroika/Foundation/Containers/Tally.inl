/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_inl_
#define _Stroika_Foundation_Containers_Tally_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"
#include    "Tally.h"

#include    "Concrete/Tally_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             * Protected helper class to convert from an iterator of TallyEntries
             * to an iterator over individual items.
             *
             *  @todo   BUG - _TallyEntryToItemIteratorHelperRep should stop N times where N is the value of count
             *          in the source iterator!
             *
             *          But document and verify the calling code is expecting this.
             */
            template    <typename T, typename TRAITS>
            class  Tally<T, TRAITS>::_IRep::_TallyEntryToItemIteratorHelperRep : public Iterator<T>::IRep {
            private:
                typedef typename    Iterator<T>::IRep   inherited;

            public:
                _TallyEntryToItemIteratorHelperRep (const Iterator<TallyEntry<T>>& delegateTo)
                    : inherited ()
                    , fDelegateTo_ (delegateTo)
                {
                }
                virtual void    More (Memory::Optional<T>* result, bool advance) override
                {
                    bool done = fDelegateTo_.Done ();
                    if (not done and advance) {
                        fDelegateTo_++;
                        done = fDelegateTo_.Done ();
                    }
                    if (done) {
                        result->clear ();
                    }
                    else {
                        *result = fDelegateTo_->fItem;
                    }
                }
                virtual typename Iterator<T>::SharedIRepPtr Clone () const override
                {
                    return typename Iterator<T>::SharedIRepPtr (new _TallyEntryToItemIteratorHelperRep (Iterator<TallyEntry<T>> (fDelegateTo_)));
                }
                virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override
                {
                    AssertNotImplemented ();
                    return false;
                }

            private:
                Iterator<TallyEntry<T>> fDelegateTo_;
            };


            /*
             *  @todo SERIOUS REFRENCE COUNTING BUG HERE - NOTHING HANIGN ONTL LIFETIME OF TALLYREP!
            */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep::_ElementsIterableHelperRep : public Iterable<T>::_IRep {
            private:
                typename Tally<T, TRAITS>::_IRep&   fTallyRep_;
            public:
                _ElementsIterableHelperRep (const typename Tally<T, TRAITS>::_IRep& rhs)
                    : fTallyRep_ (rhs)
                {
                }
                virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override
                {
                    return Iterable<T>::_SharedPtrIRep (new _ElementsIterableHelperRep (fTallyRep_));
                }
                virtual Iterator<T>                             MakeIterator () const override
                {
                    return Iterator<T> (new _TallyEntryToItemIteratorHelperRep (fTallyRep_->MakeIterator ()));
                }
                virtual size_t                                  GetLength () const override
                {
                    size_t  n = 0;
                    for (Iterator<TallyEntry> i = fTallyRep_->MakeIterator (); not i.Done (); ++i) {
                        n += (*i).fCount;
                    }
                    return n;
                }
                virtual bool                                    IsEmpty () const override
                {
                    return fTallyRep_->IsEmpty ();  // subtle but since we never store items with a zero count, this is OK
                }
                virtual void                                    Apply (_APPLY_ARGTYPE doToElement) const override
                {
                    return this->_Apply (doToElement);
                }
                virtual Iterator<T>                             ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
            };


            /*
             */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep::_UniqueElementsIterableHelperRep : public Iterable<T>::_IRep {
            private:
                typename Tally<T, TRAITS>::_IRep&   fTallyRep_;
            public:
                _UniqueElementsIterableHelperRep (const typename Tally<T, TRAITS>::_IRep& rhs)
                    : fTallyRep_ (rhs)
                {
                }
                virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override
                {
                    return Iterable<T>::_SharedPtrIRep (new _ElementsIterableHelperRep (fTallyRep_));
                }
                virtual Iterator<T>                             MakeIterator () const override
                {
                    return Iterator<T> (new _TallyEntryToItemIteratorHelperRep (fTallyRep_->MakeIterator ()));
                }
                virtual size_t                                  GetLength () const override
                {
                    return fTallyRep_->GetLength ();
                }
                virtual bool                                    IsEmpty () const override
                {
                    return fTallyRep_->IsEmpty ();
                }
                virtual void                                    Apply (_APPLY_ARGTYPE doToElement) const override
                {
                    return this->_Apply (doToElement);
                }
                virtual Iterator<T>                             ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
            };


            /*
             ********************************************************************************
             ****************************** TallyEntry<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  TallyEntry<T>::TallyEntry (T item)
                : fItem (item)
                , fCount (1)
            {
            }
            template    <typename T>
            inline  TallyEntry<T>::TallyEntry (T item, size_t count)
                : fItem (item)
                , fCount (count)
            {
            }
            template    <typename T>
            inline bool   TallyEntry<T>::operator== (const TallyEntry<T>& rhs)  const
            {
                return (fCount == rhs.fCount and fItem == rhs.fItem);
            }
            template    <typename T>
            inline bool   TallyEntry<T>::operator!= (const TallyEntry<T>& rhs)  const
            {
                return not (operator== (rhs));
            }


            /*
             ********************************************************************************
             ************************* Tally<T, TRAITS>::_IRep ******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Tally<T, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            bool  Tally<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
            {
                if (this == &rhs) {
                    return true;
                }
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    if (i->fCount != rhs.TallyOf (i->fItem)) {
                        return false;
                    }
                }
                return true;
            }
            template    <typename T, typename TRAITS>
            Iterable<T>  Tally<T, TRAITS>::_IRep::_Elements_Reference_Implementation (const _IRep& rhs) const
            {
                return Iterable<T> (new _ElementsIterableHelperRep (rhs));
            }
            template    <typename T, typename TRAITS>
            Iterable<T>  Tally<T, TRAITS>::_IRep::_UniqueElements_Reference_Implementation (const _IRep& rhs) const
            {
                return Iterable<T> (new _UniqueElementsIterableHelperRep (rhs));
            }


            /*
             ********************************************************************************
             ******************************** Tally<T, TRAITS> ******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            Tally<T, TRAITS>::Tally ()
                : inherited (Concrete::Tally_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Tally<T, TRAITS>::Tally (const Tally<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Tally<T, TRAITS>::Tally (const CONTAINER_OF_T& src)
                : inherited (Concrete::Tally_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                operator+= (src);
            }
            template    <typename T, typename TRAITS>
            inline  Tally<T, TRAITS>::Tally (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            Tally<T, TRAITS>::Tally (const std::initializer_list<T>& s)
                : inherited (Concrete::Tally_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (begin (s), end (s));
            }
            template    <typename T, typename TRAITS>
            Tally<T, TRAITS>::Tally (const std::initializer_list<TallyEntry<T>>& s)
                : inherited (Concrete::Tally_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (begin (s), end (s));
            }
            template    <typename T, typename TRAITS>
            Tally<T, TRAITS>::Tally (const T* start, const T* end)
                : inherited (Concrete::Tally_Factory<T, TRAITS>::mk ())
            {
                AddAll (start, end);
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            Tally<T, TRAITS>::Tally (const TallyEntry<T>* start, const TallyEntry<T>* end)
                : inherited (Concrete::Tally_Factory<T, TRAITS>::mk ())
            {
                AddAll (start, end);
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  const typename Tally<T, TRAITS>::_IRep&  Tally<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename Tally<T, TRAITS>::_IRep&        Tally<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            void   Tally<T, TRAITS>::RemoveAll (T item)
            {
                Remove (item, TallyOf (item));
            }
            template    <typename T, typename TRAITS>
            size_t Tally<T, TRAITS>::TotalTally () const
            {
                size_t sum = 0;
                for (auto i = this->begin (); i != this->end (); ++i) {
                    sum += (*i).fCount;
                }
                return sum;
            }
            template    <typename T, typename TRAITS>
            inline  bool  Tally<T, TRAITS>::Equals (const Tally<T, TRAITS>& rhs) const
            {
                return _GetRep ().Equals (rhs._GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  bool    Tally<T, TRAITS>::Contains (T item) const
            {
                return _GetRep ().Contains (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            inline  Iterator<T>    Tally<T, TRAITS>::MakeBagIterator () const
            {
                return _GetRep ().MakeBagIterator ();
            }
            template    <typename T, typename TRAITS>
            inline  Iterator<T>    Tally<T, TRAITS>::bagbegin () const
            {
                return MakeBagIterator ();
            }
            template    <typename T, typename TRAITS>
            inline  Iterator<T>       Tally<T, TRAITS>::bagend () const
            {
                return Iterator<T>::GetEmptyIterator ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::Add (T item)
            {
                _GetRep ().Add (item, 1);
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::Add (T item, size_t count)
            {
                _GetRep ().Add (item, count);
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::Add (const TallyEntry<T>& item)
            {
                _GetRep ().Add (item.fItem, item.fCount);
            }
            template    <typename T, typename TRAITS>
            void   Tally<T, TRAITS>::AddAll (const T* start, const T* end)
            {
                for (const T* i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::AddAll (const TallyEntry<T>* start, const TallyEntry<T>* end)
            {
                for (auto i = start; i != end; ++i) {
                    _GetRep ().Add (i->fItem, i->fEnd);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            void    Tally<T, TRAITS>::AddAll (const CONTAINER_OF_T& src)
            {
                operator+= (src);
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::Remove (T item)
            {
                _GetRep ().Remove (item, 1);
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::Remove (T item, size_t count)
            {
                _GetRep ().Remove (item, count);
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::Remove (const Iterator<TallyEntry<T>>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T, typename TRAITS>
            inline  void    Tally<T, TRAITS>::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
            {
                _GetRep ().UpdateCount (i, newCount);
            }
            template    <typename T, typename TRAITS>
            inline  size_t  Tally<T, TRAITS>::TallyOf (T item) const
            {
                return _GetRep ().TallyOf (item);
            }
            template    <typename T, typename TRAITS>
            inline  Tally<T, TRAITS>&   Tally<T, TRAITS>::operator+= (T item)
            {
                _GetRep ().Add (item, 1);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  bool   Tally<T, TRAITS>::operator== (const Tally<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Tally<T, TRAITS>::operator!= (const Tally<T, TRAITS>& rhs) const
            {
                return not (Equals (rhs));
            }
            template    <typename T, typename TRAITS>
            Tally<T, TRAITS>&  Tally<T, TRAITS>::operator+= (const Tally<T, TRAITS>& t)
            {
                for (auto i = t.begin (); i != t.end (); ++i) {
                    Add (i->fItem, i->fCount);
                }
                return *this;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Tally_inl_ */
