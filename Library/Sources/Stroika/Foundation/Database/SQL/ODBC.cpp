/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>
#endif

#if qHasLibrary_ODBC
#include <sql.h>
#include <sqlext.h>
#endif

#include "../../Characters/CString/Utilities.h"
#include "../../Characters/Format.h"

#include "../Exception.h"

#include "ODBC.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::SQL::ODBC;
using namespace Debug;

#if qHasLibrary_ODBC

namespace {
    void ThrowIfSQLError_ (SQLRETURN r, const String& message)
    {
        if ((r != SQL_SUCCESS) and (r != SQL_SUCCESS_WITH_INFO)) [[UNLIKELY_ATTR]] {
            Execution::Throw (Exception{message});
        }
    }
}

/*
 ********************************************************************************
 *************************** ODBC::Connection::Rep_ *****************************
 ********************************************************************************
 */
struct Connection::Rep_ final : IRep {
    SQLHDBC fConnectionHandle{nullptr};
    SQLHENV fODBCEnvironmentHandle{nullptr};

    Rep_ (const Options& options)
    {
        TraceContextBumper ctx{"SQLite::Connection::Rep_::Rep_"};
        if (not options.fDSN) {
            Execution::Throw (Exception{L"DSN Required"sv});
        }
        try {
            ThrowIfSQLError_ (::SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &fODBCEnvironmentHandle), L"Error AllocHandle"sv);
            ThrowIfSQLError_ (::SQLSetEnvAttr (fODBCEnvironmentHandle, SQL_ATTR_ODBC_VERSION, reinterpret_cast<void*> (SQL_OV_ODBC3), 0), L"Error SetEnv"sv);
            ThrowIfSQLError_ (::SQLAllocHandle (SQL_HANDLE_DBC, fODBCEnvironmentHandle, &fConnectionHandle), L"Error AllocHDB"sv);

            ::SQLSetConnectAttr (fConnectionHandle, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER*> (5), 0);
            {
                // See the docs on SQLConnect - the error handling needs to be VASTLY more complex. We need some mechanism to return
                // warnings to the caller (to be ignored or whatever).
                // And ONLY thorw exceptions on ERROR!
                SQLRETURN return_value = ::SQLConnect (
                    fConnectionHandle,
                    reinterpret_cast<SQLTCHAR*> (const_cast<TCHAR*> (options.fDSN->AsSDKString ().c_str ())),
                    SQL_NTS,
                    nullptr, SQL_NTS,
                    nullptr, SQL_NTS);
                if ((return_value != SQL_SUCCESS) && (return_value != SQL_SUCCESS_WITH_INFO)) {
                    // This logic for producing an error message completely sucks and is largely incorrect
                    wstring     errorString = L"Error SQLConnect: ";
                    SQLTCHAR    sqlState[6];
                    SQLINTEGER  errorCode;
                    SQLSMALLINT messageLength;
                    SQLTCHAR    errorMessage[1024];
                    DISABLE_COMPILER_MSC_WARNING_START (4267)
                    long errValue = ::SQLGetDiagRec (
                        SQL_HANDLE_DBC, fConnectionHandle, 1,
                        reinterpret_cast<SQLTCHAR*> (sqlState), &errorCode,
                        reinterpret_cast<SQLTCHAR*> (errorMessage),
                        NEltsOf (errorMessage), &messageLength);
                    DISABLE_COMPILER_MSC_WARNING_END (4267)
                    if (errValue == SQL_SUCCESS) {
                        // TCHAR isn't the same SQLTCHAR for 'ANSI' because for some crazy reason, they
                        // used unsigned char for SQLCHAR!
                        errorString += SDKString2Wide (reinterpret_cast<TCHAR*> (errorMessage));
                    }
                    else if (errValue == SQL_SUCCESS_WITH_INFO) {
                        errorString = L"Error message too long at";
                    }
                    else if (errValue == SQL_ERROR) {
                        errorString += L"RecNumber was negative or 0 or BufferLength was less tha 0";
                    }
                    else if (errValue == SQL_NO_DATA) {
                        errorString += L"SQL no data";
                    }
                    Execution::Throw (Exception{errorString});
                }
            }
        }
        catch (...) {
            if (fConnectionHandle != nullptr) {
                ::SQLFreeHandle (SQL_HANDLE_DBC, fConnectionHandle);
                fConnectionHandle = nullptr;
            }
            if (fODBCEnvironmentHandle != nullptr) {
                ::SQLFreeHandle (SQL_HANDLE_ENV, fODBCEnvironmentHandle);
                fODBCEnvironmentHandle = nullptr;
            }
        }
    }
    ~Rep_ ()
    {
        if (fConnectionHandle != nullptr) {
            ::SQLFreeHandle (SQL_HANDLE_DBC, fConnectionHandle);
        }
        if (fODBCEnvironmentHandle != nullptr) {
            ::SQLFreeHandle (SQL_HANDLE_ENV, fODBCEnvironmentHandle);
        }
    }
    virtual shared_ptr<const EngineProperties> GetEngineProperties () const override
    {
        AssertNotImplemented ();
        struct MyEngineProperties_ final : EngineProperties {
            virtual String GetEngineName () const override
            {
                return L"ODBC"sv; // must indirect to connection to get more info (from dns at least? not clear)
            }
            virtual String GetSQL ([[maybe_unused]] NonStandardSQL n) const override
            {
                // see https://stackoverflow.com/questions/167576/check-if-table-exists-in-sql-server
                AssertNotImplemented ();
                return String{};
            }
            virtual bool SupportsNestedTransactions () const override
            {
                return false;
            }
        };
        return make_shared<const MyEngineProperties_> (); // dynamic info based on connection/dsn
    }
    virtual SQL::Statement mkStatement (const String& sql) override
    {
        Connection::Ptr conn = Connection::Ptr{dynamic_pointer_cast<Connection::IRep> (shared_from_this ())};
        return ODBC::Statement{conn, sql};
    }
    virtual SQL::Transaction mkTransaction () override
    {
        Connection::Ptr conn = Connection::Ptr{dynamic_pointer_cast<Connection::IRep> (shared_from_this ())};
        return ODBC::Transaction{conn};
    }
    virtual void Exec (const String& /*sql*/) override
    {
        AssertNotImplemented ();
    }
};

