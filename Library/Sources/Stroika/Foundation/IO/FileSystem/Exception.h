/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Exception_h_
#define _Stroika_Foundation_IO_FileSystem_Exception_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Execution/Exceptions.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 */

namespace Stroika::Foundation::IO::FileSystem {

    using Characters::String;

    /**
     *  Simple wrapper on std::filesystem_error, but adding support for Stroika String, and other utility methods.
     *
     *  \note see https://en.cppreference.com/w/cpp/error/errc for a mapping of errc conditions and ERRNO values.
     *
     *  \par Example Usage
     *      \code
     *          try {
     *              FileSystem::ThrowPOSIXErrNo (make_error_code (errc::filename_too_long).value ());
     *          }
     *          catch (const std::filesystem_error& e) {
     *              VerifyTestResult (e.code ().value () == make_error_code (errc::filename_too_long).value ());
     *              VerifyTestResult (e.code ().category () == system_category () or e.code ().category () == generic_category ());
     *          }
     *      \endcode
     *
      *  \par Example Usage (can catch as system_error as well since thats a base class of filesystem_error)
     *      \code
     *          try {
     *              FileSystem::ThrowPOSIXErrNo (make_error_code (errc::filename_too_long).value ());
     *          }
     *          catch (const std::system_error& e) {
     *              VerifyTestResult (e.code ().value () == make_error_code (errc::filename_too_long).value ());
     *              VerifyTestResult (e.code ().category () == system_category () or e.code ().category () == generic_category ());
     *          }
     *      \endcode
     *
     */
    class Exception : public Execution::SystemErrorException<filesystem_error> {
    private:
        using inherited = Execution::SystemErrorException<filesystem_error>;

    public:
        /**
         */
        Exception (error_code errCode, const path& p1 = {}, const path& p2 = {});
        Exception (error_code errCode, const Characters::String& message, const path& p1 = {}, const path& p2 = {});

    public:
        /**
         *  \brief treats errNo as a `POSIX errno` value, and throws a FileSystem::Exception (subclass of @std::filesystem_error) exception with it.
         *
         *  \req errNo != 0
         *  \req if (p1.empty() then require (p2.empty ()); but neither is required
         *
         *  See:
         *      @see SystemErrorException<>::ThrowPOSIXErrNo ();
         *      @see ThrowSystemErrNo ();
         */
        [[noreturn]] static void ThrowPOSIXErrNo (errno_t errNo, const path& p1 = {}, const path& p2 = {});

    public:
        /**
         *  \brief treats errNo as a platform-defined error number, and throws a FileSystem::Exception (subclass of @std::filesystem_error) exception with it.
         *
         *  \req sysErr != 0
         *  \req if (p1.empty() then require (p2.empty ()); but neither is required
         *
         *  See:
         *      @see SystemErrorException<>::ThrowSystemErrNo ();
         *      @see ThrowPOSIXErrNo ();
         */
        [[noreturn]] static void ThrowSystemErrNo (int sysErr, const path& p1 = {}, const path& p2 = {});

    private:
        static Characters::String mkMsg_ (error_code errCode, const path& p1, const path& p2);
        static Characters::String mkMsg_ (error_code errCode, const Characters::String& message, const path& p1, const path& p2);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_IO_FileSystem_Exception_h_*/
