/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_Utilities_h_
#define _Stroika_Foundation_Streams_iostream_Utilities_h_ 1

#include "../../StroikaPreComp.h"

#include <iostream>

#include "../../Characters/SDKString.h"

/**
*   FILE DEPRECATED IN STROIKA v3.0d2
 */

namespace Stroika::Foundation::Streams::iostream {

    [[deprecated ("Since Stroika v3.0d2, use Foundation::InputStream")]] wstring ReadTextStream (istream& in);
    [[deprecated ("Since Stroika v3.0d2, use Foundation::InputStream")]] wstring ReadTextStream (wistream& in);

    [[deprecated ("Since Stroika v3.0d2, use Foundation::InputStream")]] vector<byte> ReadBytes (istream& in);
    [[deprecated ("Since Stroika v3.0d2, use Foundation::OutputStream")]] void        WriteBytes (ostream& out, const vector<byte>& s);

}

#endif /*_Stroika_Foundation_Streams_iostream_Utilities_h_*/
