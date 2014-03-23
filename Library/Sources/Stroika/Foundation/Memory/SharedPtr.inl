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
#include    "../Execution/SpinLock.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            namespace   Private_ {
                // OK to declare this way because we cannot have threads before main, and since declared this way till be
                // properly zero initialized
                extern  Execution::SpinLock sSharedPtrCopyLock_;
            }


            /*
             ********************************************************************************
             ************ Private_::ReferenceCounterContainerType_ **************************
             ********************************************************************************
             */
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline      Private_::ReferenceCounterContainerType_::ReferenceCounterContainerType_ ()
                : fCount (0)
                , fDeleteCounter_ (true)
            {
            }
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  Private_::ReferenceCounterContainerType_::ReferenceCounterContainerType_ (bool deleteCounter)
                : fCount (0)
                , fDeleteCounter_ (deleteCounter)
            {
            }


            /*
             ********************************************************************************
             *************************** Private_::Envelope_ ********************************
             ********************************************************************************
             */
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
#if     qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
                    template    <typename T2>
                    inline  Envelope_ (Envelope_<T2>&& from) noexcept
                :
                    fPtr_ (from.GetPtr ())
                    , fCountHolder_ (from.fCountHolder_)
                    {
                        from.fPtr_ = nullptr;
                        from.fCountHolder_ = nullptr;
                    }
#endif
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
                    template    <typename T2>
                    Envelope_& operator= (const Envelope_<T2>& rhs)
                    {
                        fPtr_ = rhs.fPtr_;
                        fCountHolder_ = rhs.fCountHolder_;
                        return *this;
                    }
#if     qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
                    template    <typename T2>
                    Envelope_& operator= (Envelope_<T2> && rhs)
                    {
                        fPtr_ = rhs.fPtr_;
                        fCountHolder_ = rhs.fCountHolder_;
                        rhs.fPtr_ = nullptr;
                        rhs.fCountHolder_ = nullptr;
                        return *this;
                    }
