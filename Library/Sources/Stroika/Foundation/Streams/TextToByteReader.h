/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextToByteReader_h_
#define _Stroika_Foundation_Streams_TextToByteReader_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Characters/UTFConvert.h"

#include "InputStream.h"
#include "InternallySynchronizedInputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Streams::TextToByteReader {

    using Characters::Character;

    /**
     *  \brief Stream wrapper that takes an InputStream<Character> and transforms it into an
     *         InputStream<byte> (like TextWriter does, but pull rather than push based).
     * 
     *  Draft implementation (not very performant, but doesnt seem used much and easy to tweak)
     * 
     * DOC CONNECTION TO TextWriter and maybe share output/format flags?
     * 
     * @todo NOTE - this CURRENTLY HARDWIRES converting to UTF-8
     * 
     *      @todo take optional CodeCvt argument, or things you would pass to CodeCvt (character coing) for what binary rep to create!
     * 
     *      WONT change this part of the API - just adding overloads, so OK to release as-is - 2023-07-10
     * 
     *      @todo this also should take Iterable<Character> and therefore String...
     */
    using Ptr = InputStream<byte>::Ptr;

    /**
         */
    Ptr New (const InputStream<Character>::Ptr& srcStream);
    Ptr New (const Traversal::Iterable<Character>& srcText);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextToByteReader.inl"

#endif /*_Stroika_Foundation_Streams_TextToByteReader_h_*/
