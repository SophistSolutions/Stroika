/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"

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
    : inherited{"Database error: {}"_f(message)}
{
}

/*
 ********************************************************************************
 ************************* ODBCSupport::NoDataException *************************
 ********************************************************************************
 */
Database::NoDataException::NoDataException ()
    : Exception{"No Data"sv}
{
}
