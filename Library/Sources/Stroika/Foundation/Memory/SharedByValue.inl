/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_inl_
#define _Stroika_Foundation_Memory_SharedByValue_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     *************** SharedByValue_CopyByDefault<T,SHARED_IMLP> *********************
     ********************************************************************************
     */
    template <typename T, typename SHARED_IMLP>
    inline SHARED_IMLP SharedByValue_CopyByDefault<T, SHARED_IMLP>::operator() (const T& t) const
    {
        return SHARED_IMLP{new T (t)};
    }

    /*
     ********************************************************************************
     ****** SharedByValue_CopySharedPtrExternallySynchronized<T,SHARED_IMLP> ********
     ********************************************************************************
     */
    template <typename T, typename SHARED_IMLP>
    inline SHARED_IMLP SharedByValue_CopySharedPtrExternallySynchronized<T, SHARED_IMLP>::Load (const SHARED_IMLP& copyFrom)
    {
        return copyFrom;
    }
    template <typename T, typename SHARED_IMLP>
    inline void SharedByValue_CopySharedPtrExternallySynchronized<T, SHARED_IMLP>::Store (SHARED_IMLP* storeTo, const SHARED_IMLP& o)
    {
        RequireNotNull (storeTo);
        *storeTo = o;
    }

    /*
     ********************************************************************************
     *************************** SharedByValue<T, TRAITS> ***************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue () noexcept
        : fCopier_{element_copier_type{}}
        , fSharedImpl_{}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue ([[maybe_unused]] nullptr_t n) noexcept
        : fCopier_{element_copier_type{}}
        , fSharedImpl_{}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (const SharedByValue& from) noexcept
        : fCopier_{from.fCopier_}
        , fSharedImpl_{shared_impl_copier_type::Load (from.fSharedImpl_)}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (SharedByValue&& from) noexcept
        : fCopier_{from.fCopier_}
        , fSharedImpl_{move (from.fSharedImpl_)}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (const shared_ptr_type& from, const element_copier_type& copier) noexcept
        : fCopier_{copier}
        , fSharedImpl_{shared_impl_copier_type::Load (from)}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (const element_type& from, const element_copier_type& copier) noexcept
        : fCopier_{copier}
        , fSharedImpl_{copier (from)}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (shared_ptr_type&& from, const element_copier_type&& copier) noexcept
        : fCopier_{copier}
        , fSharedImpl_{move (from)}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>::SharedByValue (element_type* from, const element_copier_type& copier)
        : fCopier_{copier}
        , fSharedImpl_{from}
    {
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>& SharedByValue<T, TRAITS>::operator= (const SharedByValue& rhs)
    {
        // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
        // and we can avoid the (common) and costly memory barrier
        if (fSharedImpl_.get () != rhs.fSharedImpl_.get ()) [[LIKELY_ATTR]] {
            fCopier_ = rhs.fCopier_;
            shared_impl_copier_type::Store (&fSharedImpl_, shared_impl_copier_type::Load (rhs.fSharedImpl_));
        }
        return *this;
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>& SharedByValue<T, TRAITS>::operator= (SharedByValue&& rhs) noexcept
    {
        // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
        // and we can avoid the (common) and costly memory barrier
        if (fSharedImpl_.get () != rhs.fSharedImpl_.get ()) [[LIKELY_ATTR]] {
            fCopier_ = rhs.fCopier_;
            // ASSUME if doing a move() then this doesn't need to be a multithread safe copy (from the source), since
            // if its a temporary, you cannot have mulitple peopel referring to me
            shared_impl_copier_type::Store (&fSharedImpl_, rhs.fSharedImpl_);
        }
        return *this;
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>& SharedByValue<T, TRAITS>::operator= (const shared_ptr_type& from)
    {
        // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
        // and we can avoid the (common) and costly memory barrier
        if (fSharedImpl_.get () != from.get ()) [[LIKELY_ATTR]] {
            shared_impl_copier_type::Store (&fSharedImpl_, shared_impl_copier_type::Load (from));
        }
        return *this;
    }
    template <typename T, typename TRAITS>
    inline SharedByValue<T, TRAITS>& SharedByValue<T, TRAITS>::operator= (shared_ptr_type&& from)
    {
        // If the pointers are the same, there is no need to copy, as the reference counts must also be the same,
        // and we can avoid the (common) and costly memory barrier
        if (fSharedImpl_.get () != from.get ()) [[LIKELY_ATTR]] {
            // ASSUME if doing a move() then this doesn't need to be a multithread safe copy (from the source), since
            // if its a temporary, you cannot have mulitple peopel referring to me
            shared_impl_copier_type::Store (&fSharedImpl_, from);
        }
        return *this;
    }
    template <typename T, typename TRAITS>
    inline const typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::get () const noexcept
    {
        return fSharedImpl_.get ();
    }
    template <typename T, typename TRAITS>
    template <typename... COPY_ARGS>
    inline typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::get (COPY_ARGS&&... copyArgs)
    {
        element_type* ptr = fSharedImpl_.get ();
        /*
         * For non-const pointing, we must clone ourselves (if there are
         * extra referneces). If we are a nullptr pointer, nobody could actually
         * rereference it anyhow, so don't bother with the Assure1Reference()
         * in that case.
         */
        if (ptr != nullptr) {
            Assure1Reference (forward<COPY_ARGS> (copyArgs)...);
            ptr = fSharedImpl_.get ();
            EnsureNotNull (ptr);
        }
        return ptr;
    }
    template <typename T, typename TRAITS>
    inline const typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::cget () const noexcept
    {
        return get ();
    }
    template <typename T, typename TRAITS>
    template <typename... COPY_ARGS>
    auto SharedByValue<T, TRAITS>::GetAndMaybeCopySavingOriginal (shared_ptr_type* oldValue, COPY_ARGS&&... copyArgs) -> shared_ptr_type
    {
        RequireNotNull (oldValue);
        /*
         *  Increment refCount before assureNReferences/breakreferencs so we can save
         *  the original shared_ptr and return it in case its needed (e.g. to update iterators).
         * 
         *  Save this way so no race (after Assure1Reference() other remaining ptr could go away.
         */
        shared_ptr_type origPtr = fSharedImpl_;
        *oldValue               = origPtr;
        if (origPtr != nullptr) [[LIKELY_ATTR]] {
            Assure2AtLeastReferences (forward<COPY_ARGS> (copyArgs)...);
            shared_ptr_type result = fSharedImpl_;
            Ensure (result.count () == 1 or (result.count () == 3 and result == origPtr));
            return result;
        }
        return nullptr;
    }
    template <typename T, typename TRAITS>
    inline const typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::operator-> () const
    {
        return fSharedImpl_.get ();
    }
    template <typename T, typename TRAITS>
    inline typename SharedByValue<T, TRAITS>::element_type* SharedByValue<T, TRAITS>::operator-> ()
    {
        return get ();
    }
    template <typename T, typename TRAITS>
    inline const typename SharedByValue<T, TRAITS>::element_type& SharedByValue<T, TRAITS>::operator* () const
    {
        const element_type* ptr = get ();
        EnsureNotNull (ptr);
        return *ptr;
    }
    template <typename T, typename TRAITS>
    inline typename SharedByValue<T, TRAITS>::element_type& SharedByValue<T, TRAITS>::operator* ()
    {
        /*
         * For non-const dereferencing, we must clone ourselves (if there are
         * extra referneces).
         */
        Assure1Reference ();
        element_type* ptr = get ();
        EnsureNotNull (ptr);
        return *ptr;
    }
    template <typename T, typename TRAITS>
    constexpr bool SharedByValue<T, TRAITS>::operator== (nullptr_t) const
    {
        return fSharedImpl_ == nullptr;
    }