/*
 ********************************************************************************
 ************************* SQL::ODBC::Connection::Ptr ***************************
 ********************************************************************************
 */
SQL::ODBC::Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
    : inherited{src}
{
}

/*
 ********************************************************************************
 ************************** SQL::ODBC::Connection *******************************
 ********************************************************************************
 */
auto SQL::ODBC::Connection::New (const Options& options) -> Ptr
{
    auto tmp = make_shared<Rep_> (options);
    Ptr  result{tmp};
    return result;
}

/*
 ********************************************************************************
 ******************************* SQLite::Statement ******************************
 ********************************************************************************
 */
struct Statement::MyRep_ : IRep {
    MyRep_ (const Connection::Ptr& db, const String& query)
        : fConnectionPtr_{db}
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContextBumper ctx{"SQLite::Statement::MyRep_::CTOR", Stroika_Foundation_Debug_OptionalizeTraceArgs (L "db=%p, query='%s'", db, query.c_str ())};
#endif
        RequireNotNull (db);
#if qDebug
        _fSharedContext = fConnectionPtr_.GetSharedContext ();
#endif
        string                                                    queryUTF8 = query.AsUTF8 ();
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        AssertNotImplemented ();
    }
    ~MyRep_ ()
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
    }
    virtual String GetSQL ([[maybe_unused]] WhichSQLFlag whichSQL) const override
    {
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        AssertNotImplemented ();
        return String{};
    }
    virtual Sequence<ColumnDescription> GetColumns () const override
    {
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        AssertNotImplemented ();
        return Sequence<ColumnDescription>{};
    };
    virtual Sequence<ParameterDescription> GetParameters () const override
    {
        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        return fParameters_;
    };
    virtual void Bind (unsigned int parameterIndex, const VariantValue& v) override
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        fParameters_[parameterIndex].fValue = v;
        AssertNotImplemented ();
    }
    virtual void Bind (const String& parameterName, const VariantValue& v) override
    {
        Require (not parameterName.empty ());
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        AssertNotImplemented ();
        String pn = parameterName;
        if (pn[0] != ':') {
            pn = L":" + pn;
        }
        for (unsigned int i = 0; i < fParameters_.length (); ++i) {
            if (fParameters_[i].fName == pn) {
                Bind (i, v);
                return;
            }
        }
        DbgTrace (L"Statement::Bind: Parameter '%s' not found in list %s", parameterName.c_str (), Characters::ToString (fParameters_.Select<String> ([] (const auto& i) { return i.fName; })).c_str ());
        RequireNotReached (); // invalid paramter name provided
    }
    virtual void Reset () override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContextBumper ctx{"SQLite::Statement::MyRep_::Statement::Reset"};
#endif
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        AssertNotImplemented ();
    }
    virtual optional<Row> GetNextRow () override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContextBumper ctx{"SQLite::Statement::MyRep_::Statement::GetNextRow"};
#endif
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        AssertNotImplemented ();
        return nullopt;
    }

    Connection::Ptr                fConnectionPtr_;
    vector<ColumnDescription>      fColumns_;
    Sequence<ParameterDescription> fParameters_;
};

Statement::Statement (const Connection::Ptr& db, const String& query)
    : inherited{make_unique<MyRep_> (db, query)}
{
}

/*
 ********************************************************************************
 ******************************* SQLite::Transaction ****************************
 ********************************************************************************
 */
