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
 * TODO:
 *      @todo   Re-implemnt using atomics to avoid critical section (cheaper).
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *
             * This class is threadsafe - meaning _Read() can safely be called from multiple threads at a time freely.
             */
            class   MemoryBinaryInputStream : public BinaryInputStream {
            private:
                class   IRep_;
            public:
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
