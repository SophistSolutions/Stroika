/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputSubStream_h_
#define _Stroika_Foundation_Streams_InputSubStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"
#include "../Memory/Optional.h"

#include "InputStream.h"
#include "InternallySyncrhonizedInputStream.h"

/**
 *  \file
 *
 */

namespace Stroika {
    namespace Foundation {
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
                class Ptr;

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
                static Ptr New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn, const Memory::Optional<SeekOffsetType>& start, const Memory::Optional<SeekOffsetType>& end);

            private:
                class Rep_;
            };

            /**
             *  Ptr is a copyable smart pointer to a InputSubStream.
             */
            template <typename ELEMENT_TYPE>
            class InputSubStream<ELEMENT_TYPE>::Ptr : public InputStream<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename InputStream<ELEMENT_TYPE>::Ptr;

            public:
                /**
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
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;

            protected:
                Ptr (const shared_ptr<Rep_>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;

            private:
                friend class InputSubStream;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputSubStream.inl"

#endif /*_Stroika_Foundation_Streams_InputSubStream_h_*/
