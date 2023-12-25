/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_h_
#define _Stroika_Foundation_Streams_BufferedInputStream_h_ 1

#include "../StroikaPreComp.h"

#include <memory>

#include "../Configuration/Common.h"

#include "InputStream.h"
#include "InternallySynchronizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   BufferedInputStream::IRep_ IMPLEMENTATION NEEDED. It does no buffering!
 *
 *      @todo   Seekable! Must think out if this should mixin Seekable or not. I THINK it must be.
 *              I THINK if one SEEKS this, but the underlying
 *              class doesn't support seeking, we MAY need to either BUFFER MORE, or throw not supported.
 *
 *      @todo   Add tuning parameters, such as buffer size
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-608 - probbaly be made more efficent in sycn form - using direct mutex
 */

namespace Stroika::Foundation::Streams::BufferedInputStream {

    /**
     *  @brief  BufferedInputStream is an InputStream<ELEMENT_TYPE>::Ptr which provides buffered access.
     *          This is useful if calls to the underling stream source can be expensive. This class
     *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    template <typename ELEMENT_TYPE>
    using Ptr = typename InputStream<ELEMENT_TYPE>::Ptr;

    /**
         *  \par Example Usage
         *      \code
         *          InputStream<byte>::Ptr in = BufferedInputStream::New<byte> (fromStream);
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          CallExpectingBinaryInputStreamPtr (BufferedInputStream::New<byte> (fromStream))
         *      \endcode
         */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename InputStream<ELEMENT_TYPE>::Ptr& realIn);

    template <typename ELEMENT_TYPE>
    class Rep_;

#if 0
    template < typename ELEMENT_TYPE>
        using InternalSyncRep_ =
            InternallySynchronizedInputStream<ELEMENT_TYPE, Streams::BufferedInputStream<ELEMENT_TYPE>, typename BufferedInputStream<ELEMENT_TYPE>::Rep_>;
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BufferedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_BufferedInputStream_h_*/
