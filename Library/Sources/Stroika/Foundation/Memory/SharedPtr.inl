/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedPtr_inl_
#define _Stroika_Foundation_Memory_SharedPtr_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockAllocated.h"

#include    "../Execution/Exceptions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /*
             ********************************************************************************
             ************ Private_::ReferenceCounterContainerType_ **************************
             ********************************************************************************
             */
            inline  Private_::ReferenceCounterContainerType_::ReferenceCounterContainerType_ ()
                : fCount (0)
                , fDeleteCounter_ (true)
            {
            }
            inline  Private_::ReferenceCounterContainerType_::ReferenceCounterContainerType_ (bool deleteCounter)
                : fCount (0)
                , fDeleteCounter_ (deleteCounter)
            {
            }


            namespace   Private_ {
                template    <typename   T>
                class   Envelope_ {
                private:
                    T*                              fPtr_;
                    ReferenceCounterContainerType_* fCountHolder_;
                public:
                    Envelope_ (T* ptr, ReferenceCounterContainerType_* countHolder )
                        : fPtr_ (ptr)
                        , fCountHolder_ (countHolder)
                    {
                        Require ((fPtr_ == nullptr) == (fCountHolder_ == nullptr));
                    }
                    template    <typename T2>
                    inline  Envelope_ (Envelope_<T2>&& from) noexcept
                :
                    fPtr_ (from.GetPtr ())
                    , fCountHolder_ (from.fCountHolder_)
                    {
                        from.fPtr_ = nullptr;
                        from.fCountHolder_ = nullptr;
                    }
                    template    <typename T2>
                    inline  Envelope_ (const Envelope_<T2>& from) noexcept
                :
                    fPtr_ (from.GetPtr ())
                    , fCountHolder_ (from.fCountHolder_)
                    {
                    }
                    template <typename T2>
                    inline  Envelope_ (const Envelope_<T2>& from, T* newP) noexcept
                :
                    fPtr_ (newP)
                    , fCountHolder_ (from.fCountHolder_)
                    {
                        // reason for this is for dynamic cast. We allow replacing the P with a newP, but the
                        // actual ptr cannot change, and this assert check automatically converts pointers to
                        // a common base pointer type
                        Require (newP == from.GetPtr ());
                    }
                    inline  T*      GetPtr () const noexcept
                    {
                        return fPtr_;
                    }
                    inline  void    SetPtr (T* p) noexcept {
                        fPtr_ = p;
                    }
                    inline  SharedPtrBase::ReferenceCountType CurrentRefCount () const noexcept
                    {
                        return fCountHolder_ == nullptr ? 0 : fCountHolder_->fCount.load ();
                    }
                    inline  void    Increment () noexcept {
                        RequireNotNull (fCountHolder_);
                        fCountHolder_->fCount++;
                    }
                    inline  bool    Decrement () noexcept {
                        Require (CurrentRefCount () > 0);
                        if (--fCountHolder_->fCount == 0)
                        {
                            return true;
                        }
                        return false;
                    }
                    inline  void    DoDeleteCounter () noexcept {
                        RequireNotNull (fCountHolder_);
                        if (fCountHolder_->fDeleteCounter_)
                        {
                            ManuallyBlockAllocated<ReferenceCounterContainerType_>::Delete (fCountHolder_);
                        }
                        fCountHolder_ = nullptr;
                    }

                private:
                    template    <typename T2>
                    friend  class   Envelope_;
                };
            }


            /*
             ********************************************************************************
             ********************************** SharedPtr<T> ********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr () noexcept
:
            fEnvelope_ (nullptr, nullptr)
            {
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (nullptr_t) noexcept
:
            fEnvelope_ (nullptr, nullptr)
            {
            }
            template    <typename T>
            template    <typename CHECK_KEY>
            inline  SharedPtr<T>::SharedPtr (T* from, typename enable_if <is_convertible<T*, enable_shared_from_this<CHECK_KEY>*>::value>::type*)
                : fEnvelope_ (from, from)
            {
                Assert (fEnvelope_.GetPtr () == from);
                if (from != nullptr) {
                    from->fPtr_ = from;
                }
                if (from != nullptr) {
                    // NB: the fEnvelope_.CurrentRefCount () USUALLY == 0, but not necessarily, if the refcount is stored
                    // in the 'from' - (see SharedPtrBase) - in which case the refcount might already be larger.
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            template    <typename CHECK_KEY>
            inline  SharedPtr<T>::SharedPtr (T* from, typename enable_if < !is_convertible<T*, enable_shared_from_this<CHECK_KEY>*>::value >::type*)
                : fEnvelope_ (from, from == nullptr ? nullptr : ManuallyBlockAllocated<Private_::ReferenceCounterContainerType_>::New ())
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    // NB: the fEnvelope_.CurrentRefCount () USUALLY == 0, but not necessarily, if the refcount is stored
                    // in the 'from' - (see SharedPtrBase) - in which case the refcount might already be larger.
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (const Envelope_& from) noexcept
:
            fEnvelope_ (from)
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (const SharedPtr<T>& from) noexcept
:
            fEnvelope_ (from.fEnvelope_)
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (SharedPtr<T>&& from) noexcept
:
            fEnvelope_ (move (from.fEnvelope_))
            {
                // no need to increment refcount here because the entire envelope moved from from to this, and so total counts same
            }
            template    <typename T>
            template    <typename T2>
            SharedPtr<T>::SharedPtr (const SharedPtr<T2>& from) noexcept
:
            fEnvelope_ (from.fEnvelope_)
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    fEnvelope_.Increment ();
                }
            }
            template    <typename T>
            inline  SharedPtr<T>& SharedPtr<T>::operator= (const SharedPtr<T>& rhs) noexcept {
                if (rhs.fEnvelope_.GetPtr () != fEnvelope_.GetPtr ())
                {
                    if (fEnvelope_.GetPtr () != nullptr) {
                        if (fEnvelope_.Decrement ()) {
                            fEnvelope_.DoDeleteCounter ();
                            delete fEnvelope_.GetPtr ();
                            fEnvelope_.SetPtr (nullptr);
                        }
                    }
                    fEnvelope_ = rhs.fEnvelope_;
                    if (fEnvelope_.GetPtr () != nullptr) {
                        Assert (fEnvelope_.CurrentRefCount () > 0);
                        fEnvelope_.Increment ();
                    }
                }
                return *this;
            }
            template    <typename T>
            inline  SharedPtr<T>::~SharedPtr ()
            {
                if (fEnvelope_.GetPtr () != nullptr) {
                    if (fEnvelope_.Decrement ()) {
                        fEnvelope_.DoDeleteCounter ();
                        delete fEnvelope_.GetPtr ();
                    }
                }
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::IsNull () const noexcept
            {
                return fEnvelope_.GetPtr () == nullptr;
            }
            template    <typename T>
            inline  T&  SharedPtr<T>::GetRep () const noexcept
            {
                RequireNotNull (fEnvelope_.GetPtr ());
                Assert (fEnvelope_.CurrentRefCount () > 0);
                return *fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  T* SharedPtr<T>::operator-> () const noexcept
            {
                return &GetRep ();
            }
            template    <typename T>
            inline  T& SharedPtr<T>::operator* () const noexcept
            {
                return GetRep ();
            }
            template    <typename T>
            inline  SharedPtr<T>::operator T* () const noexcept
            {
                return fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  T*  SharedPtr<T>::get () const noexcept
            {
                return fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  void    SharedPtr<T>::release () noexcept {
                *this = SharedPtr<T> (nullptr);
            }
            template    <typename T>
            inline  void    SharedPtr<T>::clear () noexcept {
                release ();
            }
            template    <typename T>
            inline  void    SharedPtr<T>::reset (T* p)
            {
                if (fEnvelope_.GetPtr () != p) {
                    *this = SharedPtr<T> (p);
                }
            }
            template    <typename T>
            template    <typename T2>
            inline  SharedPtr<T2> SharedPtr<T>::Dynamic_Cast () const  noexcept
            {
                return SharedPtr<T2> (typename SharedPtr<T2>::Envelope_ (fEnvelope_, dynamic_cast<T2*> (get ())));
            }
            template    <typename T>
            inline  typename SharedPtr<T>::ReferenceCountType   SharedPtr<T>::CurrentRefCount () const noexcept
            {
                return fEnvelope_.CurrentRefCount ();
            }
            template    <typename T>
            inline  typename SharedPtr<T>::ReferenceCountType   SharedPtr<T>::use_count () const noexcept
            {
                return fEnvelope_.CurrentRefCount ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::IsUnique () const noexcept
            {
                return fEnvelope_.CurrentRefCount () == 1;
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::unique () const noexcept
            {
                // respect the stl-ish names
                return IsUnique ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator< (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () < rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator<= (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () <= rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator> (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () > rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator>= (const SharedPtr<T>& rhs) const noexcept
            {
                // not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
                // this does the same thing...
                //      -- LGP 2009-01-11
                return fEnvelope_.GetPtr () >= rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator== (const SharedPtr<T>& rhs) const noexcept
            {
                return fEnvelope_.GetPtr () == rhs.fEnvelope_.GetPtr ();
            }
            template    <typename T>
            inline  bool    SharedPtr<T>::operator!= (const SharedPtr<T>& rhs) const noexcept
            {
                return fEnvelope_.GetPtr () != rhs.fEnvelope_.GetPtr ();
            }


            /*
             ********************************************************************************
             ************************* enable_shared_from_this<T> ***************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  enable_shared_from_this<T>::enable_shared_from_this ()
                : ReferenceCounterContainerType_ (false)
#if     qDebug
                // only initialized for assertion in shared_from_this()
                , fPtr_ (nullptr)
#endif
            {
            }
            template    <typename   T>
            inline  SharedPtr<T> enable_shared_from_this<T>::shared_from_this ()
            {
                /*
                 * The Constructor for SharedPtr<T> expects a T*. However, we don't have a T*. But recall,
                 * the ONLY legal way to use this enable_shared_from_this is:
                 *
                 *       struct  TTT : Memory::enable_shared_from_this<TTT> {
                 *           string x;
                 *           ....
                 *       };
                 *
                 *   and so if we have a legal pointer to enable_shared_from_this<T>, then it MUST also be castable to a pointer to T*!!!
                 */
                AssertNotNull (fPtr_);
                return (SharedPtr<T> (typename SharedPtr<T>::Envelope_ (fPtr_, this)));
            }


        }


        namespace   Execution {
            template    <typename T>
            inline  void    ThrowIfNull (const Memory::SharedPtr<T>& p)
            {
                if (p.get () == nullptr) {
                    Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<T> ()) - throwing bad_alloc ()");
                }
            }
        }


    }
}
namespace std {
    template    <class TO_TYPE_T,   class FROM_TYPE_T>
    inline  Stroika::Foundation::Memory::SharedPtr<TO_TYPE_T>   dynamic_pointer_cast (const Stroika::Foundation::Memory::SharedPtr<FROM_TYPE_T>& sp) noexcept {
        return sp.template Dynamic_Cast<TO_TYPE_T> ();
    }
}
#endif  /*_Stroika_Foundation_Memory_SharedPtr_inl_*/
