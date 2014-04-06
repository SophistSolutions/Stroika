/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_inl_
#define _Stroika_Foundation_Containers_MultiSet_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"
#include    "../Traversal/IterableFromIterator.h"
#include    "MultiSet.h"

#include    "Concrete/MultiSet_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::IteratorOwnerID;


            /*
             ********************************************************************************
             ********* MultiSet<T, TRAITS>::_IRep::ElementsIteratorHelperContext_ ***********
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            struct  MultiSet<T, TRAITS>::_IRep::ElementsIteratorHelperContext_ {
                ElementsIteratorHelperContext_ (const typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep& tally, const Iterator<MultiSetEntry<T>>& delegateTo, size_t countMoreTimesToGoBeforeAdvance = 0, Memory::Optional<T> saved2Return = Memory::Optional<T> ())
                    : fMultiSet (tally)
                    , fMultiSetIterator (delegateTo)
                    , fCountMoreTimesToGoBeforeAdvance (countMoreTimesToGoBeforeAdvance)
                    , fSaved2Return (saved2Return)
                {
                }
                typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep fMultiSet;
                Iterator<MultiSetEntry<T>>                          fMultiSetIterator;
                size_t                                              fCountMoreTimesToGoBeforeAdvance;
                Memory::Optional<T>                                 fSaved2Return;
            };


            template    <typename T, typename TRAITS>
            struct  MultiSet<T, TRAITS>::_IRep::ElementsIteratorHelper_ : public Iterator<T> {
                struct  Rep : public Iterator<T>::IRep {
                    using   inherited   =   typename    Iterator<T>::IRep;
                    ElementsIteratorHelperContext_      fContext;
                    DECLARE_USE_BLOCK_ALLOCATION(Rep);
                    Rep (const ElementsIteratorHelperContext_& context)
                        : inherited ()
                        , fContext (context)
                    {
                        if (not fContext.fMultiSetIterator.Done ()) {
                            fContext.fSaved2Return = fContext.fMultiSetIterator->fItem;
                            if (fContext.fSaved2Return.IsPresent ()) {
                                fContext.fCountMoreTimesToGoBeforeAdvance = fContext.fMultiSetIterator->fCount - 1;
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
                            bool done = fContext.fMultiSetIterator.Done ();
                            if (not done and advance) {
                                fContext.fMultiSetIterator++;
                                done = fContext.fMultiSetIterator.Done ();
                            }
                            if (done) {
                                result->clear ();
                            }
                            else {
                                *result = fContext.fMultiSetIterator->fItem;
                            }
                            if (advance) {
                                fContext.fSaved2Return = *result;
                                if (fContext.fSaved2Return.IsPresent ()) {
                                    fContext.fCountMoreTimesToGoBeforeAdvance = fContext.fMultiSetIterator->fCount - 1;
                                }
                            }
                        }
                    }
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override
                    {
                        return typename Iterator<T>::SharedIRepPtr (new Rep (*this));
                    }
                    virtual IteratorOwnerID GetOwner () const override
                    {
                        return fContext.fMultiSetIterator.GetOwner ();
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
             *  Protected helper class to convert from an iterator of MultiSetEntries
             *  to an iterator over individual items - repeating items the appropriate number of times
             *  depending on its count.
             */
            template    <typename T, typename TRAITS>
            struct  MultiSet<T, TRAITS>::_IRep::_ElementsIterableHelper : public Iterable<T> {
                using   MyIteratorRep_  =   typename ElementsIteratorHelper_::Rep;
                using   MyDataBLOB_     =   ElementsIteratorHelperContext_;
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                using   IterableOfTSharedPtrIRep        =   typename Iterable<T>::_SharedPtrIRep;
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
                        for (Iterator<MultiSetEntry<T>> i = this->_fContextForEachIterator.fMultiSet->MakeIterator (this); not i.Done (); ++i) {
                            n += i->fCount;
                        }
                        return n;
                    }
                    virtual bool    IsEmpty () const override
                    {
                        return this->_fContextForEachIterator.fMultiSet->IsEmpty ();
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                        return IterableOfTSharedPtrIRep (new MyIterableRep_ (*this));
#else
                        return typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (*this));
