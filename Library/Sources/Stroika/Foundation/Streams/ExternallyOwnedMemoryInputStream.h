/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/BLOB.h"

#include    "InputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   ADD TO DOCUMENTATION AND IMPLEMNENTATION - when CONTAINER (smartptr)
 *              goes out of scope - AUTO-CLOSE the stream (so any future reads just return
 *              EOF, or special 'CLOSED'? Or ASSERT ERROR? DECIDE AND DOCUMENT AND DO.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  ExternallyOwnedMemoryInputStream takes a block of binary data
             *          and exposes it as a BinaryInputStream (references)
             *
             *  ExternallyOwnedMemoryInputStream is a subtype of BinaryInputStream but the
             *  creator must gaurantee, so long as the memory pointed to in the argument has a
             *      o   lifetime > lifetime of the ExternallyOwnedMemoryInputStream object,
             *      o   and data never changes value
             *
             *  This class is threadsafe - meaning Read() can safely be called from multiple threads at a time freely.
             *
             *  NB: Be VERY careful about using this. It can be assigned to a BinaryInputStream pointer, and
             *  if any of its constructor arguments are destroyed, it will contain invalid memory references.
             *  Use VERY CAREFULLY. If in doubt, use BasicBinaryInputStream - which is MUCH safer.
             *
             *  ExternallyOwnedMemoryInputStream is Seekable.
             *
             *  @see MemoryStream
             */
            template    <typename ELEMENT_TYPE>
            class   ExternallyOwnedMemoryInputStream : public InputStream<ELEMENT_TYPE> {
            private:
                using   inherited = InputStream<ELEMENT_TYPE>;

            public:
                /**
                 *  \note   The CTOR with ELEMENT_RANDOM_ACCESS_ITERATOR is safe because you can (always take diff between two
                 *          random access iterators and (for now convert to pointers, but that may not be safe????).
                 */
                ExternallyOwnedMemoryInputStream (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
                template    <typename   ELEMENT_RANDOM_ACCESS_ITERATOR>
                ExternallyOwnedMemoryInputStream (ELEMENT_RANDOM_ACCESS_ITERATOR start, ELEMENT_RANDOM_ACCESS_ITERATOR end);

            private:
                class   Rep_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ExternallyOwnedMemoryInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_*/
