﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <mutex>

#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/SpinLock.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Memory {
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    namespace Private_ {
        // OK to declare this way because we cannot have threads before main, and since declared this way till be
        // properly zero initialized
        inline conditional_t<Execution::kSpinLock_IsFasterThan_mutex, Execution::SpinLock, mutex> sSharedPtrCopyLock_;
    }

    /*
     ********************************************************************************
     ************ Private_::ReferenceCounterContainerType_ **************************
     ********************************************************************************
     */
    inline Private_::ReferenceCounterContainerType_::ReferenceCounterContainerType_ ()
        : fCount{0}
        , fDeleteCounter_{true}
    {
    }
    inline Private_::ReferenceCounterContainerType_::ReferenceCounterContainerType_ (bool deleteCounter)
        : fCount{0}
        , fDeleteCounter_{deleteCounter}
    {
    }

    /*
     ********************************************************************************
     *************************** Private_::Envelope_ ********************************
     ********************************************************************************
     */
    namespace Private_ {
        template <typename T>
        class Envelope_ {
        private:
            T*                              fPtr_;
            ReferenceCounterContainerType_* fCountHolder_;

        public:
            Envelope_ (T* ptr, ReferenceCounterContainerType_* countHolder)
                : fPtr_ (ptr)
                , fCountHolder_ (countHolder)
            {
                Require ((fPtr_ == nullptr) == (fCountHolder_ == nullptr));
            }
            inline Envelope_ (Envelope_&& from) noexcept
                : fPtr_ (from.GetPtr ())
                , fCountHolder_ (from.fCountHolder_)
            {
                from.fPtr_         = nullptr;
                from.fCountHolder_ = nullptr;
            }
            template <typename T2>
            inline Envelope_ (Envelope_<T2>&& from) noexcept
                : fPtr_ (from.GetPtr ())
                , fCountHolder_ (from.fCountHolder_)
            {
                from.fPtr_         = nullptr;
                from.fCountHolder_ = nullptr;
            }
            inline Envelope_ (const Envelope_& from) noexcept
                : fPtr_ (from.GetPtr ())
                , fCountHolder_ (from.fCountHolder_)
            {
            }
            template <typename T2>
            inline Envelope_ (const Envelope_<T2>& from) noexcept
                : fPtr_ (from.GetPtr ())
                , fCountHolder_ (from.fCountHolder_)
            {
            }
            template <typename T2>
            inline Envelope_ (const Envelope_<T2>& from, T* newP) noexcept
                : fPtr_ (newP)
                , fCountHolder_ (from.fCountHolder_)
            {
                // reason for this is for dynamic cast. We allow replacing the P with a newP, but the
                // actual ptr cannot change, and this assert check automatically converts pointers to
                // a common base pointer type
                Require (newP == from.GetPtr ());
            }
            inline Envelope_& operator= (const Envelope_& rhs)
            {
                fPtr_         = rhs.fPtr_;
                fCountHolder_ = rhs.fCountHolder_;
                return *this;
            }
            template <typename T2>
            inline Envelope_& operator= (const Envelope_<T2>& rhs)
            {
                fPtr_         = rhs.fPtr_;
                fCountHolder_ = rhs.fCountHolder_;
                return *this;
            }
            inline Envelope_& operator= (Envelope_&& rhs) noexcept
            {
                fPtr_             = rhs.fPtr_;
                fCountHolder_     = rhs.fCountHolder_;
                rhs.fPtr_         = nullptr;
                rhs.fCountHolder_ = nullptr;
                return *this;
            }
            template <typename T2>
            inline Envelope_& operator= (Envelope_<T2>&& rhs)
            {
                fPtr_             = rhs.fPtr_;
                fCountHolder_     = rhs.fCountHolder_;
                rhs.fPtr_         = nullptr;
                rhs.fCountHolder_ = nullptr;
                return *this;
            }
            inline void swap (Envelope_& rhs)
            {
                swap (fPtr_, rhs.fPtr_);
                swap (fCountHolder_, rhs.fCountHolder_);
            }
            inline T* GetPtr () const noexcept
            {
                return fPtr_;
            }
            inline void SetPtr (T* p) noexcept
            {
                fPtr_ = p;
            }
            inline SharedPtrBase::ReferenceCountType CurrentRefCount () const noexcept
            {
                return fCountHolder_ == nullptr ? 0 : fCountHolder_->fCount.load ();
            }
            inline void Increment () noexcept
            {
                RequireNotNull (fCountHolder_);
                ++fCountHolder_->fCount;
            }
            inline bool Decrement () noexcept
            {
                Require (CurrentRefCount () > 0);
                if (--fCountHolder_->fCount == 0) {
                    return true;
                }
                return false;
            }
            inline void DoDeleteCounter () noexcept
            {
                RequireNotNull (fCountHolder_);
                if (fCountHolder_->fDeleteCounter_) {
                    ManuallyBlockAllocated<ReferenceCounterContainerType_>::Delete (fCountHolder_);
                }
                fCountHolder_ = nullptr;
            }
#if qStroika_FeatureSupported_Valgrind
            // See http://valgrind.org/docs/manual/hg-manual.html section 7.5
            inline void ValgrindAnnotateGotLockZeroCaseClearing ()
            {
                RequireNotNull (fCountHolder_);
                ANNOTATE_HAPPENS_AFTER (&fCountHolder_->fCount);
                ANNOTATE_HAPPENS_BEFORE_FORGET_ALL (&fCountHolder_->fCount);
            }
            inline void ValgrindAnnotateNotGotZeroCase ()
            {
                RequireNotNull (fCountHolder_);
                ANNOTATE_HAPPENS_BEFORE (&fCountHolder_->fCount);
            }
#endif
        private:
            template <typename T2>
            friend class Envelope_;
        };
    }

    /*
     ********************************************************************************
     ********************************** SharedPtr<T> ********************************
     ********************************************************************************
     */
    template <typename T>
    inline SharedPtr<T>::SharedPtr () noexcept
        : fEnvelope_ (nullptr, nullptr)
    {
    }
    template <typename T>
    inline SharedPtr<T>::SharedPtr (nullptr_t) noexcept
        : fEnvelope_ (nullptr, nullptr)
    {
    }
    template <typename T>
    template <typename T2, enable_if_t<is_convertible_v<T2*, T*>>*>
    inline SharedPtr<T>::SharedPtr (T2* from)
        : fEnvelope_ (mkEnvelope_ (from))
    {
        Assert (fEnvelope_.GetPtr () == from);
        if (from != nullptr) {
            // NB: the fEnvelope_.CurrentRefCount () USUALLY == 0, but not necessarily, if the refcount is stored
            // in the 'from' - (see SharedPtrBase) - in which case the refcount might already be larger.
            fEnvelope_.Increment ();
        }
    }
    template <typename T>
    inline SharedPtr<T>::SharedPtr (const Envelope_& from) noexcept
        : fEnvelope_ (from)
    {
        if (fEnvelope_.GetPtr () != nullptr) {
            fEnvelope_.Increment ();
        }
    }
    template <typename T>
    inline SharedPtr<T>::SharedPtr (const SharedPtr& from) noexcept
        : fEnvelope_ (from.fEnvelope_)
    {
        if (fEnvelope_.GetPtr () != nullptr) {
            fEnvelope_.Increment ();
        }
    }
    template <typename T>
    inline SharedPtr<T>::SharedPtr (SharedPtr&& from) noexcept
        : fEnvelope_ (move (from.fEnvelope_))
    {
        Assert (from.fEnvelope_.GetPtr () == nullptr);
        // no need to increment refcount here because the entire envelope moved from from to this, and so total counts same
    }
    template <typename T>
    template <typename T2, enable_if_t<is_convertible_v<T2*, T*>>*>
    SharedPtr<T>::SharedPtr (const SharedPtr<T2>& from) noexcept
        : fEnvelope_ (from.fEnvelope_)
    {
        if (fEnvelope_.GetPtr () != nullptr) {
            fEnvelope_.Increment ();
        }
    }
    template <typename T>
    template <typename T2, enable_if_t<is_convertible_v<T2*, T*>>*>
    SharedPtr<T>::SharedPtr (SharedPtr<T2>&& from) noexcept
        : fEnvelope_ (move (from.fEnvelope_))
    {
        Assert (from.fEnvelope_.GetPtr () == nullptr);
        // no need to increment refcount here because the entire envelope moved from from to this, and so total counts same
    }
    template <typename T>
    template <typename T2>
    inline typename SharedPtr<T>::Envelope_
    SharedPtr<T>::mkEnvelope_ (T2* from, enable_if_t<is_convertible_v<T2*, Private_::ReferenceCounterContainerType_*>>*)
    {
        if (from == nullptr) {
            return Envelope_{nullptr, nullptr};
        }
        else {
            return Envelope_{from, from};
        }
    }
    template <typename T>
    template <typename T2>
    typename SharedPtr<T>::Envelope_ SharedPtr<T>::mkEnvelope_ (T2* from,
                                                                enable_if_t<!is_convertible_v<T2*, Private_::ReferenceCounterContainerType_*>>*)
    {
        return Envelope_{from, from == nullptr ? nullptr : ManuallyBlockAllocated<Private_::ReferenceCounterContainerType_>::New ()};
    }
    template <typename T>
    SharedPtr<T>& SharedPtr<T>::operator= (const SharedPtr& rhs) noexcept
    {
        if (rhs.fEnvelope_.GetPtr () != fEnvelope_.GetPtr ()) {
            if (fEnvelope_.GetPtr () != nullptr) {
                if (fEnvelope_.Decrement ()) {
#if qStroika_FeatureSupported_Valgrind
                    fEnvelope_.ValgrindAnnotateGotLockZeroCaseClearing ();
#endif
                    fEnvelope_.DoDeleteCounter ();
                    delete fEnvelope_.GetPtr ();
                    fEnvelope_.SetPtr (nullptr);
                }
#if qStroika_FeatureSupported_Valgrind
                else {
                    fEnvelope_.ValgrindAnnotateNotGotZeroCase ();
                }
#endif
            }
            fEnvelope_ = rhs.fEnvelope_;
            if (fEnvelope_.GetPtr () != nullptr) {
                Assert (fEnvelope_.CurrentRefCount () > 0);
                fEnvelope_.Increment ();
            }
        }
        return *this;
    }
    template <typename T>
    SharedPtr<T>& SharedPtr<T>::operator= (SharedPtr&& rhs) noexcept
    {
        if (rhs.fEnvelope_.GetPtr () != fEnvelope_.GetPtr ()) {
            if (fEnvelope_.GetPtr () != nullptr) {
                if (fEnvelope_.Decrement ()) {
#if qStroika_FeatureSupported_Valgrind
                    fEnvelope_.ValgrindAnnotateGotLockZeroCaseClearing ();
#endif
                    fEnvelope_.DoDeleteCounter ();
                    delete fEnvelope_.GetPtr ();
                    fEnvelope_.SetPtr (nullptr);
                }
#if qStroika_FeatureSupported_Valgrind
                else {
                    fEnvelope_.ValgrindAnnotateNotGotZeroCase ();
                }
#endif
            }
            fEnvelope_ = move (rhs.fEnvelope_);           // no need to bump refcounts - moved from one to another
            Assert (rhs.fEnvelope_.GetPtr () == nullptr); // NB: we ONLY gaurantee this if not self-moving
        }
        return *this;
    }
    template <typename T>
    inline SharedPtr<T>::~SharedPtr ()
    {
        if (fEnvelope_.GetPtr () != nullptr) {
            if (fEnvelope_.Decrement ()) {
#if qStroika_FeatureSupported_Valgrind
                fEnvelope_.ValgrindAnnotateGotLockZeroCaseClearing ();
#endif
                fEnvelope_.DoDeleteCounter ();
                delete fEnvelope_.GetPtr ();
            }
#if qStroika_FeatureSupported_Valgrind
            else {
                fEnvelope_.ValgrindAnnotateNotGotZeroCase ();
            }
#endif
        }
    }
    template <typename T>
    inline bool SharedPtr<T>::IsNull () const noexcept
    {
        return fEnvelope_.GetPtr () == nullptr;
    }
    template <typename T>
    inline T& SharedPtr<T>::GetRep () const noexcept
    {
        RequireNotNull (fEnvelope_.GetPtr ());
        Assert (fEnvelope_.CurrentRefCount () > 0);
        return *fEnvelope_.GetPtr ();
    }
    template <typename T>
    inline T* SharedPtr<T>::operator->() const noexcept
    {
        return &GetRep ();
    }
    template <typename T>
    inline T& SharedPtr<T>::operator* () const noexcept
    {
        return GetRep ();
    }
    template <typename T>
    inline SharedPtr<T>::operator T* () const noexcept
    {
        return fEnvelope_.GetPtr ();
    }
    template <typename T>
    inline T* SharedPtr<T>::get () const noexcept
    {
        return fEnvelope_.GetPtr ();
    }
    template <typename T>
    inline void SharedPtr<T>::release () noexcept
    {
        if (fEnvelope_.GetPtr () != nullptr) {
            if (fEnvelope_.Decrement ()) {
#if qStroika_FeatureSupported_Valgrind
                fEnvelope_.ValgrindAnnotateGotLockZeroCaseClearing ();
#endif
                fEnvelope_.DoDeleteCounter ();
                delete fEnvelope_.GetPtr ();
                fEnvelope_.SetPtr (nullptr);
            }
#if qStroika_FeatureSupported_Valgrind
            else {
                fEnvelope_.ValgrindAnnotateNotGotZeroCase ();
            }
#endif
        }
    }
    template <typename T>
    inline void SharedPtr<T>::clear () noexcept
    {
        release ();
    }
    template <typename T>
    inline void SharedPtr<T>::reset (T* p)
    {
        if (fEnvelope_.GetPtr () != p) {
            *this = SharedPtr<T> (p);
        }
    }
    template <typename T>
    template <typename T2>
    inline SharedPtr<T2> SharedPtr<T>::Dynamic_Cast () const noexcept
    {
        return SharedPtr<T2> (typename SharedPtr<T2>::Envelope_ (fEnvelope_, dynamic_cast<T2*> (get ())));
    }
    template <typename T>
    inline void SharedPtr<T>::swap (SharedPtr& rhs)
    {
        swap (fEnvelope_, rhs.fEnvelope_);
    }
    template <typename T>
    inline typename SharedPtr<T>::ReferenceCountType SharedPtr<T>::CurrentRefCount () const noexcept
    {
        return fEnvelope_.CurrentRefCount ();
    }
    template <typename T>
    inline typename SharedPtr<T>::ReferenceCountType SharedPtr<T>::use_count () const noexcept
    {
        return fEnvelope_.CurrentRefCount ();
    }
    template <typename T>
    inline bool SharedPtr<T>::IsUnique () const noexcept
    {
        return fEnvelope_.CurrentRefCount () == 1;
    }
    template <typename T>
    inline bool SharedPtr<T>::unique () const noexcept
    {
        // respect the stl-ish names
        return IsUnique ();
    }
    template <typename T>
    constexpr bool SharedPtr<T>::operator== (const SharedPtr& rhs) const
    {
        return fEnvelope_.GetPtr () == rhs.fEnvelope_.GetPtr ();
    }
    template <typename T>
    constexpr bool SharedPtr<T>::operator== (nullptr_t) const
    {
        return fEnvelope_.GetPtr () == nullptr;
    }
    template <typename T>
    constexpr strong_ordering SharedPtr<T>::operator<=> (const SharedPtr& rhs) const
    {
        return fEnvelope_.GetPtr () <=> rhs.fEnvelope_.GetPtr ();
    }
    template <typename T>
    inline SharedPtr<T>::operator bool () const noexcept
    {
        return get () != nullptr;
    }

    /*
     ********************************************************************************
     ************************* enable_shared_from_this<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    constexpr inline enable_shared_from_this<T>::enable_shared_from_this ()
        : ReferenceCounterContainerType_ (false)
    {
    }
    template <typename T>
    constexpr inline enable_shared_from_this<T>::enable_shared_from_this (const enable_shared_from_this& /*src*/)
        : ReferenceCounterContainerType_ (false)
    {
    }
    template <typename T>
    inline SharedPtr<T> enable_shared_from_this<T>::shared_from_this ()
    {
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
        T* ptr = static_cast<T*> (this);
        return (SharedPtr<T> (typename SharedPtr<T>::Envelope_ (ptr, this)));
    }
}

