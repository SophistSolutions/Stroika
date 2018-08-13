/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

#include "../Debug/Assertions.h"
#include "../Execution/Exceptions.h"
#include "Common.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ************************* SmallStackBuffer<T, BUF_SIZE> ************************
     ********************************************************************************
     */
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer ()
        : fLiveData_ (BufferAsT_ ())
    {
#if qDebug
        ::memcpy (fGuard1_, kGuard1_, sizeof (kGuard1_));
        ::memcpy (fGuard2_, kGuard2_, sizeof (kGuard2_));
#endif
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (size_t nElements)
        : SmallStackBuffer ()
    {
        resize (nElements);
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <typename ITERATOR_OF_T>
    SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (ITERATOR_OF_T start, ITERATOR_OF_T end)
        : SmallStackBuffer (distance (start, end))
    {
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
        Configuration::uninitialized_copy_MSFT_BWA (start, end, this->begin ());
#else
        uninitialized_copy (start, end, this->begin ());
#endif
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <size_t FROM_BUF_SIZE>
    SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer<T, FROM_BUF_SIZE>& from)
        : SmallStackBuffer (from.begin (), from.end ())
    {
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer& from)
        : SmallStackBuffer (from.begin (), from.end ())
    {
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::~SmallStackBuffer ()
    {
        Invariant ();
        DestroyElts_ (this->begin (), this->end ());
        if (fLiveData_ != BufferAsT_ ()) {
            // we must have used the heap...
            Deallocate_ (LiveDataAsAllocatedBytes_ ());
        }
    }
    template <typename T, size_t BUF_SIZE>
    template <size_t FROM_BUF_SIZE>
    SmallStackBuffer<T, BUF_SIZE>& SmallStackBuffer<T, BUF_SIZE>::operator= (const SmallStackBuffer<T, FROM_BUF_SIZE>& rhs)
    {
        Invariant ();
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
            Invariant ();
        }
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::GrowToSize (size_t nElements)
    {
        if (nElements > size ()) {
            resize (nElements);
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::resize (size_t nElements)
    {
        if (nElements > fSize_) {
            if (nElements > capacity ()) {
                /*
                 *   If we REALLY must grow, the double in size so unlikely we'll have to grow/malloc/copy again.
                 */
                reserve (max (nElements, capacity () * 2));
            }
            uninitialized_fill (this->begin () + fSize_, this->begin () + nElements, T{});
            fSize_ = nElements;
        }
        else if (nElements < fSize_) {
            DestroyElts_ (this->begin () + nElements, this->end ());
            fSize_ = nElements;
        }
        Assert (fSize_ == nElements);
        Ensure (size () <= capacity ());
    }
    template <typename T, size_t BUF_SIZE>
    void SmallStackBuffer<T, BUF_SIZE>::reserve_ (size_t nElements)
    {
        Invariant ();
        //
        // note - we currently only GROW the capacity. That is probably a mistake, but in practice not a real problem.
        //      -- LGP 2017-04-13
        //
        size_t oldEltCount = capacity ();
        if (nElements > oldEltCount) {
            Assert (nElements > BUF_SIZE); // because capacity is always at least BUF_SIZE
            Memory::Byte* newPtr = Allocate_ (SizeInBytes_ (nElements));
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
            Configuration::uninitialized_copy_MSFT_BWA (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));
#else
            uninitialized_copy (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));
#endif
            DestroyElts_ (this->begin (), this->end ()); // no change to fSize_!!!!
            if (fLiveData_ != BufferAsT_ ()) {
                // we must have used the heap...
                Deallocate_ (LiveDataAsAllocatedBytes_ ());
            }
            fLiveData_ = reinterpret_cast<T*> (newPtr);

            fCapacityOfFreeStoreAllocation_ = nElements;
        }
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
        return (fLiveData_ == BufferAsT_ ()) ? BUF_SIZE : fCapacityOfFreeStoreAllocation_; // @see class Design Note
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::reserve (size_t newCapacity)
    {
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
    inline void SmallStackBuffer<T, BUF_SIZE>::push_back (const T& e)
    {
        size_t s = size ();
        if (s < capacity ()) {
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
            Configuration::uninitialized_copy_MSFT_BWA (&e, &e + 1, this->end ());
#else
            uninitialized_copy (&e, &e + 1, this->end ());
#endif
            this->fSize_++;
        }
        else {
            resize (s + 1);
            fLiveData_[s] = e;
        }
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
    inline void SmallStackBuffer<T, BUF_SIZE>::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename T, size_t BUF_SIZE>
    void SmallStackBuffer<T, BUF_SIZE>::Invariant_ () const
    {
        Assert (capacity () >= size ());
        ValidateGuards_ ();
    }
#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
    template <typename T, size_t BUF_SIZE>
    constexpr Byte SmallStackBuffer<T, BUF_SIZE>::kGuard1_[8];
    template <typename T, size_t BUF_SIZE>
    constexpr Byte SmallStackBuffer<T, BUF_SIZE>::kGuard2_[8];
#endif
    template <typename T, size_t BUF_SIZE>
    void SmallStackBuffer<T, BUF_SIZE>::ValidateGuards_ () const
    {
        Assert (::memcmp (kGuard1_, fGuard1_, sizeof (kGuard1_)) == 0);
        Assert (::memcmp (kGuard2_, fGuard2_, sizeof (kGuard2_)) == 0);
    }
#endif
    template <typename T, size_t BUF_SIZE>
    inline T* SmallStackBuffer<T, BUF_SIZE>::BufferAsT_ ()
    {
        return reinterpret_cast<T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    inline const T* SmallStackBuffer<T, BUF_SIZE>::BufferAsT_ () const
    {
        return reinterpret_cast<const T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::DestroyElts_ (T* start, T* end)
    {
        for (auto i = start; i != end; ++i) {
            i->~T ();
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline Memory::Byte* SmallStackBuffer<T, BUF_SIZE>::LiveDataAsAllocatedBytes_ ()
    {
        Require (fLiveData_ != BufferAsT_ ());
        return reinterpret_cast<Memory::Byte*> (fLiveData_);
    }
    template <typename T, size_t BUF_SIZE>
    inline Memory::Byte* SmallStackBuffer<T, BUF_SIZE>::Allocate_ (size_t bytes)
    {
        void* p = malloc (bytes);
        Execution::ThrowIfNull (p);
        return reinterpret_cast<Memory::Byte*> (p);
    }
    template <typename T, size_t BUF_SIZE>
    inline void SmallStackBuffer<T, BUF_SIZE>::Deallocate_ (Memory::Byte* bytes)
    {
        if (bytes != nullptr) {
            free (bytes);
        }
    }

}

#endif /*_Stroika_Foundation_Memory_SmallStackBuffer_inl_*/
