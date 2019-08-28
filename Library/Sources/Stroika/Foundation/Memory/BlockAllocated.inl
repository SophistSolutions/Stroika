/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BlockAllocated_inl_
#define _Stroika_Foundation_Memory_BlockAllocated_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ************************* BlockAllocationUseHelper<T> **************************
     ********************************************************************************
     */
    template <typename T>
    inline void* BlockAllocationUseHelper<T>::operator new (size_t n)
    {
        return BlockAllocator<T>::Allocate (n);
    }
    template <typename T>
    inline void* BlockAllocationUseHelper<T>::operator new (size_t n, int, const char*, int)
    {
        return BlockAllocator<T>::Allocate (n);
    }
    template <typename T>
    inline void BlockAllocationUseHelper<T>::operator delete (void* p)
    {
        BlockAllocator<T>::Deallocate (p);
    }
    template <typename T>
    inline void BlockAllocationUseHelper<T>::operator delete (void* p, int, const char*, int)
    {
        BlockAllocator<T>::Deallocate (p);
    }

    /*
     ********************************************************************************
     ******************* BlockAllocationUseGlobalAllocatorHelper<T> *****************
     ********************************************************************************
     */
    template <typename T>
    inline void* BlockAllocationUseGlobalAllocatorHelper<T>::operator new (size_t n)
    {
        return ::operator new (n);
    }
    template <typename T>
    inline void* BlockAllocationUseGlobalAllocatorHelper<T>::operator new (size_t n, int, const char*, int)
    {
        return ::operator new (n);
    }
    template <typename T>
    inline void BlockAllocationUseGlobalAllocatorHelper<T>::operator delete (void* p)
    {
        ::operator delete (p);
    }
    template <typename T>
    inline void BlockAllocationUseGlobalAllocatorHelper<T>::operator delete (void* p, int, const char*, int)
    {
        ::operator delete (p);
    }

    /*
     ********************************************************************************
     *************************** ManuallyBlockAllocated<T> **************************
     ********************************************************************************
     */
    template <typename T>
    template <typename... ARGS>
    inline T* ManuallyBlockAllocated<T>::New (ARGS&&... args)
    {
#if qAllowBlockAllocation
        return new (BlockAllocator<T>::Allocate (sizeof (T))) T (forward<ARGS> (args)...);
#else
        return new T (forward<ARGS> (args)...);
#endif
    }
    template <typename T>
    inline void ManuallyBlockAllocated<T>::Delete (T* p) noexcept
    {
#if qAllowBlockAllocation
        if (p != nullptr) {
            (p)->~T ();
            BlockAllocator<T>::Deallocate (p);
        }
#else
        delete p;
#endif
    }

}

#endif /*_Stroika_Foundation_Memory_BlockAllocated_inl_*/
