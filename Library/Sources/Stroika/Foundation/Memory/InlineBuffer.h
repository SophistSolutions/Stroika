/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_InlineBuffer_h_
#define _Stroika_Foundation_Memory_InlineBuffer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <span>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/TypeHints.h"
#include "Stroika/Foundation/Memory/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Memory {

    /**
     */
    template <typename T = byte>
    constexpr size_t InlineBuffer_DefaultInlineSize ()
    {
        // note must be defined here, not in inl file, due to use as default template argument
        auto r = ((4096 / sizeof (T)) == 0 ? 1 : (4096 / sizeof (T)));
        Ensure (r >= 1);
        return r;
    }

    /**
     *  \brief Smart 'direct memory array' - which when needed automatically switches to heap based so can grow - BUF_SIZE is number of T elements allocated inline
     *
     *  Typically, InlineBuffer<> combines the performance of using a pre-allocated fixed-sized buffer to store arrays with
     *  the safety and flexibility of using the free store (malloc).
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
     *  \note Until Stroika 2.1r4, this class was called SmallStackBuffer<>, cuz that's basically the most common
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
     *          Memory::InlineBuffer<byte> useKey{Memory::eUninitialized, keyLen};
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
    template <typename T = byte, size_t BUF_SIZE = InlineBuffer_DefaultInlineSize<T> ()>
    class InlineBuffer {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         */
        using pointer       = T*;
        using const_pointer = const T*;

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
        InlineBuffer () noexcept;
        InlineBuffer (size_t nElements);
        InlineBuffer (UninitializedConstructorFlag, size_t nElements)
            requires (is_trivially_copyable_v<T>);
        template <size_t FROM_BUF_SIZE>
        InlineBuffer (const InlineBuffer<T, FROM_BUF_SIZE>& src);
        InlineBuffer (const InlineBuffer& src);
        InlineBuffer (InlineBuffer&& src);
        template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2>
        InlineBuffer (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end);
        template <ISpanOfT<T> SPAN_T>
        InlineBuffer (const SPAN_T& copyFrom);
        ~InlineBuffer ();

    public:
        /** 
         */
        nonvirtual InlineBuffer& operator= (const InlineBuffer& rhs);
        nonvirtual InlineBuffer& operator= (InlineBuffer&& rhs);
        template <ISpanOfT<T> SPAN_T>
        nonvirtual InlineBuffer& operator= (const SPAN_T& copyFrom);

    public:
        /**
         *  \brief returns the same value as data () - a live pointer to the start of the buffer.
         * 
         *  \note This was changed from non-explicit to explicit in Stroika v3.0d1
         */
        nonvirtual explicit operator const T* () const noexcept;
        nonvirtual explicit operator T* () noexcept;

    public:
        /**
         *  \brief returns a (possibly const) pointer to the start of the live buffer data. This return value can be invalidated
         *         by any changes in size/capacity of the InlineBuffer (but not by other changes, like at).
         */
        nonvirtual pointer       data () noexcept;
        nonvirtual const_pointer data () const noexcept;

    public:
        /**
         */
        nonvirtual iterator       begin () noexcept;
        nonvirtual const_iterator begin () const noexcept;

    public:
        /**
         */
        nonvirtual iterator       end () noexcept;
        nonvirtual const_iterator end () const noexcept;

    public:
        /**
         *  \req i < size ()
         */
        nonvirtual reference       at (size_t i) noexcept;
        nonvirtual const_reference at (size_t i) const noexcept;

    public:
        /**
         *  \req i < size ()
         */
        nonvirtual reference       operator[] (size_t i) noexcept;
        nonvirtual const_reference operator[] (size_t i) const noexcept;

    public:
        /**
         *  Returns the 'size' the InlineBuffer can be resized up to without any additional memory allocations.
         *  This always returns a value at least as large as the BUF_SIZE template parameter.
         *
         *  @see reserve
         */
        constexpr size_t capacity () const noexcept;

    public:
        /**
         *  Provide a hint as to how much (contiguous) space to reserve.
         *
         *  if (default true) atLeast flag is true, newCapacity is adjusted (increased) with GetScaledUpCapacity
         *  to minimize needless copies as buffer grows, but only if any memory allocation would have been needed anyhow.
         * 
         *  if atLeast is false, then reserve sets the capacity to exactly the amount prescribed (unless its less than BUF_SIZE).
         *  This can be used to free-up used memory.
         *
         *  @see capacity
         *
         *  \req  (newCapacity >= size ());
         *  \ens  (newCapacity <= BUF_SIZE and capacity () == BUF_SIZE) or (newCapacity > BUF_SIZE and newCapacity == capacity ());
         */
        nonvirtual void reserve (size_t newCapacity, bool atLeast = true);

    public:
        [[deprecated ("Since Stroika v3.0d1, just use reserve with atLeast flag=true)")]] void ReserveAtLeast (size_t newCapacityAtLeast)
        {
            reserve (newCapacityAtLeast, true);
        }

    public:
        /**
         *  Returns the number of (constructed) elements in the buffer in ELEMENTS (not necessarily in bytes).
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual size_t GetSize () const noexcept;

    public:
        /**
         *  Returns the number of (constructed) elements in the buffer.
         *
         *  @see GetSize ();        // alias
         *  @see capacity ();
         *
         *  \ensure size () <= capacity ();
         */
        nonvirtual size_t size () const noexcept;

    public:
        /**
         *  returns true iff size () == 0
         */
        nonvirtual bool empty () const noexcept;

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
        nonvirtual void resize_uninitialized (size_t nElements)
            requires (is_trivially_copyable_v<T> and is_trivially_destructible_v<T>);

    public:
        /**
         *  Same as resize (nElements), except asserts (documents) the new size must be smaller or equal to the old size.
         * 
         *  \req nElements <= size ()
         */
        nonvirtual void ShrinkTo (size_t nElements);

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
        nonvirtual void GrowToSize_uninitialized (size_t nElements)
            requires (is_trivially_copyable_v<T>);

    public:
        /**
         *  With a single T argument, this is somewhat STLISH, but also takes overload of a span, so you can append multiple.
         * 
         *  \note ALIAS: Append
         * 
         *  \see also push_back_coerced ()
         */
        nonvirtual void push_back (Common::ArgByValueType<T> e);
        template <ISpanOfT<T> SPAN_T>
        nonvirtual void push_back (const SPAN_T& copyFrom);

    public:
        /**
         *  \brief same as push_back (span{}) except that the span type doesn't need to match exactly, so long as indirected items can be copied to destination (with static_cast).
         */
        template <ISpan SPAN_T>
        nonvirtual void push_back_coerced (const SPAN_T& copyFrom);

    public:
        /**
         */
        nonvirtual void clear () noexcept;

