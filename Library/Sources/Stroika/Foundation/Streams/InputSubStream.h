/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputSubStream_h_
#define _Stroika_Foundation_Streams_InputSubStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>
#include <optional>

#include "../Configuration/Common.h"

#include "InputStream.h"
#include "InternallySyncrhonizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation {
    namespace Streams {

        /**
         *  @brief  InputSubStream is an InputStream<ELEMENT_TYPE>::Ptr which provides buffered access.
         *          This is useful if calls to the underling stream source can be expensive. This class
         *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
         *
         *  \note Execution::InternallySyncrhonized not supported cuz cannot externally synchronize withot a lock around multiple operations.
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
         */
        template <typename ELEMENT_TYPE>
        class InputSubStream : public InputStream<ELEMENT_TYPE> {
        public:
            InputSubStream ()                      = delete;
            InputSubStream (const InputSubStream&) = delete;

        public:
            using typename InputStream<ELEMENT_TYPE>::Ptr;

        public:
            /**
             *  start and end are offsets in the real underlying stream which map to 0, and if specified, end-start (which is this streams ends);
             *  start defaults (if Missing) to the current offset, and end defaults to the actual end of the underlying stream.
             *
             *  \par Example Usage
             *      \code
             *          InputStream<Byte>::Ptr in = InputSubStream<Byte>::New (fromStream, 0, contentLength);
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          CallExpectingBinaryInputStreamPtr (InputSubStream<Byte>::New (fromStream, 0, contentLength))
             *      \endcode
             */
            static Ptr New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn, const optional<SeekOffsetType>& start, const optional<SeekOffsetType>& end);

        private:
            class Rep_;
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputSubStream.inl"

#endif /*_Stroika_Foundation_Streams_InputSubStream_h_*/
