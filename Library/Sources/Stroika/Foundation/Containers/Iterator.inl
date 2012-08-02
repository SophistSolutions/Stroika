/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Iterator_inl_
#define _Stroika_Foundation_Containers_Iterator_inl_

#include    "../Debug/Assertions.h"

// we allow fIterator to equal nullptr, as a sentinal value during iteration, signaling that iteration is Done
#define qIteratorUsingNullRepAsSentinalValue    1


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
                Support for ranged for syntax: for (it : v) { it.Current (); }
                This typedef lets you easily construct iterators other than the basic
                iterator for the container.
                Sample usage:
                typedef RangedForIterator<Tally<T>, TallyMutator<T> >       Mutator;
            */
            template    <typename Container, typename IteratorClass>
            class   RangedForIterator {
            public:
                RangedForIterator (Container& t) :
                    fIt (t) {
                }

                RangedForIterator (const Container& t) :
                    fIt (t) {
                }
                nonvirtual  IteratorClass    begin () const {
                    return fIt;
                }

                IteratorClass end () const {
                    return (IteratorClass::GetEmptyIterator ());
                }

            private:
                IteratorClass   fIt;
            };



            // class IRep<T>
            template    <typename T>
            inline Iterator<T>::IRep::IRep ()
            {
            }
            template    <typename T>
            inline Iterator<T>::IRep::~IRep ()
            {
            }
            template    <typename T>
            inline bool    Iterator<T>::IRep::Done () const
            {
                return not const_cast<IRep*> (this)->More (nullptr, false);
            }



            // class Iterator<T>
            template    <typename T>
            inline Iterator<T>::Iterator (const Iterator<T>& from)
                : fIterator_ (from.fIterator_)
                , fCurrent_ (from.fCurrent_)
            {
                RequireNotNull (from.fIterator_.GetPointer ());
            }
            template    <typename T>
            inline Iterator<T>::Iterator (IRep* it)
                : fIterator_ (it)
                , fCurrent_ ()
            {
                RequireNotNull (it);
            }
            template    <typename T>
            inline Iterator<T>::~Iterator ()
            {
            }
            template    <typename T>
            inline Iterator<T>&    Iterator<T>::operator= (const Iterator<T>& rhs)
            {
                RequireNotNull (rhs.fIterator_.GetPointer ());
                fIterator_ = rhs.fIterator_;
                fCurrent_ = rhs.fCurrent_;
                return (*this);
            }
            template    <typename T>
            inline  typename    Iterator<T>::IRep&         Iterator<T>::_GetRep ()
            {
                EnsureNotNull (fIterator_.get ());
                return *fIterator_;
            }
            template    <typename T>
            inline  const typename Iterator<T>::IRep&   Iterator<T>::_GetRep () const
            {
                EnsureNotNull (fIterator_.get ());
                return *fIterator_;
            }
            template    <typename T>
            inline T   Iterator<T>::Current () const
            {
                RequireNotNull (fIterator_);
                return (fCurrent_);
            }
            template    <typename T>
            inline bool    Iterator<T>::Done () const
            {
                return fIterator_->Done ();
            }
            template    <typename T>
            inline    T   Iterator<T>::operator* () const
            {
                RequireNotNull (fIterator_);
                return (fCurrent_);
            }
            template    <typename T>
            inline   void  Iterator<T>::operator++ ()
            {
                RequireNotNull (fIterator_);
                fIterator_->More (&fCurrent_, true);
            }
            template    <typename T>
            inline   void  Iterator<T>::operator++ (int)
            {
                RequireNotNull (fIterator_);
                fIterator_->More (&fCurrent_, true);
            }
            template    <typename T>
            bool    Iterator<T>::WeakEquals (const Iterator& rhs) const
            {
                bool    lDone   =   Done ();
                bool    rDone   =   rhs.Done ();
                if (lDone != rDone) {
                    return false;
                }
                if (lDone) {
                    Assert (rDone);
                    return true;
                }
                Assert (not lDone and not rDone);
                // assigning to local variables to ensure const version called
                const   Iterator<T>::IRep* lhsRep = fIterator_.GetPointer ();
                const   Iterator<T>::IRep* rhsRep = rhs.fIterator_.GetPointer ();
                return (lhsRep == rhsRep);
            }
            template    <typename T>
            bool    Iterator<T>::StrongEquals (const Iterator& rhs) const
            {
                /*
                 *  StrongEquals is checked by first checking handling the case of special 'done' iterators. If two
                 *  iterators differ on Done () - they cannot be equal. And if they are both done (this is special - even if from different sources)
                 *  they are considered equal).
                 *
                 *  But then - we check that they are the same dynamic type, and if so, hand to one, and let it do the dynamic/concrete type
                 *  specific checks for equality.
                 */
                bool    lDone   =   Done ();
                bool    rDone   =   rhs.Done ();
                if (lDone != rDone) {
                    return false;
                }
                if (lDone) {
                    Assert (rDone);
                    return true;
                }
                Assert (not lDone and not rDone);
                // assigning to local variables to ensure const version called
                const   Iterator<T>::IRep* lhsRep = fIterator_.GetPointer ();
                const   Iterator<T>::IRep* rhsRep = rhs.fIterator_.GetPointer ();
                if (typeid (lhsRep) != typeid (rhsRep)) {
                    return false;
                }
                Ensure (lhsRep->StrongEquals (rhsRep) == rhsRep->StrongEquals (lhsRep));
                return (lhsRep->StrongEquals (rhsRep));
            }
            template    <typename T>
            inline  bool   Iterator<T>::operator== (const Iterator& rhs)  const
            {
                return WeakEquals (rhs);
            }
            template    <typename T>
            inline bool   Iterator<T>::operator!= (const Iterator& rhs)  const
            {
                return not operator== (rhs);
            }
            template    <typename T>
            inline  typename Iterator<T>::IRep*   Iterator<T>::Clone_ (const typename Iterator<T>::IRep& rep)
            {
                return rep.Clone ();
            }
            template    <typename T>
            Iterator<T> Iterator<T>::GetEmptyIterator ()
            {
                class   RepSentinal_ : public Iterator<T>::IRep  {
                public:
                    virtual bool    More (T* current, bool advance) override {
                        return false;
                    }
                    virtual bool    StrongEquals (typename Iterator<T>::IRep* rhs) override {
                        RequireNotNull (rhs);
                        return (rhs == this) or rhs->Done ();
                    }
                    virtual IRep*    Clone () const override {
                        RequireNotReached ();
                        return nullptr;
                    }
                };
                static  Iterator<T> kSentinal = Iterator<T> (new RepSentinal_ ());
                return kSentinal;
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Iterator_inl_ */