namespace Stroika::Foundation::Execution {
    template <typename T>
    inline void ThrowIfNull (const Memory::SharedPtr<T>& p)
    {
        if (p.get () == nullptr) [[unlikely]] {
            Execution::Throw (bad_alloc{}, "ThrowIfNull (SharedPtr<T> ()) - throwing bad_alloc ()");
        }
    }
}

namespace std {
    template <typename TO_TYPE_T, typename FROM_TYPE_T>
    inline Stroika::Foundation::Memory::SharedPtr<TO_TYPE_T> dynamic_pointer_cast (const Stroika::Foundation::Memory::SharedPtr<FROM_TYPE_T>& sp) noexcept
    {
        return sp.template Dynamic_Cast<TO_TYPE_T> ();
    }
    template <typename T>
    inline Stroika::Foundation::Memory::SharedPtr<T> atomic_load_explicit (const Stroika::Foundation::Memory::SharedPtr<T>* copyFrom, memory_order)
    {
        using namespace Stroika::Foundation;
        RequireNotNull (copyFrom);
        [[maybe_unused]] auto&&                   critSec = lock_guard{Memory::Private_::sSharedPtrCopyLock_};
        Stroika::Foundation::Memory::SharedPtr<T> result  = *copyFrom;
        return result;
    }
    template <typename T>
    inline Stroika::Foundation::Memory::SharedPtr<T> atomic_load (const Stroika::Foundation::Memory::SharedPtr<T>* p)
    {
        RequireNotNull (p);
        return atomic_load_explicit (p, memory_order_seq_cst);
    }
    template <typename T>
    inline void atomic_store_explicit (Stroika::Foundation::Memory::SharedPtr<T>* storeTo, Stroika::Foundation::Memory::SharedPtr<T> o, memory_order)
    {
        using namespace Stroika::Foundation;
        [[maybe_unused]] lock_guard critSec{Memory::Private_::sSharedPtrCopyLock_};
        storeTo->swap (o);
    }
    template <typename T>
    inline void atomic_store (Stroika::Foundation::Memory::SharedPtr<T>* storeTo, Stroika::Foundation::Memory::SharedPtr<T> o)
    {
        atomic_store_explicit (storeTo, o, memory_order_seq_cst);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
}
