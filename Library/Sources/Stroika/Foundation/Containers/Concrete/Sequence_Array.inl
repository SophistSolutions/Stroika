/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"

#include    "Private/Array.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Sequence_Array<T>::Rep_ : public Sequence<T>::_IRep {
                public:
                    Rep_ ();
                    ~Rep_ ();

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual shared_ptr<typename Iterable<T>::_IRep>    Clone () const override;
                    virtual Iterator<T>                     MakeIterator () const override;
                    virtual size_t                          GetLength () const override;
                    virtual bool                            IsEmpty () const override;
                    virtual void                            Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                     ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Sequence<T>::_IRep overrides
                public:
                    virtual T       GetAt (size_t i) const override;
                    virtual size_t  IndexOf (const Iterator<T>& i) const override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Insert (size_t at, const T* from, const T* to) override;
                    virtual void    Remove (size_t from, size_t to) override;

                private:
                    Array_Patch<T>  fData_;
                    friend  class Sequence_Array<T>::IteratorRep_;
                };


                template    <typename T>
                class  Sequence_Array<T>::IteratorRep_ : public Iterator<T>::IRep {
                public:
                    explicit IteratorRep_ (typename Sequence_Array<T>::Rep_& owner);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                    // Iterator<T>::IRep
                public:
                    virtual shared_ptr<typename Iterator<T>::IRep>     Clone () const override;
                    virtual bool                            More (T* current, bool advance) override;
                    virtual bool                            StrongEquals (const typename Iterator<T>::IRep* rhs) const override;

                private:
                    mutable ForwardArrayMutator_Patch<T>    fIterator_;

                private:
                    friend  class   Rep_;
                };


                /*
                ********************************************************************************
                ************************* Sequence_Array<T>::IteratorRep_ **********************
                ********************************************************************************
                */
                template    <typename T>
                Sequence_Array<T>::IteratorRep_::IteratorRep_ (typename Sequence_Array<T>::Rep_& owner)
                    : Iterator<T>::IRep ()
                    , fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                bool    Sequence_Array<T>::IteratorRep_::More (T* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                bool    Sequence_Array<T>::IteratorRep_::StrongEquals (const typename Iterator<T>::IRep* rhs) const
                {
                    AssertNotImplemented ();
                    return false;
                }
                template    <typename T>
                shared_ptr<typename Iterator<T>::IRep>  Sequence_Array<T>::IteratorRep_::Clone () const
                {
                    return shared_ptr<typename Iterator<T>::IRep> (new IteratorRep_ (*this));
                }


                /*
                ********************************************************************************
                *************************** Sequence_Array<T>::Rep_ ****************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Sequence_Array<T>::Rep_::Rep_ ()
                    : fData_ ()
                {
                }
                template    <typename T>
                Sequence_Array<T>::Rep_::~Rep_ ()
                {
                }
                template    <typename T>
                shared_ptr<typename Iterable<T>::_IRep>  Sequence_Array<T>::Rep_::Clone () const
                {
                    return shared_ptr<typename Iterable<T>::_IRep> (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Sequence_Array<T>::Rep_::MakeIterator () const
                {
                    Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                    Iterator<T> tmp = Iterator<T> (typename Iterator<T>::SharedByValueRepType (new IteratorRep_ (*NON_CONST_THIS)));
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T>
                size_t  Sequence_Array<T>::Rep_::GetLength () const
                {
                    return (fData_.GetLength ());
                }
                template    <typename T>
                bool  Sequence_Array<T>::Rep_::IsEmpty () const
                {
                    return (fData_.GetLength () == 0);
                }
                template    <typename T>
                void      Sequence_Array<T>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Sequence_Array<T>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                T    Sequence_Array<T>::Rep_::GetAt (size_t i) const
                {
                    return fData_.GetAt (i);
                }
                template    <typename T>
                size_t    Sequence_Array<T>::Rep_::IndexOf (const Iterator<T>& i) const
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_Array<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_Array<T>::IteratorRep_&> (ir);
                    return mir.fIterator_.CurrentIndex ();
                }
                template    <typename T>
                void    Sequence_Array<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_Array<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_Array<T>::IteratorRep_&> (ir);
                    mir.fIterator_.RemoveCurrent ();
                }
                template    <typename T>
                void    Sequence_Array<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    const typename Sequence_Array<T>::IteratorRep_&       mir =   dynamic_cast<const typename Sequence_Array<T>::IteratorRep_&> (ir);
                    mir.fIterator_.UpdateCurrent (newValue);
                }
                template    <typename T>
                void    Sequence_Array<T>::Rep_::Insert (size_t at, const T* from, const T* to)
                {
                    // quickie poor impl
                    for (auto i = from; i != to; ++i) {
                        fData_.InsertAt (*i, at++);
                    }
                }
                template    <typename T>
                void    Sequence_Array<T>::Rep_::Remove (size_t from, size_t to)
                {
                    // quickie poor impl
                    for (size_t i = from; i < to; ++i) {
                        fData_.RemoveAt (from);
                    }
                }


                /*
                ********************************************************************************
                **************************** Sequence_Array<T> *********************************
                ********************************************************************************
                */
                template    <typename T>
                Sequence_Array<T>::Sequence_Array ()
                    : Sequence<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T>
                Sequence_Array<T>::Sequence_Array (const Sequence<T>& s)
                    : Sequence<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    SetCapacity (s.GetLength ());
                    operator+= (s);
                }
                template    <typename T>
                Sequence_Array<T>::Sequence_Array (const T* start, const T* end)
                    : Sequence<T> (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    if (start != end) {
                        SetCapacity (end - start);
                        Add (start, end);
                    }
                }
                template    <typename T>
                inline  Sequence_Array<T>::Sequence_Array (const Sequence_Array<T>& s)
                    : Sequence<T> (s)
                {
                }
                template    <typename T>
                inline  Sequence_Array<T>&   Sequence_Array<T>::operator= (const Sequence_Array<T>& s)
                {
                    Sequence<T>::operator= (s);
                    return *this;
                }
                template    <typename T>
                inline  const typename Sequence_Array<T>::Rep_&  Sequence_Array<T>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    return (static_cast<const Rep_&> (Sequence<T>::_GetRep ()));
                }
                template    <typename T>
                inline  typename Sequence_Array<T>::Rep_&    Sequence_Array<T>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    return (static_cast<const Rep_&> (Sequence<T>::_GetRep ()));
                }
                template    <typename T>
                inline  void    Sequence_Array<T>::Compact ()
                {
                    GetRep_ ().fData_.Compact ();
                }
                template    <typename T>
                inline  size_t  Sequence_Array<T>::GetCapacity () const
                {
                    return (GetRep_ ().fData_.GetCapacity ());
                }
                template    <typename T>
                inline  void    Sequence_Array<T>::SetCapacity (size_t slotsAlloced)
                {
                    GetRep_ ().fData_.SetCapacity (slotsAlloced);
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Sequence_Array_inl_ */
