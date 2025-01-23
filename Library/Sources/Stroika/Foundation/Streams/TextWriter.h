/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_h_
#define _Stroika_Foundation_Streams_TextWriter_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/FromText.h"

/**
 *  FILE DEPRECATED SINCE STROIKA v3.0d15 - use Streams::FromText
 */
_DeprecatedFile_ ("DEPRECATED v3.0d15 - instead of TextWriter - use FromText::Writer");

namespace Stroika::Foundation::Streams::TextWriter {

    using namespace Streams::FromText;
    using namespace Streams::FromText::Writer;

    using Ptr = OutputStream::Ptr<Character>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Streams_TextWriter_h_*/
