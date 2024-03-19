/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ToSeekableInputStream_h_
#define _Stroika_Foundation_Streams_ToSeekableInputStream_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/Iterable.h"

#include "InputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
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
     *  If the argument stream is already seekable, New () just returns its argument (so perhaps a misnomer but I thought better
     *  to follow factory pattern).
     * 
     *  \par Example Usage
     *      \code
     *          InputStream::Ptr<Character> in = ToSeekableInputStream::New<Character> (existingInputStream);
     *      \endcode
     * 
     *  \note this helper does not require it be given the input stream at SeekOffset 0, but for pretty obvious reasons
     *        it cannot produce a stream that permits seeking backwards from where it starts. This is checked via assertions.
     * 
     *  \note this helper may be problematic with very large streams, as it caches the stream in memory as it reads, and would
     *        eventually run out.
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