#endif
                    }
                };
                _ElementsIterableHelper (const typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep& iterateOverMultiSet)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (ElementsIteratorHelperContext_ (iterateOverMultiSet, iterateOverMultiSet->MakeIterator (iterateOverMultiSet.get ())))))
                {
                }
            };


            template    <typename T, typename TRAITS>
            struct  MultiSet<T, TRAITS>::_IRep::UniqueElementsIteratorHelperContext_ {
                UniqueElementsIteratorHelperContext_ (const typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep& tally, const Iterator<MultiSetEntry<T>>& delegateTo)
                    : fMultiSet (tally)
                    , fMultiSetIterator (delegateTo)
                {
                }
                typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep    fMultiSet;
                Iterator<MultiSetEntry<T>>                             fMultiSetIterator;
            };


            template    <typename T, typename TRAITS>
            struct  MultiSet<T, TRAITS>::_IRep::UniqueElementsIteratorHelper_ : public Iterator<T> {
                struct  Rep : public Iterator<T>::IRep {
                    using   inherited   =   typename    Iterator<T>::IRep;
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
                        bool done = fContext.fMultiSetIterator.Done ();
                        if (not done and advance) {
                            fContext.fMultiSetIterator++;
                            done = fContext.fMultiSetIterator.Done ();
                        }
                        if (done) {
                            result->clear ();
                        }
                        else {
                            *result = fContext.fMultiSetIterator->fItem;
                        }
                    }
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override
                    {
                        return typename Iterator<T>::SharedIRepPtr (new Rep (*this));
                    }
                    virtual IteratorOwnerID GetOwner () const override
                    {
                        return fContext.fMultiSetIterator.GetOwner ();
                    }
                    virtual bool    Equals (const typename Iterator<T>::IRep* rhs) const override
                    {
                        AssertNotImplemented ();
                        return false;
                    }
                };
                UniqueElementsIteratorHelper_ (const typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep& tally)
                    : Iterator<T> (typename Iterator<T>::SharedIRepPtr (new Rep (UniqueElementsIteratorHelperContext_ (tally, tally->MakeIterator (tally.get ())))))
                {
                }
            };


            /**
             *  Protected helper class to convert from an iterator of MultiSetEntries
             *  to an iterator over unique individual items.
             */
            template    <typename T, typename TRAITS>
            struct  MultiSet<T, TRAITS>::_IRep::_UniqueElementsHelper : public Iterable<T> {
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                using   IterableOfTSharedPtrIRep    =   typename Iterable<T>::_SharedPtrIRep;
#endif
                using   MyIteratorRep_              =   typename UniqueElementsIteratorHelper_::Rep;
                using   MyDataBLOB_                 =   UniqueElementsIteratorHelperContext_;
                struct  MyIterableRep_ : Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep {
                    using   inherited = typename Traversal::IterableFromIterator<T, MyIteratorRep_, MyDataBLOB_>::_Rep;
                    DECLARE_USE_BLOCK_ALLOCATION(MyIterableRep_);
                    MyIterableRep_ (const UniqueElementsIteratorHelperContext_& context)
                        : inherited (context)
                    {
                    }
                    virtual size_t  GetLength () const override
                    {
                        return this->_fContextForEachIterator.fMultiSet->GetLength ();
                    }
                    virtual bool    IsEmpty () const override
                    {
                        return this->_fContextForEachIterator.fMultiSet->IsEmpty ();
                    }
                    virtual typename Iterable<T>::_SharedPtrIRep Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
#if     qTemplateAccessCheckConfusionProtectedNeststingBug
                        return IterableOfTSharedPtrIRep (new MyIterableRep_ (*this));
#else
                        return typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (*this));
#endif
                    }
                };
                _UniqueElementsHelper (const typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep& tally)
                    : Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIterableRep_ (UniqueElementsIteratorHelperContext_ (tally, tally->MakeIterator (tally.get ())))))
                {
                }
            };


            /*
             ********************************************************************************
             ****************************** MultiSetEntry<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  MultiSetEntry<T>::MultiSetEntry (T item)
                : fItem (item)
                , fCount (1)
            {
            }
            template    <typename T>
            inline  MultiSetEntry<T>::MultiSetEntry (T item, size_t count)
                : fItem (item)
                , fCount (count)
            {
            }
            template    <typename T>
            inline  MultiSetEntry<T>::MultiSetEntry (pair<T, size_t> item)
                : fItem (item.first)
                , fCount (item.second)
            {
            }
            template    <typename T>
            inline bool   MultiSetEntry<T>::operator== (const MultiSetEntry<T>& rhs)  const
            {
                return (fCount == rhs.fCount and fItem == rhs.fItem);
            }
            template    <typename T>
            inline bool   MultiSetEntry<T>::operator!= (const MultiSetEntry<T>& rhs)  const
            {
                return not (operator== (rhs));
            }


            /*
             ********************************************************************************
             ************************* MultiSet<T, TRAITS>::_IRep ***************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            bool  MultiSet<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
            {
                if (this == &rhs) {
                    return true;
                }
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }
                for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
                    if (i->fCount != rhs.OccurrencesOf (i->fItem)) {
                        return false;
                    }
                }
                return true;
            }
            template    <typename T, typename TRAITS>
            Iterable<T>  MultiSet<T, TRAITS>::_IRep::_Elements_Reference_Implementation (const _SharedPtrIRep& rep) const
            {
                Require (rep.get () == this);   // allows reference counting but without using enable_shared_from_this (so cheap!)
                return _ElementsIterableHelper (rep);
            }
            template    <typename T, typename TRAITS>
            Iterable<T>  MultiSet<T, TRAITS>::_IRep::_UniqueElements_Reference_Implementation (const _SharedPtrIRep& rep) const
            {
                Require (rep.get () == this);   // allows reference counting but without using enable_shared_from_this (so cheap!)
                return _UniqueElementsHelper (rep);
            }


            /*
             ********************************************************************************
             ******************************** MultiSet<T, TRAITS> ******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet ()
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>::MultiSet (const MultiSet<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  MultiSet<T, TRAITS>::MultiSet (const CONTAINER_OF_T& src)
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (src);
            }
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>::MultiSet (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const std::initializer_list<T>& s)
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (s);
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const std::initializer_list<MultiSetEntry<T>>& s)
                : inherited (Concrete::MultiSet_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (s);
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const T* start, const T* end)
                : inherited (Concrete::MultiSet_Factory<T, TRAITS>::mk ())
            {
                AddAll (start, end);
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const MultiSetEntry<T>* start, const MultiSetEntry<T>* end)
                : inherited (Concrete::MultiSet_Factory<T, TRAITS>::mk ())
            {
                AddAll (start, end);
                AssertMember (&inherited::_GetRep (), _IRep);
            }
#if     qDebug
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::~MultiSet ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _GetRep ().AssertNoIteratorsReferenceOwner (this);
                }
            }
#endif
            template    <typename T, typename TRAITS>
            inline  const typename  MultiSet<T, TRAITS>::_IRep&    MultiSet<T, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  const typename MultiSet<T, TRAITS>::_IRep&  MultiSet<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename MultiSet<T, TRAITS>::_IRep&        MultiSet<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            void   MultiSet<T, TRAITS>::RemoveAll (T item)
            {
                Remove (item, OccurrencesOf (item));
            }
            template    <typename T, typename TRAITS>
            size_t  MultiSet<T, TRAITS>::TotalOccurrences () const
            {
                size_t sum = 0;
                for (MultiSetEntry<T> i : *this) {
                    sum += i.fCount;
                }
                return sum;
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   MultiSet<T, TRAITS>::Elements () const
            {
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                _SharedPtrIRep  ss = dynamic_pointer_cast<typename _SharedPtrIRep::element_type> (const_cast<MultiSet<T, TRAITS>*> (this)->_GetRep ().shared_from_this ());
#else
                _SharedPtrIRep  ss = const_cast<MultiSet<T, TRAITS>*> (this)->_GetRep ().shared_from_this ();
#endif
                AssertNotNull (ss.get ());
                return ss->Elements (ss);
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   MultiSet<T, TRAITS>::UniqueElements () const
            {
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                _SharedPtrIRep  ss = dynamic_pointer_cast<typename _SharedPtrIRep::element_type> (const_cast<MultiSet<T, TRAITS>*> (this)->_GetRep ().shared_from_this ());
#else
                _SharedPtrIRep  ss = const_cast<MultiSet<T, TRAITS>*> (this)->_GetRep ().shared_from_this ();
#endif
                AssertNotNull (ss.get ());
                return ss->UniqueElements (ss);
            }
            template    <typename T, typename TRAITS>
            inline  bool  MultiSet<T, TRAITS>::Equals (const MultiSet<T, TRAITS>& rhs) const
            {
                return _SafeReadRepAccessor<_IRep> (*this)._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep> (rhs)._ConstGetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  bool    MultiSet<T, TRAITS>::Contains (T item) const
            {
                return _SafeReadRepAccessor<_IRep> (*this)._ConstGetRep ().Contains (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Add (T item)
            {
                _GetRep ().Add (item, 1);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Add (T item, size_t count)
            {
                _GetRep ().Add (item, count);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Add (const MultiSetEntry<T>& item)
            {
                _GetRep ().Add (item.fItem, item.fCount);
            }
            template    <typename T, typename TRAITS>
            void   MultiSet<T, TRAITS>::AddAll (const T* start, const T* end)
            {
                for (const T* i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::AddAll (const MultiSetEntry<T>* start, const MultiSetEntry<T>* end)
            {
                for (auto i = start; i != end; ++i) {
                    _GetRep ().Add (i->fItem, i->fEnd);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            void    MultiSet<T, TRAITS>::AddAll (const CONTAINER_OF_T& src)
            {
                for (auto i : src) {
                    Add (i);
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Remove (T item)
            {
                _GetRep ().Remove (item, 1);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Remove (T item, size_t count)
            {
                _GetRep ().Remove (item, count);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Remove (const Iterator<MultiSetEntry<T>>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount)
            {
                _GetRep ().UpdateCount (i, newCount);
            }
            template    <typename T, typename TRAITS>
            inline  size_t  MultiSet<T, TRAITS>::OccurrencesOf (T item) const
            {
                return _GetRep ().OccurrencesOf (item);
            }
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>&   MultiSet<T, TRAITS>::operator+= (T item)
            {
                _GetRep ().Add (item, 1);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  bool   MultiSet<T, TRAITS>::operator== (const MultiSet<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    MultiSet<T, TRAITS>::operator!= (const MultiSet<T, TRAITS>& rhs) const
            {
                return not (Equals (rhs));
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>&  MultiSet<T, TRAITS>::operator+= (const MultiSet<T, TRAITS>& t)
            {
                for (auto i = t.begin (); i != t.end (); ++i) {
                    Add (i->fItem, i->fCount);
                }
                return *this;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_MultiSet_inl_ */
