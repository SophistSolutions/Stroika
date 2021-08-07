/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"

#include "Exception.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;

/*
 ********************************************************************************
 **************************** ODBCSupport::Exception ****************************
 ********************************************************************************
 */
Database::Exception::Exception (const String& message)
    : inherited{Format (L"Database error: %s", message.c_str ())}
{
}

/*
 ********************************************************************************
 ************************* ODBCSupport::NoDataException *************************
 ********************************************************************************
 */
Database::NoDataException::NoDataException ()
    : Exception{L"No Data"sv}
{
}
