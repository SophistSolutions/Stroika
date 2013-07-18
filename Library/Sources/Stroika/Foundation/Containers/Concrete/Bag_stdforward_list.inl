/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_stdforward_list_inl_
#define _Stroika_Foundation_Containers_Concrete_Bag_stdforward_list_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <forward_list>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ******************* Bag_stdforward_list<T, TRAITS>::Rep_ ***********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Bag_stdforward_list<T, TRAITS>::Rep_ : public Bag<T, TRAITS>::_IRep {
                private:
                    typedef typename    Bag<T, TRAITS>::_IRep   inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override;
                    virtual Iterator<T>                             MakeIterator () const override;
                    virtual size_t                                  GetLength () const override;
                    virtual bool                                    IsEmpty () const override;
                    virtual void                                    Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Bag<T, TRAITS>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Bag<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual size_t  TallyOf (T item) const override;
                    virtual void    Add (T item) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Remove (T item) override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    RemoveAll () override;

                private:
                    typedef Private::PatchingDataStructures::STLContainerWrapper<std::forward_list<T>>      DataStructureImplType_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;

                private:
                    friend  class   Bag_stdforward_list<T, TRAITS>::IteratorRep_;
                };


                /*
                 ********************************************************************************
                 ************ Bag_stdforward_list<T, TRAITS>::IteratorRep_ **********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class  Bag_stdforward_list<T, TRAITS>::IteratorRep_ : public Iterator<T>::IRep {
                private:
                    typedef typename    Iterator<T>::IRep   inherited;

                public:
                    explicit IteratorRep_ (typename Bag_stdforward_list<T, TRAITS>::Rep_& owner)
                        : inherited ()
                        , fLockSupport_ (owner.fLockSupport_)
                        , fIterator_ (&owner.fData_) {
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::SharedIRepPtr Clone () const override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool   More (T* current, bool advance) override {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return (fIterator_.More (current, advance));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool   StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    Private::ContainerRepLockDataSupport_&                          fLockSupport_;
                    mutable typename Rep_::DataStructureImplType_::ForwardIterator  fIterator_;

                private:
                    friend  class   Bag_stdforward_list<T, TRAITS>::Rep_;
                };


                /*
                ********************************************************************************
                ***************** Bag_stdforward_list<T, TRAITS>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Bag_stdforward_list<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Bag_stdforward_list<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
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
                typename Iterable<T>::_SharedPtrIRep  Bag_stdforward_list<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Bag_stdforward_list<T, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (*NON_CONST_THIS));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<T> tmp = Iterator<T> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T, typename TRAITS>
                size_t  Bag_stdforward_list<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        size_t  cnt = 0;
                        for (auto i = fData_.begin (); i != fData_.end (); ++i, cnt++)
                            ;
                        return cnt;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Bag_stdforward_list<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.empty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Bag_stdforward_list<T, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Bag_stdforward_list<T, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    Bag_stdforward_list<T, TRAITS>::Rep_::Equals (const typename Bag<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    Bag_stdforward_list<T, TRAITS>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.FindIf ([item] (T i) {
                            return TRAITS::EqualsCompareFunctionType::Equals (i, item);
                        });
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t    Bag_stdforward_list<T, TRAITS>::Rep_::TallyOf (T item) const
                {
                    return this->_TallyOf_Reference_Implementation (item);
                }
                template    <typename T, typename TRAITS>
                void    Bag_stdforward_list<T, TRAITS>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.push_front (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_stdforward_list<T, TRAITS>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_stdforward_list<T, TRAITS>::IteratorRep_&      mir =   dynamic_cast<const typename Bag_stdforward_list<T, TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Assert (not i.Done ());
                        *mir.fIterator_.fStdIterator = newValue;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_stdforward_list<T, TRAITS>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto ei = fData_.before_begin ();
                        for (auto i = fData_.begin (); i != fData_.end (); ++i) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (*i, item)) {
                                Memory::SmallStackBuffer<typename DataStructureImplType_::ForwardIterator*>   items2Patch (0);
                                fData_.TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
                                auto newI = fData_.erase_after (ei);
                                fData_.TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                                return;
                            }
                            ei = i;
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_stdforward_list<T, TRAITS>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_stdforward_list<T, TRAITS>::IteratorRep_&      mir =   dynamic_cast<const typename Bag_stdforward_list<T, TRAITS>::IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        //mir.fIterator_.RemoveCurrent ();

                        // HORRIBLE BUT ADEQUITE IMPL...FOR NOW...
                        {
                            auto ei = fData_.before_begin ();
                            for (auto i = fData_.begin (); i != fData_.end (); ++i) {
                                if (i == mir.fIterator_.fStdIterator) {
                                    Memory::SmallStackBuffer<typename DataStructureImplType_::ForwardIterator*>   items2Patch (0);
                                    fData_.TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
                                    auto newI = fData_.erase_after (ei);
                                    fData_.TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                                    return;
                                }
                                ei = i;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_stdforward_list<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }



                /*
                ********************************************************************************
                ************************ Bag_stdforward_list<T, TRAITS> ************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Bag_stdforward_list<T, TRAITS>::Bag_stdforward_list ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                Bag_stdforward_list<T, TRAITS>::Bag_stdforward_list (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    this->AddAll (start, end);
                }
                template    <typename T, typename TRAITS>
                Bag_stdforward_list<T, TRAITS>::Bag_stdforward_list (const Bag<T, TRAITS>& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    inherited::operator+= (src);
                }
                template    <typename T, typename TRAITS>
                Bag_stdforward_list<T, TRAITS>::Bag_stdforward_list (const Bag_stdforward_list<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T, typename TRAITS>
                inline  Bag_stdforward_list<T, TRAITS>& Bag_stdforward_list<T, TRAITS>::operator= (const Bag_stdforward_list<T, TRAITS>& bag)
                {
                    inherited::operator= (static_cast<const inherited&> (bag));
                    return *this;
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Bag_stdforward_list_inl_ */
