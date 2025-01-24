/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_h_
#define _Stroika_Foundation_Streams_TextWriter_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/TextToBinary.h"

/**
 *  FILE DEPRECATED SINCE STROIKA v3.0d15 - use Streams::TextToBinary
 */
_DeprecatedFile_ ("DEPRECATED v3.0d15 - instead of TextWriter - use TextToBinary::Writer");

namespace Stroika::Foundation::Streams::TextWriter {

    using namespace Streams::TextToBinary;
    using namespace Streams::TextToBinary::Writer;

    using Ptr = OutputStream::Ptr<Character>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Streams_TextWriter_h_*/
