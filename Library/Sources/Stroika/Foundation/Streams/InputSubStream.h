/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputSubStream_h_
#define _Stroika_Foundation_Streams_InputSubStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>
#include <optional>

#include "../Configuration/Common.h"

#include "InputStream.h"
#include "InternallySynchronizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::Streams::InputSubStream {

    /**
     *  @brief  InputSubStream is an InputStream<ELEMENT_TYPE>::Ptr which provides buffered access.
     *          This is useful if calls to the underling stream source can be expensive. This class
     *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
     *
     *  \note Execution::InternallySynchronized not supported cuz cannot externally synchronize withot a lock around multiple operations.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    template <typename ELEMENT_TYPE>
    using Ptr = typename InputStream<ELEMENT_TYPE>::Ptr;

    /**
     *  start and end are offsets in the real underlying stream which map to 0, and if specified, end-start (which is this streams ends);
     *  start defaults (if Missing) to the current offset (not necessarily zero for that stream), and end defaults to the actual end of the underlying stream.
     *
     *  \par Example Usage
     *      \code
     *          InputStream<byte>::Ptr in = InputSubStream<byte>::New (fromStream, 0, contentLength);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          CallExpectingBinaryInputStreamPtr (InputSubStream<byte>::New (fromStream, 0, contentLength))
     *      \endcode
     */
    template <typename ELEMENT_TYPE>
    auto New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn, const optional<SeekOffsetType>& start, const optional<SeekOffsetType>& end)
        -> Ptr<ELEMENT_TYPE>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputSubStream.inl"

#endif /*_Stroika_Foundation_Streams_InputSubStream_h_*/
