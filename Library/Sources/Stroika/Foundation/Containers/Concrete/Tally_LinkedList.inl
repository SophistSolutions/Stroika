/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_    1



#include    "Private/LinkedList.h"

#include    "../../Memory/BlockAllocated.h"





namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Tally_LinkedList<T>::Rep_ : public Tally<T>::_IRep {
                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_IRep*    Clone () const override;
                    virtual size_t                          GetLength () const override;
                    virtual bool                            IsEmpty () const override;
                    virtual Iterator<T>                     MakeIterator () const override;
                    virtual void                            Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                     ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Tally<T>::_IRep overrides
                public:
                    virtual bool    Contains (T item) const override;
                    virtual void    Compact () override;
                    virtual void    RemoveAll () override;
                    virtual void    Add (T item, size_t count) override;
                    virtual void    Remove (T item, size_t count) override;
                    virtual size_t  TallyOf (T item) const override;

                    virtual typename Iterator<TallyEntry<T> >::IRep* MakeTallyIterator () const override;
                    virtual TallyMutator             MakeTallyMutator () override;

                private:
                    LinkedList_Patch<TallyEntry<T>>    fData;

                    friend  class   MutatorRep_;
                };


                template    <typename T>
                class   Tally_LinkedList<T>::MutatorRep_ : public Tally<T>::TallyMutator::IRep {
                public:
                    MutatorRep_ (typename Tally_LinkedList<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (MutatorRep_);

                public:
                    virtual bool            More (TallyEntry<T>* current, bool advance) override;
                    virtual bool            StrongEquals (typename const Iterator<TallyEntry<T> >::IRep* rhs) const override;

                    virtual typename Iterator<TallyEntry<T> >::IRep* Clone () const override;

                    virtual void    RemoveCurrent () override;
                    virtual void    UpdateCount (size_t newCount) override;

                private:
                    LinkedListMutator_Patch<TallyEntry<T>> fIterator;
                };








                /*
                 ********************************************************************************
                 ********************** Tally_LinkedList<T>::MutatorRep_ ************************
                 ********************************************************************************
                 */
                template    <typename T>
                Tally_LinkedList<T>::MutatorRep_::MutatorRep_ (Rep_& owner)
                    : fIterator (owner.fData)
                {
                }
                template    <typename T>
                bool    Tally_LinkedList<T>::MutatorRep_::More (TallyEntry<T>* current, bool advance)
                {
                    return (fIterator.More (current, advance));
                }
                template    <typename T>
                bool    Tally_LinkedList<T>::MutatorRep_::StrongEquals (typename const Iterator<TallyEntry<T>>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                typename Iterator<TallyEntry<T> >::IRep* Tally_LinkedList<T>::MutatorRep_::Clone () const
                {
                    return (new MutatorRep_ (*this));
                }
                template    <typename T>
                void   Tally_LinkedList<T>::MutatorRep_::RemoveCurrent ()
                {
                    fIterator.RemoveCurrent ();
                }
                template    <typename T>
                void   Tally_LinkedList<T>::MutatorRep_::UpdateCount (size_t newCount)
                {
                    if (newCount == 0) {
                        fIterator.RemoveCurrent ();
                    }
                    else {
                        TallyEntry<T>   c   =   fIterator.Current ();
                        c.fCount = newCount;
                        fIterator.UpdateCurrent (c);
                    }
                }








                /*
                 ********************************************************************************
                 ************************* Tally_LinkedList<T>::Rep_ ****************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Tally_LinkedList<T>::Rep_::Rep_ ()
                    : fData ()
                {
                }
                template    <typename T>
                inline  Tally_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
                    : fData (from.fData)
                {
                }
                template    <typename T>
                size_t  Tally_LinkedList<T>::Rep_::GetLength () const
                {
                    return (fData.GetLength ());
                }
                template    <typename T>
                bool  Tally_LinkedList<T>::Rep_::IsEmpty () const
                {
                    return (fData.GetLength () == 0);
                }
                template    <typename T>
                Iterator<T> Tally_LinkedList<T>::Rep_::MakeIterator () const
                {
                    return Iterator<T> (typename Iterator<T>::SharedByValueRepType (new _TallyEntryToItemIterator (Iterator<TallyEntry<T>> (typename Iterator<TallyEntry<T>>::SharedByValueRepType (MakeTallyIterator ())))));
                }
                template    <typename T>
                void      Tally_LinkedList<T>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    return _Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Tally_LinkedList<T>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return _ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                bool   Tally_LinkedList<T>::Rep_::Contains (T item) const
                {
                    TallyEntry<T>   c;
                    for (LinkedListIterator<TallyEntry<T> > it (fData); it.More (&c, true); ) {
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
                template    <typename T>
                typename Iterable<T>::_IRep*   Tally_LinkedList<T>::Rep_::Clone () const
                {
                    return (new Rep_ (*this));
                }
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::Add (T item, size_t count)
                {
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        for (LinkedListMutator_Patch<TallyEntry<T> > it (fData); it.More (&current, true); ) {
                            if (current.fItem == item) {
                                current.fCount += count;
                                it.UpdateCurrent (current);
                                return;
                            }
                        }
                        fData.Prepend (TallyEntry<T> (item, count));
                    }
                }
                template    <typename T>
                void   Tally_LinkedList<T>::Rep_::Remove (T item, size_t count)
                {
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        for (LinkedListMutator_Patch<TallyEntry<T> > it (fData); it.More (&current, true); ) {

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
                    fData.RemoveAll ();
                }
                template    <typename T>
                size_t Tally_LinkedList<T>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T>   c;
                    for (LinkedListIterator<TallyEntry<T> > it (fData); it.More (&c, true); ) {
                        if (c.fItem == item) {
                            Ensure (c.fCount != 0);
                            return (c.fCount);
                        }
                    }
                    return (0);
                }
                template    <typename T>
                typename Iterator<TallyEntry<T> >::IRep*    Tally_LinkedList<T>::Rep_::MakeTallyIterator () const
                {
                    // const cast cuz this mutator won't really be used to change anything - except stuff like
                    // link list of owned iterators
                    return (new MutatorRep_ (*const_cast<Rep_*> (this)));
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
