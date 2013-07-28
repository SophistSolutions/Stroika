/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterator_inl_
#define _Stroika_Foundation_Traversal_Iterator_inl_

#include    "../Debug/Assertions.h"

// we allow fIterator to equal nullptr, as a sentinal value during iteration, signaling that iteration is Done
#define qIteratorUsingNullRepAsSentinalValue    1


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
            ********************************************************************************
            *********************** Iterator<T>::Rep_Cloner_ *******************************
            ********************************************************************************
            */
            template    <typename T>
            inline  shared_ptr<typename    Iterator<T>::IRep>  Iterator<T>::Rep_Cloner_::Copy (const IRep& t)
            {
                return Iterator<T>::Clone_ (t);
            }


            /*
            ********************************************************************************
            ****************************** Iterator<T>::IRep *******************************
            ********************************************************************************
            */
            template    <typename T>
            inline Iterator<T>::IRep::IRep ()
            {
            }
            template    <typename T>
            inline Iterator<T>::IRep::~IRep ()
            {
            }


            /*
            ********************************************************************************
            ********************************** Iterator<T> *********************************
            ********************************************************************************
            */
            template    <typename T>
            inline Iterator<T>::Iterator (const Iterator<T>& from)
                : inherited ()
                , fIterator_ (from.fIterator_)
                , fDoneFlag_ (DoneFlag::eUnknown)
                , fCurrent_ (from.fCurrent_)
            {
                RequireNotNull (from.fIterator_.get ());
            }
            template    <typename T>
            inline Iterator<T>::Iterator (const SharedIRepPtr& rep)
                : fIterator_ (rep)
                , fDoneFlag_ (DoneFlag::eUnknown)
                , fCurrent_ ()
            {
            }
            template    <typename T>
            inline Iterator<T>&    Iterator<T>::operator= (const Iterator<T>& rhs)
            {
                RequireNotNull (rhs.fIterator_.get ());
                fIterator_ = rhs.fIterator_;
                fDoneFlag_ = rhs.fDoneFlag_;
                fCurrent_ = rhs.fCurrent_;
                return *this;
            }
            template    <typename T>
            inline  typename    Iterator<T>::IRep&         Iterator<T>::GetRep ()
            {
                EnsureNotNull (fIterator_.get ());
                return *fIterator_;
            }
            template    <typename T>
            inline  const typename Iterator<T>::IRep&   Iterator<T>::GetRep () const
            {
                EnsureNotNull (fIterator_.get ());
                return *fIterator_;
            }
            template    <typename T>
            inline T   Iterator<T>::Current () const
            {
                RequireNotNull (fIterator_);
                Require (fCurrent_.IsPresent ());
                return *fCurrent_;
            }
            template    <typename T>
            inline bool    Iterator<T>::Done () const
            {
                if (fDoneFlag_ == DoneFlag::eUnknown) {
                    // must redo to use cached done flag.
                    // Reason for cast stuff is to avoid Clone if unneeded.
                    //
                    IRep*   rep =   const_cast<IRep*> (fIterator_.get ());
                    Memory::Optional<T> tmp;
                    rep->More (&tmp, false);
                    fDoneFlag_ = tmp.IsPresent () ? DoneFlag::eNotDone : DoneFlag::eDone;
                }
                return fDoneFlag_ == DoneFlag::eDone;
            }
            template    <typename T>
            inline    T   Iterator<T>::operator* () const
            {
                RequireNotNull (fIterator_);
                Require (not Done ());
                return *fCurrent_;
            }
            template    <typename T>
            inline    T*   Iterator<T>::operator-> ()
            {
                RequireNotNull (fIterator_);
                Require (not Done ());
                return fCurrent_.get ();
            }
            template    <typename T>
            inline    const T*   Iterator<T>::operator-> () const
            {
                RequireNotNull (fIterator_);
                Require (not Done ());
                return fCurrent_.get ();
            }
            template    <typename T>
            inline   void  Iterator<T>::operator++ ()
            {
                RequireNotNull (fIterator_);
                fIterator_->More (&fCurrent_, true);
                fDoneFlag_ = fCurrent_.IsPresent () ? DoneFlag::eNotDone : DoneFlag::eDone;
            }
            template    <typename T>
            inline   void  Iterator<T>::operator++ (int)
            {
                RequireNotNull (fIterator_);
                fIterator_->More (&fCurrent_, true);
                fDoneFlag_ = fCurrent_.IsPresent () ? DoneFlag::eNotDone : DoneFlag::eDone;
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
                const   Iterator<T>::IRep* lhsRep = fIterator_.get ();
                const   Iterator<T>::IRep* rhsRep = rhs.fIterator_.get ();
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
                const   Iterator<T>::IRep* lhsRep = fIterator_.get ();
                const   Iterator<T>::IRep* rhsRep = rhs.fIterator_.get ();
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
            inline  typename Iterator<T>::SharedIRepPtr   Iterator<T>::Clone_ (const typename Iterator<T>::IRep& rep)
            {
                return rep.Clone ();
            }
            template    <typename T>
            Iterator<T> Iterator<T>::GetEmptyIterator ()
            {
                class   RepSentinal_ : public Iterator<T>::IRep  {
                public:
                    virtual void    More (Memory::Optional<T>* result, bool advance) override {
                        RequireNotNull (result);
                        result->clear ();
                    }
                    virtual bool    StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                        RequireNotNull (rhs);
                        Memory::Optional<T> tmp;
                        const_cast<typename Iterator<T>::IRep*> (rhs)->More (&tmp, false);
                        return tmp.empty ();
                    }
                    virtual shared_ptr<IRep>    Clone () const override {
                        // May never be called since we never really call More() - except in special case of
                        // checking for at end. But it would be legal, and possible to call, so must fix at
                        // some point.
                        AssertNotImplemented ();
                        return nullptr;
                    }
                };
                static  Iterator<T> kSentinal_ = Iterator<T> (typename Iterator<T>::SharedIRepPtr (new RepSentinal_ ()));
                return kSentinal_;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Traversal_Iterator_inl_ */

