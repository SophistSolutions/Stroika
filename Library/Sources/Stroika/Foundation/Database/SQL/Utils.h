/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_Utils_h_
#define _Stroika_Foundation_Database_SQL_Utils_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::SQL::Utils {

    using Characters::String;

    /**
     *  @todo - probably move to common area - for all DB stuff - not specific to SQLite (maybe have folder for "SQL")
     */
    String QuoteStringForDB (const String& s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Utils.inl"

#endif /*_Stroika_Foundation_Database_SQL_Utils_h_*/
