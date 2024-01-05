/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ToSeekableInputStream_h_
#define _Stroika_Foundation_Streams_ToSeekableInputStream_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/Iterable.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

namespace Stroika::Foundation::Streams::ToSeekableInputStream {

    using InputStream::Ptr;

    /**
     *  Not all input streams are seekable, but throwing a little memory at it, its easy to assure they are all
     *  seekable.
     * 
     *  That's what this utility does: maps the given input stream into a functionally identical one, except possibly
     *  adding seekability.
     * 
     *  \par Example Usage
     *      \code
     *          InputStream::Ptr<Character> in = ToSeekableInputStream::New<Character> (existingInputStream...);
     *      \endcode
     * 
     *  \note this helper does not require it be given the input stream at SeekOffset 0, but for pretty obvious reasons
     *        it cannot produce a stream that permits seeking backwards from where it starts. This is checked via assertions.
     */
    template <typename ELEMENT_TYPE>
    auto New (const Ptr<ELEMENT_TYPE>& in) -> Ptr<ELEMENT_TYPE>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ToSeekableInputStream.inl"

#endif /*_Stroika_Foundation_Streams_ToSeekableInputStream_h_*/