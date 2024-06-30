/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_h_
#define _Stroika_Foundation_Streams_BufferedInputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   BufferedInputStream::IRep_ IMPLEMENTATION NEEDED. It does no buffering!
 *
 *      @todo   Seekable! Must think out if this should mixin Seekable or not. I THINK it must be.
 *              I THINK if one SEEKS this, but the underlying
 *              class doesn't support seeking, we MAY need to either BUFFER MORE, or throw not supported.
 *
 *      @todo   Add tuning parameters, such as buffer size
 */

namespace Stroika::Foundation::Streams::BufferedInputStream {

    using InputStream::Ptr;

    /**
     *  @brief  BufferedInputStream is an InputStream::Ptr<ELEMENT_TYPE> which provides buffered access.
     *          This is useful if calls to the underling stream source can be expensive. This class
     *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
     *
     *  \par Example Usage
     *      \code
     *          InputStream::Ptr<byte> in = BufferedInputStream::New<byte> (fromStream);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          CallExpectingBinaryInputStreamPtr (BufferedInputStream::New<byte> (fromStream))
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename InputStream::Ptr<ELEMENT_TYPE>& realIn);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BufferedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_BufferedInputStream_h_*/
