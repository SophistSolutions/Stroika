/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_Utilities_h_
#define _Stroika_Foundation_Streams_iostream_Utilities_h_ 1

#include "../../StroikaPreComp.h"

#include <iostream>

#include "../../Characters/SDKString.h"

/**
 * TODO:
 *      o   While we have a lot of code that is iostream based, and before the Stroika stream stuff matures, many of these
 *          utiltiies maybe handy. I'm not sure we will want them long-term however.
 */

namespace Stroika::Foundation::Streams::iostream {

    using std::byte;

    wstring ReadTextStream (istream& in);
    wstring ReadTextStream (wistream& in);

    vector<byte> ReadBytes (istream& in);
    void         WriteBytes (ostream& out, const vector<byte>& s);

}

#endif /*_Stroika_Foundation_Streams_iostream_Utilities_h_*/
