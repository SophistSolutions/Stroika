/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_Statement_h_
#define _Stroika_Foundation_Database_SQL_Statement_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"
#include "../../Common/Property.h"
#include "../../Containers/Mapping.h"
#include "../../Containers/Sequence.h"
#include "../../DataExchange/VariantValue.h"
#include "../../Debug/AssertExternallySynchronizedMutex.h"

#include "Connection.h"

/**
 *  \file
 * 
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 */

namespace Stroika::Foundation::Database::SQL {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::VariantValue;

    /**
     *  'Statement' is a non-copyable (but moveable) object, referencing a particalar SQL Connection object.
     *
     *  \note - Design Note - we use String for the result-column-name - and could use int or Atom. But
     *        String slightly simpler, and nearly as performant, so going with that for now.
     *
     *  \todo   CONSIDER redo Row as iterator; or maybe GetResults () method that returns iterable of Rows? and lazy pulls them?
     */
    class Statement : protected Debug::AssertExternallySynchronizedMutex {
    public:
        class IRep;

    public:
        /**
         */
        Statement ()                 = delete;
        Statement (Statement&&)      = default;
        Statement (const Statement&) = delete;

    protected:
        Statement (unique_ptr<IRep>&& rep);

    public:
        /**
         */
        ~Statement () = default;

    public:
        /**
         */
        nonvirtual Statement& operator= (const Statement&) = delete;

    public:
        /**
         *  Could use 'int' index for this and faster, but tracking names maybe slightly better for readability and logging,
         *  and given that string shared_ptr, not too bad to copy around/compare
         *  COULD use Atom<> - which would be a good performance/funcitonality compromise, but would want to use private atomMgr,
         *  so scope of names limited (else list of interned strings could become large). Not worth the efforts/risk for small benefit for now.
         */
        using ColumnName = String;

    public:
        /**
         *  This describes an SQL column. The 'type' is a string (and optional at that), and refers to the SQLite type system.
         */
        struct ColumnDescription {
            /**
             */
            ColumnName fName;

            /**
             * sqlite3_column_decltype
             */
            optional<String> fType;

            /**
             *  @see Characters::ToString ()
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
         *  \note the types returned in .fType are generally wrong until we've run our first query).
         * 
         * @ todo consider rename to GetResultColumns
         */
        nonvirtual Sequence<ColumnDescription> GetColumns () const;

    public:
        /**
         *  Parameters are set with a call to "Bind" so maybe also called parameter bindings.
         */
        struct ParameterDescription {
            /**
             *  name is optional - bindings can be specified by index
             */
            optional<String> fName;

            /**
             */
            VariantValue fValue;

            /**
             *  @see Characters::ToString ()
             */
            nonvirtual String ToString () const;
        };

    public:
        /**
         *  Gets the names and values of all the current parameters to this sql statement.
         * 
         *  \see Bind ()
         */
        nonvirtual Sequence<ParameterDescription> GetParameters () const;

    public:
        /**
         *  Specify one, or a collection of parameter bindings. This never changes the order of the bindings, just
         *  applies them to the appropriate binding elements.
         * 
         *  \note the paramterIndex is 'zero-based' unlike sqlite native APIs
         *
         *  \req parameterIndex < GetParameters ().length ()
         *  \req paramterName FOUND in GetParameters ().fName's
         *  and similarly for other overloads
         *
         *  \note - parameterName can be the name of the variable with or without the prefixing :
         *
         *  If iterable argument to Bind (), the if the arguments have parameter names, the association will be done by name.
         *  if they do not have names, the order in the bind argument list will be interpretted as argument order (parameter order) for that item)
         * 
         *  \see GetParameters ()
         */
        nonvirtual void Bind (unsigned int parameterIndex, const VariantValue& v);
        nonvirtual void Bind (const String& parameterName, const VariantValue& v);
        nonvirtual void Bind (const Traversal::Iterable<ParameterDescription>& parameters);
        nonvirtual void Bind (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters);

    public:
        enum class WhichSQLFlag {
            /**
             *  This is the original SQL passed in as argument to the statement.
             */
            eOriginal,

            /**
             * string containing the SQL text of prepared statement P with [bound parameters] expanded
             */
            eExpanded,

            /**
             * This option is available iff CompiledOptions::kThe.ENABLE_NORMALIZE
             */
            eNormalized
        };

    public:
        /**
         *  Return the original (or normalized or expanded with bindings) SQL associated with this statement.
         */
        nonvirtual String GetSQL (WhichSQLFlag whichSQL = WhichSQLFlag::eOriginal) const;

