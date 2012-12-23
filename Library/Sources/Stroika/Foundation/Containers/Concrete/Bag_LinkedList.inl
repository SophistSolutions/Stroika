/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */

#include    "Private/LinkedList.h"

#include    "../../Memory/BlockAllocated.h"





namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Bag_LinkedList<T>::Rep_ : public Bag<T>::_IRep {
                public:
                    Rep_ ();

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_IRep*    Clone () const override;
                    virtual Iterator<T>                     MakeIterator () const override;
                    virtual size_t                          GetLength () const override;
                    virtual bool                            IsEmpty () const override;
                    virtual void                            Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                     ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Bag<T>::_IRep overrides
                public:
                    virtual void                        Compact () override;
                    virtual bool                        Contains (T item) const override;
                    virtual void                        Add (T item) override;
                    virtual void                        Update (const Iterator<T>& i, T newValue) override;
                    virtual void                        Remove (T item) override;
                    virtual void                        Remove (const Iterator<T>& i) override;
                    virtual void                        RemoveAll () override;

                private:
                    LinkedList_Patch<T> fData_;

                private:
                    friend  class   Bag_LinkedList<T>::IteratorRep_;
                };


                // One rep for BOTH iterator and mutator - to save code - mutator just adds invisible functionality
                template    <typename T>
                class  Bag_LinkedList<T>::IteratorRep_ : public Iterator<T>::IRep {
                public:
                    explicit IteratorRep_ (typename Bag_LinkedList<T>::Rep_& owner);
                    explicit IteratorRep_ (typename Bag_LinkedList<T>::IteratorRep_& from);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual typename Iterator<T>::IRep*     Clone () const override;
                    virtual bool                            More (T* current, bool advance) override;
                    virtual bool                            StrongEquals (typename Iterator<T>::IRep* rhs) override;

                private:
                    mutable LinkedListMutator_Patch<T>  fIterator_;
                private:
                    friend  class   Bag_LinkedList<T>::Rep_;
                };







                //  class   Bag_LinkedList<T>::Rep_
                template    <typename T>
                inline  Bag_LinkedList<T>::Rep_::Rep_ ()
                    : fData_ ()
                {
                }
                template    <typename T>
                typename    Iterable<T>::_IRep*  Bag_LinkedList<T>::Rep_::Clone () const
                {
                    return (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Bag_LinkedList<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    return Iterator<T> (new Bag_LinkedList<T>::IteratorRep_ (*NON_CONST_THIS));
                }
                template    <typename T>
                size_t  Bag_LinkedList<T>::Rep_::GetLength () const
                {
                    return (fData_.GetLength ());
                }
                template    <typename T>
                bool  Bag_LinkedList<T>::Rep_::IsEmpty () const
                {
                    return (fData_.GetLength () == 0);
                }
                template    <typename T>
                void      Bag_LinkedList<T>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    return _Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Bag_LinkedList<T>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return _ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Compact ()
                {
                }
                template    <typename  T>
                bool    Bag_LinkedList<T>::Rep_::Contains (T item) const
                {
                    return (fData_.Contains (item));
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Add (T item)
                {
                    fData_.Prepend (item);
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_LinkedList<T>::IteratorRep_&      mir =   dynamic_cast<const typename Bag_LinkedList<T>::IteratorRep_&> (ir);
                    mir.fIterator_.UpdateCurrent (newValue);
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Remove (T item)
                {
                    fData_.Remove (item);
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Bag_LinkedList<T>::IteratorRep_&      mir =   dynamic_cast<const typename Bag_LinkedList<T>::IteratorRep_&> (ir);
                    mir.fIterator_.RemoveCurrent ();
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::RemoveAll ()
                {
                    fData_.RemoveAll ();
                }



                //Bag_LinkedList<T>::IteratorRep_
                template    <typename T>
                Bag_LinkedList<T>::IteratorRep_::IteratorRep_ (typename Bag_LinkedList<T>::Rep_& owner)
                    : fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                Bag_LinkedList<T>::IteratorRep_::IteratorRep_ (IteratorRep_& from)
                    : fIterator_ (from.fIterator_)
                {
                }
                template    <typename T>
                bool    Bag_LinkedList<T>::IteratorRep_::More (T* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Bag_LinkedList<T>::IteratorRep_::StrongEquals (typename Iterator<T>::IRep*  rhs)
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                typename Iterator<T>::IRep*  Bag_LinkedList<T>::IteratorRep_::Clone () const
                {
                    return (new IteratorRep_ (*const_cast<IteratorRep_*> (this)));
                }




                // Bag_LinkedList
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList ()
                    : Bag<T> (new Rep_ ())
                {
                }
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList (const T* start, const T* end)
                    : Bag<T> (new Rep_ ())
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    Add (start, end);
                }
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList (const Bag<T>& src)
                    : Bag<T> (new Rep_ ())
                {
                    Bag<T>::operator+= (src);
                }
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList (const Bag_LinkedList<T>& src)
                    : Bag<T> (src)
                {
                }
                template    <typename T>
                inline  Bag_LinkedList<T>& Bag_LinkedList<T>::operator= (const Bag_LinkedList<T>& bag)
                {
                    Bag<T>::operator= (bag);
                    return (*this);
                }

            }
        }
    }
}


