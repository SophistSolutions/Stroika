/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQLite_h_
#define _Stroika_Foundation_Database_SQLite_h_ 1

#include "../StroikaPreComp.h"

#include "SQL/SQLite.h"

#pragma message("Warning: Database::SQLite deprecated - use Database::SQL::SQLLite")
namespace Stroika::Foundation::Database::SQLite {
    using namespace Database::SQL::SQLite;
}

#endif /*_Stroika_Foundation_Database_SQLite_h_*/
