/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_InlineBuffer_h_
#define _Stroika_Foundation_Memory_InlineBuffer_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/TypeHints.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Memory {

    using std::byte;

    /**
     *  Typically, InlineBuffer<> combines the performance of using a pre-allocated fixed-sized buffer to store arrays with
     *  the safety and flexability of using the free store (malloc).
     *
     *  Think of it as a hybrid between std::vector<> and std::array - with functionality like
     *  std::vector, but performance more like std::array.
     *
     *  Internally, InlineBuffer maintains a fixed buffer (of size given by its BUFFER_SIZE template parameter)
     *  and it uses that while things fit, and switches to using a free-store-allocated data as needed. Pick the BUF_SIZE
     *  wisely, and you always end up with fixed sized objects. Pick poorly, and it will still work, but allocating the
     *  data on the free store.
     *
     *  typically sizeof(InlineBuffer<T,BUF_SIZE>) will come to roughly BUF_SIZE*sizeof(T).
     * 
     *  \see also StackBuffer<T,BUF_SIZE> - similar, but less flexible, and could someday be more performant.
     *
     *  All allocated objects are default initialized, unless they are allocated through a call to resize_uninitialized(), or
     *  the constructor with the argument eUninitialized
     *
     *  \note Until Stroika 2.1r4, this class was called SmallStackBuffer<>, cuz thats basically the most common
     *        scenario it was used for up til that point.
     *
     *  \par Example Usage
     *      @see Samples/SimpleService project
     *      \code
     *          Memory::InlineBuffer<byte> useKey{keyLen};  // no need to default initialize cuz done automatically
     *          (void)::memcpy (useKey.begin (), key.begin (), min (keyLen, key.size ()));
     *      \endcode
     * OR
     *      \code
     *          Memory::InlineBuffer<byte> useKey{Memory::eUninitiialized, keyLen};
     *          (void)::memset (useKey.begin (), 0, keyLen);
     *          (void)::memcpy (useKey.begin (), key.begin (), min (keyLen, key.size ()));
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   InlineBuffer<T> can roughly be used as a replacement for vector<> - behaving similarly, except that its optimized
     *          for the case where the caller statically knows GENERALLY the right size for the buffer, in which case it can be
     *          allocated more cheaply.
     *
     *          InlineBuffer<T> CAN be copied, and will properly construct/destruct array members as they are added/removed.
     *          (new feature as of Stroika v2.1b6).
     *
     *  \note   We do not provide an operator[] overload because this creates ambiguity with the operator* overload.
     *
     *  \note   Implementation Note - we store the store the 'capacity' in a union (fCapacityOfFreeStoreAllocation_) overlapping with fInlinePreallocatedBuffer_ if its > BUF_SIZE, and pin it at the minimum
     *          to BUF_SIZE
     *
     */
    template <typename T = std::byte, size_t BUF_SIZE = ((4096 / sizeof (T)) == 0 ? 1 : (4096 / sizeof (T)))>
    class InlineBuffer {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         */
        using iterator       = T*;
        using const_iterator = const T*;

    public:
        /**
         */
        using reference       = T&;
        using const_reference = const T&;

    public:
        /**
         *  kMinCapacity is the baked in minimum capacity of this buff (inline allocated part).
         */
        static constexpr size_t kMinCapacity = BUF_SIZE;

    public:
        /**
         *  InlineBuffer::InlineBuffer (size_t) specifies the initial size - like InlineBuffer::InlineBuffer {} followed by resize (n); 
         *  InlineBuffer::default-ctor creates a zero-sized stack buffer (so resize with resize, or push_back etc).
         */
        InlineBuffer ();
        InlineBuffer (size_t nElements);
        InlineBuffer (UninitializedConstructorFlag, size_t nElements);
        template <size_t FROM_BUF_SIZE>
        InlineBuffer (const InlineBuffer<T, FROM_BUF_SIZE>& src);
        InlineBuffer (const InlineBuffer& src);
        InlineBuffer (InlineBuffer&& src);
        template <typename ITERATOR_OF_T, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_T>, char>* = nullptr>
        InlineBuffer (ITERATOR_OF_T start, ITERATOR_OF_T end);
        ~InlineBuffer ();

    public:
        /** 
         */
        template <size_t FROM_BUF_SIZE>
        nonvirtual InlineBuffer& operator= (const InlineBuffer<T, FROM_BUF_SIZE>& rhs);
        nonvirtual InlineBuffer& operator= (const InlineBuffer& rhs);
        nonvirtual InlineBuffer& operator= (InlineBuffer&& rhs);

    public:
        /**
         */
        nonvirtual operator const T* () const;
        nonvirtual operator T* ();

    public:
        /**
         */
        nonvirtual iterator       begin ();
        nonvirtual const_iterator begin () const;

    public:
        /**
         */
        nonvirtual iterator       end ();
        nonvirtual const_iterator end () const;

    public:
        /**
         */
        nonvirtual reference       at (size_t i);
        nonvirtual const_reference at (size_t i) const;

    public:
        /**
         *  Returns the 'size' the InlineBuffer can be resized up to without any additional memory allocations.
         *  This always returns a value at least as large as the BUF_SIZE template parameter.
         *
         *  @see reserve
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         *  Provide a hint as to how much (contiguous) space to reserve.
         *
         *  @see capacity
         *
         *  \req  (newCapacity >= size ());
         *  \ens  ((nElements <= BUF_SIZE && capacity () == BUF_SIZE) or (nElements > BUF_SIZE and nElements == capacity ()));
         */
        nonvirtual void reserve (size_t newCapacity);

    public:
        /**
         *  Provide a hint as to how much (contiguous) space to reserve. Like @reserve () but this will not shrink the buffer.
         *
         *  @see reserve
         */
        nonvirtual void ReserveAtLeast (size_t newCapacityAtLeast);

    public:
        /**
         *  Returns the number of (constructed) elements in the buffer in ELEMENTS (not necessarily in bytes).
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual size_t GetSize () const;

    public:
        /**
         *  Returns the number of (constructed) elements in the buffer.
         *
         *  @see GetSize ();        // alias
         *  @see capacity ();
         *
         *  \ensure size () <= capacity ();
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  Grow or shrink the buffer. The 'size' is the number of constructed elements, and this function automatically
         *  assures the capacity is maintained at least as large as the size.
         *
         *  If resize () causes the list to grow, the new elements are default-initialized()
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void resize (size_t nElements);

    public:
        /**
         * \brief same as resize (), except leaves newly created elements uninitialized (requires is_trivially_copyable_v<T>) 
         *
         *  \req is_trivially_copyable_v<T>
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void resize_uninitialized (size_t nElements);

    public:
        /**
         *  Grow the buffer to at least nElements in size (wont shrink). The 'size' is the number of constructed elements,
         *  and this function automatically assures the capacity is maintained at least as large as the size.
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void GrowToSize (size_t nElements);

    public:
        /**
         * \brief same as GrowToSize (), except leaves newly created elements uninitialized (requires is_trivially_copyable_v<T>)
         *
         *  \req is_trivially_copyable_v<T>
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void GrowToSize_uninitialized (size_t nElements);

    public:
        nonvirtual void push_back (Configuration::ArgByValueType<T> e);

    public:
        /**
         */
        nonvirtual void clear ();

    private:
        nonvirtual void reserve_ (size_t nElements);

#if qDebug
    private:
        static constexpr byte kGuard1_[8] = {
            byte{0x45},
            byte{0x23},
            byte{0x12},
            byte{0x56},
            byte{0x99},
            byte{0x76},
            byte{0x12},
            byte{0x55},
        };
        static constexpr byte kGuard2_[8] = {
            byte{0x15},
            byte{0x32},
            byte{0xa5},
            byte{0x16},
            byte{0x11},
            byte{0x7a},
            byte{0x90},
            byte{0x10},
        };
#endif

    private:
        // note must be inline declared here since used in type definition below
        static constexpr size_t SizeInBytes_ (size_t nElts)
        {
            return sizeof (T[1]) * nElts; // not sure why return sizeof (T[nElts]); fails on vs2k17?
        }

    private:
        nonvirtual byte* LiveDataAsAllocatedBytes_ ();

    private:
        static byte* Allocate_ (size_t bytes);

    private:
        static void Deallocate_ (byte* bytes);

    private:
        size_t fSize_{};
#if qDebug
        byte fGuard1_[sizeof (kGuard1_)];
#endif
        DISABLE_COMPILER_MSC_WARNING_START (4324)
        union {
            size_t fCapacityOfFreeStoreAllocation_;                               // only valid if fLiveData_ != &fInlinePreallocatedBuffer_[0]
            alignas (T) byte fInlinePreallocatedBuffer_[SizeInBytes_ (BUF_SIZE)]; // alignas both since sometimes accessed as array of T, and sometimes as size_t
        };
        DISABLE_COMPILER_MSC_WARNING_END (4324)

#if qDebug
        byte fGuard2_[sizeof (kGuard2_)];
#endif
        T* fLiveData_{};

    private:
        nonvirtual bool UsingInlinePreallocatedBuffer_ () const;

    public:
        nonvirtual void Invariant () const noexcept;

    private:
#if qDebug
        nonvirtual void Invariant_ () const noexcept;
        nonvirtual void ValidateGuards_ () const noexcept;
#endif

    private:
        nonvirtual T*       BufferAsT_ () noexcept;
        nonvirtual const T* BufferAsT_ () const noexcept;

    private:
        static void DestroyElts_ (T* start, T* end) noexcept;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InlineBuffer.inl"

#endif /*_Stroika_Foundation_Memory_InlineBuffer_h_*/
