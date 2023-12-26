/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_ 1

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
 *      @todo   ADD TO DOCUMENTATION AND IMPLEMNENTATION - when CONTAINER (smartptr)
 *              goes out of scope - AUTO-CLOSE the stream (so any future reads just return
 *              EOF, or special 'CLOSED'? Or ASSERT ERROR? DECIDE AND DOCUMENT AND DO.
 */

namespace Stroika::Foundation::Streams::ExternallyOwnedMemoryInputStream {

    using InputStream::Ptr;

    /**
    * 
    * 
    * @todo REPALCE ITERATOR APIS WITH SPAN APSI
    * 
    * 
     *  \brief  ExternallyOwnedMemoryInputStream takes a (memory contiguous) sequence of ELEMENT_TYPE objects and exposes it as a InputStream<ELEMENT_TYPE>
     *
     *  ExternallyOwnedMemoryInputStream is a subtype of InputStream<ELEMENT_TYPE> but the
     *  creator must guarantee, so long as the memory pointed to in the argument has a
     *      o   lifetime > lifetime of the ExternallyOwnedMemoryInputStream object,
     *      o   and data never changes value
     *
     *  This class is threadsafe - meaning Read() can safely be called from multiple threads at a time freely.
     *
     *  NB: Be VERY careful about using this. It can be assigned to a InputStream::Ptr<ELEMENT_TYPE>, and
     *  if any of its constructor arguments are destroyed, it will contain invalid memory references.
     *  Use VERY CAREFULLY. If in doubt, use @MemoryStream<ELEMENT_TYPE> - which is MUCH safer (because it copies its CTOR-argument data)
     *
     *  ExternallyOwnedMemoryInputStream is Seekable.
     *
     *  @see MemoryStream
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */

    /**
     *  \note   The CTOR with random_access_iterator ELEMENT_ITERATOR is safe because you can (always take diff between two
     *          random access iterators and (for now convert to pointers, but that may not be safe????).
     * 
     *  \todo Support RANGES - since thats REALLY what this takes as argument (stl range not Stroika range)
     *
     *  \par Example Usage
     *      \code
     *          InputStream::Ptr<byte> in = ExternallyOwnedMemoryInputStream::New<byte> (begin (buf), begin (buf) + nBytesRead);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          CallExpectingBinaryInputStreamPtr (ExternallyOwnedMemoryInputStream::New<byte> (begin (buf), begin (buf) + nBytesRead))
     *      \endcode
     * 
     *  \note Though generally for these constructors, the pointer types of the arguments must match ELEMENT_TYPE, we have a
     *        few exceptions allowed, for common C++ backward compatability.
     * 
     *          if ELEMENT_TYPE==byte:
     *              allow iterator/pointer of uint8_t
     *              allow iterator/pointer of char
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
    template <typename ELEMENT_TYPE, random_access_iterator ELEMENT_ITERATOR>
    Ptr<ELEMENT_TYPE> New (ELEMENT_ITERATOR start, ELEMENT_ITERATOR end)
        requires is_same_v<typename ELEMENT_ITERATOR::value_type, ELEMENT_TYPE> or
                 (is_same_v<ELEMENT_TYPE, byte> and is_same_v<typename ELEMENT_ITERATOR::value_type, char>);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
    template <typename ELEMENT_TYPE, random_access_iterator ELEMENT_ITERATOR>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, ELEMENT_ITERATOR start, ELEMENT_ITERATOR end)
        requires is_same_v<typename ELEMENT_ITERATOR::value_type, ELEMENT_TYPE> or
                 (is_same_v<ELEMENT_TYPE, byte> and is_same_v<typename ELEMENT_ITERATOR::value_type, char>);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const uint8_t* start, const uint8_t* end)
        requires is_same_v<ELEMENT_TYPE, byte>;

    template <typename ELEMENT_TYPE>
    class Rep_;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ExternallyOwnedMemoryInputStream.inl"

#endif /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_*/
