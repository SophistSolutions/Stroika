/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SmallStackBuffer_h_
#define _Stroika_Foundation_Memory_SmallStackBuffer_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Minor - but we could do better than alignas(size_t) by just adjusting the offset of the size pointer in the buff, and just assure
 *              buf already large enuf (ptr + alignof(size_t) - 1) % alignof(size_t)) + ptr;
 *
 *  Long-Term TOD:
 *      @todo   https://stroika.atlassian.net/browse/STK-159
 *              Support non-POD type 'T' values properly.
 *              IE Lose \req std::is_trivially_constructible<T>::value,
 *              std::is_trivially_destructible<T>::value,
 *              std::is_trivially_copyable<T>::value
 */

namespace Stroika::Foundation::Memory {

    /**
     *  SmallStackBuffer<> combines the performance of using a stack buffer to store arrays with
     *  the safety and flexability of using the free store (malloc).
     *
     *  In steps <code>SmallStackBuffer&ltT&gt</code>. Just declare one of these, and it
     *  automaticlly uses the stack for the buffer if it will fit, and silently allocates heap memory
     *  and returns it to the free store on stack based destruction if needed. Use it just like you
     *  would a stack-based array. You don't need to know about this to use any of Led's APIs, but its
     *  a helpful class you may want to use elsewhere in your code.</p>
     *
     *  typically sizeof(SmallStackBuffer<T,BUF_SIZE>) will come to roughly 4K, and always at least something.
     *
     *  \par Example Usage
     *      @see Samples/SimpleService project
     *      \code
     *          Memory::SmallStackBuffer<Byte> useKey{keyLen};
     *          (void)::memset (useKey.begin (), 0, keyLen);
     *          (void)::memcpy (useKey.begin (), key.begin (), min (keyLen, key.size ()));
     *      \endcode
     *
     *  \req std::is_trivially_constructible<T>::value -- @see https://stroika.atlassian.net/browse/STK-159
     *  \req std::is_trivially_destructible<T>::value
     *  \req std::is_trivially_copyable<T>::value
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   We do not provide an operator[] overload because this creates ambiguity with the operator* overload.
     *
     *  \note   Implementation Note - we store the store the 'capacity' in the fInlinePreallocatedBuffer_ if its > BUF_SIZE, and pin it at the minimum
     *          to BUF_SIZE
     *
     */
    template <typename T, size_t BUF_SIZE = ((4096 / sizeof (T)) == 0 ? 1 : (4096 / sizeof (T)))>
    class SmallStackBuffer {
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
         *  SmallStackBuffer::SmallStackBuffer (size_t) specifies the initial size - like SmallStackBuffer::SmallStackBuffer {} followed by resize (n); 
         *  SmallStackBuffer::default-ctor creates a zero-sized stack buffer (so resize with resize, or push_back etc).
         */
        SmallStackBuffer ();
        SmallStackBuffer (size_t nElements);
        template <size_t FROM_BUF_SIZE>
        SmallStackBuffer (const SmallStackBuffer<T, FROM_BUF_SIZE>& from);
        SmallStackBuffer (const SmallStackBuffer& from);
        SmallStackBuffer (SmallStackBuffer&&) = delete;
        template <typename ITERATOR_OF_T>
        SmallStackBuffer (ITERATOR_OF_T start, ITERATOR_OF_T end);
        ~SmallStackBuffer ();

    public:
        /** 
         */
        template <size_t FROM_BUF_SIZE>
        nonvirtual SmallStackBuffer& operator= (const SmallStackBuffer<T, FROM_BUF_SIZE>& rhs);
        nonvirtual SmallStackBuffer& operator= (const SmallStackBuffer& rhs);
        nonvirtual SmallStackBuffer& operator= (SmallStackBuffer&&) = delete;

    public:
        // @todo - lift these restrictions - closely related to https://stroika.atlassian.net/browse/STK-159
        static_assert (is_trivially_constructible<T>::value, "require T is is_trivially_constructible");
        static_assert (is_trivially_destructible<T>::value, "require T is is_trivially_destructible");
        static_assert (is_trivially_copyable<T>::value, "require T is is_trivially_copyable");

    public:
        /**
         */
        nonvirtual operator const T* () const;
        nonvirtual operator T* ();

    public:
        /**
         */
        nonvirtual iterator begin ();
        nonvirtual const_iterator begin () const;

    public:
        /**
         */
        nonvirtual iterator end ();
        nonvirtual const_iterator end () const;

    public:
        /**
         */
        nonvirtual reference at (size_t i);
        nonvirtual const_reference at (size_t i) const;

    public:
        /**
         *  Returns the 'size' the SmallStackBuffer can be resized up to without any additional memory allocations.
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
         *  \note @todo - current implementation never really shrinks - for no good reason. Fix that.
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
         *  \note @todo TBD what happens with newly created elements. Initializing is safest, but most costly.
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void resize (size_t nElements);

    public:
        /**
         *  Grow the buffer to at least nElements in size (wont shrink). The 'size' is the number of constructed elements,
         *  and this function automatically assures the capacity is maintained at least as large as the size.
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void GrowToSize (size_t nElements);

    private:
        nonvirtual void reserve_ (size_t nElements);

    public:
        nonvirtual void push_back (const T& e);

#if qDebug
    private:
        static constexpr Byte kGuard1_[8] = {
            0x45,
            0x23,
            0x12,
            0x56,
            0x99,
            0x76,
            0x12,
            0x55,
        };
        static constexpr Byte kGuard2_[8] = {
            0x15,
            0x32,
            0xa5,
            0x16,
            0x11,
            0x7a,
            0x90,
            0x10,
        };
#endif

    private:
        size_t fSize_{};
#if qDebug
        Byte fGuard1_[sizeof (kGuard1_)];
#endif
        alignas (T) alignas (size_t) Memory::Byte fInlinePreallocatedBuffer_[sizeof (T[BUF_SIZE])]; // alignas both since sometimes accessed as array of T, and sometimes as size_t
#if qDebug
        Byte fGuard2_[sizeof (kGuard2_)];
#endif
        T* fLiveData_{};

    public:
        nonvirtual void Invariant () const;

    private:
#if qDebug
        nonvirtual void Invariant_ () const;
        nonvirtual void ValidateGuards_ () const;
#endif

    private:
        nonvirtual T*    BufferAsT_ ();
        nonvirtual const T* BufferAsT_ () const;

    private:
        static void DestroyElts_ (T* start, T* end);

    public:
        static_assert (sizeof (SmallStackBuffer::fInlinePreallocatedBuffer_) >= sizeof (size_t), "When fPointer == fBuffer, then capacity is whole thing, and if we malloced, save size in unused buffer (so make sure big enuf).");
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SmallStackBuffer.inl"

#endif /*_Stroika_Foundation_Memory_SmallStackBuffer_h_*/
