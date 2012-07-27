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
                    Rep_ (const Rep_& from);

                public:
                    static  void*   operator new (size_t size);
                    static  void    operator delete (void* p);

                public:
                    virtual size_t      GetLength () const override;
                    virtual bool        Contains (T item) const override;
                    virtual void        Compact () override;
                    virtual typename    Bag<T>::_IRep*  Clone () const override;

                    virtual Iterator<T>	MakeIterator () override;
                    virtual typename Bag<T>::_IMutatorRep*     MakeBagMutator () override;

                    virtual void    Add (T item) override;
                    virtual void    Remove (T item) override;
                    virtual void    RemoveAll () override;

                private:
                    LinkedList_Patch<T> fData_;

                private:
                    friend  class   Bag_LinkedList<T>::MutatorRep_;
                };


                // One rep for BOTH iterator and mutator - to save code - mutator just adds invisible functionality
                template    <typename T>
                class  Bag_LinkedList<T>::MutatorRep_ : public Bag<T>::_IMutatorRep {
                public:
                    MutatorRep_ (typename Bag_LinkedList<T>::Rep_& owner);
                    MutatorRep_ (typename Bag_LinkedList<T>::MutatorRep_& from);

                public:
                    static  void*   operator new (size_t size);
                    static  void    operator delete (void* p);

                public:
                    virtual bool    More (T* current, bool advance) override;

                    virtual typename Iterator<T>::Rep*      Clone () const override;

                    virtual void    RemoveCurrent () override;
                    virtual void    UpdateCurrent (T newValue) override;

                private:
                    LinkedListMutator_Patch<T>  fIterator_;
                };







                //  class   Bag_LinkedList<T>::Rep_
                template    <typename  T>
                inline  void*   Bag_LinkedList<T>::Rep_::operator new (size_t size)
                {
                    return (Memory::BlockAllocated<Rep_>::operator new (size));
                }
                template    <typename  T>
                inline  void    Bag_LinkedList<T>::Rep_::operator delete (void* p)
                {
                    Memory::BlockAllocated<Rep_>::operator delete (p);
                }
                template    <typename T>
                inline  Bag_LinkedList<T>::Rep_::Rep_ ()
                    : fData_ ()
                {
                }
                template    <typename T>
                inline  Bag_LinkedList<T>::Rep_::Rep_ (const Rep_& from)
                    : fData_ (from.fData_)
                {
                }
                template    <typename T>
                size_t  Bag_LinkedList<T>::Rep_::GetLength () const
                {
                    return (fData_.GetLength ());
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Compact ()
                {
                }
                template    <typename T>
                typename    Bag<T>::_IRep*  Bag_LinkedList<T>::Rep_::Clone () const
                {
                    return (new Rep_ (*this));
                }
                template    <typename  T>
                bool    Bag_LinkedList<T>::Rep_::Contains (T item) const
                {
                    return (fData_.Contains (item));
                }
                template    <typename T>
                Iterator<T>  Bag_LinkedList<T>::Rep_::MakeIterator ()
                {
                    return Iterator<T> (new Bag_LinkedList<T>::MutatorRep_ (*this));
                }
                template    <typename T>
                typename    Bag<T>::_IMutatorRep*   Bag_LinkedList<T>::Rep_::MakeBagMutator ()
                {
                    return (new Bag_LinkedList<T>::MutatorRep_ (*this));
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Add (T item)
                {
                    fData_.Prepend (item);
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::Remove (T item)
                {
                    fData_.Remove (item);
                }
                template    <typename T>
                void    Bag_LinkedList<T>::Rep_::RemoveAll ()
                {
                    fData_.RemoveAll ();
                }



                //Bag_LinkedList<T>::MutatorRep_
                template    <typename  T>
                inline  void*   Bag_LinkedList<T>::MutatorRep_::operator new (size_t size)
                {
                    return (Memory::BlockAllocated<MutatorRep_>::operator new (size));
                }
                template    <typename  T>
                inline  void    Bag_LinkedList<T>::MutatorRep_::operator delete (void* p)
                {
                    Memory::BlockAllocated<MutatorRep_>::operator delete (p);
                }
                template    <typename T>
                Bag_LinkedList<T>::MutatorRep_::MutatorRep_ (typename Bag_LinkedList<T>::Rep_& owner) :
                    fIterator_ (owner.fData_)
                {
                }
                template    <typename T>
                Bag_LinkedList<T>::MutatorRep_::MutatorRep_ (MutatorRep_& from)
                    : fIterator_ (from.fIterator_)
                {
                }
                template    <typename T>
                bool    Bag_LinkedList<T>::MutatorRep_::More (T* current, bool advance)
                {
                    return (fIterator_.More (current, advance));
                }
                template    <typename T>
                typename Iterator<T>::Rep*  Bag_LinkedList<T>::MutatorRep_::Clone () const
                {
                    return (new MutatorRep_ (*const_cast<MutatorRep_*> (this)));
                }
                template    <typename  T>
                void    Bag_LinkedList<T>::MutatorRep_::UpdateCurrent (T newValue)
                {
                    fIterator_.UpdateCurrent (newValue);
                }
                template    <typename  T>
                void    Bag_LinkedList<T>::MutatorRep_::RemoveCurrent ()
                {
                    fIterator_.RemoveCurrent ();
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
                    Add (start, end);
                }
                template    <typename T>
                Bag_LinkedList<T>::Bag_LinkedList (const Bag<T>& src)
                    : Bag<T> (new Rep_ ())
                {
                    operator+= (src);
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


