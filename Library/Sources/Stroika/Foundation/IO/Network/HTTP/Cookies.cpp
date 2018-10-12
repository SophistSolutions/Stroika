/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/CString/Utilities.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Execution/Exceptions.h"

#include "Cookies.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

/*
********************************************************************************
************************************ HTTP::Cookie ******************************
********************************************************************************
*/
String Cookie::Encode () const
{
    AssertNotImplemented ();

    return String ();
}

Cookie Cookie::Decode (const String& src)
{
    AssertNotImplemented ();
    return Cookie ();
}
