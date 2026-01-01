/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Samples_Document_EmployeesDB_h_
#define _Samples_Document_EmployeesDB_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/Database/Document/Connection.h"

namespace Stroika::Samples::Document {

    using namespace Stroika::Foundation::Database::Document;

    /*
     *  Simple test of creating a plain (employees) db and adding to it, and a few simple queries.
     */
    void EmployeesDB (const std::function<Connection::Ptr ()>& connectionFactory);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Samples_Document_EmployeesDB_h_*/
