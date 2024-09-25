/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_Transaction_h_
#define _Stroika_Foundation_Database_SQL_Transaction_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Database/SQL/Connection.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

/**
 *  \file
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Database::SQL {

    using Characters::String;

    /**
     *  \note Transactions are not required. This is for explicit transactions. If you omit
     *        using transactions, sqlite creates mini transactions automatically for each statement.
     *
     *  \note Nested transactions not supported (for sqlite - add featre for detecting if engine supports)
     * 
     *  \par Example Usage
     *      \code
     *          T DB::AddOrMergeUpdate (ORM::TableConnection<T>* dbConnTable, const T& d)
     *          {
     *              SQL::Transaction t{dbConnTable->connection ()->mkTransaction ()};
     *              std::optional<T> result;
     *              if (auto dbObj = dbConnTable->Get (id)) {
     *                  result = T::Merge (*dbObj, d);
     *                  dbConnTable->Update (*result);
     *              }
     *              else {
     *                  result = d;
     *                  dbConnTable->AddNew (d);
     *              }
     *              t.Commit ();
     *              return Memory::ValueOf (result);
     *          }
     *      \endcode
     */
    class [[nodiscard]] Transaction {
    public:
        class IRep;

    public:
        /**
         */
        Transaction (Transaction&&)      = default;
        Transaction (const Transaction&) = delete;

    protected:
        Transaction (unique_ptr<IRep>&& rep);

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
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex _fThisAssertExternallySynchronized;
        unique_ptr<IRep>                                               _fRep;
    };

    /**
     *  Transaction::IRep provides an (abstract) API for transactions, supported by each backend connection type.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each Statement can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes).
     */
    class Transaction::IRep {
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
        /**
         *  \note   Common::DefaultNames<> supported
         */
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

    protected:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex _fThisAssertExternallySynchronized;

    private:
        friend class Transaction;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Transaction.inl"

#endif /*_Stroika_Foundation_Database_SQL_Transaction_h_*/
