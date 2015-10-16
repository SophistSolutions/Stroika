/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                        return typename Iterator<T>::SharedIRepPtr (Iterator<T>::template MakeSharedPtr<Rep> (*this));
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
                    : Iterator<T> (typename Iterator<T>::template MakeSharedPtr<Rep> (context))
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
                        return typename Iterator<T>::SharedIRepPtr (Iterator<T>::template MakeSharedPtr<Rep> (*this));
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
                    : Iterator<T> (typename Iterator<T>::SharedIRepPtr (Iterator<T>::template MakeSharedPtr<Rep> (UniqueElementsIteratorHelperContext_ (tally, tally->MakeIterator (tally.get ())))))
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
             ****************************** MultiSetEntry<T> ********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  MultiSetEntry<T>::MultiSetEntry (ArgByValueType<T> item)
                : fItem (item)
                , fCount (1)
            {
            }
            template    <typename T>
            inline  MultiSetEntry<T>::MultiSetEntry (ArgByValueType<T> item, size_t count)
                : fItem (item)
                , fCount (count)
            {
            }
            template    <typename T>
            inline  MultiSetEntry<T>::MultiSetEntry (const pair<T, size_t>& item)
                : fItem (item.first)
                , fCount (item.second)
            {
            }
            template    <typename T>
            inline bool   MultiSetEntry<T>::operator== (const MultiSetEntry<T>& rhs) const
            {
                return (fCount == rhs.fCount and fItem == rhs.fItem);
            }
            template    <typename T>
            inline bool   MultiSetEntry<T>::operator!= (const MultiSetEntry<T>& rhs) const
            {
                return not (operator== (rhs));
            }


            /*
             ********************************************************************************
             ************************* MultiSet<T, TRAITS>::_IRep ***************************
             ********************************************************************************
             */
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
             ***************************** MultiSet<T, TRAITS> ******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet ()
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>::MultiSet (const MultiSet<T, TRAITS>& src)
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline  MultiSet<T, TRAITS>::MultiSet (const CONTAINER_OF_T& src)
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>::MultiSet (const _SharedPtrIRep& rep)
                : inherited (rep)
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>::MultiSet (_SharedPtrIRep&& rep)
                : inherited (move (rep))
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const initializer_list<T>& s)
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (s);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const initializer_list<MultiSetEntry<T>>& s)
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (s);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const T* start, const T* end)
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (start, end);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::MultiSet (const MultiSetEntry<T>* start, const MultiSetEntry<T>* end)
                : inherited (move (Concrete::MultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                AddAll (start, end);
                _AssertRepValidType ();
            }
#if     qDebug
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>::~MultiSet ()
            {
                if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
                    _ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
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
            void   MultiSet<T, TRAITS>::RemoveAll (ArgByValueType<T> item)
            {
                Remove (item, OccurrencesOf (item));
            }
            template    <typename T, typename TRAITS>
            size_t  MultiSet<T, TRAITS>::TotalOccurrences () const
            {
                size_t  sum = 0;
                for (MultiSetEntry<T> i : *this) {
                    sum += i.fCount;
                }
                return sum;
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   MultiSet<T, TRAITS>::Elements () const
            {
                _SafeReadRepAccessor<_IRep> accessor { this };
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                _SharedPtrIRep  ss = dynamic_pointer_cast<typename _SharedPtrIRep::element_type> (const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ());
#else
                _SharedPtrIRep  ss = const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ();
#endif
                AssertNotNull (ss.get ());
                return ss->Elements (ss);
            }
            template    <typename T, typename TRAITS>
            inline  Iterable<T>   MultiSet<T, TRAITS>::UniqueElements () const
            {
                _SafeReadRepAccessor<_IRep> accessor { this };
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                _SharedPtrIRep  ss = dynamic_pointer_cast<typename _SharedPtrIRep::element_type> (const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ());
#else
                _SharedPtrIRep  ss = const_cast<_IRep&> (accessor._ConstGetRep ()).shared_from_this ();
#endif
                AssertNotNull (ss.get ());
                return ss->UniqueElements (ss);
            }
            template    <typename T, typename TRAITS>
            inline  bool  MultiSet<T, TRAITS>::Equals (const MultiSet<T, TRAITS>& rhs) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep> { &rhs } ._ConstGetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  bool    MultiSet<T, TRAITS>::Contains (ArgByValueType<T> item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().Contains (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::RemoveAll ()
            {
                _SafeReadWriteRepAccessor<_IRep> tmp { this };
                if (not tmp._ConstGetRep ().IsEmpty ()) {
                    tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Add (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (item, 1);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Add (ArgByValueType<T> item, size_t count)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (item, count);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Add (const MultiSetEntry<T>& item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (item.fItem, item.fCount);
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
                    _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (i->fItem, i->fEnd);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T, typename ENABLE_IF>
            void    MultiSet<T, TRAITS>::AddAll (const CONTAINER_OF_T& src)
            {
                for (auto i : src) {
                    Add (i);
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Remove (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (item, 1);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Remove (ArgByValueType<T> item, size_t count)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (item, count);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::Remove (const Iterator<MultiSetEntry<T>>& i)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Remove (i);
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().UpdateCount (i, newCount);
            }
            template    <typename T, typename TRAITS>
            inline  size_t  MultiSet<T, TRAITS>::OccurrencesOf (ArgByValueType<T> item) const
            {
                return _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().OccurrencesOf (item);
            }
            template    <typename T, typename TRAITS>
            inline  MultiSet<T, TRAITS>&   MultiSet<T, TRAITS>::operator+= (ArgByValueType<T> item)
            {
                _SafeReadWriteRepAccessor<_IRep> { this } ._GetWriteableRep ().Add (item, 1);
                return *this;
            }
            template    <typename T, typename TRAITS>
            MultiSet<T, TRAITS>&  MultiSet<T, TRAITS>::operator+= (const MultiSet<T, TRAITS>& t)
            {
                for (auto i = t.begin (); i != t.end (); ++i) {
                    Add (i->fItem, i->fCount);
                }
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    MultiSet<T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


            /*
              ********************************************************************************
              **************************** MultiSet operators ********************************
              ********************************************************************************
              */
            template    <typename T, typename TRAITS>
            inline  bool   operator== (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs)
            {
                return lhs.Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    operator!= (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs)
            {
                return not lhs.Equals (rhs);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_MultiSet_inl_ */
