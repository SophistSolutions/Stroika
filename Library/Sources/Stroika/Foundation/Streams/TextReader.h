/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_h_
#define _Stroika_Foundation_Streams_TextReader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/BinaryToText.h"

/**
 *  FILE DEPRECATED - USE Streams/BinaryToText.h
 */
_DeprecatedFile_ ("DEPRECATED v3.0d15 - use Streams::BinaryToText::Reader");

namespace Stroika::Foundation::Streams::TextReader {

    using namespace Streams::BinaryToText;
    using namespace Streams::BinaryToText::Reader;

    using Ptr = InputStream::Ptr<Character>; //not replicated in BinaryToText

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Streams_TextReader_h_*/
