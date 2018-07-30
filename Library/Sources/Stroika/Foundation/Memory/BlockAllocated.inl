/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

    ////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////// DEPRECATED STUFF///////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    /**
         *  \note DEPRECATED - USE UseBlockAllocationIfAppropriate
         */
#if qAllowBlockAllocation
#define DECLARE_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)                                                                                         \
    static void* operator new (size_t n) { return (Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Allocate (n)); }                        \
    static void* operator new (size_t n, int, const char*, int) { return (Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Allocate (n)); } \
    static void  operator delete (void* p) { Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Deallocate (p); }                             \
    static void  operator delete (void* p, int, const char*, int) { Stroika::Foundation::Memory::BlockAllocator<THIS_CLASS>::Deallocate (p); }
#else
#define DECLARE_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)
#endif

// Deprecated in Stroika v2.1d4 - USE UseBlockAllocationIfAppropriate
#define DECLARE_USE_BLOCK_ALLOCATION(THIS_CLASS) use = "DECLARE_USE_BLOCK_ALLOCATION_DEPRECATED"

// Deprecated in Stroika v2.1d4 - USE UseBlockAllocationIfAppropriate
#if qAllowBlockAllocation
#define DECLARE_DONT_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)        \
    static void* operator new (size_t n) { return ::operator new (n); } \
    static void  operator delete (void* p) { ::operator delete (p); }
#else
#define DECLARE_DONT_USE_BLOCK_ALLOCATION_DEPRECATED(THIS_CLASS)
#endif

// Deprecated in Stroika v2.1d4 - USE BlockAllocationUseGlobalAllocatorHelper
#define DECLARE_DONT_USE_BLOCK_ALLOCATION(THIS_CLASS) use = "DECLARE_DONT_USE_BLOCK_ALLOCATION_DEPRECATED"

    /**
     * \brief  Utility class to implement special memory allocator pattern which can greatly improve performance - @see DECLARE_USE_BLOCK_ALLOCATION()
     *
     * AutomaticallyBlockAllocated<T> is a templated class designed to allow easy use
     * of a block-allocated memory strategy. This means zero overhead malloc/memory allocation for fixed
     * size blocks (with the only problem being the storage is never - or almost never - returned to the
     * free store - it doesn't leak - but cannot be used for other things. This is often a useful
     * tradeoff for things you allocate a great number of.
     *
     * You shouldn't disable it lightly. But you may wish to temporarily disable block-allocation
     * while checking for memory leaks by shutting of the qAllowBlockAllocation compile-time configuration variable.
     * 
     *  If qAllowBlockAllocation true (default) - this will use the optimized block allocation store, but if qAllowBlockAllocation is
     *  false (0), this will just default to the global ::new/::delete
     *
     */
    template <typename T>
    class [[deprecated ("bad idea - if you use to allocate - must retain type AutomaticallyBlockAllocated<T>* or do wrong delete - use UseBlockAllocationIfAppropriate instead- deprecated in v2.1d4")]] AutomaticallyBlockAllocated : public UseBlockAllocationIfAppropriate<T>
    {
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    public:
        AutomaticallyBlockAllocated ()
            : fValue_ ()
        {
        }
        AutomaticallyBlockAllocated (const AutomaticallyBlockAllocated& t)
            : fValue_ (t)
        {
        }
        AutomaticallyBlockAllocated (const T& t)
            : fValue_ (t)
        {
        }
        AutomaticallyBlockAllocated (T && t)
            : fValue_ (move (t))
        {
        }

    public:
        const AutomaticallyBlockAllocated<T>& operator= (const AutomaticallyBlockAllocated& t)
        {
            fValue_ = t.fValue_;
            return *this;
        }
        const AutomaticallyBlockAllocated<T>& operator= (const T& t)
        {
            fValue_ = t;
            return *this;
        }
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")

    public:
        operator T () const
        {
            return fValue_;
        }

    public:
        T* get ()
        {
            return &fValue_;
        }

    private:
        T fValue_;
    };

}

#endif /*_Stroika_Foundation_Memory_BlockAllocated_inl_*/
