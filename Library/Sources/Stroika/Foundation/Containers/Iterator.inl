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
                    return (IteratorClass (nullptr));
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
                : fIterator_ (from.fIterator_, &Clone_)
                , fCurrent_ (from.fCurrent_)
            {
                RequireNotNull (from.fIterator_);
            }
            template    <typename T>
            inline Iterator<T>::Iterator (IRep* it)
                : fIterator_ (it, &Clone_)
            {
                if (it == nullptr) {
                    fIterator_ = GetSentinal ().fIterator_;
                }
                EnsureNotNull (fIterator_);
            }
            template    <typename T>
            inline Iterator<T>::~Iterator ()
            {
            }
            template    <typename T>
            inline Iterator<T>&    Iterator<T>::operator= (const Iterator<T>& rhs)
            {
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
            inline bool   Iterator<T>::operator!= (Iterator rhs)  const
            {
                return not operator== (rhs);
            }
            template    <typename T>
            inline bool   Iterator<T>::operator== (Iterator rhs)  const
            {
                if (rhs.Done ()) {
                    return Done ();
                }
                else if (not Done ()) {
                    return false;
                }

                // assigning to local variables to ensure const version called
                const   Iterator<T>::IRep* lhsRep = fIterator_.GetPointer ();
                const   Iterator<T>::IRep* rhsRep = fIterator_.GetPointer ();
                return (lhsRep == rhsRep and fCurrent_ == rhs.fCurrent_);
            }
            template    <typename T>
            inline  typename Iterator<T>::IRep*   Iterator<T>::Clone_ (const typename Iterator<T>::IRep& rep)
            {
                return rep.Clone ();
            }
            template    <typename T>
            Iterator<T> Iterator<T>::GetSentinal ()
            {
                class   RepSentinal : public Iterator<T>::IRep  {
                public:
                    RepSentinal () {}
                public:
                    virtual bool    More (T* current, bool advance) override {
                        return false;
                    }
                    virtual IRep*    Clone () const override {
                        RequireNotReached ();
                        return nullptr;
                    }
                };
                static  Iterator<T> kSentinal = Iterator<T> (new RepSentinal ());
                return Iterator<T> (kSentinal);
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Iterator_inl_ */

