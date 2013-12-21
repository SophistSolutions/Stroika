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


#if 0
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


            /**
            */
            template    <typename T, typename TRAITS>
            class  Tally<T, TRAITS>::_IRep::_TallyEntryToItemIteratorWithCountsHelperRep : public _TallyEntryToItemIteratorHelperRep {
            private:
                typedef _TallyEntryToItemIteratorHelperRep  inherited;
            private:
                size_t              fCountMoreTimesToGoBeforeAdvance;
                Memory::Optional<T> fSaved2Return_;
            public:
                _TallyEntryToItemIteratorWithCountsHelperRep (const Iterator<TallyEntry<T>>& delegateTo)
                    : inherited (delegateTo)
                    , fCountMoreTimesToGoBeforeAdvance (0)
                    , fSaved2Return_ ()
                {
                }
                _TallyEntryToItemIteratorWithCountsHelperRep (const _TallyEntryToItemIteratorWithCountsHelperRep& rhs)
                    : inherited (Iterator<TallyEntry<T>> (rhs.fDelegateTo_))
                    , fCountMoreTimesToGoBeforeAdvance (rhs.fCountMoreTimesToGoBeforeAdvance)
                    , fSaved2Return_ (rhs.fSaved2Return_)
                {
                }
                virtual void    More (Memory::Optional<T>* result, bool advance) override
                {
                    if (fCountMoreTimesToGoBeforeAdvance > 0) {
                        *result = fSaved2Return_;
                        if (advance) {
                            fCountMoreTimesToGoBeforeAdvance--;
                        }
                    }
                    else {
                        inherited::More (result, advance);
                        if (advance) {
                            fSaved2Return_ = *result;
                            if (fSaved2Return_.IsPresent ()) {
                                fCountMoreTimesToGoBeforeAdvance = fDelegateTo_->fCount - 1;
                            }
                        }
                    }
                }
                virtual typename Iterator<T>::SharedIRepPtr Clone () const override
                {
                    return typename Iterator<T>::SharedIRepPtr (new _TallyEntryToItemIteratorWithCountsHelperRep (*this));
                }
            };
