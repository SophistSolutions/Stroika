/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <vector>

#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *      @todo   ADD TO DOCUMENTATION AND IMPLEMNENTATION - when Ptr
 *              goes out of scope - AUTO-CLOSE the stream (so any future reads just return
 *              EOF, or special 'CLOSED'? Or ASSERT ERROR? DECIDE AND DOCUMENT AND DO.
 * 
 *              But - then must be very careful about MOVES of Ptr instead of copies, so not sure a good idea.
 */

namespace Stroika::Foundation::Streams::ExternallyOwnedSpanInputStream {

    using InputStream::Ptr;

    /**
     *  \brief  ExternallyOwnedSpanInputStream takes a (memory contiguous) sequence of ELEMENT_TYPE objects and exposes it as a InputStream<ELEMENT_TYPE>
     *
     *  ExternallyOwnedSpanInputStream is a subtype of InputStream<ELEMENT_TYPE> but the
     *  creator must guarantee, so long as the memory pointed to in the argument has a
     *      o   lifetime > lifetime of the ExternallyOwnedSpanInputStream object,
     *      o   and data never changes value
     *
     *  \note NB: Be VERY careful about using this. It can be assigned to a InputStream::Ptr<ELEMENT_TYPE>, and
     *        if its constructor argument is destroyed, it will contain invalid memory references.
     *        Use VERY CAREFULLY. If in doubt, use @MemoryStream<ELEMENT_TYPE> - which is MUCH safer (because it copies its CTOR-argument data)
     *
     *  ExternallyOwnedSpanInputStream is Seekable.
     *
     *  @see MemoryStream
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *
     *  \par Example Usage
     *      \code
     *          InputStream::Ptr<byte> in = ExternallyOwnedSpanInputStream::New<byte> (span{buf});
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          CallExpectingBinaryInputStreamPtr (ExternallyOwnedSpanInputStream::New<byte> (span{buf})
     *      \endcode
     * 
     *  \note Though generally for these constructors, the pointer types of the arguments must match ELEMENT_TYPE, we have a
     *        few exceptions allowed, for common C++ backward compatibility.
     * 
     *          if ELEMENT_TYPE==byte:
     *              allow iterator/pointer of uint8_t
     *              allow iterator/pointer of char
     */
    template <typename ELEMENT_TYPE, typename ELEMENT_TYPE2, size_t EXTENT2>
    Ptr<ELEMENT_TYPE> New (span<ELEMENT_TYPE2, EXTENT2> s)
        requires (same_as<ELEMENT_TYPE, remove_cvref_t<ELEMENT_TYPE2>> or
                  (same_as<ELEMENT_TYPE, byte> and (same_as<remove_cvref_t<ELEMENT_TYPE2>, char> or same_as<remove_cvref_t<ELEMENT_TYPE2>, uint8_t>)));
    template <typename ELEMENT_TYPE, typename ELEMENT_TYPE2, size_t EXTENT2>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, span<ELEMENT_TYPE2, EXTENT2> s)
        requires (same_as<ELEMENT_TYPE, remove_cvref_t<ELEMENT_TYPE2>> or
                  (same_as<ELEMENT_TYPE, byte> and (same_as<remove_cvref_t<ELEMENT_TYPE2>, char> or same_as<remove_cvref_t<ELEMENT_TYPE2>, uint8_t>)));

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ExternallyOwnedSpanInputStream.inl"

#endif /*_Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_h_*/
