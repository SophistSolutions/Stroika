/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SmallStackBuffer_h_
#define _Stroika_Foundation_Memory_SmallStackBuffer_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Correctly support capacity/reserve - https://stroika.atlassian.net/browse/STK-573 - not just take.
 *
 *      @todo   Support more flexible CTOR - like SmallStackBuffer (const SmallStackBuffer<T, BUF_SIZE>& from)
 *              using different SIZE value. At same time - same for operator=
 *
 *      @todo   Minor - but we could do better than alignas(size_t) by just adjusting the offset of the size pointer in the buff, and just assure
 *              buf already large enuf (ptr + alignof(size_t) - 1) % alignof(size_t)) + ptr;
 *
 *  Long-Term TOD:
 *      @todo   Support non-POD type 'T' values properly.
 *              IE Lose \req std::is_trivially_constructible<T>::value,
 *              std::is_trivially_destructible<T>::value,
 *              std::is_trivially_copyable<T>::value
 */

namespace Stroika {
    namespace Foundation {
        namespace Memory {

            /**
             *  Often times in Led I must copy a bunch of characters to operate on them as an array
             *  (say when I'm computing word wrap info, for example). But the number of characters can vary,
             *  depending on the particular text you've entered. But it ALMOST ALWAYS would fit nicely in a
             *  little stack buffer. But you cannot GAURANTEE that is safe.</p>
             *
             *  In steps <code>SmallStackBuffer&ltT&gt</code>. Just declare one of these, and it
             *  automaticlly uses the stack for the buffer if it will fit, and silently allocates heap memory
             *  and returns it to the free store on stack based destruction if needed. Use it just like you
             *  would a stack-based array. You don't need to know about this to use any of Led's APIs, but its
             *  a helpful class you may want to use elsewhere in your code.</p>
             *
             *  typically sizeof(SmallStackBuffer<T,BUF_SIZE>) will come to roughly 4K, and always at least something.
             *
             *  \req std::is_trivially_constructible<T>::value
             *  \req std::is_trivially_destructible<T>::value
             *  \req std::is_trivially_copyable<T>::value
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#POD-Level-Thread-Safety">POD-Level-Thread-Safety</a>
             *
             *  \note   We do not provide an operator[] overload because this creates ambiguity with the operator* overload.
             *
             *  \note   Implementation Note - we store the store the 'capacity' in the fBuffer_ if its > BUF_SIZE, and pin it at the minimum
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
                using iterator       = T*;
                using const_iterator = const T*;

            public:
                using reference       = T&;
                using const_reference = const T&;

            public:
                explicit SmallStackBuffer (size_t nElements);
                SmallStackBuffer (const SmallStackBuffer<T, BUF_SIZE>& from);
                template <typename ITERATOR_OF_T>
                SmallStackBuffer (ITERATOR_OF_T start, ITERATOR_OF_T end);
                SmallStackBuffer () = delete;
                ~SmallStackBuffer ();

            public:
                nonvirtual SmallStackBuffer<T, BUF_SIZE>& operator= (const SmallStackBuffer<T, BUF_SIZE>& rhs);

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
                 *
                 *  @todo   Correctly support capacity/reserve - https://stroika.atlassian.net/browse/STK-573 - not just take.
                 */
                nonvirtual size_t capacity () const;

            public:
                /**
                 *  Provide a hint as to how much (contiguous) space to reserve.
                 *
                 *  @see capacity
                 *
                 *  @todo   Correctly support capacity/reserve - https://stroika.atlassian.net/browse/STK-573 - not just take.
                 */
                nonvirtual void reserve (size_t newCapacity);

            public:
                /**
                 *  Provide a hint as to how much (contiguous) space to reserve. Like @reserve () but this will not shrink the buffer.
                 *
                 *  @see reserve
                 */
                nonvirtual void reserveAtLeast (size_t newCapacityAtLeast);

            public:
                /**
                 *  Returns the size of the buffer in ELEMENTS (not necessarily in bytes).
                 *
                 *  \ensure GetSize () <= capacity ();
                 */
                nonvirtual size_t GetSize () const;

            public:
                /**
                 *  Despite the name, it is OK if nElements shrinks the list. This really should be better called Resize (), except when shriking we dont want to speicy
                 *  the filled new value, and when enlarging we must one way or the other (either implicitly or otherwise).
                 *
                 *  \ensure GetSize () <= capacity ();
                 */
                nonvirtual void resize (size_t nElements);

                nonvirtual void GrowToSize (size_t nElements)
                {
                    resize (nElements);
                }

            private:
                nonvirtual void reserve_ (size_t nElements);

            public:
                nonvirtual void push_back (const T& e);

#if qDebug
            private:
                static constexpr Byte kGuard1_[8] = {
                    0x45, 0x23, 0x12, 0x56, 0x99, 0x76, 0x12, 0x55,
                };
                static constexpr Byte kGuard2_[8] = {
                    0x15, 0x32, 0xa5, 0x16, 0x11, 0x7a, 0x90, 0x10,
                };
#endif

            private:
                size_t fSize_{};
#if qDebug
                Byte fGuard1_[sizeof (kGuard1_)];
#endif
                alignas (T) alignas (size_t) T fBuffer_[BUF_SIZE]; // alignas both since sometimes accessed as array of T, and sometimes as size_t
#if qDebug
                Byte fGuard2_[sizeof (kGuard2_)];
#endif
                T* fPointer_{};

#if qDebug
            private:
                nonvirtual void ValidateGuards_ ();
#endif
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SmallStackBuffer.inl"

#endif /*_Stroika_Foundation_Memory_SmallStackBuffer_h_*/
