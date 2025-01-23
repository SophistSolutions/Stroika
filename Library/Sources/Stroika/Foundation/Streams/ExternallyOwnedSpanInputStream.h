/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>
#include <vector>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/Common.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
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
     *  \par Example Usage
     *      \code
     *          const char kJSONExample_[] = "{"...;
     *          // JSON Reader takes InputStream::Ptr<byte> argument
     *          auto reader = DataExchange::Variant::JSON::Reader{};
     *          VariantValue v1 = reader.Read (Streams::ExternallyOwnedSpanInputStream::New (span{kJSONExample_}));
     *          VariantValue vSameAs = reader.Read (Streams::ExternallyOwnedSpanInputStream::New (Memory::SpanBytesCast<span<const byte>>(span{kJSONExample_})));
     *      \endcode
     */
    template <typename ELEMENT_TYPE, Memory::ISpanBytesCastable<span<const ELEMENT_TYPE>> FROM_SPAN>
    Ptr<ELEMENT_TYPE> New (FROM_SPAN s);
    template <typename ELEMENT_TYPE, Memory::ISpanBytesCastable<span<const ELEMENT_TYPE>> FROM_SPAN>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, FROM_SPAN s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ExternallyOwnedSpanInputStream.inl"

#endif /*_Stroika_Foundation_Streams_ExternallyOwnedSpanInputStream_h_*/
