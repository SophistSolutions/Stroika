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

#include    "../Private/PatchingDataStructures/Array.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************** Tally_Array<T, TRAITS>::Rep_ ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Tally_Array<T, TRAITS>::Rep_ : public Tally<T, TRAITS>::_IRep {
                private:
                    typedef typename    Tally<T, TRAITS>::_IRep inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

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

                    // Tally<T, TRAITS>::_IRep overrides
                public:
                    virtual bool                                    Equals (const typename Tally<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                                    Contains (T item) const override;
                    virtual void                                    RemoveAll () override;
                    virtual void                                    Add (T item, size_t count) override;
                    virtual void                                    Remove (T item, size_t count) override;
                    virtual void                                    Remove (const Iterator<TallyEntry<T>>& i) override;
                    virtual void                                    UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) override;
                    virtual size_t                                  TallyOf (T item) const override;
                    virtual Iterator<T>                             MakeBagIterator () const override;

                    // Tally_Array<T, TRAITS>::_IRep overrides
                public:
                    nonvirtual void                                 Compact ();

                private:
                    typedef Private::PatchingDataStructures::Array_Patch<TallyEntry<T>>     DataStructureImplType_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;

                private:
                    DEFINE_CONSTEXPR_CONSTANT(size_t, kNotFound_, (size_t) - 1);

                    nonvirtual  size_t  Find_ (TallyEntry<T>& item) const;

                private:
                    friend  class   Tally_Array<T, TRAITS>;
                    friend  class   IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ********************* Tally_Array<T, TRAITS>::IteratorRep_ *********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class  Tally_Array<T, TRAITS>::IteratorRep_ : public Iterator<TallyEntry<T>>::IRep {
                private:
                    typedef     typename Iterator<TallyEntry<T>>::IRep  inherited;

                public:
                    IteratorRep_ (typename Tally_Array<T, TRAITS>::Rep_& owner)
                        : inherited ()
                        , fLockSupport_ (owner.fLockSupport_)
                        , fIterator_ (owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                public:
                    virtual shared_ptr<typename Iterator<TallyEntry<T>>::IRep> Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return shared_ptr<typename Iterator<TallyEntry<T>>::IRep> (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    More (TallyEntry<T>* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return (fIterator_.More (current, advance));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    StrongEquals (const typename Iterator<TallyEntry<T>>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    Private::ContainerRepLockDataSupport_&                          fLockSupport_;
                    mutable typename Rep_::DataStructureImplType_::ForwardIterator  fIterator_;

                private:
                    friend  class   Tally_Array<T, TRAITS>::Rep_;
                };


                /*
                 ********************************************************************************
                 *********************** Tally_Array<T, TRAITS>::Rep_ ***************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  Tally_Array<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Tally_Array<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t  Tally_Array<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Tally_Array<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>> Tally_Array<T, TRAITS>::Rep_::MakeIterator () const
                {
                    // const cast cuz this mutator won't really be used to change anything - except stuff like
                    // link list of owned iterators
                    typename Iterator<TallyEntry<T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        tmpRep = typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (*const_cast<Rep_*> (this)));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<TallyEntry<T>> tmp = Iterator<TallyEntry<T>> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T, typename TRAITS>
                void      Tally_Array<T, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>>     Tally_Array<T, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    Tally_Array<T, TRAITS>::Rep_::Equals (const typename Tally<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    Tally_Array<T, TRAITS>::Rep_::Contains (T item) const
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (bool (Find_ (tmp) != kNotFound_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_Array<T, TRAITS>::Rep_::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename T, typename TRAITS>
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Tally_Array<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename T, typename TRAITS>
                void    Tally_Array<T, TRAITS>::Rep_::Add (T item, size_t count)
                {
                    TallyEntry<T> tmp (item, count);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        size_t index = Find_ (tmp);
                        if (index == kNotFound_) {
                            fData_.InsertAt (fData_.GetLength (), tmp);
                        }
                        else {
                            tmp.fCount += count;
                            fData_.SetAt (index, tmp);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_Array<T, TRAITS>::Rep_::Remove (T item, size_t count)
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        size_t index = Find_ (tmp);
                        if (index != kNotFound_) {
                            Assert (index < fData_.GetLength ());
                            Assert (tmp.fCount >= count);
                            tmp.fCount -= count;
                            if (tmp.fCount == 0) {
                                fData_.RemoveAt (index);
                            }
                            else {
                                fData_.SetAt (index, tmp);
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_Array<T, TRAITS>::Rep_::Remove (const Iterator<TallyEntry<T>>& i)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Tally_Array<T, TRAITS>::IteratorRep_&       mir =   dynamic_cast<const typename Tally_Array<T, TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator_);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_Array<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_Array<T, TRAITS>::Rep_::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const typename Tally_Array<T, TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (newCount == 0) {
                            fData_.RemoveAt (mir.fIterator_);
                        }
                        else {
                            TallyEntry<T>   c   =   mir.fIterator_.Current ();
                            c.fCount = newCount;
                            fData_.SetAt (mir.fIterator_, c);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t  Tally_Array<T, TRAITS>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        size_t index = Find_ (tmp);
                        if (index == kNotFound_) {
                            return 0;
                        }
                        Assert (index >= 0);
                        Assert (index < fData_.GetLength ());
                        return (tmp.fCount);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>    Tally_Array<T, TRAITS>::Rep_::MakeBagIterator () const
                {
                    // Note - no locking needed here because this is just a wrapper on the real iterator that does the locking.
                    Iterator<T> tmp =   Iterator<T> (typename Iterator<T>::SharedIRepPtr (new typename Rep_::_TallyEntryToItemIteratorHelperRep (MakeIterator ())));
                    //tmphack - must fix to have iteratorrep dont proerply and not need to init owning itgerator object
                    tmp++;
                    return tmp;
                }
                template    <typename T, typename TRAITS>
                size_t  Tally_Array<T, TRAITS>::Rep_::Find_ (TallyEntry<T>& item) const
                {
                    // this code assumes locking done by callers
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
                 ***************************** Tally_Array<T, TRAITS> ***************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                Tally_Array<T, TRAITS>::Tally_Array ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                Tally_Array<T, TRAITS>::Tally_Array (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    SetCapacity (end - start);
                    Add (start, end);
                }
                template    <typename T, typename TRAITS>
                inline  Tally_Array<T, TRAITS>::Tally_Array (const Tally_Array<T, TRAITS>& src) :
                    inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T, typename TRAITS>
                Tally_Array<T, TRAITS>::Tally_Array (const Tally<T, TRAITS>& src) :
                    inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    SetCapacity (src.GetLength ());
                    operator+= (src);
                }
                template    <typename T, typename TRAITS>
                inline  Tally_Array<T, TRAITS>& Tally_Array<T, TRAITS>::operator= (const Tally_Array<T, TRAITS>& src)
                {
                    inherited::operator= (static_cast<const inherited&> (src));
                    return *this;
                }
                template    <typename T, typename TRAITS>
                inline  const typename Tally_Array<T, TRAITS>::Rep_&    Tally_Array<T, TRAITS>::GetRep_ () const
                {
                    return reinterpret_cast<const Tally_Array<T, TRAITS>::Rep_&> (this->_GetRep ());
                }
                template    <typename T, typename TRAITS>
                inline  typename Tally_Array<T, TRAITS>::Rep_&  Tally_Array<T, TRAITS>::GetRep_ ()
                {
                    return reinterpret_cast<Tally_Array<T, TRAITS>::Rep_&> (this->_GetRep ());
                }
                template    <typename T, typename TRAITS>
                inline  size_t  Tally_Array<T, TRAITS>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        return (GetRep_ ().fData_.GetCapacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  void    Tally_Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  void    Tally_Array<T, TRAITS>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}


#endif  /* _Stroika_Foundation_Containers_Tally_Array_inl_ */