#if qDebug
    private:
        static constexpr byte kGuard1_[8] = {
            0x45_b, 0x23_b, 0x12_b, 0x56_b, 0x99_b, 0x76_b, 0x12_b, 0x55_b,
        };
        static constexpr byte kGuard2_[8] = {
            0x15_b, 0x32_b, 0xa5_b, 0x16_b, 0x11_b, 0x7a_b, 0x90_b, 0x10_b,
        };
#endif

    private:
        // note must be inline declared here since used in type definition below
        static constexpr size_t SizeInBytes_ (size_t nElts) noexcept
        {
            return sizeof (T[1]) * nElts; // not sure why return sizeof (T[nElts]); fails on vs2k21?
        }

    private:
        nonvirtual byte* LiveDataAsAllocatedBytes_ () noexcept;

    private:
        static byte* Allocate_ (size_t bytes);

    private:
        static void Deallocate_ (byte* bytes) noexcept;

    private:
        static byte* Reallocate_ (byte* bytes, size_t n)
            requires (is_trivially_copyable_v<T>);

    private:
        size_t fSize_{};
#if qDebug
        byte fGuard1_[sizeof (kGuard1_)];
#endif
        DISABLE_COMPILER_MSC_WARNING_START (4324)
        union {
            size_t fCapacityOfFreeStoreAllocation_; // only valid if fLiveData_ != &fInlinePreallocatedBuffer_[0]
            alignas (T) byte fInlinePreallocatedBuffer_[SizeInBytes_ (BUF_SIZE)]; // alignas both since sometimes accessed as array of T, and sometimes as size_t
        };
        DISABLE_COMPILER_MSC_WARNING_END (4324)

#if qDebug
        byte fGuard2_[sizeof (kGuard2_)];
#endif
        T* fLiveData_{};

    private:
        // generally unneeded optimization, but allows quick check of sz against just BUF_SIZE in most cases
        constexpr bool HasEnoughCapacity_ (size_t sz) const
        {
            // Computing capacity - while simple and quick, is much slower than this check which
            // is nearly always sufficient. So a slight performance tweak
            if (sz <= BUF_SIZE) [[likely]] {
                return true;
            }
            return sz <= capacity ();
        }

    private:
        constexpr bool UsingInlinePreallocatedBuffer_ () const noexcept;

    public:
        nonvirtual void Invariant () const noexcept;

    private:
#if qDebug
        nonvirtual void Invariant_ () const noexcept;
        nonvirtual void ValidateGuards_ () const noexcept;
#endif

    private:
        constexpr T*       BufferAsT_ () noexcept;
        constexpr const T* BufferAsT_ () const noexcept;

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
