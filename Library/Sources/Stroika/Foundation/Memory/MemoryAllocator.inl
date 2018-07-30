/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_MemoryAllocator_inl_
#define _Stroika_Foundation_Memory_MemoryAllocator_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Memory {

    /*
        ********************************************************************************
        ****************** Memory::STLAllocator<T,BASE_ALLOCATOR> **********************
        ********************************************************************************
        */
    template <typename T, typename BASE_ALLOCATOR>
    inline typename STLAllocator<T, BASE_ALLOCATOR>::pointer STLAllocator<T, BASE_ALLOCATOR>::address (typename STLAllocator<T, BASE_ALLOCATOR>::reference _Val) const noexcept
    {
        return &_Val;
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline typename STLAllocator<T, BASE_ALLOCATOR>::const_pointer STLAllocator<T, BASE_ALLOCATOR>::address (typename STLAllocator<T, BASE_ALLOCATOR>::const_reference _Val) const noexcept
    {
        return (&_Val);
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline STLAllocator<T, BASE_ALLOCATOR>::STLAllocator ()
        : fBaseAllocator ()
    {
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline STLAllocator<T, BASE_ALLOCATOR>::STLAllocator (const STLAllocator<T, BASE_ALLOCATOR>& from)
        : fBaseAllocator (from.fBaseAllocator)
    {
    }
    template <typename T, typename BASE_ALLOCATOR>
    template <typename OTHER>
    inline STLAllocator<T, BASE_ALLOCATOR>::STLAllocator (const STLAllocator<OTHER, BASE_ALLOCATOR>& from)
        : fBaseAllocator (from.fBaseAllocator)
    {
    }
    template <typename T, typename BASE_ALLOCATOR>
    template <typename OTHER>
    inline STLAllocator<T, BASE_ALLOCATOR>& STLAllocator<T, BASE_ALLOCATOR>::operator= (const STLAllocator<OTHER, BASE_ALLOCATOR>& rhs)
    {
        fBaseAllocator = rhs.from.fBaseAllocator;
        return (*this);
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline STLAllocator<T, BASE_ALLOCATOR> STLAllocator<T, BASE_ALLOCATOR>::select_on_container_copy_construction () const
    {
        return (*this);
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline typename STLAllocator<T, BASE_ALLOCATOR>::pointer STLAllocator<T, BASE_ALLOCATOR>::allocate (size_type nElements)
    {
        // allocate storage for _Count elements of type T
        return ((T*)fBaseAllocator.Allocate (nElements * sizeof (T)));
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline typename STLAllocator<T, BASE_ALLOCATOR>::pointer STLAllocator<T, BASE_ALLOCATOR>::allocate (size_type nElements, const void*)
    {
        return (allocate (nElements));
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline void STLAllocator<T, BASE_ALLOCATOR>::deallocate (pointer ptr, size_type)
    {
        if (ptr != nullptr) {
            fBaseAllocator.Deallocate (ptr);
        }
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline void STLAllocator<T, BASE_ALLOCATOR>::construct (pointer ptr)
    {
        new (ptr) T ();
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline void STLAllocator<T, BASE_ALLOCATOR>::construct (pointer ptr, const T& v)
    {
        new (ptr) T (v);
    }
    template <typename T, typename BASE_ALLOCATOR>
    template <typename OTHERT>
    inline void STLAllocator<T, BASE_ALLOCATOR>::destroy (OTHERT* p)
    {
        p->~OTHERT ();
    }
    template <typename T, typename BASE_ALLOCATOR>
    template <typename... ARGS>
    inline void STLAllocator<T, BASE_ALLOCATOR>::construct (pointer p, ARGS&&... args)
    {
        ::new ((void*)p) T (forward<ARGS> (args)...);
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline size_t STLAllocator<T, BASE_ALLOCATOR>::max_size () const noexcept
    {
        return numeric_limits<size_type>::max () / sizeof (T);
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline bool STLAllocator<T, BASE_ALLOCATOR>::operator== (const STLAllocator<T, BASE_ALLOCATOR>& /*rhs*/) const
    {
        return true;
    }
    template <typename T, typename BASE_ALLOCATOR>
    inline bool STLAllocator<T, BASE_ALLOCATOR>::operator!= (const STLAllocator<T, BASE_ALLOCATOR>& rhs) const
    {
        return not(*this == rhs);
    }

}

#endif /*_Stroika_Foundation_Memory_MemoryAllocator_inl_*/
