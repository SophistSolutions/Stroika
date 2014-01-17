/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_inl_
#define _Stroika_Foundation_Containers_Tally_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"
#include    "../Traversal/IterableFromIterator.h"
#include    "Tally.h"

#include    "Concrete/Tally_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename T, typename TRAITS>
            struct  Tally<T, TRAITS>::_IRep::ElementsIteratorHelperContext_ {
                ElementsIteratorHelperContext_ (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& tally, const Iterator<TallyEntry<T>>& delegateTo, size_t countMoreTimesToGoBeforeAdvance = 0, Memory::Optional<T> saved2Return = Memory::Optional<T> ())
                    : fTally (tally)
                    , fTallyIterator (delegateTo)
                    , fCountMoreTimesToGoBeforeAdvance (countMoreTimesToGoBeforeAdvance)
                    , fSaved2Return (saved2Return)
                {
                }
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    fTally;
                Iterator<TallyEntry<T>>                             fTallyIterator;
                size_t                                              fCountMoreTimesToGoBeforeAdvance;
                Memory::Optional<T>                                 fSaved2Return;
            };


            template    <typename T, typename TRAITS>
            struct  Tally<T, TRAITS>::_IRep::ElementsIteratorHelper_ : public Iterator<T> {
                struct  Rep : public Iterator<T>::IRep {
                    using   inherited   =   typename    Iterator<T>::IRep;
                    using   OwnerID     =   typename Iterator<T>::IRep::OwnerID;
                    ElementsIteratorHelperContext_      fContext;
                    DECLARE_USE_BLOCK_ALLOCATION(Rep);
                    Rep (const ElementsIteratorHelperContext_& context)
                        : inherited ()
                        , fContext (context)
                    {
                        if (not fContext.fTallyIterator.Done ()) {
                            fContext.fSaved2Return = fContext.fTallyIterator->fItem;
                            if (fContext.fSaved2Return.IsPresent ()) {
                                fContext.fCountMoreTimesToGoBeforeAdvance = fContext.fTallyIterator->fCount - 1;
                            }
                        }
                    }
                    virtual void    More (Memory::Optional<T>* result, bool advance) override
                    {
                        RequireNotNull (result);
                        if (fContext.fCountMoreTimesToGoBeforeAdvance > 0) {
                            *result = fContext.fSaved2Return;
                            if (advance) {
                                fContext.fCountMoreTimesToGoBeforeAdvance--;
                            }
                        }
                        else {
                            bool done = fContext.fTallyIterator.Done ();
                            if (not done and advance) {
                                fContext.fTallyIterator++;
                                done = fContext.fTallyIterator.Done ();
                            }
                            if (done) {
                                result->clear ();
                            }
                            else {
                                *result = fContext.fTallyIterator->fItem;
                            }
                            if (advance) {
                                fContext.fSaved2Return = *result;
                                if (fContext.fSaved2Return.IsPresent ()) {
                                    fContext.fCountMoreTimesToGoBeforeAdvance = fContext.fTallyIterator->fCount - 1;
                                }
                            }
                        }
                    }
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override
                    {
                        return typename Iterator<T>::SharedIRepPtr (new Rep (*this));
                    }
                    virtual OwnerID GetOwner () const override
                    {
                        //tmphack but adequate
                        // should NOT require locking is readonly immutable value provided at construction
                        return nullptr;
                    }
                    virtual bool    Equals (const typename Iterator<T>::IRep* rhs) const override
                    {
                        AssertNotImplemented ();
                        return false;
                    }
                };
                ElementsIteratorHelper_ (const ElementsIteratorHelperContext_& context)
                    : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (context)))
                {
                }
            };


            /**
             *  Protected helper class to convert from an iterator of TallyEntries
             *  to an iterator over individual items - repeating items the appropriate number of times
             *  depending on its count.
             */
            template    <typename T, typename TRAITS>
            struct  Tally<T, TRAITS>::_IRep::_ElementsIterableHelper : public Iterable<T> {
                typedef typename ElementsIteratorHelper_::Rep   MyIteratorRep_;
                typedef ElementsIteratorHelperContext_          MyDataBLOB_;
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                typedef typename Iterable<T>::_SharedPtrIRep          IterableOfTSharedPtrIRep;
#endif
                struct MyIterableRep_ : Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep {
                    using   inherited = typename Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep;
                    DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                    MyIterableRep_ (const ElementsIteratorHelperContext_& context)
                        : inherited (context)
                    {
                    }
                    virtual size_t  GetLength () const override
                    {
                        size_t  n = 0;
                        for (Iterator<TallyEntry<T>> i = this->_fDataBlob.fTally->MakeIterator (this); not i.Done (); ++i) {
                            n += i->fCount;
                        }
                        return n;
                    }
                    virtual bool    IsEmpty () const override
                    {
                        return this->_fDataBlob.fTally->IsEmpty ();
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep Clone () const override
                    {
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                        return IterableOfTSharedPtrIRep (new MyIterableRep_ (*this));
#else
                        return typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (*this));
#endif
                    }
                };
                _ElementsIterableHelper (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& iterateOverTally)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (ElementsIteratorHelperContext_ (iterateOverTally, iterateOverTally->MakeIterator (iterateOverTally.get ())))))
                {
                }
            };


            template    <typename T, typename TRAITS>
            struct  Tally<T, TRAITS>::_IRep::UniqueElementsIteratorHelperContext_ {
                UniqueElementsIteratorHelperContext_ (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& tally, const Iterator<TallyEntry<T>>& delegateTo)
                    : fTally (tally)
                    , fTallyIterator (delegateTo)
                {
                }
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    fTally;
                Iterator<TallyEntry<T>>                             fTallyIterator;
            };


            template    <typename T, typename TRAITS>
            struct  Tally<T, TRAITS>::_IRep::UniqueElementsIteratorHelper_ : public Iterator<T> {
                struct  Rep : public Iterator<T>::IRep {
                    using   inherited   =   typename    Iterator<T>::IRep;
                    using   OwnerID     =   typename Iterator<T>::IRep::OwnerID;
                    UniqueElementsIteratorHelperContext_      fContext;
                    DECLARE_USE_BLOCK_ALLOCATION(Rep);
                    Rep (const UniqueElementsIteratorHelperContext_& context)
                        : inherited ()
                        , fContext (context)
                    {
                    }
                    virtual void    More (Memory::Optional<T>* result, bool advance) override
                    {
                        RequireNotNull (result);
                        bool done = fContext.fTallyIterator.Done ();
                        if (not done and advance) {
                            fContext.fTallyIterator++;
                            done = fContext.fTallyIterator.Done ();
                        }
                        if (done) {
                            result->clear ();
                        }
                        else {
                            *result = fContext.fTallyIterator->fItem;
                        }
                    }
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override
                    {
                        return typename Iterator<T>::SharedIRepPtr (new Rep (*this));
                    }
                    virtual OwnerID GetOwner () const override
                    {
                        //tmphack but adequate
                        // should NOT require locking is readonly immutable value provided at construction
                        return nullptr;
                    }
                    virtual bool    Equals (const typename Iterator<T>::IRep* rhs) const override
                    {
                        AssertNotImplemented ();
                        return false;
                    }
                };
                UniqueElementsIteratorHelper_ (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& tally)
                    : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (UniqueElementsIteratorHelperContext_ (tally, tally->MakeIterator (tally.get ())))))
                {
                }
            };


            /**
             *  Protected helper class to convert from an iterator of TallyEntries
             *  to an iterator over unique individual items.
             */
            template    <typename T, typename TRAITS>
            struct  Tally<T, TRAITS>::_IRep::_UniqueElementsHelper : public Iterable<T> {
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                typedef typename Iterable<T>::_SharedPtrIRep          IterableOfTSharedPtrIRep;
#endif
                typedef typename UniqueElementsIteratorHelper_::Rep   MyIteratorRep_;
                typedef UniqueElementsIteratorHelperContext_          MyDataBLOB_;
                struct MyIterableRep_ : Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep {
                    using   inherited = typename Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep;
                    DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                    MyIterableRep_ (const UniqueElementsIteratorHelperContext_& context)
                        : inherited (context)
                    {
                    }
                    virtual size_t  GetLength () const override
                    {
                        return this->_fDataBlob.fTally->GetLength ();
                    }
                    virtual bool    IsEmpty () const override
                    {
                        return this->_fDataBlob.fTally->IsEmpty ();
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep Clone () const override
                    {
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                        return IterableOfTSharedPtrIRep (new MyIterableRep_ (*this));
#else
                        return typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (*this));
#endif
                    }
                };
                _UniqueElementsHelper (const typename Iterable<TallyEntry<T>>::_SharedPtrIRep& tally)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (UniqueElementsIteratorHelperContext_ (tally, tally->MakeIterator (tally.get ())))))
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
            inline  TallyEntry<T>::TallyEntry (pair<T, size_t> item)
                : fItem (item.first)
                , fCount (item.second)
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
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
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
                // Sent email to Sterl - I think the bug is that we must BreakReferences when
                // copying a new object - if it has any active iterators. But think that through!
                // -- LGP 2014-01-01
                if (kHackBWA_) {
                    return _ElementsIterableHelper (rep->Clone ());
                }
                return _ElementsIterableHelper (rep);
            }
            template    <typename T, typename TRAITS>
            Iterable<T>  Tally<T, TRAITS>::_IRep::_UniqueElements_Reference_Implementation (const _SharedPtrIRep& rep) const
            {
                Require (rep.get () == this);   // allows reference counting but without using enable_shared_from_this (so cheap!)
                const bool kHackBWA_ = true;        // ILL-UNDERSTOOD BUG!!! - SOMETHING TODO WITH REFCOUNTS? ETC
                // Sent email to Sterl - I think the bug is that we must BreakReferences when
                // copying a new object - if it has any active iterators. But think that through!
                // -- LGP 2014-01-01
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
                _SharedPtrIRep  ss = const_cast<Tally<T, TRAITS>*> (this)->_GetRep ().shared_from_this ();
                AssertNotNull (ss.get ());
                return ss->Elements (ss);
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   Tally<T, TRAITS>::UniqueElements () const
            {
                _SharedPtrIRep  ss = const_cast<Tally<T, TRAITS>*> (this)->_GetRep ().shared_from_this ();
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
