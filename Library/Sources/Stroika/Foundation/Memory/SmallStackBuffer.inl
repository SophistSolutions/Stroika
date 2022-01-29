/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SmallStackBuffer_inl_
#define _Stroika_Foundation_Memory_SmallStackBuffer_inl_ 1

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
#include "../Execution/Throw.h"
#include "Common.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ************************* SmallStackBuffer<T, BUF_SIZE> ************************
     ********************************************************************************
     */
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer ()
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
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (size_t nElements)
        : SmallStackBuffer{}
    {
        resize (nElements);
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (UninitializedConstructorFlag, size_t nElements)
        : SmallStackBuffer{}
    {
        static_assert (is_trivially_default_constructible_v<T>);
        resize_uninitialized (nElements);
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <typename ITERATOR_OF_T, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_T>, char>*>
    SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (ITERATOR_OF_T start, ITERATOR_OF_T end)
        : SmallStackBuffer{}
    {
        static_assert (is_convertible_v<Configuration::ExtractValueType_t<ITERATOR_OF_T>, T>);
        auto sz = static_cast<size_t> (distance (start, end));
        ReserveAtLeast (sz); // reserve not resize() so we can do uninitialized_copy (avoid constructing empty objects to be assigned over)
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
        Configuration::uninitialized_copy_MSFT_BWA (start, end, this->begin ());
#else
        uninitialized_copy (start, end, this->begin ());
#endif
        fSize_ = sz;
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <size_t FROM_BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer<T, FROM_BUF_SIZE>& src)
        : SmallStackBuffer{src.begin (), src.end ()}
    {
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer& src)
        : SmallStackBuffer{src.begin (), src.end ()}
    {
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (SmallStackBuffer&& src)
        : SmallStackBuffer{}
    {
#if qDebug
        size_t origSize     = src.size ();
        size_t origCapacity = src.capacity ();
#endif
        if (src.UsingInlinePreallocatedBuffer_ ()) {
            // then little to be saved from a move, and technically we really cannot do much, except that we can 'move' the data elements
            // This 'moving' is done via make_move_iterator () - rather that magically makes the uninitialized_copy really move instead of copying
            reserve (src.capacity ());
            size_t sz = src.size ();
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
            Configuration::uninitialized_copy_MSFT_BWA (std::make_move_iterator (src.begin ()), std::make_move_iterator (src.begin () + sz), this->begin ());
#else
            uninitialized_copy (std::make_move_iterator (src.begin ()), std::make_move_iterator (src.begin () + sz), this->begin ());
#endif
            fSize_ = sz;
        }
        else {
            // OK - we can do some trickery here to steal the underlying pointers
            this->fLiveData_ = src.LiveDataAsAllocatedBytes_ ();
            src.fLiveData_   = src.BufferAsT_ ();
            this->fSize_     = src.fSize_;
            src.fSize_       = 0;
            Ensure (src.fSize_ == 0);
            Ensure (src.capacity () == BUF_SIZE);
        }
        Ensure (this->size () == origSize);
        Ensure (this->capacity () == origCapacity);
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::~SmallStackBuffer ()
    {
        Invariant ();
        DestroyElts_ (this->begin (), this->end ());
        if (not UsingInlinePreallocatedBuffer_ ()) [[UNLIKELY_ATTR]] {
            // we must have used the heap...
            Deallocate_ (LiveDataAsAllocatedBytes_ ());
        }
    }
    template <typename T, size_t BUF_SIZE>
    template <size_t FROM_BUF_SIZE>
    SmallStackBuffer<T, BUF_SIZE>& SmallStackBuffer<T, BUF_SIZE>::operator= (const SmallStackBuffer<T, FROM_BUF_SIZE>& rhs)
    {
        Invariant ();
        Assert (this != &rhs); // because if same address should be same type so operator=(const SmallStackBuffer&) should be called
        // @todo this simple implementation could be more efficient
        DestroyElts_ (this->begin (), this->end ());
        fSize_ = 0;
        ReserveAtLeast (rhs.size ());
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
        Configuration::uninitialized_copy_MSFT_BWA (rhs.begin (), rhs.end (), this->begin ());
#else
        uninitialized_copy (rhs.begin (), rhs.end (), this->begin ());
#endif
        Invariant ();
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    SmallStackBuffer<T, BUF_SIZE>& SmallStackBuffer<T, BUF_SIZE>::operator= (const SmallStackBuffer& rhs)
    {
        Invariant ();
        if (this != &rhs) {
            // @todo this simple implementation could be more efficient
            DestroyElts_ (this->begin (), this->end ());
            fSize_ = 0;
            ReserveAtLeast (rhs.size ());
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
            Configuration::uninitialized_copy_MSFT_BWA (rhs.begin (), rhs.end (), this->begin ());
#else
            uninitialized_copy (rhs.begin (), rhs.end (), this->begin ());
#endif
            fSize_ = rhs.size ();
            Invariant ();
        }
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>& SmallStackBuffer<T, BUF_SIZE>::operator= (SmallStackBuffer&& rhs)
    {
        Invariant ();
        WeakAssertNotImplemented (); // @todo this simple implementation could be more efficient (sb pretty easy based on existing move CTOR code...
        operator= (rhs);             // call copy assign for now
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::GrowToSize (size_t nElements)
    {
        if (nElements > size ()) {
            resize (nElements);
        }
        Ensure (size () >= nElements);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::GrowToSize_uninitialized (size_t nElements)
    {
        static_assert (is_trivially_default_constructible_v<T>);
        if (nElements > size ()) {
            resize_uninitialized (nElements);
        }
        Ensure (size () >= nElements);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::resize (size_t nElements)
    {
        if (nElements > fSize_) {
            // Growing
            if (nElements > capacity ()) {
                /*
                 *   tweak grow rate to avoid needless copy/realloc
                 */
                reserve (Foundation::Containers::Support::ReserveTweaks::GetScaledUpCapacity (nElements, sizeof (T)));
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
    inline void SmallStackBuffer<T, BUF_SIZE>::resize_uninitialized (size_t nElements)
    {
        static_assert (is_trivially_default_constructible_v<T>);
        if (nElements > fSize_) {
            // Growing
            if (nElements > capacity ()) [[UNLIKELY_ATTR]] {
                /*
                 *   If we REALLY must grow, the double in size so unlikely we'll have to grow/malloc/copy again.
                 */
                reserve (max (nElements, capacity () * 2));
            }
            fSize_ = nElements;
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
    void SmallStackBuffer<T, BUF_SIZE>::reserve_ (size_t nElements)
    {
        Assert (nElements >= size ());
        Invariant ();
        size_t oldEltCount = capacity ();
        if (nElements != oldEltCount) {
            bool oldInPlaceBuffer = oldEltCount <= BUF_SIZE;
            bool newInPlaceBuffer = nElements <= BUF_SIZE;
            // Only if we changed if using inplace buffer, or if was and is using ramBuffer, and eltCount changed do we need to do anything
            if (oldInPlaceBuffer != newInPlaceBuffer or (not newInPlaceBuffer)) {
                bool  memoryAllocationNeeded = not newInPlaceBuffer;
                byte* newPtr                 = memoryAllocationNeeded ? Allocate_ (SizeInBytes_ (nElements)) : std::begin (fInlinePreallocatedBuffer_);

                // Initialize new memory from old
                Assert (this->begin () != reinterpret_cast<T*> (newPtr));
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
                Configuration::uninitialized_copy_MSFT_BWA (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));
#else
                uninitialized_copy (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));
#endif

                // destroy objects in old memory
                DestroyElts_ (this->begin (), this->end ());

                // free old memory if needed
                if (not oldInPlaceBuffer) {
                    Assert (not UsingInlinePreallocatedBuffer_ ());
                    Deallocate_ (LiveDataAsAllocatedBytes_ ());
                }

                fLiveData_ = reinterpret_cast<T*> (newPtr);
                if (not newInPlaceBuffer) {
                    fCapacityOfFreeStoreAllocation_ = nElements;
                }
            }
        }
        Ensure ((nElements <= BUF_SIZE && capacity () == BUF_SIZE) or (nElements > BUF_SIZE and nElements == capacity ()));
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline typename SmallStackBuffer<T, BUF_SIZE>::iterator SmallStackBuffer<T, BUF_SIZE>::begin ()
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename SmallStackBuffer<T, BUF_SIZE>::iterator SmallStackBuffer<T, BUF_SIZE>::end ()
    {
        return fLiveData_ + fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename SmallStackBuffer<T, BUF_SIZE>::const_iterator SmallStackBuffer<T, BUF_SIZE>::begin () const
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename SmallStackBuffer<T, BUF_SIZE>::const_iterator SmallStackBuffer<T, BUF_SIZE>::end () const
    {
        return fLiveData_ + fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline size_t SmallStackBuffer<T, BUF_SIZE>::capacity () const
    {
        return UsingInlinePreallocatedBuffer_ () ? BUF_SIZE : fCapacityOfFreeStoreAllocation_; // @see class Design Note
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::reserve (size_t newCapacity)
    {
        Require (newCapacity >= size ());
        reserve_ (newCapacity);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::ReserveAtLeast (size_t newCapacity)
    {
        if (newCapacity > capacity ()) {
            reserve_ (newCapacity);
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline size_t SmallStackBuffer<T, BUF_SIZE>::GetSize () const
    {
        Ensure (fSize_ <= capacity ());
        return fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline size_t SmallStackBuffer<T, BUF_SIZE>::size () const
    {
        Ensure (fSize_ <= capacity ());
        return fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename SmallStackBuffer<T, BUF_SIZE>::reference SmallStackBuffer<T, BUF_SIZE>::at (size_t i)
    {
        Require (i < fSize_);
        return *(fLiveData_ + i);
    }
    template <typename T, size_t BUF_SIZE>
    inline typename SmallStackBuffer<T, BUF_SIZE>::const_reference SmallStackBuffer<T, BUF_SIZE>::at (size_t i) const
    {
        Require (i < fSize_);
        return *(fLiveData_ + i);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::push_back (Configuration::ArgByValueType<T> e)
    {
        size_t s = size ();
        if (s < capacity ()) {
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
            Configuration::uninitialized_copy_MSFT_BWA (&e, &e + 1, this->end ());
#else
            uninitialized_copy (&e, &e + 1, this->end ());
#endif
            ++this->fSize_;
        }
        else {
            if constexpr (is_trivially_default_constructible_v<T>) {
                resize_uninitialized (s + 1);
            }
            else {
                resize (s + 1);
            }
            fLiveData_[s] = e;
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::clear ()
    {
        resize (0);
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::operator T* ()
    {
        AssertNotNull (fLiveData_);
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::operator const T* () const
    {
        AssertNotNull (fLiveData_);
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename T, size_t BUF_SIZE>
    void SmallStackBuffer<T, BUF_SIZE>::Invariant_ () const noexcept
    {
        Assert (capacity () >= size ());
        ValidateGuards_ ();
    }
    template <typename T, size_t BUF_SIZE>
    void SmallStackBuffer<T, BUF_SIZE>::ValidateGuards_ () const noexcept
    {
        Assert (::memcmp (kGuard1_, fGuard1_, sizeof (kGuard1_)) == 0);
        Assert (::memcmp (kGuard2_, fGuard2_, sizeof (kGuard2_)) == 0);
    }
#endif
    template <typename T, size_t BUF_SIZE>
    inline T* SmallStackBuffer<T, BUF_SIZE>::BufferAsT_ () noexcept
    {
        return reinterpret_cast<T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    inline const T* SmallStackBuffer<T, BUF_SIZE>::BufferAsT_ () const noexcept
    {
        return reinterpret_cast<const T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::DestroyElts_ (T* start, T* end) noexcept
    {
        for (auto i = start; i != end; ++i) {
            destroy_at (i);
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline byte* SmallStackBuffer<T, BUF_SIZE>::LiveDataAsAllocatedBytes_ ()
    {
        Require (not UsingInlinePreallocatedBuffer_ ());
        return reinterpret_cast<byte*> (fLiveData_);
    }
    template <typename T, size_t BUF_SIZE>
    inline byte* SmallStackBuffer<T, BUF_SIZE>::Allocate_ (size_t bytes)
    {
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
        void* p = ::malloc (bytes);
        Execution::ThrowIfNull (p);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
        return reinterpret_cast<byte*> (p);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::Deallocate_ (byte* bytes)
    {
        if (bytes != nullptr) {
            ::free (bytes);
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline bool SmallStackBuffer<T, BUF_SIZE>::UsingInlinePreallocatedBuffer_ () const
    {
        return (fLiveData_ == BufferAsT_ ());
    }

}

#endif /*_Stroika_Foundation_Memory_SmallStackBuffer_inl_*/
