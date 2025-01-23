/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_h_
#define _Stroika_Foundation_Streams_TextReader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/ToText.h"

/**
 *  FILE DEPRECATED - USE Streams/ToText.h
 */

namespace Stroika::Foundation::Streams::TextReader {

    using namespace Streams::ToText;
    using namespace Streams::ToText::Reader;

    using Ptr = InputStream::Ptr<Character>; //not replicated in ToText

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Streams_TextReader_h_*/
