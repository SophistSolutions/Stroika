/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_    1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "Private/LinkedList.h"

#include    "../../Memory/BlockAllocated.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Tally_LinkedList<T>::Rep_ : public Tally<T>::_IRep {
                private:
                    typedef typename Tally<T>::_IRep   inherited;
                    
                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

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
                    virtual bool    Contains (T item) const override;
                    virtual void    Compact () override;
                    virtual void    RemoveAll () override;
                    virtual void    Add (T item, size_t count) override;
                    virtual void    Remove (T item, size_t count) override;
                    virtual size_t  TallyOf (T item) const override;
                    virtual shared_ptr<typename Iterator<T>::IRep>  MakeBagIterator () const override;
                    virtual typename Tally<T>::TallyMutator         MakeTallyMutator () override;

                private:
                    LinkedList_Patch<TallyEntry<T>>    fData_;

                    friend  class Tally_LinkedList<T>::MutatorRep_;
                };


                template    <typename T>
                class   Tally_LinkedList<T>::MutatorRep_ : public Tally<T>::TallyMutator::IRep {
                public:
                    MutatorRep_ (typename Tally_LinkedList<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (MutatorRep_);

                public:
                    virtual bool            More (TallyEntry<T>* current, bool advance) override;
                    virtual bool            StrongEquals (const typename Iterator<TallyEntry<T> >::IRep* rhs) const override;

                    virtual shared_ptr<typename Iterator<TallyEntry<T> >::IRep> Clone () const override;

                    virtual void    RemoveCurrent () override;
                    virtual void    UpdateCount (size_t newCount) override;

                private:
                    LinkedListMutator_Patch<TallyEntry<T>> fIterator_;
                };


                /*
                 ********************************************************************************
                 ********************** Tally_LinkedList<T>::MutatorRep_ ************************
                 ********************************************************************************
                 */
                template    <typename T>
                Tally_LinkedList<T>::MutatorRep_::MutatorRep_ (Rep_& owner)
                    : fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                bool    Tally_LinkedList<T>::MutatorRep_::More (TallyEntry<T>* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Tally_LinkedList<T>::MutatorRep_::StrongEquals (const typename Iterator<TallyEntry<T>>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                shared_ptr<typename Iterator<TallyEntry<T>>::IRep> Tally_LinkedList<T>::MutatorRep_::Clone () const
                {
                    return shared_ptr<typename Iterator<TallyEntry<T>>::IRep> (new MutatorRep_ (*this));
                }
                template    <typename T>
                void   Tally_LinkedList<T>::MutatorRep_::RemoveCurrent ()
                {
                    fIterator_.RemoveCurrent ();
                }
                template    <typename T>
                void   Tally_LinkedList<T>::MutatorRep_::UpdateCount (size_t newCount)
                {
                    if (newCount == 0) {
                        fIterator_.RemoveCurrent ();
                    }
                    else {
                        TallyEntry<T>   c   =   fIterator_.Current ();
                        c.fCount = newCount;
                        fIterator_.UpdateCurrent (c);
                    }
                }


                /*
                 ********************************************************************************
                 ************************* Tally_LinkedList<T>::Rep_ ****************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Tally_LinkedList<T>::Rep_::Rep_ ()
                    : fData_ ()
                {
                }
                template    <typename T>
                inline  Tally_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
                    : fData_ (from.fData_)
                {
                }
                template    <typename T>
                size_t  Tally_LinkedList<T>::Rep_::GetLength () const
                {
                    return (fData_.GetLength ());
                }
                template    <typename T>
                bool  Tally_LinkedList<T>::Rep_::IsEmpty () const
                {
                    return (fData_.GetLength () == 0);
                }
                template    <typename T>
                Iterator<TallyEntry<T>> Tally_LinkedList<T>::Rep_::MakeIterator () const
                {
                    // const cast cuz this mutator won't really be used to change anything - except stuff like
                    // link list of owned iterators
                    Iterator<TallyEntry<T>> tmp = Iterator<TallyEntry<T>> (typename Iterator<TallyEntry<T>>::SharedByValueRepType (shared_ptr<typename Iterator<TallyEntry<T>>::IRep> (new MutatorRep_ (*const_cast<Rep_*> (this)))));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                void      Tally_LinkedList<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    return _Apply (doToElement);
                }
                template    <typename T>
                Iterator<TallyEntry<T>>     Tally_LinkedList<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return _ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool   Tally_LinkedList<T>::Rep_::Contains (T item) const
                {
                    TallyEntry<T>   c;
                    for (LinkedListIterator<TallyEntry<T>> it (fData_); it.More (&c, true); ) {
                        if (c.fItem == item) {
                            Assert (c.fCount != 0);
                            return (true);
                        }
                    }
                    return (false);
                }
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::Compact ()
                {
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverTBug
                template    <typename T>
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep   Tally_LinkedList<T>::Rep_::Clone () const
                {
                    return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::Add (T item, size_t count)
                {
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        for (LinkedListMutator_Patch<TallyEntry<T> > it (fData_); it.More (&current, true); ) {
                            if (current.fItem == item) {
                                current.fCount += count;
                                it.UpdateCurrent (current);
                                return;
                            }
                        }
                        fData_.Prepend (TallyEntry<T> (item, count));
                    }
                }
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::Remove (T item, size_t count)
                {
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        for (LinkedListMutator_Patch<TallyEntry<T> > it (fData_); it.More (&current, true); ) {

                            if (current.fItem == item) {
                                if (current.fCount > count) {
                                    current.fCount -= count;
                                }
                                else {
                                    current.fCount = 0;     // Should this be an underflow excpetion, assertion???
                                }
                                if (current.fCount == 0) {
                                    it.RemoveCurrent ();
                                }
                                else {
                                    it.UpdateCurrent (current);
                                }
                                break;
                            }
                        }
                    }
                }
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::RemoveAll ()
                {
                    fData_.RemoveAll ();
                }
                template    <typename T>
                size_t Tally_LinkedList<T>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T>   c;
                    for (LinkedListIterator<TallyEntry<T> > it (fData_); it.More (&c, true); ) {
                        if (c.fItem == item) {
                            Ensure (c.fCount != 0);
                            return (c.fCount);
                        }
                    }
                    return (0);
                }
                template    <typename T>
                shared_ptr<typename Iterator<T>::IRep>    Tally_LinkedList<T>::Rep_::MakeBagIterator () const
                {
                    return shared_ptr<typename Iterator<T>::IRep> (new _TallyEntryToItemIterator (MakeIterator ()));
                }
                template    <typename T>
                typename Tally<T>::TallyMutator   Tally_LinkedList<T>::Rep_::MakeTallyMutator ()
                {
                    return TallyMutator (new MutatorRep_ (*this));
                }


                /*
                 ********************************************************************************
                 **************************** Tally_LinkedList<T> *******************************
                 ********************************************************************************
                 */
                template    <typename T>
                Tally_LinkedList<T>::Tally_LinkedList ()
                    : inherited (new Rep_ ())
                {
                }
                template    <typename T>
                Tally_LinkedList<T>::Tally_LinkedList (const T* items, size_t size) :
                    Tally<T> (new Rep_ ())
                {
                    AddItems (items, size);
                }
                template    <typename T>
                Tally_LinkedList<T>::Tally_LinkedList (const Tally<T>& src) :
                    Tally<T> (new Rep_ ())
                {
                    operator+= (src);
                }
                template    <typename T>
                inline Tally_LinkedList<T>::Tally_LinkedList (const Tally_LinkedList<T>& src) :
                    Tally<T> (src)
                {
                }
                template    <typename T>
                inline Tally_LinkedList<T>& Tally_LinkedList<T>::operator= (const Tally_LinkedList<T>& src)
                {
                    Tally<T>::operator= (src);
                    return (*this);
                }


            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_ */
