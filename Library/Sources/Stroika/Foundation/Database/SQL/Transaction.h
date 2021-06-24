/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_Transaction_h_
#define _Stroika_Foundation_Database_SQL_Transaction_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Configuration/Enumeration.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"

#include "Connection.h"

/**
 *  \file
 * 
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 */

namespace Stroika::Foundation::Database::SQL {

    using Characters::String;

    /**
     *  \note Transactions are not required. This is for explicit transactions. If you omit
     *        using transactions, sqlite creates mini transactions automatically for each statement.
     *
     *  \note Nested transactions not supported (for sqlite - add featre for detecting if engine supports)
     */
    class Transaction : protected Debug::AssertExternallySynchronizedLock {
    public:
        class IRep;

    protected:
        /**
         */
        Transaction (unique_ptr<IRep>&& rep);
        Transaction (const Transaction&) = delete;

    public:
        /**
         *  If Commit() has not already been called, this automatically calls Rollback() and suppresses any
         *  exceptions.
         */
        ~Transaction ();

    public:
        /**
         */
        nonvirtual Transaction& operator= (const Transaction&) = delete;

    public:
        /**
         *  Cause the transaction to end successfully, flushing to the database.
         *  It is (an assertion) error to call this multiple times. And calling OMITTING
         *  a call before the destructor causes the transaction to Rollback.
         */
        nonvirtual void Commit ();

    public:
        /**
         *  This cannot be called after a rollback or commit.
         * 
         *  This causes no data to be written for the commands already issued in the transaction.
         * 
         *  This is equivilent to just destroying this object, except that it can propagate
         *  exceptions if needed, whereas a destructor cannot.
         */
        nonvirtual void Rollback ();

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    protected:
        unique_ptr<IRep> _fRep;
    };

    /**
     *  Transaction::IRep provides an (abstract) API for transactions, supported by each backend connection type.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each Statement can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes).
     */
    class Transaction::IRep : public Debug::AssertExternallySynchronizedLock {
    public:
        /**
         */
        virtual ~IRep () = default;

    public:
        /**
         *  Cause the transaction to end successfully, flushing to the database.
         *  It is (an assertion) error to call this multiple times. And calling OMITTING
         *  a call before the destructor causes the transaction to Rollback.
         */
        virtual void Commit () = 0;

    public:
        /**
         *  This cannot be called after a rollback or commit.
         * 
         *  This causes no data to be written for the commands already issued in the transaction.
         * 
         *  This is equivilent to just destroying this object, except that it can propagate
         *  exceptions if needed, whereas a destructor cannot.
         */
        virtual void Rollback () = 0;

    public:
        enum class Disposition {
            eNone,
            eRolledBack,
            eCompleted,
            eFailed,

            Stroika_Define_Enum_Bounds (eNone, eFailed)
        };

    public:
        /**
         *  Mostly this reports eNone or something else. But extra status could be useful for logging.
         */
        virtual Disposition GetDisposition () const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Transaction.inl"

#endif /*_Stroika_Foundation_Database_SQL_Transaction_h_*/