#if __cpp_impl_three_way_comparison < 201907
    template <typename T, typename TRAITS>
    constexpr bool SharedByValue<T, TRAITS>::operator!= (nullptr_t) const
    {
        return fSharedImpl_ != nullptr;
    }
#endif
    template <typename T, typename TRAITS>
    inline typename SharedByValue<T, TRAITS>::element_copier_type SharedByValue<T, TRAITS>::GetCopier () const
    {
        return fCopier_;
    }
    template <typename T, typename TRAITS>
    inline SharedByValue_State SharedByValue<T, TRAITS>::GetSharingState () const
    {
        switch (fSharedImpl_.use_count ()) {
            case 0:
                Assert (fSharedImpl_.get () == nullptr);
                return SharedByValue_State::eNull;
            case 1:
                Assert (fSharedImpl_.get () != nullptr);
                return SharedByValue_State::eSolo;
            default:
                Assert (fSharedImpl_.get () != nullptr);
                //NOT NECESSARILY - cuz there is no lock
                // between the use_count and this unique call, so another object could decrement its count
                // and this could be that it was shared, but is no solo.
                // Assert (not fSharedImpl_.unique ());
                // -- LGP 2015-01-10
                return SharedByValue_State::eShared;
        }
    }
    template <typename T, typename TRAITS>
    inline bool SharedByValue<T, TRAITS>::unique () const
    {
        return fSharedImpl_.use_count () == 1;
    }
    template <typename T, typename TRAITS>
    inline unsigned int SharedByValue<T, TRAITS>::use_count () const
    {
        return fSharedImpl_.use_count ();
    }
    template <typename T, typename TRAITS>
    template <typename... COPY_ARGS>
    inline void SharedByValue<T, TRAITS>::Assure1Reference (COPY_ARGS&&... copyArgs)
    {
        if (fSharedImpl_.use_count () > 1) {
            BreakReferences_ (forward<COPY_ARGS> (copyArgs)...);
        }
    }
    template <typename T, typename TRAITS>
    template <typename... COPY_ARGS>
    inline void SharedByValue<T, TRAITS>::Assure2OrFewerReferences (COPY_ARGS&&... copyArgs)
    {
        if (fSharedImpl_.use_count () > 2) {
            BreakReferences_ (forward<COPY_ARGS> (copyArgs)...);
        }
    }
    template <typename T, typename TRAITS>
    template <typename... COPY_ARGS>
    void SharedByValue<T, TRAITS>::BreakReferences_ (COPY_ARGS&&... copyArgs)
    {
        shared_ptr_type ptr2Clone{shared_impl_copier_type::Load (fSharedImpl_)}; // other thread could change this (if other thread accesses same envelope)
        // but this copy prevents the bare ptr from possibly becoming invalidated
        element_type* ptr = ptr2Clone.get ();
        RequireNotNull (ptr);
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
        //Require (!SHARED_IMLP::unique ());    This is not NECESSARILY so. Another thread could have just released this pointer, in which case
        // the creation of a new object was pointless, but harmless, as the assignemnt should decrement to zero the old
        // value and it should go away.
        *this = SharedByValue<T, TRAITS>{fCopier_ (*ptr, forward<COPY_ARGS> (copyArgs)...), fCopier_};

#if qDebug
        // technically not 100% guaranteed if two threads did this at the same time, but so rare interesting if ever triggered.
        // probably a bug, but not necessarily
        WeakAssert (unique ());
#endif
    }

}

#endif /*_Stroika_Foundation_Memory_SharedByValue_inl_*/
