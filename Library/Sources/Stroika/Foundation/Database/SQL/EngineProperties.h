/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_EngineProperties_h_
#define _Stroika_Foundation_Database_SQL_EngineProperties_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"

/**
 *  \file
 * 
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 */

namespace Stroika::Foundation::Database::SQL {

    using Characters::String;

    /**
     *  \brief EngineProperties captures the features associated with a given database engine (being talked to through a SQL::Connection::Ptr).
     *
     *  @todo This will require significant building out/elaboration to support more database backends
     *        with the ORM (or any attempted cross-database-portable) applicaiton.
     */
    class EngineProperties {
    public:
        /**
         */
        EngineProperties () = default;

    public:
        /**
         */
        virtual ~EngineProperties () = default;

    public:
        /**
         */
        virtual String GetEngineName () const = 0;

    public:
        virtual bool SupportsNestedTransactions () const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "EngineProperties.inl"

#endif /*_Stroika_Foundation_Database_SQL_EngineProperties_h_*/
