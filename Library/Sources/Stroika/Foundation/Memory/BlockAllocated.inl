/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
    inline void* BlockAllocationUseHelper<T>::operator new ([[maybe_unused]] size_t n)
    {
        Require (n == sizeof (T));
        return BlockAllocator<T>{}.allocate (1);
    }
    template <typename T>
    inline void* BlockAllocationUseHelper<T>::operator new ([[maybe_unused]] size_t n, int, const char*, int)
    {
        Require (n == sizeof (T));
        return BlockAllocator<T>{}.allocate (1);
    }
    template <typename T>
    inline void BlockAllocationUseHelper<T>::operator delete (void* p)
    {
        BlockAllocator<T>{}.deallocate (reinterpret_cast<T*> (p), 1);
    }
    template <typename T>
    inline void BlockAllocationUseHelper<T>::operator delete (void* p, int, const char*, int)
    {
        BlockAllocator<T>{}.deallocate (reinterpret_cast<T*> (p), 1);
    }

    /*
     ********************************************************************************
     *************************** UsesBlockAllocation<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    constexpr bool UsesBlockAllocation ()
    {
        return is_base_of_v<BlockAllocationUseHelper<T>, T>;
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
        return new (BlockAllocator<T>{}.allocate (1)) T{forward<ARGS> (args)...};
#else
        return new T{forward<ARGS> (args)...};
#endif
    }
    template <typename T>
    inline void ManuallyBlockAllocated<T>::Delete (T* p) noexcept
    {
#if qAllowBlockAllocation
        if (p != nullptr) {
            destroy_at (p);
            BlockAllocator<T>{}.deallocate (p, 1);
        }
#else
        delete p;
#endif
    }

}

#endif /*_Stroika_Foundation_Memory_BlockAllocated_inl_*/
