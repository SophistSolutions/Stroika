/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <condition_variable>

#include "Stroika/Foundation/Time/Common.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ***************************** DEPRECATED (v3.0d25) *****************************
     ********************************************************************************
     */
    /**
     *  \deprecated Since Stroika v3.0d25 - use ThrowError (errc::timed_out).
     */
    [[deprecated ("Since Stroika v3.0d25 - use ThrowError (errc::timed_out)")]] [[noreturn]] void ThrowTimeOutException ();

    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    /**
     *  \deprecated Since Stroika v3.0d25 - catch the CONDITION instead of this type:
     *      \code
     *          catch (const system_error& e) {
     *              if (e.code () == errc::timed_out) { ... }
     *          }
     *      \endcode
     *      That also matches timeouts raised outside Stroika, which catching by type never did.
     */
    class [[deprecated ("Since Stroika v3.0d25 - catch (const system_error&) and test e.code () == errc::timed_out")]] TimeOutException
        : public Execution::SystemErrorException {
    public:
        TimeOutException ();
        TimeOutException (error_code ec);
        TimeOutException (const Characters::String& message);
        TimeOutException (error_code ec, const Characters::String& message);

    public:
        static const TimeOutException kThe;
    };
    inline const TimeOutException TimeOutException::kThe;
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    /*
     ********************************************************************************
     ******************* Execution::TimeOutException [[deprecated]] *****************
     ********************************************************************************
     */
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    inline TimeOutException::TimeOutException (error_code ec, const Characters::String& message)
        : SystemErrorException{ec, message}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

}
