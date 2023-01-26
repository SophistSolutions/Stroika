/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_StackBuffer_inl_
#define _Stroika_Foundation_Memory_StackBuffer_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <algorithm>
#include <cstring>
#include <type_traits>

#include "../Containers/Support/ReserveTweaks.h"
#include "../Debug/Assertions.h"
//#include "../Execution/Throw.h"   // avoid include cuz creates deadly embrace, and not critical to resolve
#include "Common.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     **************************** StackBuffer<T, BUF_SIZE> **************************
     ********************************************************************************
     */
    template <typename T, size_t BUF_SIZE>
    inline StackBuffer<T, BUF_SIZE>::StackBuffer () noexcept
        : fLiveData_{BufferAsT_ ()}
    {
#if qDebug
        Assert (UsingInlinePreallocatedBuffer_ ()); // cuz empty size so always fits
        (void)::memcpy (fGuard1_, kGuard1_, sizeof (kGuard1_));
        (void)::memcpy (fGuard2_, kGuard2_, sizeof (kGuard2_));
#endif
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline StackBuffer<T, BUF_SIZE>::StackBuffer (size_t nElements)
        : StackBuffer{}
    {
        resize (nElements);
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline StackBuffer<T, BUF_SIZE>::StackBuffer (UninitializedConstructorFlag, size_t nElements)
        requires (is_trivially_copyable_v<T>)
        : StackBuffer{}
    {
        resize_uninitialized (nElements);
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <typename ITERATOR_OF_T, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_T>, char>*>
    StackBuffer<T, BUF_SIZE>::StackBuffer (ITERATOR_OF_T start, ITERATOR_OF_T end)
        : StackBuffer{}
    {
        static_assert (is_convertible_v<Configuration::ExtractValueType_t<ITERATOR_OF_T>, T>);
        auto sz = static_cast<size_t> (distance (start, end));
        if (not this->HasEnoughCapacity_ (sz)) [[unlikely]] {
            reserve (sz); // reserve not resize() so we can do uninitialized_copy (avoid constructing empty objects to be assigned over)
        }
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
        Configuration::uninitialized_copy_MSFT_BWA (start, end, this->begin ());
#else
        uninitialized_copy (start, end, this->begin ());
#endif
        fSize_ = sz;
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline StackBuffer<T, BUF_SIZE>::~StackBuffer ()
    {
        Invariant ();
        DestroyElts_ (this->begin (), this->end ());
        if (not UsingInlinePreallocatedBuffer_ ()) [[unlikely]] {
            // we must have used the heap...
            Deallocate_ (LiveDataAsAllocatedBytes_ ());
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::GrowToSize (size_t nElements)
    {
        if (nElements > size ()) {
            resize (nElements);
        }
        Ensure (size () >= nElements);
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::GrowToSize_uninitialized (size_t nElements)
        requires (is_trivially_copyable_v<T>)
    {
        static_assert (is_trivially_copyable_v<T>);
        if (nElements > size ()) {
            resize_uninitialized (nElements);
        }
        Ensure (size () >= nElements);
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::resize (size_t nElements)
    {
        if (nElements > fSize_) {
            // Growing
            if (not this->HasEnoughCapacity_ (nElements)) [[unlikely]] {
                reserve (nElements);
            }
            Assert (this->begin () + fSize_ == this->end ()); // docs/clarity
            auto newEnd = this->begin () + nElements;
            Assert (this->end () < newEnd);
            // uninitialized_fill() guarantees filling all or none - if an exception doing some, it undoes the ones it did (so setting fsize after safe)
            uninitialized_fill (this->end (), newEnd, T{});
            fSize_ = nElements;
            Assert (this->end () == newEnd);
        }
        else if (nElements < fSize_) {
            // Shrinking
            DestroyElts_ (this->begin () + nElements, this->end ());
            fSize_ = nElements;
        }
        Assert (fSize_ == nElements);
        Ensure (size () <= capacity ());
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::resize_uninitialized (size_t nElements)
        requires (is_trivially_copyable_v<T> and is_trivially_destructible_v<T>)
    {
        if (not this->HasEnoughCapacity_ (nElements)) [[unlikely]] {
            reserve (nElements);
        }
        fSize_ = nElements;
        Assert (fSize_ == nElements);
        Ensure (size () <= capacity ());
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::iterator StackBuffer<T, BUF_SIZE>::begin () noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::iterator StackBuffer<T, BUF_SIZE>::end () noexcept
    {
        return fLiveData_ + fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::const_iterator StackBuffer<T, BUF_SIZE>::begin () const noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::const_iterator StackBuffer<T, BUF_SIZE>::end () const noexcept
    {
        return fLiveData_ + fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::pointer StackBuffer<T, BUF_SIZE>::data () noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::const_pointer StackBuffer<T, BUF_SIZE>::data () const noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    constexpr size_t StackBuffer<T, BUF_SIZE>::capacity () const noexcept
    {
        // @see class Design Note
        if (UsingInlinePreallocatedBuffer_ ()) [[likely]] {
            return BUF_SIZE;
        }
        else {
            return fCapacityOfFreeStoreAllocation_;
        }
    }
    template <typename T, size_t BUF_SIZE>
    void StackBuffer<T, BUF_SIZE>::reserve (size_t newCapacity, bool atLeast)
    {
        Require (atLeast or newCapacity >= size ());
        size_t useNewCapacity = newCapacity;
        size_t oldCapacity    = capacity ();
        if (atLeast) {
            if (useNewCapacity < oldCapacity) {
                return; // no work todo here...
            }
            // if fits in inline buffer, round up to that size. If exceeding that, use ScalledUpCapcity exponential growth algorithm
            if (useNewCapacity < BUF_SIZE) {
                useNewCapacity = BUF_SIZE;
            }
            else {
                useNewCapacity = Foundation::Containers::Support::ReserveTweaks::GetScaledUpCapacity (useNewCapacity, sizeof (T));
            }
        }
        Invariant ();
        // NOTE - could be upsizing or downsizing, and could be moving to for from allocated or inline buffers
        if (useNewCapacity != oldCapacity) {
            bool oldInPlaceBuffer = oldCapacity <= BUF_SIZE;
            bool newInPlaceBuffer = useNewCapacity <= BUF_SIZE;
            // Only if we changed if using inplace buffer, or if was and is using ramBuffer, and eltCount changed do we need to do anything
            if (oldInPlaceBuffer != newInPlaceBuffer or (not newInPlaceBuffer)) {
                bool  memoryAllocationNeeded = not newInPlaceBuffer;
                byte* newPtr = memoryAllocationNeeded ? Allocate_ (SizeInBytes_ (useNewCapacity)) : std::begin (fInlinePreallocatedBuffer_);

                // Initialize new memory from old
                Assert (this->begin () != reinterpret_cast<T*> (newPtr));
                uninitialized_copy (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));

                // destroy objects in old memory
                DestroyElts_ (this->begin (), this->end ());

                // free old memory if needed
                if (not oldInPlaceBuffer) {
                    Assert (not UsingInlinePreallocatedBuffer_ ());
                    Deallocate_ (LiveDataAsAllocatedBytes_ ());
                }

                fLiveData_ = reinterpret_cast<T*> (newPtr);
                if (not newInPlaceBuffer) {
                    fCapacityOfFreeStoreAllocation_ = useNewCapacity;
                }
            }
        }
        Ensure ((useNewCapacity <= BUF_SIZE && capacity () == BUF_SIZE) or (useNewCapacity > BUF_SIZE and useNewCapacity == capacity ()));
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline size_t StackBuffer<T, BUF_SIZE>::GetSize () const noexcept
    {
        Ensure (fSize_ <= capacity ());
        return fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline size_t StackBuffer<T, BUF_SIZE>::size () const noexcept
    {
        Ensure (fSize_ <= capacity ());
        return fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::reference StackBuffer<T, BUF_SIZE>::at (size_t i) noexcept
    {
        Require (i < fSize_);
        return *(fLiveData_ + i);
    }
    template <typename T, size_t BUF_SIZE>
    inline typename StackBuffer<T, BUF_SIZE>::const_reference StackBuffer<T, BUF_SIZE>::at (size_t i) const noexcept
    {
        Require (i < fSize_);
        return *(fLiveData_ + i);
    }
    template <typename T, size_t BUF_SIZE>
    inline auto StackBuffer<T, BUF_SIZE>::operator[] (size_t i) noexcept -> reference
    {
        return at (i);
    }
    template <typename T, size_t BUF_SIZE>
    inline auto StackBuffer<T, BUF_SIZE>::operator[] (size_t i) const noexcept -> const_reference
    {
        return at (i);
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::push_back (Configuration::ArgByValueType<T> e)
    {
        size_t s = size ();
        if (this->HasEnoughCapacity_ (s + 1)) [[likely]] {
            if constexpr (is_trivially_copyable_v<T>) {
                fLiveData_[s] = e;
            }
            else {
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
                Configuration::uninitialized_copy_MSFT_BWA (&e, &e + 1, this->end ());
#else
                uninitialized_copy (&e, &e + 1, this->end ());
#endif
            }
            ++this->fSize_;
        }
        else {
            if constexpr (is_trivially_copyable_v<T>) {
                resize_uninitialized (s + 1);
            }
            else {
                resize (s + 1);
            }
            fLiveData_[s] = e;
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::clear () noexcept
    {
        resize (0);
    }
    template <typename T, size_t BUF_SIZE>
    inline StackBuffer<T, BUF_SIZE>::operator T* () noexcept
    {
        AssertNotNull (fLiveData_);
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline StackBuffer<T, BUF_SIZE>::operator const T* () const noexcept
    {
        AssertNotNull (fLiveData_);
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename T, size_t BUF_SIZE>
    void StackBuffer<T, BUF_SIZE>::Invariant_ () const noexcept
    {
        Assert (capacity () >= size ());
        ValidateGuards_ ();
    }
    template <typename T, size_t BUF_SIZE>
    void StackBuffer<T, BUF_SIZE>::ValidateGuards_ () const noexcept
    {
        Assert (::memcmp (kGuard1_, fGuard1_, sizeof (kGuard1_)) == 0);
        Assert (::memcmp (kGuard2_, fGuard2_, sizeof (kGuard2_)) == 0);
    }
#endif
    template <typename T, size_t BUF_SIZE>
    inline T* StackBuffer<T, BUF_SIZE>::BufferAsT_ () noexcept
    {
        return reinterpret_cast<T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    constexpr const T* StackBuffer<T, BUF_SIZE>::BufferAsT_ () const noexcept
    {
        return reinterpret_cast<const T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::DestroyElts_ (T* start, T* end) noexcept
    {
        for (auto i = start; i != end; ++i) {
            destroy_at (i);
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline byte* StackBuffer<T, BUF_SIZE>::LiveDataAsAllocatedBytes_ () noexcept
    {
        Require (not UsingInlinePreallocatedBuffer_ ());
        return reinterpret_cast<byte*> (fLiveData_);
    }
    template <typename T, size_t BUF_SIZE>
    inline byte* StackBuffer<T, BUF_SIZE>::Allocate_ (size_t bytes)
    {
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
        void* p = ::malloc (bytes);
        //Execution::ThrowIfNull (p);       // avoid include which is creating deadly embrace
        if (p == nullptr) {
            //DbgTrace ("***THROW BAD ALLOC***");
            throw bad_alloc{};
        }
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
        return reinterpret_cast<byte*> (p);
    }
    template <typename T, size_t BUF_SIZE>
    inline void StackBuffer<T, BUF_SIZE>::Deallocate_ (byte* bytes) noexcept
    {
        if (bytes != nullptr) {
            ::free (bytes);
        }
    }
    template <typename T, size_t BUF_SIZE>
    constexpr bool StackBuffer<T, BUF_SIZE>::UsingInlinePreallocatedBuffer_ () const noexcept
    {
        return fLiveData_ == BufferAsT_ ();
    }

}

#endif /*_Stroika_Foundation_Memory_StackBuffer_inl_*/
