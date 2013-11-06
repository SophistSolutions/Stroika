/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SmallStackBuffer_h_
#define _Stroika_Foundation_Memory_SmallStackBuffer_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Support more flexible CTOR - like SmallStackBuffer (const SmallStackBuffer<T, BUF_SIZE>& from)
*               using different SIZE value. At same time - same for operator=
 *
 *  Long-Term TOD:
 *      @todo   Support non-POD type 'T' values properly.
 *              IE Lose \req std::is_trivially_constructible<T>::value,
 *              std::is_trivially_destructible<T>::value,
 *              std::is_trivially_copyable<T>::value
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


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
             */
            template    < typename   T, size_t BUF_SIZE = ((4096 / sizeof(T)) == 0 ? 1 : (4096 / sizeof(T))) >
            class   SmallStackBuffer {
            public :
                explicit SmallStackBuffer (size_t nElements);
                SmallStackBuffer (const SmallStackBuffer<T, BUF_SIZE>& from);
                SmallStackBuffer () = delete;
                ~SmallStackBuffer ();

            public:
                nonvirtual  SmallStackBuffer<T, BUF_SIZE>&   operator= (const SmallStackBuffer<T, BUF_SIZE>& rhs);

            public:
                /**
                 */
                nonvirtual  operator const T* () const;
                nonvirtual  operator T* ();

            public:
                typedef T*          iterator;
                typedef const T*    const_iterator;

            public:
                /**
                 */
                nonvirtual  iterator        begin ();
                nonvirtual  const_iterator  begin () const;

            public:
                /**
                 */
                nonvirtual  iterator        end ();
                nonvirtual  const_iterator  end () const;

            public:
                /**
                 *  Returns the size of the buffer in ELEMENTS (not necessarily in bytes)
                 */
                nonvirtual  size_t  GetSize () const;

            public:
                /**
                 *  Despite the name, it is OK if nElements shrinks the list. This really should be better called Resize ()
                 */
                nonvirtual  void    GrowToSize (size_t nElements);

            private:
                nonvirtual  void    GrowToSize_ (size_t nElements);

            public:
                nonvirtual  void    push_back (const T& e);

            private:
                size_t  fSize_;
                T       fBuffer_[BUF_SIZE];
                T*      fPointer_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SmallStackBuffer.inl"

#endif  /*_Stroika_Foundation_Memory_SmallStackBuffer_h_*/