    public:
        /**
         *  Execute the given statement, and ignore its result value. Do NOT mix Execute() with GetNextRow() or GetAllRows ().
         *  It is legal to call this on an SQL statement that returns results, but you will not see the results.
         * 
         *  Execute () with a list of ParameterDescriptions is like:
         *      >   Reset
         *      >   Bind () with that list of parameters and then  (can be empty list/missing)
         *      >   Execute ()
         * 
         *  No need to call Reset() before calling Execute () as it calls it internally.
         */
        nonvirtual void Execute ();
        nonvirtual void Execute (const Traversal::Iterable<ParameterDescription>& parameters);
        nonvirtual void Execute (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters);

    public:
        /**
         *  This mimics the sqlite3_reset () functionality/API.
         * 
         *  This clears any ongoing query, so the next call to GetNextRow () will start with the first row from the current query.
         *  This does NOT affect any values currently bound.
         * 
         *  BUT NOTE, it is required to call Reset() (if there are any ongoing queries) before calling Bind.
         */
        nonvirtual void Reset ();

    public:
        /**
         */
        using Row = Mapping<ColumnName, VariantValue>;

    public:
        /**
         *  If called on a new Statement, or on a statement that has been reset (since the last call to GetNextRow() - this re-runs the query.
         *  But either way, it returns the next row.
         *
         * returns 'missing' on EOF, exception on error
         */
        nonvirtual optional<Row> GetNextRow ();

    public:
        /**
         *  \brief - Call GetNextRow () repeatedly, and accumulate Rows into a Sequence (@see GetAllRows ())
         * 
         *  The overloads that take a sequence of column numbers return each row as a tuple of columns (VariantValue)
         *  for just the specified column numbers.
         * 
         * ... @todo use variadic templates to generatelas GetAllRows()
         * ... @todo COULD overload so columns named by 'name' instead of index, but simple to use index (as specified by result of
         *           GetColumns ()
         */
        nonvirtual Sequence<Row> GetAllRemainingRows ();
        nonvirtual Sequence<VariantValue> GetAllRemainingRows (size_t restrictToColumn);
        nonvirtual Sequence<tuple<VariantValue, VariantValue>> GetAllRemainingRows (size_t restrictToColumn1, size_t restrictToColumn2);
        nonvirtual Sequence<tuple<VariantValue, VariantValue, VariantValue>> GetAllRemainingRows (size_t restrictToColumn1, size_t restrictToColumn2, size_t restrictToColumn3);

    public:
        /**
         *  \brief - call Reset (), and then GetAllRemainingRows () - which always starts current statement with current bindings from the beginning.
         * 
         *  The overloads that take a sequence of column numbers return each row as a tuple of columns (VariantValue)
         *  for just the specified column numbers.
         * 
         * ... @todo use variadic templates to generatelas GetAllRows()
         * ... @todo COULD overload so columns named by 'name' instead of index, but simple to use index (as specified by result of
         *           GetColumns ()
         */
        nonvirtual Sequence<Row> GetAllRows ();
        nonvirtual Sequence<VariantValue> GetAllRows (size_t restrictToColumn);
        nonvirtual Sequence<tuple<VariantValue, VariantValue>> GetAllRows (size_t restrictToColumn1, size_t restrictToColumn2);
        nonvirtual Sequence<tuple<VariantValue, VariantValue, VariantValue>> GetAllRows (size_t restrictToColumn1, size_t restrictToColumn2, size_t restrictToColumn3);

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    protected:
        unique_ptr<IRep> _fRep;
    };

    /**
     *  Statement::IRep provides an (abstract) API for accessing an SQL database.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each Statement can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes).
     */
    class Statement::IRep : protected Debug::AssertExternallySynchronizedMutex {
    public:
        /**
         */
        virtual ~IRep () = default;

    public:
        /**
         *  Return the original (or normalized or expanded with bindings) SQL associated with this statement.
         */
        virtual String GetSQL (WhichSQLFlag whichSQL) const = 0;

    public:
        virtual Sequence<ColumnDescription> GetColumns () const = 0;

    public:
        virtual Sequence<ParameterDescription> GetParameters () const = 0;

    public:
        virtual void Bind (unsigned int parameterIndex, const VariantValue& v) = 0;
        virtual void Bind (const String& parameterName, const VariantValue& v) = 0;

    public:
        /**
         *  This mimics the sqlite3_reset () functionality/API.
         * 
         *  This clears any ongoing query, so the next call to GetNextRow () will start with the first row from the current query.
         *  This does NOT affect any values currently bound.
         * 
         *  BUT NOTE, it is required to call Reset() (if there are any ongoing queries) before calling Bind.
         */
        virtual void Reset () = 0;

    public:
        /**
         *  If called on a new Statement, or on a statement that has been reset (since the last call to GetNextRow() - this re-runs the query.
         *  But either way, it returns the next row.
         *
         * returns 'missing' on EOF, exception on error
         */
        virtual optional<Row> GetNextRow () = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Statement.inl"

#endif /*_Stroika_Foundation_Database_SQL_Statement_h_*/