#endif



            /**
             */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep::_ElementsHelper : public Iterable<T> {
                typename Iterable<T>::_SharedPtrIRep    _SharedPtrIRep;
                class   Rep_ : public Iterable<T>::_IRep {
                private:
                    Tally<T, TRAITS>   fTally_;
                public:
                    Rep_ (const Tally<T, TRAITS>& iterateOverTally)
                        : fTally_ (iterateOverTally)
                    {
                    }
                    virtual _SharedPtrIRep    Clone () const override
                    {
                        return _SharedPtrIRep (new Rep_ (fTally_));
                    }
                    virtual Iterator<T>                             MakeIterator () const override
                    {
                        struct  TallyEntryToItemIterator : public Iterator<T> {
                            struct  Rep : public Iterator<T>::IRep {
                                typedef typename    Iterator<T>::IRep   inherited;
                                Rep (const Iterator<TallyEntry<T>>& delegateTo)
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
                                    return typename Iterator<T>::SharedIRepPtr (new Rep (Iterator<TallyEntry<T>> (fDelegateTo_)));
                                }
                                virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override
                                {
                                    AssertNotImplemented ();
                                    return false;
                                }
                                Iterator<TallyEntry<T>> fDelegateTo_;
                            };
                            TallyEntryToItemIterator (const Iterator<TallyEntry<T>>& delegateTo)
                                : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (delegateTo)))
                            {
                            }
                        };
                        return TallyEntryToItemIterator (fTally_.MakeIterator ());
                    }
                    virtual size_t                                  GetLength () const override
                    {
                        size_t  n = 0;
                        for (Iterator<TallyEntry<T>> i = fTally_.MakeIterator (); not i.Done (); ++i) {
                            n += (*i).fCount;
                        }
                        return n;
                    }
                    virtual bool                                    IsEmpty () const override
                    {
                        return fTally_.IsEmpty ();
                    }
                    virtual void                                    Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override
                    {
                        return this->_Apply (doToElement);
                    }
                    virtual Iterator<T>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override
                    {
                        return this->_ApplyUntilTrue (doToElement);
                    }
                };
            public:
                _ElementsHelper (const Tally<T, TRAITS>& iterateOverTally)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new Rep_ (iterateOverTally)))
                {
                }
            };


            /**
             */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep::_UniqueElementsHelper : public Iterable<T> {
                class   Rep_ : public Iterable<T>::_IRep {
                private:
                    Tally<T, TRAITS>   fTally_;
                public:
                    Rep_ (const Tally<T, TRAITS>& iterateOverTally)
                        : fTally_ (iterateOverTally)
                    {
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override
                    {
                        return _SharedPtrIRep (new Rep_ (fTally_));
                    }
                    virtual Iterator<T>                             MakeIterator () const override
                    {
                        struct  TallyEntryToItemIterator : public Iterator<T> {
                            struct  Rep : public Iterator<T>::IRep {
                                typedef typename    Iterator<T>::IRep   inherited;
                                Rep (const Iterator<TallyEntry<T>>& delegateTo)
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
                                    return typename Iterator<T>::SharedIRepPtr (new Rep (Iterator<TallyEntry<T>> (fDelegateTo_)));
                                }
                                virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override
                                {
                                    AssertNotImplemented ();
                                    return false;
                                }
                                Iterator<TallyEntry<T>> fDelegateTo_;
                            };
                            TallyEntryToItemIterator (const Iterator<TallyEntry<T>>& delegateTo)
                                : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (delegateTo)))
                            {
                            }
                        };
                        return TallyEntryToItemIterator (fTally_.MakeIterator ());
                    }
                    virtual size_t                                  GetLength () const override
                    {
                        return fTally_.GetLength ();
                    }
                    virtual bool                                    IsEmpty () const override
                    {
                        return fTally_.IsEmpty ();
                    }
                    virtual void                                    Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override
                    {
                        return this->_Apply (doToElement);
                    }
                    virtual Iterator<T>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override
                    {
                        return this->_ApplyUntilTrue (doToElement);
                    }
                };
            public:
                _UniqueElementsHelper (const Tally<T, TRAITS>& iterateOverTally)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new Rep_ (iterateOverTally)))
                {
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
            Iterable<T>  Tally<T, TRAITS>::_IRep::_Elements_Reference_Implementation () const
            {
                // Clone - just cuz thats the simlest, safest way to get an object whose lifetime extends
                // past this objects lifetime (without using shared_from_this).
                typename Tally<T, TRAITS>::_SharedPtrIRep   s = dynamic_pointer_cast<typename Tally<T, TRAITS>::_SharedPtrIRep::element_type> (this->Clone ());
                return _ElementsHelper (Tally<T, TRAITS> (s));
            }
            template    <typename T, typename TRAITS>
            Iterable<T>  Tally<T, TRAITS>::_IRep::_UniqueElements_Reference_Implementation () const
            {
                // Clone - just cuz thats the simlest, safest way to get an object whose lifetime extends
                // past this objects lifetime (without using shared_from_this).
                typename Tally<T, TRAITS>::_SharedPtrIRep   s = dynamic_pointer_cast<typename Tally<T, TRAITS>::_SharedPtrIRep::element_type> (this->Clone ());
                return _UniqueElementsHelper (Tally<T, TRAITS> (s));
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
                AddAll (src);
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
            size_t  Tally<T, TRAITS>::TotalTally () const
            {
                size_t sum = 0;
                for (TallyEntry<T> i : *this) {
                    sum += i.fCount;
                }
                return sum;
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   Tally<T, TRAITS>::Elements () const
            {
                return _GetRep ().Elements ();
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   Tally<T, TRAITS>::UniqueElements () const
            {
                return _GetRep ().UniqueElements ();
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
                for (auto i : src) {
                    Add (i);
                }
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
