/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Memory {

    template <typename T, typename SHARED_IMPL, typename COPIER>
    [[deprecated ("Since Stroika v3.0d23 - not used - doesnt do anything")]] constexpr bool SharedByValue_IsCopier ()
    {
        // @todo should match API function<SHARED_IMPL(const T&)>
        return true;
    }

    template <typename T, typename SHARED_IMPL = shared_ptr<T>>
    using SharedByValue_CopyByDefault [[deprecated ("Since Stroika v3.0d23 - use SharedByValueSupport directly")]] =
        SharedByValueSupport::DefaultValueCopier<T, SHARED_IMPL>;

    template <typename T, typename SHARED_IMPL = shared_ptr<T>, typename COPIER = DefaultValueCopier<T, SHARED_IMPL>>
    using SharedByValue_Traits [[deprecated ("Since Stroika v3.0d23 - use DefaultTraits directly")]] =
        SharedByValueSupport::DefaultTraits<T, SHARED_IMPL>;

    using SharedByValue_State [[deprecated ("Since Stroika v3.0d23 - use SharedByValueSupport::SharingState directly")]] = SharedByValueSupport::SharingState;


    /*
     ********************************************************************************
     *********** SharedByValueSupport::DefaultValueCopier<T,SHARED_IMPL> ************
     ********************************************************************************
     */
    template <typename T, typename SHARED_IMPL>
    inline SHARED_IMPL SharedByValueSupport::DefaultValueCopier<T, SHARED_IMPL>::operator() (const T& t)
    {
        if constexpr (same_as<SHARED_IMPL, shared_ptr<T>>) {
            return Memory::MakeSharedPtr<T> (t); // more efficient
        }
        else {
            return SHARED_IMPL{new T{t}};
        }
    }

    /*
     ********************************************************************************
     *************************** SharedByValue<T, TRAITS> ***************************
     ********************************************************************************
     */
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue ([[maybe_unused]] nullptr_t) noexcept
        : fCopier_{element_copier_type{}}
        , fSharedImpl_{}
    {
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (const shared_ptr_type& from, const element_copier_type& copier) noexcept
        : fCopier_{copier}
        , fSharedImpl_{from}
    {
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (const element_type& from, const element_copier_type& copier) noexcept
        : fCopier_{copier}
        , fSharedImpl_{copier (from)}
    {
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (shared_ptr_type&& from, const element_copier_type&& copier) noexcept
        : fCopier_{move (copier)}
        , fSharedImpl_{move (from)}
    {
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (element_type* from, const element_copier_type& copier)
        : fCopier_{copier}
        , fSharedImpl_{from}
    {
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>& SharedByValue<T, TRAITS>::operator= (const shared_ptr_type& from) noexcept
    {
        // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
        // and we can avoid the (common) and costly memory barrier
        if (fSharedImpl_ != from) [[likely]] {
            fSharedImpl_ = from;
        }
        return *this;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>::operator bool () const noexcept
    {
        return fSharedImpl_.get () != nullptr;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline SharedByValue<T, TRAITS>& SharedByValue<T, TRAITS>::operator= (shared_ptr_type&& from) noexcept
    {
        // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
        // and we can avoid the (common) and costly memory barrier
        if (fSharedImpl_ != from) [[likely]] {
            fSharedImpl_ = move (from);
        }
        return *this;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline const typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::cget () const noexcept
    {
        return fSharedImpl_.get ();
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline auto SharedByValue<T, TRAITS>::cget_ptr () const -> shared_ptr_type
    {
        return fSharedImpl_;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline auto SharedByValue<T, TRAITS>::rwget_ptr () -> shared_ptr_type
    {
        return rwget_ptr (fCopier_);
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    template <typename COPIER>
    auto SharedByValue<T, TRAITS>::rwget_ptr (COPIER&& copier) -> shared_ptr_type
    {
        if (fSharedImpl_ != nullptr) [[likely]] {
            auto result = forward<COPIER> (copier) (fSharedImpl_);
            Assert (result.use_count () == 1);
            return result;
        }
        return nullptr;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline auto SharedByValue<T, TRAITS>::rwget () -> element_type*
    {
        return rwget (fCopier_);
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    template <typename COPIER>
    inline auto SharedByValue<T, TRAITS>::rwget (COPIER&& copier) -> element_type*
    {
        /*
         *  Increment refCount before assureNReferences/breakreferencs so we can save
         *  the original shared_ptr and return it in case its needed (e.g. to update iterators).
         * 
         *  Save this way so no race (after Assure1Reference() other remaining ptr could go away.
         */
        if (fSharedImpl_ != nullptr) [[likely]] {
            AssureNOrFewerReferences (forward<COPIER> (copier));
            Ensure (fSharedImpl_.use_count () == 1);
            return fSharedImpl_.get ();
        }
        return nullptr;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline const typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::operator->() const
    {
        return fSharedImpl_.get ();
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::operator->()
    {
        return rwget ();
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline const typename SharedByValue<T, TRAITS>::element_type& SharedByValue<T, TRAITS>::operator* () const
    {
        const element_type* ptr = cget ();
        EnsureNotNull (ptr);
        return *ptr;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    constexpr bool SharedByValue<T, TRAITS>::operator== (nullptr_t) const
    {
        return fSharedImpl_ == nullptr;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline typename SharedByValue<T, TRAITS>::element_copier_type SharedByValue<T, TRAITS>::GetDefaultCopier () const
    {
        return fCopier_;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline auto SharedByValue<T, TRAITS>::GetSharingState () const -> SharingState
    {
        switch (fSharedImpl_.use_count ()) {
            case 0:
                Assert (fSharedImpl_.get () == nullptr);
                return SharingState::eNull;
            case 1:
                Assert (fSharedImpl_.get () != nullptr);
                return SharingState::eSolo;
            default:
                Assert (fSharedImpl_.get () != nullptr);
                //NOT NECESSARILY - cuz there is no lock
                // between the use_count and this unique call, so another object could decrement its count
                // and this could be that it was shared, but is no solo.
                // Assert (not fSharedImpl_.unique ());
                // -- LGP 2015-01-10
                return SharingState::eShared;
        }
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline bool SharedByValue<T, TRAITS>::unique () const
    {
        return fSharedImpl_.use_count () == 1;
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline unsigned int SharedByValue<T, TRAITS>::use_count () const
    {
        return fSharedImpl_.use_count ();
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    template <typename COPIER>
    inline void SharedByValue<T, TRAITS>::AssureNOrFewerReferences (COPIER&& copier, unsigned int n)
    {
        Require (n != 0);
        if (static_cast<unsigned int> (fSharedImpl_.use_count ()) > n) [[unlikely]] {
            BreakReferences_ (forward<COPIER> (copier));
            Assert (this->use_count () == 1);
        }
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    inline void SharedByValue<T, TRAITS>::AssureNOrFewerReferences (unsigned int n)
    {
        AssureNOrFewerReferences (fCopier_, n);
    }
    template <typename T, SharedByValueSupport::ITraits<T> TRAITS>
    template <typename COPIER>
    void SharedByValue<T, TRAITS>::BreakReferences_ (COPIER&& copier)
    {
        RequireNotNull (fSharedImpl_);
        /*
         *      For a valid pointer that is reference counted and multiply shared,
         *  make a copy of that pointer via our fCloner function, and assign
         *  that cloned reference to this.
         *
         *      Note that by doing so, we remove any references to the current
         *  item, and end up with our having the sole reference to the new copy of fPtr.
         *
         *      Since we will be cloning the given pointer, we assume(assert) that
         *  it is non-nullptr.
         */

        // This is not NECESSARILY so. Another thread could have just released this pointer, in which case
        // the creation of a new object was pointless, but harmless, as the assignemnt should decrement to zero the old
        // value and it should go away.
        //
        // Actually, this gets triggered alot when used in heavily multithreaded applications, like
        // Test10_MutlipleThreadsReadingOneUpdateUsingSynchronizedContainer_, so comment out the weak assert - just too noisy
        // and really not a problem -- LGP 2021-11-06
        //WeakAssert (not unique ());

        fSharedImpl_ = forward<COPIER> (copier) (*fSharedImpl_); // make a new shared_ptr (clone) and assign-overwriting.

        // this assignment requires overwriting THIS object, so must be externally synchronized. ASSERT EXTERNALLY SYNRCHONIZED HERE
        // so treat this as real assertion erorr -- LGP 2021-10-15
        Ensure (unique ());
    }

}
