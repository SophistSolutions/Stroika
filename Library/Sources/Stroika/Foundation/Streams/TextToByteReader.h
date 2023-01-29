/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

namespace Stroika::Foundation::Streams {

    using Characters::Character;


    class TextToByteReader : public InputStream<std::byte> {
    public:
        TextToByteReader ()            = delete;
        TextToByteReader (const TextToByteReader&) = delete;

    public:
        using Ptr = InputStream<std::byte>::Ptr;

   

    public:
       
        static Ptr New (InputStream<Character>::Ptr srcStream);


  
    };



}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextToByteReader.inl"

#endif /*_Stroika_Foundation_Streams_TextToByteReader_h_*/
