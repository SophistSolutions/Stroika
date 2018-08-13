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
#include "Common.h"

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ************************* SmallStackBuffer<T, BUF_SIZE> ************************
     ********************************************************************************
     */
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer ()
        : fSize_ (0)
        , fLiveData_ (BufferAsT_ ())
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
        T* outI = this->begin ();
        for (ITERATOR_OF_T i = start; i != end; ++i, ++outI) {
            *outI = *i;
        }
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <size_t FROM_BUF_SIZE>
    SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer<T, FROM_BUF_SIZE>& from)
        : SmallStackBuffer (from.size ())
    {
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
        Memory::Private::VC_BWA_std_copy (from.fLiveData_, from.fLiveData_ + from.size (), fLiveData_);
#else
        copy (from.fLiveData_, from.fLiveData_ + from.size (), fLiveData_);
#endif
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::SmallStackBuffer (const SmallStackBuffer& from)
        : SmallStackBuffer (from.size ())
    {
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
        Memory::Private::VC_BWA_std_copy (from.fLiveData_, from.fLiveData_ + from.size (), fLiveData_);
#else
        copy (from.fLiveData_, from.fLiveData_ + from.size (), fLiveData_);
#endif
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline SmallStackBuffer<T, BUF_SIZE>::~SmallStackBuffer ()
    {
        Invariant ();
        if (fLiveData_ != BufferAsT_ ()) {
            // we must have used the heap...
            delete[] fLiveData_;
        }
    }
    template <typename T, size_t BUF_SIZE>
    template <size_t FROM_BUF_SIZE>
    SmallStackBuffer<T, BUF_SIZE>& SmallStackBuffer<T, BUF_SIZE>::operator= (const SmallStackBuffer<T, FROM_BUF_SIZE>& rhs)
    {
        Invariant ();
        ReserveAtLeast (rhs.size ());
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
        Memory::Private::VC_BWA_std_copy (rhs.fLiveData_, rhs.fLiveData_ + rhs.size (), fLiveData_);
#else
        copy (rhs.fLiveData_, rhs.fLiveData_ + rhs.size (), fLiveData_);
#endif
        Invariant ();
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    SmallStackBuffer<T, BUF_SIZE>& SmallStackBuffer<T, BUF_SIZE>::operator= (const SmallStackBuffer& rhs)
    {
        Invariant ();
        ReserveAtLeast (rhs.size ());
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
        Memory::Private::VC_BWA_std_copy (rhs.fLiveData_, rhs.fLiveData_ + rhs.size (), fLiveData_);
#else
        copy (rhs.fLiveData_, rhs.fLiveData_ + rhs.size (), fLiveData_);
#endif
        Invariant ();
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
        if (nElements > capacity ()) {
            /*
             *   If we REALLY must grow, the double in size so unlikely we'll have to grow/malloc/copy again.
             */
            reserve (max (nElements, capacity () * 2));
        }
        fSize_ = nElements;
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
            // @todo note this is wrong because it CONSTRUCTS too many elements - we want to only construct fSize elements.
            // BUt OK cuz for now we only use on POD data
            T* newPtr = new T[nElements]; // NB: We are careful not to update our size field til this has succeeded (exception safety)

            // Not totally safe for T with CTOR/DTOR/Op= ... Don't use this class in that case!!!
            // No idea how many to copy!!! - do worst case(maybe should keep old size if this ever
            // bus errors???)
            (void)::memcpy (newPtr, fLiveData_, fSize_ * sizeof (T));
            if (fLiveData_ != BufferAsT_ ()) {
                // we must have used the heap...
                delete[] fLiveData_;
            }
            fLiveData_ = newPtr;

            // since we are using the heap, we can store the size in our fInlinePreallocatedBuffer_
            *reinterpret_cast<size_t*> (BufferAsT_ ()) = nElements;
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
        return (fLiveData_ == BufferAsT_ ()) ? BUF_SIZE : *reinterpret_cast<const size_t*> (BufferAsT_ ()); // @see class Design Note
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
        resize (s + 1);
        fLiveData_[s] = e;
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

}

#endif /*_Stroika_Foundation_Memory_SmallStackBuffer_inl_*/
