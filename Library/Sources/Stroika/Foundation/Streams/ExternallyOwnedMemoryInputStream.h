/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <vector>

#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputStream.h"
#include "InternallySyncrhonizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   ADD TO DOCUMENTATION AND IMPLEMNENTATION - when CONTAINER (smartptr)
 *              goes out of scope - AUTO-CLOSE the stream (so any future reads just return
 *              EOF, or special 'CLOSED'? Or ASSERT ERROR? DECIDE AND DOCUMENT AND DO.
 */

namespace Stroika::Foundation {
    namespace Streams {

        /**
         *  \brief  ExternallyOwnedMemoryInputStream takes a sequence of ELEMENT_TYPE and exposes it as a InputStream<ELEMENT_TYPE>
         *
         *  ExternallyOwnedMemoryInputStream is a subtype of InputStream<ELEMENT_TYPE> but the
         *  creator must gaurantee, so long as the memory pointed to in the argument has a
         *      o   lifetime > lifetime of the ExternallyOwnedMemoryInputStream object,
         *      o   and data never changes value
         *
         *  This class is threadsafe - meaning Read() can safely be called from multiple threads at a time freely.
         *
         *  NB: Be VERY careful about using this. It can be assigned to a InputStream<ELEMENT_TYPE>::Ptr, and
         *  if any of its constructor arguments are destroyed, it will contain invalid memory references.
         *  Use VERY CAREFULLY. If in doubt, use @MemoryStream<ELEMENT_TYPE> - which is MUCH safer.
         *
         *  ExternallyOwnedMemoryInputStream is Seekable.
         *
         *  @see MemoryStream
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
         */
        template <typename ELEMENT_TYPE>
        class ExternallyOwnedMemoryInputStream : public InputStream<ELEMENT_TYPE> {

        private:
            using inherited = InputStream<ELEMENT_TYPE>;

        public:
            ExternallyOwnedMemoryInputStream ()                                        = delete;
            ExternallyOwnedMemoryInputStream (const ExternallyOwnedMemoryInputStream&) = delete;

        public:
            /**
             */
            using typename inherited::Ptr;

        public:
            /**
             *  \note   The CTOR with ELEMENT_RANDOM_ACCESS_ITERATOR is safe because you can (always take diff between two
             *          random access iterators and (for now convert to pointers, but that may not be safe????).
             *
             *  \par Example Usage
             *      \code
             *          InputStream<Byte>::Ptr in = ExternallyOwnedMemoryInputStream<Byte>::New (begin (buf), begin (buf) + nBytesRead);
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          CallExpectingBinaryInputStreamPtr (ExternallyOwnedMemoryInputStream<Byte>::New (begin (buf), begin (buf) + nBytesRead))
             *      \endcode
             */
            static Ptr New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
            template <typename ELEMENT_RANDOM_ACCESS_ITERATOR>
            static Ptr New (ELEMENT_RANDOM_ACCESS_ITERATOR start, ELEMENT_RANDOM_ACCESS_ITERATOR end);
            static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
            template <typename ELEMENT_RANDOM_ACCESS_ITERATOR>
            static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, ELEMENT_RANDOM_ACCESS_ITERATOR start, ELEMENT_RANDOM_ACCESS_ITERATOR end);

        private:
            class Rep_;

        private:
            using InternalSyncRep_ = InternallySyncrhonizedInputStream<ELEMENT_TYPE, Streams::ExternallyOwnedMemoryInputStream, typename ExternallyOwnedMemoryInputStream<ELEMENT_TYPE>::Rep_>;
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ExternallyOwnedMemoryInputStream.inl"

#endif /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_*/