#endif
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
            template    <typename T2>
            inline  SharedPtr<T>::SharedPtr (T2* from)
                : fEnvelope_ (mkEnvelope_ (from))
            {
                Assert (fEnvelope_.GetPtr () == from);
                if (from != nullptr) {
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
#if     qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
            template    <typename T>
            inline  SharedPtr<T>::SharedPtr (SharedPtr<T>&& from) noexcept
:
            fEnvelope_ (move (from.fEnvelope_))
            {
                // no need to increment refcount here because the entire envelope moved from from to this, and so total counts same
            }
#endif
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
            template    <typename T2>
            typename SharedPtr<T>::Envelope_    SharedPtr<T>::mkEnvelope_ (T2* from, typename enable_if<is_convertible<T2*, Private_::ReferenceCounterContainerType_*>::value >::type*)
            {
                if (from == nullptr) {
                    return  Envelope_ (nullptr, nullptr);
                }
                else {
#if     qStroika_Foundation_Memory_NeedPtrStoredInEnableSharedFromThis_
                    from->fPtr_ = from;
#endif
                    return  Envelope_ (from, from);
                }
            }
            template    <typename T>
            template    <typename T2>
            typename SharedPtr<T>::Envelope_    SharedPtr<T>::mkEnvelope_ (T2* from, typename enable_if < !is_convertible<T2*, Private_::ReferenceCounterContainerType_*>::value >::type*)
            {
                return  Envelope_ (from, from == nullptr ? nullptr : ManuallyBlockAllocated<Private_::ReferenceCounterContainerType_>::New ());
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
#if     qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
            template    <typename T>
            inline  SharedPtr<T>& SharedPtr<T>::operator= (SharedPtr<T> && rhs) noexcept {
                fEnvelope_ = move (rhs.fEnvelope_); // no need to bump refcounts - moved from one to another
                return *this;
            }
#endif
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
                if (fEnvelope_.GetPtr () != nullptr)
                {
                    if (fEnvelope_.Decrement ()) {
                        fEnvelope_.DoDeleteCounter ();
                        delete fEnvelope_.GetPtr ();
                        fEnvelope_.SetPtr (nullptr);
                    }
                }
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
            inline  void    SharedPtr<T>::swap (SharedPtr<T>& rhs)
            {
                swap (fEnvelope_.fPtr_, rhs.fEnvelope_.fPtr_);
                swap (fEnvelope_.fCountHolder_, rhs.fEnvelope_.fCountHolder_);
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
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  enable_shared_from_this<T>::enable_shared_from_this ()
                : ReferenceCounterContainerType_ (false)
#if     qStroika_Foundation_Memory_NeedPtrStoredInEnableSharedFromThis_ && qDebug
                // only initialized for assertion in shared_from_this()
                , fPtr_ (nullptr)
#endif
            {
            }
            template    <typename   T>
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            inline  enable_shared_from_this<T>::enable_shared_from_this (const enable_shared_from_this& /*src*/)
                : ReferenceCounterContainerType_ (false)
#if     qStroika_Foundation_Memory_NeedPtrStoredInEnableSharedFromThis_ && qDebug
                // only initialized for assertion in shared_from_this()
                , fPtr_ (nullptr)
#endif
            {
            }
            template    <typename   T>
            inline  SharedPtr<T> enable_shared_from_this<T>::shared_from_this ()
            {
#if     qStroika_Foundation_Memory_NeedPtrStoredInEnableSharedFromThis_
                // If this assertion fails, its almost certainly because... See todo above - I see thy this doesnt fail
                // for std::shared_ptr<>>
                AssertNotNull (fPtr_);
                return (SharedPtr<T> (typename SharedPtr<T>::Envelope_ (fPtr_, this)));
#else
                /*
                 *  The Constructor for SharedPtr<T> expects a T*. However, enable_shared_from_this doesn't
                 *  own a T*. But recall, the ONLY legal way to use this enable_shared_from_this is:
                 *
                 *       struct  TTT : Memory::enable_shared_from_this<TTT> {
                 *           string x;
                 *           ....
                 *       };
                 *
                 *  So T must be a subclass of TTT, in this case (or equals T);
                 *
                 *  So if we have a legal pointer to enable_shared_from_this<T>, then it MUST also be castable to a pointer to T*!!!
                 */
                T*      ptr =   static_cast<T*> (this);
                return (SharedPtr<T> (typename SharedPtr<T>::Envelope_ (ptr, this)));
#endif
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
    template    <typename T>
    inline  Stroika::Foundation::Memory::SharedPtr<T>   atomic_load_explicit (const Stroika::Foundation::Memory::SharedPtr<T>* p, memory_order)
    {
        RequireNotNull (p);
        lock_gaurd<Execution::SpinLock> critSec (sSharedPtrCopyLock_);
        Stroika::Foundation::Memory::SharedPtr<_Ty> result = *p;
        return result;
    }
    template    <typename T>
    inline  Stroika::Foundation::Memory::SharedPtr<T>   atomic_load (const Stroika::Foundation::Memory::SharedPtr<T>* p)
    {
        RequireNotNull (p);
        return atomic_load_explicit (p, memory_order_seq_cst);
    }
    template    <typename T>
    inline  void    atomic_store_explicit (Stroika::Foundation::Memory::SharedPtr<T>* p, Stroika::Foundation::Memory::SharedPtr<T> o, memory_order)
    {
        lock_gaurd<Execution::SpinLock> critSec (sSharedPtrCopyLock_);
        p->swap (o);
    }
    template    <typename T>
    inline  void    atomic_store (Stroika::Foundation::Memory::SharedPtr<T>* p, Stroika::Foundation::Memory::SharedPtr<T> o)
    {
        atomic_store_explicit (p, o, memory_order_seq_cst);
    }
}
#endif  /*_Stroika_Foundation_Memory_SharedPtr_inl_*/
