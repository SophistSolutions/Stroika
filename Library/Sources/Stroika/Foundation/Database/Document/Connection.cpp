/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Database/Document/Transaction.h"

#include "Connection.h"

using namespace Stroika::Foundation;

using namespace Database;
using namespace Database::Document;

/*
 ********************************************************************************
 ************************* Document::Connection::Ptr ****************************
 ********************************************************************************
 */
Transaction Connection::Ptr::mkTransaction ()
{
    return get ()->mkTransaction ();
}
