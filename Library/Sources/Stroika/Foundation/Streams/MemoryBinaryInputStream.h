/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryBinaryInputStream_h_
#define _Stroika_Foundation_Streams_MemoryBinaryInputStream_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Execution/CriticalSection.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "BinaryInputStream.h"
#include    "Seekable.h"


/*
 *  \file
 *
 *      @todo   Create .INL file for 'implemenation details'.
 *
 *      @todo   Re-implemnt using atomics to avoid critical section (cheaper).
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *  \brief  MemoryBinaryInputStream takes a block of binary data and exposes it as a BinaryInputStream (copies)
             *
             *  MemoryBinaryInputStream takes a block of binary data and exposes it as a BinaryInputStream. It copies the
             *  data - so after construction, there is no requirement on the data bounded by start/end.
             *
             *  MemoryBinaryInputStream is threadsafe - meaning Read() can safely be called from
             *  multiple threads at a time freely.
             *
             *  This BinaryInputStream is Seekable.
             *
             *  @see ExternallyOwnedMemoryBinaryInputStream
             */
            class   MemoryBinaryInputStream : public BinaryInputStream {
            private:
                class   IRep_;
            public:
#if     qCompilerAndStdLib_Supports_ConstructorDelegation
                template    <typename IteratorT>
                MemoryBinaryInputStream (IteratorT start, IteratorT end);
                template    <typename ContainerOfT>
                MemoryBinaryInputStream (const ContainerOfT& c);
#else
                MemoryBinaryInputStream (const vector<Byte>& v);
#endif
                MemoryBinaryInputStream (const Byte* start, const Byte* end);
            };

        }
    }
}

#endif  /*_Stroika_Foundation_Streams_MemoryBinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if     qCompilerAndStdLib_Supports_ConstructorDelegation
template    <typename IteratorT>
inline  Stroika::Foundation::Streams::MemoryBinaryInputStream::MemoryBinaryInputStream (IteratorT start, IteratorT end)
    : MemoryBinaryInputStream (&*start, &*end)
{
}
template    <typename ContainerOfT>
inline  Stroika::Foundation::Streams::MemoryBinaryInputStream::MemoryBinaryInputStream (const ContainerOfT& c)
    : MemoryBinaryInputStream (c.begin (), c.end ())
{
}
#endif
