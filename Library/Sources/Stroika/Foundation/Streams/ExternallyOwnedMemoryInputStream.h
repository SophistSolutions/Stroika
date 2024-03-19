/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>
#include <vector>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "ExternallyOwnedSpanInputStream.h"

/**
 *  \file
 */

namespace Stroika::Foundation::Streams::ExternallyOwnedMemoryInputStream {

    _DeprecatedFile_ ("DEPRECATED FILE DEPRECATED SINCE STROIKA v3.0d5 - use ExternallyOwnedSpanInputStream");
    using InputStream::Ptr;

    template <typename ELEMENT_TYPE>
    [[deprecated ("Since Stroika v3.0d5 use ExternallyOwnedSpanInputStream")]] Ptr<ELEMENT_TYPE> New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
    {
        return ExternallyOwnedSpanInputStream::New<ELEMENT_TYPE> (span{start, end});
    }
    template <typename ELEMENT_TYPE, random_access_iterator ELEMENT_ITERATOR>
    [[deprecated ("Since Stroika v3.0d5 use ExternallyOwnedSpanInputStream")]] Ptr<ELEMENT_TYPE> New (ELEMENT_ITERATOR start, ELEMENT_ITERATOR end)
        requires is_same_v<typename ELEMENT_ITERATOR::value_type, ELEMENT_TYPE> or
                 (is_same_v<ELEMENT_TYPE, byte> and is_same_v<typename ELEMENT_ITERATOR::value_type, char>)
    {
        return ExternallyOwnedSpanInputStream::New<ELEMENT_TYPE> (span{start, end});
    }
    template <typename ELEMENT_TYPE>
    [[deprecated ("Since Stroika v3.0d5 use ExternallyOwnedSpanInputStream")]] Ptr<ELEMENT_TYPE>
    New (Execution::InternallySynchronized internallySynchronized, const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
    {
        return ExternallyOwnedSpanInputStream::New<ELEMENT_TYPE> (span{start, end - start});
    }
    template <typename ELEMENT_TYPE, random_access_iterator ELEMENT_ITERATOR>
    [[deprecated ("Since Stroika v3.0d5 use ExternallyOwnedSpanInputStream")]] Ptr<ELEMENT_TYPE>
    New (Execution::InternallySynchronized internallySynchronized, ELEMENT_ITERATOR start, ELEMENT_ITERATOR end)
        requires is_same_v<typename ELEMENT_ITERATOR::value_type, ELEMENT_TYPE> or
                 (is_same_v<ELEMENT_TYPE, byte> and is_same_v<typename ELEMENT_ITERATOR::value_type, char>)
    {
        return ExternallyOwnedSpanInputStream::New<ELEMENT_TYPE> (span{start, end - start});
    }
    template <typename ELEMENT_TYPE>
    [[deprecated ("Since Stroika v3.0d5 use ExternallyOwnedSpanInputStream")]] Ptr<ELEMENT_TYPE> New (const uint8_t* start, const uint8_t* end)
        requires is_same_v<ELEMENT_TYPE, byte>
    {
        return ExternallyOwnedSpanInputStream::New<ELEMENT_TYPE> (span{start, end});
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryInputStream_h_*/
