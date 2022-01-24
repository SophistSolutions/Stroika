/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_Utils_h_
#define _Stroika_Foundation_Database_SQL_Utils_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
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
