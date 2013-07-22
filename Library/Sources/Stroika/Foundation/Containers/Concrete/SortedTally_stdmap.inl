/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedTally_stdmap_inl_
#define _Stroika_Foundation_Containers_SortedTally_stdmap_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <map>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ******************* SortedTally_stdmap<T, TRAITS>::Rep_ ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   SortedTally_stdmap<T, TRAITS>::Rep_ : public SortedTally<T, TRAITS>::_IRep {
                private:
                    typedef typename    SortedTally<T, TRAITS>::_IRep inherited;

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

                private:
                    typedef Private::PatchingDataStructures::STLContainerWrapper<map<T, size_t>>        DataStructureImplType_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;

                private:
                    //friend  class SortedTally_stdmap<T, TRAITS>::IteratorRep_;
                    friend  class   IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ************** SortedTally_stdmap<T, TRAITS>::IteratorRep_ *********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class  SortedTally_stdmap<T, TRAITS>::IteratorRep_ : public Iterator<TallyEntry<T>>::IRep {
                private:
                    typedef     typename Iterator<TallyEntry<T>>::IRep  inherited;

                public:
                    IteratorRep_ (typename SortedTally_stdmap<T, TRAITS>::Rep_& owner)
                        : inherited ()
                        , fLockSupport_ (owner.fLockSupport_)
                        , fIterator_ (&owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                public:
                    virtual shared_ptr<typename Iterator<TallyEntry<T>>::IRep> Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    More (TallyEntry<T>* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            if (current == nullptr) {
                                return fIterator_.More (static_cast<pair<T, size_t>*> (nullptr), advance);
                            }
                            else {
                                pair<T, size_t> tmp;
                                bool result = fIterator_.More (&tmp, advance);
                                if (current != nullptr) {
                                    current->fItem = tmp.first;
                                    current->fCount = tmp.second;
                                }
                                return result;
                            }
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
                    //friend  class   SortedTally_stdmap<T, TRAITS>::Rep_;
                    friend      class       Rep_;
                };


                /*
                 ********************************************************************************
                 **************** SortedTally_stdmap<T, TRAITS>::Rep_ ***************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  SortedTally_stdmap<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  SortedTally_stdmap<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
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
                size_t  SortedTally_stdmap<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  SortedTally_stdmap<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.size () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>> SortedTally_stdmap<T, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<TallyEntry<T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<TallyEntry<T>> tmp = Iterator<TallyEntry<T>> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T, typename TRAITS>
                void      SortedTally_stdmap<T, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>>     SortedTally_stdmap<T, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    SortedTally_stdmap<T, TRAITS>::Rep_::Equals (const typename Tally<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    SortedTally_stdmap<T, TRAITS>::Rep_::Contains (T item) const
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.find (item) != fData_.end ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename T, typename TRAITS>
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    SortedTally_stdmap<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename T, typename TRAITS>
                void    SortedTally_stdmap<T, TRAITS>::Rep_::Add (T item, size_t count)
                {
                    if (count == 0) {
                        return;
                    }
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto i = fData_.find (item);
                        if (i == fData_.end ()) {
                            fData_.insert (typename map<T, size_t>::value_type (item, count));
                        }
                        else {
                            i->second += count;
                        }
                        // MUST PATCH
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedTally_stdmap<T, TRAITS>::Rep_::Remove (T item, size_t count)
                {
                    if (count == 0) {
                        return;
                    }
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto i = fData_.find (item);
                        Require (i != fData_.end ());
                        if (i != fData_.end ()) {
                            Require (i->second >= count);
                            i->second -= count;
                            if (i->second == 0) {
                                fData_.erase_WithPatching (i);
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedTally_stdmap<T, TRAITS>::Rep_::Remove (const Iterator<TallyEntry<T>>& i)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename SortedTally_stdmap<T, TRAITS>::IteratorRep_&       mir =   dynamic_cast<const typename SortedTally_stdmap<T, TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedTally_stdmap<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedTally_stdmap<T, TRAITS>::Rep_::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename SortedTally_stdmap<T, TRAITS>::IteratorRep_&       mir =   dynamic_cast<const typename SortedTally_stdmap<T, TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (newCount == 0) {
                            mir.fIterator_.RemoveCurrent ();
                        }
                        else {
                            mir.fIterator_.fStdIterator->second = newCount;
                        }
                        // TODO - PATCH
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t  SortedTally_stdmap<T, TRAITS>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto i = fData_.find (item);
                        if (i == fData_.end ()) {
                            return 0;
                        }
                        return i->second;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>    SortedTally_stdmap<T, TRAITS>::Rep_::MakeBagIterator () const
                {
                    Iterator<T> tmp =   Iterator<T> (typename Iterator<T>::SharedIRepPtr (new typename Rep_::_TallyEntryToItemIteratorHelperRep (MakeIterator ())));
                    //tmphack - must fix to have iteratorrep dont proerply and not need to init owning itgerator object
                    tmp++;
                    return tmp;
                }


                /*
                 ********************************************************************************
                 ************************ SortedTally_stdmap<T, TRAITS> *************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                SortedTally_stdmap<T, TRAITS>::SortedTally_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                SortedTally_stdmap<T, TRAITS>::SortedTally_stdmap (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Add (start, end);
                }
                template    <typename T, typename TRAITS>
                inline  SortedTally_stdmap<T, TRAITS>::SortedTally_stdmap (const SortedTally_stdmap<T, TRAITS>& src) :
                    inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                SortedTally_stdmap<T, TRAITS>::SortedTally_stdmap (const CONTAINER_OF_T& src) :
                    inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    operator+= (src);
                }
                template    <typename T, typename TRAITS>
                inline  SortedTally_stdmap<T, TRAITS>& SortedTally_stdmap<T, TRAITS>::operator= (const SortedTally_stdmap<T, TRAITS>& src)
                {
                    inherited::operator= (static_cast<const inherited&> (src));
                    return *this;
                }
                template    <typename T, typename TRAITS>
                inline  const typename SortedTally_stdmap<T, TRAITS>::Rep_&    SortedTally_stdmap<T, TRAITS>::GetRep_ () const
                {
                    return reinterpret_cast<const Rep_&> (this->_GetRep ());
                }
                template    <typename T, typename TRAITS>
                inline  typename SortedTally_stdmap<T, TRAITS>::Rep_&  SortedTally_stdmap<T, TRAITS>::GetRep_ ()
                {
                    return reinterpret_cast<Rep_&> (this->_GetRep ());
                }


            }
        }
    }
}
#endif  /* _Stroika_Foundation_Containers_SortedTally_stdmap_inl_ */
