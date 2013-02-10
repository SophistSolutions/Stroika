/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"

#include    "BinaryInputStream.h"
#include    "Seekable.h"


/**
 *  \file
 *
 *      @todo   ADD TO DOCUMENTATION AND IMPLEMNENTATION - when CONTAINER (smartptr)
 *              goes out of scope - AUTO-CLOSE the stream (so any future reads just return
 *              EOF, or special 'CLOSED'? Or ASSERT ERROR? DECIDE AND DOCUMENT AND DO.
 *
 *      @todo   Re-implemnt using atomics to avoid critical section (cheaper).
 *
 *      @todo   qCompilerAndStdLib_Supports_ConstructorDelegation - do like BasicBinaryInputStream constructors
 *              and replace vector<Byte> ctor
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  ExternallyOwnedMemoryBinaryInputStream takes a block of binary data
             *          and exposes it as a BinaryInputStream (references)
             *
             *  ExternallyOwnedMemoryBinaryInputStream is a subtype of BinaryInputStream but the
             *  creator must gaurantee, so long as the memory pointed to in the argument has a
             *      o   lifetime > lifetime of the ExternallyOwnedMemoryBinaryInputStream object,
             *      o   and data never changes value
             *
             *  This class is threadsafe - meaning Read() can safely be called from multiple threads at a time freely.
             *
             *  NB: Be VERY careful about using this. It can be assigned to a BinaryInputStream pointer, and
             *  if any of its constructor arguments are destroyed, it will contain invalid memory references.
             *  Use VERY CAREFULLY. If in doubt, use BasicBinaryInputStream - which is MUCH safer.
             *
             *  ExternallyOwnedMemoryBinaryInputStream is Seekable.
             *
             *  @see BasicBinaryInputStream
             */
            class   ExternallyOwnedMemoryBinaryInputStream : public BinaryInputStream {
            private:
                class   IRep_;
            public:
                ExternallyOwnedMemoryBinaryInputStream (const vector<Byte>& v);
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
