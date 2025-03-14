/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Database/Exception.h"

#include "MongoDBClient.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::Document::MongoDBClient;
using namespace Debug;

#if qStroika_HasComponent_mongocxxdriver && 0

namespace {
    using Connection::Options;
    struct Rep_ final : Stroika::Foundation::Database::Document::MongoDBClient::Connection::IRep {

        Rep_ (const Options& options)
        {
            TraceContextBumper ctx{"SQLite::Connection::Rep_::Rep_"};
            if (not options.fDSN) {
                Execution::Throw (Exception{"DSN Required"sv});
            }
            try {
                ThrowIfSQLError_ (::SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &fODBCEnvironmentHandle), "Error AllocHandle"sv);
                ThrowIfSQLError_ (::SQLSetEnvAttr (fODBCEnvironmentHandle, SQL_ATTR_ODBC_VERSION, reinterpret_cast<void*> (SQL_OV_ODBC3), 0),
                                  "Error SetEnv"sv);
                ThrowIfSQLError_ (::SQLAllocHandle (SQL_HANDLE_DBC, fODBCEnvironmentHandle, &fConnectionHandle), "Error AllocHDB"sv);

                ::SQLSetConnectAttr (fConnectionHandle, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER*> (5), 0);
                {
                    // See the docs on SQLConnect - the error handling needs to be VASTLY more complex. We need some mechanism to return
                    // warnings to the caller (to be ignored or whatever).
                    // And ONLY thorw exceptions on ERROR!
                    SQLRETURN return_value =
                        ::SQLConnect (fConnectionHandle, reinterpret_cast<SQLTCHAR*> (const_cast<TCHAR*> (options.fDSN->AsSDKString ().c_str ())),
                                      SQL_NTS, nullptr, SQL_NTS, nullptr, SQL_NTS);
                    if ((return_value != SQL_SUCCESS) && (return_value != SQL_SUCCESS_WITH_INFO)) {
                        // This logic for producing an error message completely sucks and is largely incorrect
                        StringBuilder errorString = "Error SQLConnect: "_k;
                        SQLTCHAR      sqlState[6];
                        SQLINTEGER    errorCode;
                        SQLSMALLINT   messageLength;
                        SQLTCHAR      errorMessage[1024];
                        DISABLE_COMPILER_MSC_WARNING_START (4267)
                        long errValue = ::SQLGetDiagRec (SQL_HANDLE_DBC, fConnectionHandle, 1, reinterpret_cast<SQLTCHAR*> (sqlState), &errorCode,
                                                         reinterpret_cast<SQLTCHAR*> (errorMessage), Memory::NEltsOf (errorMessage), &messageLength);
                        DISABLE_COMPILER_MSC_WARNING_END (4267)
                        if (errValue == SQL_SUCCESS) {
                            // TCHAR isn't the same SQLTCHAR for 'ANSI' because for some crazy reason, they
                            // used unsigned char for SQLCHAR!
                            errorString += String::FromSDKString (reinterpret_cast<TCHAR*> (errorMessage));
                        }
                        else if (errValue == SQL_SUCCESS_WITH_INFO) {
                            errorString = "Error message too long at"_k;
                        }
                        else if (errValue == SQL_ERROR) {
                            errorString += "RecNumber was negative or 0 or BufferLength was less than 0"_k;
                        }
                        else if (errValue == SQL_NO_DATA) {
                            errorString += "SQL no data"_k;
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
                    return "ODBC"sv; // must indirect to connection to get more info (from dns at least? not clear)
                }
                virtual String GetSQL ([[maybe_unused]] NonStandardSQL n) const override
                {
                    // see https://stackoverflow.com/questions/167576/check-if-table-exists-in-sql-server
                    AssertNotImplemented ();
                    return String{};
                }
                virtual bool RequireStatementResetAfterModifyingStatmentToCompleteTransaction () const override
                {
                    return false;
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
            Connection::Ptr conn = Connection::Ptr{Debug::UncheckedDynamicPointerCast<Connection::IRep> (shared_from_this ())};
            return Statement{conn, sql};
        }
        virtual SQL::Transaction mkTransaction () override
        {
            Connection::Ptr conn = Connection::Ptr{Debug::UncheckedDynamicPointerCast<Connection::IRep> (shared_from_this ())};
            return Transaction{conn};
        }
        virtual void Exec (const String& /*sql*/) override
        {
            AssertNotImplemented ();
        }
    };
}

/*
 ********************************************************************************
 ****************** Document::MongoDBClient::Connection::Ptr ********************
 ********************************************************************************
 */
Document::MongoDBClient::Connection::Ptr::Ptr (const shared_ptr<IRep>& src)
    : inherited{src}
{
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    if (src != nullptr) {
        fAssertExternallySynchronizedMutex.SetAssertExternallySynchronizedMutexContext (src->fAssertExternallySynchronizedMutex.GetSharedContext ());
    }
#endif
}

/*
 ********************************************************************************
 ******************* Document::MongoDBClient::Connection::New *******************
 ********************************************************************************
 */
auto Document::MongoDBClient::Connection::New (const Options& options) -> Ptr
{
    return Ptr{make_shared<Rep_> (options)};
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
        fConnectionPtr_->Exec ("COMMIT;"sv);
    }
    virtual void Rollback () override
    {
        Require (not fCompleted_);
        fCompleted_ = true;
        fConnectionPtr_->Exec ("ROLLBACK;"sv);
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
#endif
