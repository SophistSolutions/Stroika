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
             *  Protected helper class to convert from an iterator of TallyEntries
             *  to an iterator over individual items - repeating items the appropriate number of times
             *  depending on its count.
             */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep::_ElementsHelper : public Iterable<T> {
                typedef typename Iterable<T>::_SharedPtrIRep    _SharedPtrIRep;         // hack for gcc/windoze compiler quirks - lose when I can ....
                class   AdaptorRep_ : public Iterable<T>::_IRep {
                private:
                    typename    Iterable<TallyEntry<T>>::_SharedPtrIRep   fTallyRep_;
                public:
                    AdaptorRep_ (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally)
                        : fTallyRep_ (iterateOverTally)
                    {
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override
                    {
                        return _SharedPtrIRep (new AdaptorRep_ (fTallyRep_));
                    }
                    virtual Iterator<T>                             MakeIterator () const override
                    {
                        /*
                         *  Note that the constructed iterator CAN (and frequently does) live longer than the containing AdaptorRep_, which is why
                         *  it passes in the fTallyRep_ - so it can keep the object alive.
                         */
                        struct  MyIterator_ : public Iterator<T> {
                            struct  Rep : public Iterator<T>::IRep {
                                typedef typename    Iterator<T>::IRep   inherited;
                                Rep (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally, const Iterator<TallyEntry<T>>& delegateTo, size_t countMoreTimesToGoBeforeAdvance = 0, Memory::Optional<T> saved2Return = Memory::Optional<T> ())
                                    : inherited ()
                                    , fIterateOverTallyKeepalive_ (iterateOverTally)
                                    , fDelegateTo_ (delegateTo)
                                    , fCountMoreTimesToGoBeforeAdvance (countMoreTimesToGoBeforeAdvance)
                                    , fSaved2Return_ (saved2Return)
                                {
                                    if (not fDelegateTo_.Done ()) {
                                        fSaved2Return_ = fDelegateTo_->fItem;
                                        if (fSaved2Return_.IsPresent ()) {
                                            fCountMoreTimesToGoBeforeAdvance = fDelegateTo_->fCount - 1;
                                        }
                                    }
                                }
                                virtual void    More (Memory::Optional<T>* result, bool advance) override
                                {
                                    RequireNotNull (result);
                                    if (fCountMoreTimesToGoBeforeAdvance > 0) {
                                        *result = fSaved2Return_;
                                        if (advance) {
                                            fCountMoreTimesToGoBeforeAdvance--;
                                        }
                                    }
                                    else {
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
                                    return typename Iterator<T>::SharedIRepPtr (new Rep (fIterateOverTallyKeepalive_, Iterator<TallyEntry<T>> (fDelegateTo_), fCountMoreTimesToGoBeforeAdvance, fSaved2Return_));
                                }
                                virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override
                                {
                                    AssertNotImplemented ();
                                    return false;
                                }
                                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    fIterateOverTallyKeepalive_;    // subtle - hang onto in case container goes out of scope so not destroyed
                                Iterator<TallyEntry<T>>                             fDelegateTo_;
                                size_t                                              fCountMoreTimesToGoBeforeAdvance;
                                Memory::Optional<T>                                 fSaved2Return_;
                            };
                            MyIterator_ (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally, const Iterator<TallyEntry<T>>& delegateTo)
                                : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (iterateOverTally, delegateTo)))
                            {
                            }
                        };
                        return MyIterator_ (fTallyRep_, fTallyRep_->MakeIterator ());
                    }
                    virtual size_t                                  GetLength () const override
                    {
                        size_t  n = 0;
                        for (Iterator<TallyEntry<T>> i = fTallyRep_->MakeIterator (); not i.Done (); ++i) {
                            n += i->fCount;
                        }
                        return n;
                    }
                    virtual bool                                    IsEmpty () const override
                    {
                        return fTallyRep_->IsEmpty ();
                    }
                    virtual void                                    Apply (typename AdaptorRep_::_APPLY_ARGTYPE doToElement) const override
                    {
                        return this->_Apply (doToElement);
                    }
                    virtual Iterator<T>                             ApplyUntilTrue (typename AdaptorRep_::_APPLYUNTIL_ARGTYPE doToElement) const override
                    {
                        return this->_ApplyUntilTrue (doToElement);
                    }
                };
            public:
                _ElementsHelper (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new AdaptorRep_ (iterateOverTally)))
                {
                }
            };


            /**
             *  Protected helper class to convert from an iterator of TallyEntries
             *  to an iterator over unique individual items.
             */
            template    <typename T, typename TRAITS>
            class   Tally<T, TRAITS>::_IRep::_UniqueElementsHelper : public Iterable<T> {
                typedef typename Iterable<T>::_SharedPtrIRep    _SharedPtrIRep;     // hack for gcc/windoze compiler quirks - lose when I can ....
                class   AdaptorRep_ : public Iterable<T>::_IRep {
                private:
                    typename Iterable<TallyEntry<T>>::_SharedPtrIRep   fTallyRep_;
                public:
                    AdaptorRep_ (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally)
                        : fTallyRep_ (iterateOverTally)
                    {
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override
                    {
                        return _SharedPtrIRep (new AdaptorRep_ (fTallyRep_));
                    }
                    virtual Iterator<T>                             MakeIterator () const override
                    {
                        struct  MyIter_ : public Iterator<T> {
                            struct  Rep : public Iterator<T>::IRep {
                                typedef typename    Iterator<T>::IRep   inherited;
                                Rep (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally, const Iterator<TallyEntry<T>>& delegateTo)
                                    : inherited ()
                                    , fIterateOverTallyKeepalive_ (iterateOverTally)
                                    , fDelegateTo_ (delegateTo)
                                {
                                }
                                virtual void    More (Memory::Optional<T>* result, bool advance) override
                                {
                                    RequireNotNull (result);
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
                                    return typename Iterator<T>::SharedIRepPtr (new Rep (fIterateOverTallyKeepalive_, Iterator<TallyEntry<T>> (fDelegateTo_)));
                                }
                                virtual bool                                StrongEquals (const typename Iterator<T>::IRep* rhs) const override
                                {
                                    AssertNotImplemented ();
                                    return false;
                                }
                                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    fIterateOverTallyKeepalive_;    // subtle - hang onto in case container goes out of scope so not destroyed
                                Iterator<TallyEntry<T>>                             fDelegateTo_;
                            };
                            MyIter_ (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally, const Iterator<TallyEntry<T>>& delegateTo)
                                : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (iterateOverTally, delegateTo)))
                            {
                            }
                        };
                        return MyIter_ (fTallyRep_, fTallyRep_->MakeIterator ());
                    }
                    virtual size_t                                  GetLength () const override
                    {
                        return fTallyRep_->GetLength ();
                    }
                    virtual bool                                    IsEmpty () const override
                    {
                        return fTallyRep_->IsEmpty ();
                    }
                    virtual void                                    Apply (typename AdaptorRep_::_APPLY_ARGTYPE doToElement) const override
                    {
                        return this->_Apply (doToElement);
                    }
                    virtual Iterator<T>                             ApplyUntilTrue (typename AdaptorRep_::_APPLYUNTIL_ARGTYPE doToElement) const override
                    {
                        return this->_ApplyUntilTrue (doToElement);
                    }
                };
            public:
                _UniqueElementsHelper (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new AdaptorRep_ (iterateOverTally)))
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
            Iterable<T>  Tally<T, TRAITS>::_IRep::_Elements_Reference_Implementation (const _SharedPtrIRep& rep) const
            {
                Require (rep.get () == this);   // allows reference counting but without using enable_shared_from_this (so cheap!)
                const bool kHackBWA_ = true;        // ILL-UNDERSTOOD BUG!!! - SOMETHING TODO WITH REFCOUNTS? ETC
                if (kHackBWA_) {
                    return _ElementsHelper (rep->Clone ());
                }
                return _ElementsHelper (rep);
            }
            template    <typename T, typename TRAITS>
            Iterable<T>  Tally<T, TRAITS>::_IRep::_UniqueElements_Reference_Implementation (const _SharedPtrIRep& rep) const
            {
                Require (rep.get () == this);   // allows reference counting but without using enable_shared_from_this (so cheap!)
                const bool kHackBWA_ = true;        // ILL-UNDERSTOOD BUG!!! - SOMETHING TODO WITH REFCOUNTS? ETC
                if (kHackBWA_) {
                    return _UniqueElementsHelper (rep->Clone ());
                }
                return _UniqueElementsHelper (rep);
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
                AddAll (s);
            }
            template    <typename T, typename TRAITS>
            Tally<T, TRAITS>::Tally (const std::initializer_list<TallyEntry<T>>& s)
                : inherited (Concrete::Tally_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (s);
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
                _SharedPtrIRep  ss = dynamic_pointer_cast<typename _SharedPtrIRep::element_type> (this->_GetSharedPtrIRep ());
                AssertNotNull (ss.get ());
                return ss->Elements (ss);
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   Tally<T, TRAITS>::UniqueElements () const
            {
                _SharedPtrIRep  ss = dynamic_pointer_cast<typename _SharedPtrIRep::element_type> (this->_GetSharedPtrIRep ());
                AssertNotNull (ss.get ());
                return ss->UniqueElements (ss);
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
