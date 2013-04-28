/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_Array_inl_
#define _Stroika_Foundation_Containers_Tally_Array_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/DataStructures/Array.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                using namespace Private;


                /*
                 ********************************************************************************
                 ******************************* Tally_Array<T>::Rep_ ***************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Tally_Array<T>::Rep_ : public Tally<T>::_IRep {
                private:
                    typedef typename    Tally<T>::_IRep inherited;

                public:
                    Rep_ ();

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                    virtual typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Clone () const override {
                        return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Clone () const override;
#endif

                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual Iterator<TallyEntry<T>>                             MakeIterator () const override;
                    virtual void                                                Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<TallyEntry<T>>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Tally<T>::_IRep overrides
                public:
                    virtual bool                                    Contains (T item) const override;
                    virtual void                                    Compact () override;
                    virtual void                                    RemoveAll () override;
                    virtual void                                    Add (T item, size_t count) override;
                    virtual void                                    Remove (T item, size_t count) override;
                    virtual void                                    Remove (const Iterator<TallyEntry<T>>& i) override;
                    virtual void                                    UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) override;
                    virtual size_t                                  TallyOf (T item) const override;
                    virtual Iterator<T>                             MakeBagIterator () const override;

                private:
                    nonvirtual  void    RemoveAt_ (size_t index);

                private:
                    Array_Patch<TallyEntry<T>> fData_;

                    DEFINE_CONSTEXPR_CONSTANT(size_t, kNotFound_, (size_t) - 1);

                    nonvirtual  size_t  Find_ (TallyEntry<T>& item) const;

                    friend  class Tally_Array<T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 *********************** Tally_Array<T>::IteratorRep_ ***************************
                 ********************************************************************************
                 */
                template    <typename T>
                class  Tally_Array<T>::IteratorRep_ : public Iterator<TallyEntry<T>>::IRep {
                private:
                    typedef     typename Iterator<TallyEntry<T>>::IRep  inherited;

                public:
                    IteratorRep_ (typename Tally_Array<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                public:
                    virtual bool            More (TallyEntry<T>* current, bool advance) override;
                    virtual bool            StrongEquals (const typename Iterator<TallyEntry<T> >::IRep* rhs) const override;
                    virtual shared_ptr<typename Iterator<TallyEntry<T> >::IRep> Clone () const override;

                private:
                    mutable ForwardArrayMutator_Patch<TallyEntry<T> >   fIterator_;
                    friend  class   Tally_Array<T>::Rep_;
                };


                /*
                 ********************************************************************************
                 ************************** Tally_Array<T>::IteratorRep_ ************************
                 ********************************************************************************
                 */
                template    <class  T>
                Tally_Array<T>::IteratorRep_::IteratorRep_ (typename Tally_Array<T>::Rep_& owner)
                    : inherited ()
                    , fIterator_ (owner.fData_)
                {
                }
                template    <class  T>
                bool    Tally_Array<T>::IteratorRep_::More (TallyEntry<T>* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Tally_Array<T>::IteratorRep_::StrongEquals (const typename Iterator<TallyEntry<T> >::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                shared_ptr<typename Iterator<TallyEntry<T> >::IRep> Tally_Array<T>::IteratorRep_::Clone () const
                {
                    return shared_ptr<typename Iterator<TallyEntry<T> >::IRep> (new IteratorRep_ (*this));
                }


                /*
                 ********************************************************************************
                 *************************** Tally_Array<T>::Rep_ *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Tally_Array<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fData_ ()
                {
                }
                template    <typename T>
                size_t  Tally_Array<T>::Rep_::GetLength () const
                {
                    return (fData_.GetLength ());
                }
                template    <typename T>
                bool  Tally_Array<T>::Rep_::IsEmpty () const
                {
                    return (fData_.GetLength () == 0);
                }
                template    <typename T>
                Iterator<TallyEntry<T>> Tally_Array<T>::Rep_::MakeIterator () const
                {
                    // const cast cuz this mutator won't really be used to change anything - except stuff like
                    // link list of owned iterators
                    Iterator<TallyEntry<T>> tmp = Iterator<TallyEntry<T>> (typename Iterator<TallyEntry<T>>::SharedByValueRepType (shared_ptr<typename Iterator<TallyEntry<T>>::IRep> (new IteratorRep_ (*const_cast<Rep_*> (this)))));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                void      Tally_Array<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<TallyEntry<T>>     Tally_Array<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool    Tally_Array<T>::Rep_::Contains (T item) const
                {
                    TallyEntry<T> tmp (item);
                    return (bool (Find_ (tmp) != kNotFound_));
                }
                template    <typename T>
                void    Tally_Array<T>::Rep_::Compact ()
                {
                    fData_.Compact ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename T>
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Tally_Array<T>::Rep_::Clone () const
                {
                    return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename T>
                void    Tally_Array<T>::Rep_::Add (T item, size_t count)
                {
                    TallyEntry<T> tmp (item, count);
                    size_t index = Find_ (tmp);
                    if (index == kNotFound_) {
                        fData_.InsertAt (tmp, GetLength ());
                    }
                    else {
                        tmp.fCount += count;
                        fData_.SetAt (tmp, index);
                    }
                }
                template    <typename T>
                void    Tally_Array<T>::Rep_::Remove (T item, size_t count)
                {
                    TallyEntry<T> tmp (item);
                    size_t index = Find_ (tmp);
                    if (index != kNotFound_) {
                        Assert (index < GetLength ());
                        Assert (tmp.fCount >= count);
                        tmp.fCount -= count;
                        if (tmp.fCount == 0) {
                            RemoveAt_ (index);
                        }
                        else {
                            fData_.SetAt (tmp, index);
                        }
                    }
                }
                template    <typename T>
                void    Tally_Array<T>::Rep_::Remove (const Iterator<TallyEntry<T>>& i)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Tally_Array<T>::IteratorRep_&       mir =   dynamic_cast<const typename Tally_Array<T>::IteratorRep_&> (ir);
                    mir.fIterator_.RemoveCurrent ();
                }
                template    <typename T>
                void    Tally_Array<T>::Rep_::RemoveAll ()
                {
                    fData_.RemoveAll ();
                }
                template    <typename T>
                void    Tally_Array<T>::Rep_::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Tally_Array<T>::IteratorRep_&       mir =   dynamic_cast<const typename Tally_Array<T>::IteratorRep_&> (ir);
                    if (newCount == 0) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    else {
                        TallyEntry<T>   c   =   mir.fIterator_.Current ();
                        c.fCount = newCount;
                        mir.fIterator_.UpdateCurrent (c);
                    }
                }
                template    <typename T>
                size_t  Tally_Array<T>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T> tmp (item);
                    size_t index = Find_ (tmp);
                    if (index == kNotFound_) {
                        return 0;
                    }
                    Assert (index >= 0);
                    Assert (index < GetLength ());
                    return (tmp.fCount);
                }
                template    <typename T>
                Iterator<T>    Tally_Array<T>::Rep_::MakeBagIterator () const
                {
                    Iterator<T> tmp =   Iterator<T> (typename Iterator<T>::SharedByValueRepType (shared_ptr<typename Iterator<T>::IRep> (new typename Rep_::_TallyEntryToItemIteratorHelperRep (MakeIterator ()))));
                    //tmphack - must fix to have iteratorrep dont proerply and not need to init owning itgerator object
                    tmp++;
                    return tmp;
                }
                template    <typename T>
                void    Tally_Array<T>::Rep_::RemoveAt_ (size_t index)
                {
                    fData_.RemoveAt (index);
                }
                template    <typename T>
                size_t  Tally_Array<T>::Rep_::Find_ (TallyEntry<T>& item) const
                {
                    size_t length = fData_.GetLength ();
                    for (size_t i = 0; i < length; i++) {
                        if (fData_.GetAt (i).fItem == item.fItem) {
                            item = fData_.GetAt (i);
                            return (i);
                        }
                    }
                    return kNotFound_;
                }


                /*
                 ********************************************************************************
                 ********************************* Tally_Array<T> *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                Tally_Array<T>::Tally_Array ()
                    : Tally<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>    Tally_Array<T>::Tally_Array (const T* start, const T* end)
                    : Tally<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    SetCapacity (end - start);
                    Add (start, end);
                }
                template    <typename T>
                inline  Tally_Array<T>::Tally_Array (const Tally_Array<T>& src) :
                    Tally<T> (src)
                {
                }
                template    <typename T>
                Tally_Array<T>::Tally_Array (const Tally<T>& src) :
                    Tally<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    SetCapacity (src.GetLength ());
                    operator+= (src);
                }
                template    <typename T>
                inline  Tally_Array<T>& Tally_Array<T>::operator= (const Tally_Array<T>& src)
                {
                    Tally<T>::operator= (src);
                    return (*this);
                }
                template    <typename T>
                inline  const typename Tally_Array<T>::Rep_&    Tally_Array<T>::GetRep_ () const
                {
                    return reinterpret_cast<const Tally_Array<T>::Rep_&> (this->_GetRep ());
                }
                template    <typename T>
                inline  typename Tally_Array<T>::Rep_&  Tally_Array<T>::GetRep_ ()
                {
                    return reinterpret_cast<Tally_Array<T>::Rep_&> (this->_GetRep ());
                }
                template    <typename T>
                size_t  Tally_Array<T>::GetCapacity () const
                {
                    return (this->GetRep ().fData_.GetCapacity ());
                }
                template    <typename T>
                void    Tally_Array<T>::SetCapacity (size_t slotsAlloced)
                {
                    this->GetRep ().fData_.SetCapacity (slotsAlloced);
                }


            }
        }
    }
}


#endif  /* _Stroika_Foundation_Containers_Tally_Array_inl_ */


