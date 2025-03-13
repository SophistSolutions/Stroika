/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Transaction.h"

#include "Connection.h"

using namespace Stroika::Foundation;

using namespace Database;
using namespace Database::Document;

/*
 ********************************************************************************
 ****************************** SQL::Connection::Ptr ****************************
 ********************************************************************************
 */
Transaction Connection::Ptr::mkTransaction ()
{
    return _fRep->mkTransaction ();
}
