/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Execution/CriticalSection.h"

#include    "BinaryInputStream.h"
#include    "Seekable.h"


/**
 *  \file
 *      @todo   Re-implemnt using atomics to avoid critical section (cheaper).
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /**
             *  \brief  ExternallyOwnedMemoryBinaryInputStream takes a block of binary data and exposes it as a BinaryInputStream (references)
             *
             *  ExternallyOwnedMemoryBinaryInputStream is a subtype of BinaryInputStream but the
             *  creator must gaurantee, so long as the memory pointed to in the argument has a
             *      o   lifetime > lifetime of the ExternallyOwnedMemoryBinaryInputStream object,
             *      o   and data never changes value
             *
             *  This class is threadsafe - meaning Read() can safely be called from multiple threads at a time freely.
             *
             *  ExternallyOwnedMemoryBinaryInputStream is Seekable.
             */
            class   ExternallyOwnedMemoryBinaryInputStream : public BinaryInputStream {
            private:
                class   IRep_;
            public:
                ExternallyOwnedMemoryBinaryInputStream (const Byte* start, const Byte* end);
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
