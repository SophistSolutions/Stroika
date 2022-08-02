/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_EngineProperties_h_
#define _Stroika_Foundation_Database_SQL_EngineProperties_h_ 1

#include "../../StroikaPreComp.h"

#include <string_view>

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
        /**
         */
        static constexpr inline wstring_view kDoesTableExistParameterName{L":TABLENAME"sv};

    public:
        /**
         */
        enum NonStandardSQL {
            // variable name for parameter is :TABLENAME
            // @todo docs return
            eDoesTableExist,
        };

    public:
        /**
         */
        virtual String GetSQL (NonStandardSQL n) const = 0;

    public:
        // From https://www.sqlite.org/lang_transaction.html
        // An implicit transaction (a transaction that is started automatically, not a transaction started by BEGIN)
        // is committed automatically when the last active statement finishes. A statement finishes when its last cursor closes,
        // which is guaranteed to happen when the prepared statement is reset or finalized.
        virtual bool RequireStatementResetAfterModifyingStatmentToCompleteTransaction () const = 0;

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
