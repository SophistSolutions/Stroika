/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Samples_SQL_ThreadTest_h_
#define _Samples_SQL_ThreadTest_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/Database/SQL/Connection.h"

namespace Stroika::Samples::SQL {

    using Stroika::Foundation::Database::SQL::Connection;

    /**
     *  \brief  Simple test of using threads with SQL - one thread updates employees table, and one writes paychecks
     */
    void ThreadTest (const std::function<Connection::Ptr ()>& connectionFactory);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Samples_SQL_ThreadTest_h_*/
