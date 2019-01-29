/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <windows.h>
#endif

#if qHasLibrary_ODBC
#include <sql.h>
#include <sqlext.h>
#endif

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"

#include "ODBCClient.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;

/*
 ********************************************************************************
 **************************** ODBCSupport::Exception ****************************
 ********************************************************************************
 */
Database::Exception::Exception (const String& message)
    : StringException (Format (L"Database connection error: %s", message.c_str ()))
{
}

/*
 ********************************************************************************
 ************************* ODBCSupport::NoDataException *************************
 ********************************************************************************
 */
Database::NoDataException::NoDataException ()
    : Exception (L"No Data"sv)
{
}

#if qHasLibrary_ODBC
/*
 ********************************************************************************
 ************************* ODBCSupport::DBConnection ****************************
 ********************************************************************************
 */
namespace {
    void ThrowIfSQLError_ (SQLRETURN r, const wstring& message)
    {
        if ((r != SQL_SUCCESS) and (r != SQL_SUCCESS_WITH_INFO))
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (Exception (message));
            }
    }
}
class Database::DBConnection::Rep {
public:
    SQLHDBC      fConnectionHandle;
    SQLHENV      fODBCEnvironmentHandle;
    unsigned int fNestedTransactionCount;

public:
    Rep (const wstring& dsn)
        : fConnectionHandle (nullptr)
        , fODBCEnvironmentHandle (nullptr)
        , fNestedTransactionCount (0)
    {
        try {
            ThrowIfSQLError_ (SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &fODBCEnvironmentHandle), L"Error AllocHandle");
            ThrowIfSQLError_ (SQLSetEnvAttr (fODBCEnvironmentHandle, SQL_ATTR_ODBC_VERSION, reinterpret_cast<void*> (SQL_OV_ODBC3), 0), L"Error SetEnv");
            ThrowIfSQLError_ (SQLAllocHandle (SQL_HANDLE_DBC, fODBCEnvironmentHandle, &fConnectionHandle), L"Error AllocHDB");

            SQLSetConnectAttr (fConnectionHandle, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER*> (5), 0);
            {
                // See the docs on SQLConnect - the error handling needs to be VASTLY more complex. We need some mechanism to return
                // warnings to the caller (to be ignored or whatever).
                // And ONLY thorw exceptions on ERROR!
                SQLRETURN return_value = SQLConnect (
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
                    Execution::Throw (Exception (errorString));
                }
            }
        }
        catch (...) {
            if (fConnectionHandle != nullptr) {
                SQLFreeHandle (SQL_HANDLE_DBC, fConnectionHandle);
                fConnectionHandle = nullptr;
            }
            if (fODBCEnvironmentHandle != nullptr) {
                SQLFreeHandle (SQL_HANDLE_ENV, fODBCEnvironmentHandle);
                fODBCEnvironmentHandle = nullptr;
            }
        }
    }
    virtual ~Rep ()
    {
        if (fConnectionHandle != nullptr) {
            SQLFreeHandle (SQL_HANDLE_DBC, fConnectionHandle);
        }
        if (fODBCEnvironmentHandle != nullptr) {
            SQLFreeHandle (SQL_HANDLE_ENV, fODBCEnvironmentHandle);
        }
    }
};
Database::DBConnection::DBConnection (const wstring& dsn)
    : fRep (make_shared<Rep> (dsn))
{
}

Database::DBConnection::~DBConnection ()
{
}

unsigned int Database::DBConnection::GetNestedTransactionCount () const
{
    return fRep->fNestedTransactionCount;
}
#endif
