/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_IterableToInputStream_h_
#define _Stroika_Foundation_Streams_IterableToInputStream_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/Iterable.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::Streams::IterableToInputStream {

    /**
     */
    template <typename ELEMENT_TYPE>
    using Ptr = typename InputStream<ELEMENT_TYPE>::Ptr;

    /**
     *  Sometimes, convenient to take an object which is Iterable, and treat it as an InputStream<> (similar but not identical concepts).
     * 
     *  For example, this can be used to treat a String as an InputStream<Character>.
     * 
     *  \par Example Usage
     *      \code
     *          InputStream<Character>::Ptr in = IterableToInputStream::New<Character> (String{"hi"});
     *      \endcode
     */
    template <typename ELEMENT_TYPE>
    auto New (const Traversal::Iterable<ELEMENT_TYPE>& it) -> Ptr<ELEMENT_TYPE>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IterableToInputStream.inl"

#endif /*_Stroika_Foundation_Streams_IterableToInputStream_h_*/