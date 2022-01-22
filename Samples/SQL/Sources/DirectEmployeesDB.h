/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Samples_SQL_DirectEmployeesDB_h_
#define _Samples_SQL_DirectEmployeesDB_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/Database/SQL/Connection.h"

namespace Stroika::Samples::SQL {

    using Stroika::Foundation::Database::SQL::Connection;

    /**
     *  \brief  Simple test of creating a plain (employees) db and adding to it, and a few simple queries.
     */
    void DirectEmployeesDB (const std::function<Connection::Ptr ()>& connectionFactory);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Samples_SQL_DirectEmployeesDB_h_*/
