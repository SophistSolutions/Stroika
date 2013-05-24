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

#include    "../Private/DataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************************ SortedTally_stdmap<T>::Rep_ ***************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   SortedTally_stdmap<T>::Rep_ : public SortedTally<T>::_IRep {
                private:
                    typedef typename    SortedTally<T>::_IRep inherited;

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

                    // Tally<T>::_IRep overrides
                public:
                    virtual bool                                    Equals (const typename Tally<T>::_IRep& rhs) const override;
                    virtual bool                                    Contains (T item) const override;
                    virtual void                                    RemoveAll () override;
                    virtual void                                    Add (T item, size_t count) override;
                    virtual void                                    Remove (T item, size_t count) override;
                    virtual void                                    Remove (const Iterator<TallyEntry<T>>& i) override;
                    virtual void                                    UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) override;
                    virtual size_t                                  TallyOf (T item) const override;
                    virtual Iterator<T>                             MakeBagIterator () const override;


                private:
                    Private::ContainerRepLockDataSupport_                                                   fLockSupport_;
                    Private::DataStructures::Patching::STLContainerWrapper<pair<T, size_t>, map<T, size_t>> fData_;

                    friend  class SortedTally_stdmap<T>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 **************** SortedTally_stdmap<T>::IteratorRep_ ***************************
                 ********************************************************************************
                 */
                template    <typename T>
                class  SortedTally_stdmap<T>::IteratorRep_ : public Iterator<TallyEntry<T>>::IRep {
                private:
                    typedef     typename Iterator<TallyEntry<T>>::IRep  inherited;

                public:
                    IteratorRep_ (typename SortedTally_stdmap<T>::Rep_& owner)
                        : inherited ()
                        , fIterator_ (&owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                public:
                    virtual bool            More (TallyEntry<T>* current, bool advance) override {
                        pair<T, size_t> tmp;
                        bool result = fIterator_.More (&tmp, advance);
                        if (current != nullptr) {
                            current->fItem = tmp.first;
                            current->fCount = tmp.second;
                        }
                        return result;
                    }
                    virtual bool            StrongEquals (const typename Iterator<TallyEntry<T> >::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }
                    virtual shared_ptr<typename Iterator<TallyEntry<T> >::IRep> Clone () const override {
                        return shared_ptr<typename Iterator<TallyEntry<T> >::IRep> (new IteratorRep_ (*this));
                    }

                private:
                    mutable typename Private::DataStructures::Patching::STLContainerWrapper<pair<T, size_t>, map<T, size_t>>::BasicForwardIterator   fIterator_;
                    friend  class   SortedTally_stdmap<T>::Rep_;
                };


                /*
                 ********************************************************************************
                 ********************** SortedTally_stdmap<T>::Rep_ *****************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  SortedTally_stdmap<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  SortedTally_stdmap<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                size_t  SortedTally_stdmap<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  SortedTally_stdmap<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.size () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                Iterator<TallyEntry<T>> SortedTally_stdmap<T>::Rep_::MakeIterator () const
                {
                    // const cast cuz this mutator won't really be used to change anything - except stuff like
                    // link list of owned iterators
                    Iterator<TallyEntry<T>> tmp = Iterator<TallyEntry<T>> (typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (*const_cast<Rep_*> (this))));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                void      SortedTally_stdmap<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<TallyEntry<T>>     SortedTally_stdmap<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool    SortedTally_stdmap<T>::Rep_::Equals (const typename Tally<T>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T>
                bool    SortedTally_stdmap<T>::Rep_::Contains (T item) const
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.find (item) != fData_.end ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename T>
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    SortedTally_stdmap<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename T>
                void    SortedTally_stdmap<T>::Rep_::Add (T item, size_t count)
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
                template    <typename T>
                void    SortedTally_stdmap<T>::Rep_::Remove (T item, size_t count)
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
                template    <typename T>
                void    SortedTally_stdmap<T>::Rep_::Remove (const Iterator<TallyEntry<T>>& i)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename SortedTally_stdmap<T>::IteratorRep_&       mir =   dynamic_cast<const typename SortedTally_stdmap<T>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator_.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    SortedTally_stdmap<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear ();
                        // must fix / patch
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    SortedTally_stdmap<T>::Rep_::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename SortedTally_stdmap<T>::IteratorRep_&       mir =   dynamic_cast<const typename SortedTally_stdmap<T>::IteratorRep_&> (ir);
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
                template    <typename T>
                size_t  SortedTally_stdmap<T>::Rep_::TallyOf (T item) const
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
                template    <typename T>
                Iterator<T>    SortedTally_stdmap<T>::Rep_::MakeBagIterator () const
                {
                    Iterator<T> tmp =   Iterator<T> (typename Iterator<T>::SharedIRepPtr (new typename Rep_::_TallyEntryToItemIteratorHelperRep (MakeIterator ())));
                    //tmphack - must fix to have iteratorrep dont proerply and not need to init owning itgerator object
                    tmp++;
                    return tmp;
                }


                /*
                 ********************************************************************************
                 ************************** SortedTally_stdmap<T> *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                SortedTally_stdmap<T>::SortedTally_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>    SortedTally_stdmap<T>::SortedTally_stdmap (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Add (start, end);
                }
                template    <typename T>
                inline  SortedTally_stdmap<T>::SortedTally_stdmap (const SortedTally_stdmap<T>& src) :
                    inherited (src)
                {
                }
                template    <typename T>
                SortedTally_stdmap<T>::SortedTally_stdmap (const Tally<T>& src) :
                    inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    operator+= (src);
                }
                template    <typename T>
                inline  SortedTally_stdmap<T>& SortedTally_stdmap<T>::operator= (const SortedTally_stdmap<T>& src)
                {
                    inherited::operator= (src);
                    return *this;
                }
                template    <typename T>
                inline  const typename SortedTally_stdmap<T>::Rep_&    SortedTally_stdmap<T>::GetRep_ () const
                {
                    return reinterpret_cast<const Rep_&> (this->_GetRep ());
                }
                template    <typename T>
                inline  typename SortedTally_stdmap<T>::Rep_&  SortedTally_stdmap<T>::GetRep_ ()
                {
                    return reinterpret_cast<Rep_&> (this->_GetRep ());
                }


            }
        }
    }
}
#endif  /* _Stroika_Foundation_Containers_SortedTally_stdmap_inl_ */
