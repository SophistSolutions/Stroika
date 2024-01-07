/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "Statement.h"
#include "Transaction.h"

#include "Connection.h"

using namespace Stroika::Foundation;

using namespace Database;
using namespace Database::SQL;

/*
 ********************************************************************************
 ****************************** SQL::Connection::Ptr ****************************
 ********************************************************************************
 */
Statement Connection::Ptr::mkStatement (const String& sql)
{
    return _fRep->mkStatement (sql);
}

Transaction Connection::Ptr::mkTransaction ()
{
    return _fRep->mkTransaction ();
}