struct Transaction::MyRep_ : public SQL::Transaction::IRep {
    MyRep_ (const Connection::Ptr& db)
        : fConnectionPtr_{db}
    {
        AssertNotImplemented ();
    }
    virtual void Commit () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec (L"COMMIT;"sv);
    }
    virtual void Rollback () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec (L"ROLLBACK;"sv);
    }
    virtual Disposition GetDisposition () const override
    {
        // @todo record more info so we can report finer grained status ; try/catch in rollback/commit and dbgtraces
        return fCompleted_ ? Disposition::eCompleted : Disposition::eNone;
    }
    Connection::Ptr fConnectionPtr_;
    bool            fCompleted_{false};
};
Transaction::Transaction (const Connection::Ptr& db)
    : inherited{make_unique<MyRep_> (db)}
{
}

/*
 ********************************************************************************
 ******************************** LEGACY-DEPRECATED *****************************
 ********************************************************************************
 */
class Database::SQL::ODBC::DBConnection::Rep {
public:
    SQLHDBC      fConnectionHandle{nullptr};
    SQLHENV      fODBCEnvironmentHandle{nullptr};
    unsigned int fNestedTransactionCount{0};

public:
    Rep (const wstring& dsn)
    {
        try {
            ThrowIfSQLError_ (::SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &fODBCEnvironmentHandle), L"Error AllocHandle"sv);
            ThrowIfSQLError_ (::SQLSetEnvAttr (fODBCEnvironmentHandle, SQL_ATTR_ODBC_VERSION, reinterpret_cast<void*> (SQL_OV_ODBC3), 0), L"Error SetEnv"sv);
            ThrowIfSQLError_ (::SQLAllocHandle (SQL_HANDLE_DBC, fODBCEnvironmentHandle, &fConnectionHandle), L"Error AllocHDB"sv);

            ::SQLSetConnectAttr (fConnectionHandle, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER*> (5), 0);
            {
                // See the docs on SQLConnect - the error handling needs to be VASTLY more complex. We need some mechanism to return
                // warnings to the caller (to be ignored or whatever).
                // And ONLY thorw exceptions on ERROR!
                SQLRETURN return_value = ::SQLConnect (
                    fConnectionHandle,
                    reinterpret_cast<SQLTCHAR*> (const_cast<TCHAR*> (ToSDKString (dsn).c_str ())),
                    SQL_NTS,
                    nullptr, SQL_NTS,
                    nullptr, SQL_NTS);
                if ((return_value != SQL_SUCCESS) && (return_value != SQL_SUCCESS_WITH_INFO)) {
                    // This logic for producing an error message completely sucks and is largely incorrect
                    wstring     errorString = L"Error SQLConnect: ";
                    SQLTCHAR    sqlState[6];
                    SQLINTEGER  errorCode;
                    SQLSMALLINT messageLength;
                    SQLTCHAR    errorMessage[1024];
                    DISABLE_COMPILER_MSC_WARNING_START (4267)
                    long errValue = ::SQLGetDiagRec (
                        SQL_HANDLE_DBC, fConnectionHandle, 1,
                        reinterpret_cast<SQLTCHAR*> (sqlState), &errorCode,
                        reinterpret_cast<SQLTCHAR*> (errorMessage),
                        NEltsOf (errorMessage), &messageLength);
                    DISABLE_COMPILER_MSC_WARNING_END (4267)
                    if (errValue == SQL_SUCCESS) {
                        // TCHAR isn't the same SQLTCHAR for 'ANSI' because for some crazy reason, they
                        // used unsigned char for SQLCHAR!
                        errorString += SDKString2Wide (reinterpret_cast<TCHAR*> (errorMessage));
                    }
                    else if (errValue == SQL_SUCCESS_WITH_INFO) {
                        errorString = L"Error message too long at";
                    }
                    else if (errValue == SQL_ERROR) {
                        errorString += L"RecNumber was negative or 0 or BufferLength was less tha 0";
                    }
                    else if (errValue == SQL_NO_DATA) {
                        errorString += L"SQL no data";
                    }
                    Execution::Throw (Exception{errorString});
                }
            }
        }
        catch (...) {
            if (fConnectionHandle != nullptr) {
                ::SQLFreeHandle (SQL_HANDLE_DBC, fConnectionHandle);
                fConnectionHandle = nullptr;
            }
            if (fODBCEnvironmentHandle != nullptr) {
                ::SQLFreeHandle (SQL_HANDLE_ENV, fODBCEnvironmentHandle);
                fODBCEnvironmentHandle = nullptr;
            }
        }
    }
    virtual ~Rep ()
    {
        if (fConnectionHandle != nullptr) {
            ::SQLFreeHandle (SQL_HANDLE_DBC, fConnectionHandle);
        }
        if (fODBCEnvironmentHandle != nullptr) {
            ::SQLFreeHandle (SQL_HANDLE_ENV, fODBCEnvironmentHandle);
        }
    }
};
Database::SQL::ODBC::DBConnection::DBConnection (const wstring& dsn)
    : fRep{make_shared<Rep> (dsn)}
{
}

unsigned int Database::SQL::ODBC::DBConnection::GetNestedTransactionCount () const
{
    return fRep->fNestedTransactionCount;
}
#endif
