/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Samples_SQLite_SampleEmployeesDB_h_
#define _Samples_SQLite_SampleEmployeesDB_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Database/SQL/SQLite.h"

namespace Stroika::Samples::SQLite {
#if qHasFeature_sqlite
    using Stroika::Foundation::Database::SQL::SQLite::Options;
    /**
     *  \brief  Simple test of creating a plain (employees) db and adding to it, and a few simple queries.
     */
    void SimpleEmployeesDB (const Options& options);
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Samples_SQLite_SampleEmployeesDB_h_*/
