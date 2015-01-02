/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryInputStream_h_
#define _Stroika_Foundation_Streams_BasicBinaryInputStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "BinaryInputStream.h"
#include    "Seekable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *      @todo   Should add Close () method. Any subsequent calls to this stream - would fail?
 *
 *              (maybe close ignored).
 *
 *              If we allow for that - we may need to have check method - isOpen?. So maybe best to
 *              have flush/close allowed, and anything else generate an assert error?
 *
 *      @todo   Re-implement using atomics to avoid critical section (cheaper).
 *
 *      @todo   #if  0 BasicBinaryInputStream CTOR -- see why not working on gcc 4.7.2
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  Simplest to use BinaryInputStream; BasicBinaryInputStream takes a block of binary data and exposes it as a
             *          BinaryInputStream (copies)
             *
             *  BasicBinaryInputStream takes a block of binary data and exposes it as a BinaryInputStream.
             *  It copies the data - so after construction, there is no requirement on the data bounded by
             *  start/end.
             *
             *  BasicBinaryInputStream is threadsafe - meaning Read() can safely be called from
             *  multiple threads at a time freely.
             *
             *  BasicBinaryInputStream is Seekable.
             *
             *  @see ExternallyOwnedBasicBinaryInputStream
             */
            class   BasicBinaryInputStream : public BinaryInputStream {
            private:
                class   IRep_;
            public:
#if     0
                template    <typename RandomAccessIteratorT>
                BasicBinaryInputStream (RandomAccessIteratorT start, RandomAccessIteratorT end);
                template    <typename ContainerOfT>
                BasicBinaryInputStream (const ContainerOfT& c);
#else
                BasicBinaryInputStream (const vector<Byte>& v);
#endif
                BasicBinaryInputStream (const Memory::BLOB& blob);
                BasicBinaryInputStream (const Byte* start, const Byte* end);
#if 0
                template    <typename ITERATOR_TYPE>
                BasicBinaryInputStream (const ITERATOR_TYPE& start, const ITERATOR_TYPE& end);
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
#include    "BasicBinaryInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BasicBinaryInputStream_h_*/
