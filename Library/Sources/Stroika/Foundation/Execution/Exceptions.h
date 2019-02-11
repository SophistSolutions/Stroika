/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_h_
#define _Stroia_Foundation_Execution_Exceptions_h_ 1

#include "../StroikaPreComp.h"

#include <cerrno>
#include <exception>
#include <optional>
#include <string>
#include <system_error>

#include "../Characters/String.h"

#include "Throw.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  \note \em Design Note
 *      (essentially) All exceptions thrown by Stroika (except where needed by quirks of underlying library
 *      integrated with) inherit from std::exception, or Stroika::Foundation::Execution::SilentException, or bad_alloc.
 *
 *      This means that any code which wishes to report an exception can catch these three types, and use
 *      the 'what()' method to report the text of the exception message.
 *
 *      Sadly, there is no documentation (I'm aware of) or specification of the characterset/code page reported
 *      back by the what () on an exception. It tends to be ascii. Stroika guarantees that all exceptions it throws
 *      will use the current SDK characters (@see SDKString). But - its best to use Characters::ToString () on the
 *      caught exception, since this uses ExceptionStringHelper to properly handle characters the SDK characterset might not
 *      allow representing some unicode characters.
 *
 * TODO:
 *          @todo   And make FileAccessException subclass from SystemErrorr? instead of StringExcpetion?
 */

namespace Stroika::Foundation::Execution {

    /**
     *  The type of 'errno' variable.
     *
     *  \note   POSIX and C99 just say to assume its an int and doesn't define errno_t.
     *          But I find this type usage clearer.
     */
#if qCompilerAndStdLib_Supports_errno_t
    using errno_t = ::errno_t;
#else
    using errno_t = int;
#endif

    /**
     *  This is a base class for Execution::Exception<> template, which gets mixed with the std c++ exception class, to mix
     *  in Stroika string support.
     *
     *  This probably should NOT be used directly.
     */
    class ExceptionStringHelper {
    public:
        ExceptionStringHelper ()                             = delete;
        ExceptionStringHelper (const ExceptionStringHelper&) = default;
        ExceptionStringHelper (const Characters::String& reasonForError);

    public:
        /**
         * Only implemented for
         *      o   wstring
         *      o   String
         */
        template <typename T>
        nonvirtual T As () const;

    protected:
        nonvirtual const char* _PeekAtNarrowSDKString_ () const;

    private:
        Characters::String fErrorMessage_;
        string             fSDKCharString_;
    };
    template <>
    wstring ExceptionStringHelper::As () const;
    template <>
    Characters::String ExceptionStringHelper::As () const;

    /**
     *  \brief Exception<> is a replacement (subclass) for any std c++ exception class (e.g. the default 'std::exception'), 
     *         which adds UNICODE String support.
     */
    template <typename BASE_EXCEPTION = exception>
    class Exception : public ExceptionStringHelper, public BASE_EXCEPTION {
    private:
        static_assert (is_base_of_v<exception, BASE_EXCEPTION>);

    private:
        using inherited = BASE_EXCEPTION;

    public:
        /**
         */
        Exception ()                 = delete;
        Exception (const Exception&) = default;
        Exception (const Characters::String& reasonForError);

    protected:
        /**
         * For BASE_EXCEPTION classes with constructors OTHER than just 'message' - you cannot use a 'using X = Exception(x)' but a subclass
         * which uses this delegating method.
         */
        template <typename... BASE_EXCEPTION_ARGS>
        explicit Exception (const Characters::String& reasonForError, BASE_EXCEPTION_ARGS... baseExceptionArgs);

    public:
        /**
         *  Provide a 'c string' variant of the exception message. Convert the UNICODE
         *  string argument to a narrow-string (multibyte) in the SDK code page.
         *  @see GetDefaultSDKCodePage()
         */
        virtual const char* what () const noexcept override;
    };

    /**
     *  A wrapper on std::runtime_error, which adds Stroika UNICODE string support.
     */
    using RuntimeErrorException = Exception<runtime_error>;

    /**
     *  Simple wrapper on std::system_error, but adding support for Stroika String, and other utility methods.
     *
     *  \note see https://en.cppreference.com/w/cpp/error/errc for a mapping of errc conditions and ERRNO values.
     *
     *  \note   It's best to not catch (const SystemErrorException&) - and instead catch (const system_error&), since you can
     *          still exactract the UNICODE message with Characters::ToString () - and caching  const SystemErrorException& risks
     *          missing exceptions from non-Stroika sources (which will just throw system_error) or subclasses of system_error such as
     *          filesystem_error.
     *
     *  \par Example Usage
     *      \code
     *          try {
     *              ThrowPOSIXErrNo (make_error_code (errc::bad_address).value ());
     *          }
     *          catch (const std::system_error& e) {
     *              VerifyTestResult (e.code ().value () == make_error_code (errc::bad_address).value ());
     *              VerifyTestResult (e.code ().category () == system_category () or e.code ().category () == generic_category ());
     *              Assert (Characters::ToString (e); == L"bad address {errno: 14}"sv);
     *          }
     *      \endcode
     *
     *  Note this preserves UNICODE characters in messages, even if not using UNICODE code page/locale
     *  \par Example Usage
     *      \code
     *          try {
     *          const Characters::String kMsgWithUnicode_ = L"zß水𝄋"; // this works even if using a code page / locale which doesn't support UNICODE/Chinese
     *          try {
     *              Execution::Throw (SystemErrorException<> (make_error_code (errc::bad_address), kMsgWithUnicode_));
     *          }
     *          catch (const std::system_error& e) {
     *              Assert (Characters::ToString (e).Contains (kMsgWithUnicode_));  // message also includes the number for bad_address
     *          }
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          try {
     *              s.JoinMulticastGroup (UPnP::SSDP::V4::kSocketAddress.GetInternetAddress ());
     *          }
     *          catch (const std::system_error& e) {
     *              if (e.code () == errc::no_such_device) {
     *                  // This can happen on Linux when you start before you have a network connection - no problem - just keep trying
     *                  DbgTrace ("Got exception (errno: ENODEV) - while joining multicast group, so try again");
     *                  Execution::Sleep (1);
     *                  goto Again;
     *              }
     *              else {
     *                  Execution::ReThrow ();
     *              }
     *          }
     *      \endcode
     */
    template <typename BASE_EXCEPTION = system_error>
    class SystemErrorException : public Exception<BASE_EXCEPTION> {
    private:
        static_assert (is_base_of_v<system_error, BASE_EXCEPTION>);

    private:
        using inherited = Exception<BASE_EXCEPTION>;

    public:
        /**
         */
        SystemErrorException (error_code errCode);
        SystemErrorException (error_code errCode, const Characters::String& message);
        SystemErrorException (int ev, const std::error_category& ecat);
        SystemErrorException (int ev, const std::error_category& ecat, const Characters::String& message);

    protected:
        /**
         * For BASE_EXCEPTION classes with constructors OTHER than just 'message' - you cannot use a 'using X = Exception(x)' but a subclass
         * which uses this delegating method.
         *
         *  \note - _PeekAtSDKString_ () will probably have to be among the baseExceptionArgs.
         */
        template <typename... BASE_EXCEPTION_ARGS>
        explicit SystemErrorException (const Characters::String& reasonForError, BASE_EXCEPTION_ARGS... baseExceptionArgs);
    };

    /**
     *  \brief treats errNo as a `POSIX errno` value, and throws a SystemError (subclass of @std::system_error) exception with it.
     *
     *  \req errNo != 0
     *
     *  \note   Translates some throws to subclass of SystemErrorException like TimedException or other classes like bad_alloc.
     *
     *  \note   On a POSIX system, this amounts to a call to ThrowSystemErrNo.
     *          But even on a non-POSIX system, many APIs map their error numbers to POSIX error numbers so this can make sense to use.
     *          Also, on POSIX systems, its legal to call this with POSIX compatible extended (and therefore not POSIX) erorr nubmbers.
     *          In other words, you can call this with anything (except 0) you read out of errno on a POSIX system.
     *
     *  \note  From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf  -
     *          "If the argument ev corresponds to a POSIX errno value posv, the function
     *          shall return error_- condition(posv, generic_category()). Otherwise, the function"
     *
     *  \note this function takes errno as a default value because you almost always want to call it with the value from errno.
     *
     *  See:
     *      @see ThrowSystemErrNo ();
     */
    [[noreturn]] void ThrowPOSIXErrNo (errno_t errNo = errno);

    /**
     *  Look at the argument value and if < 0,ThrowPOSIXErrNo (), and otherwise return it.
     *
     *  \note   Many POSIX - APIs - return a number which is zero if good, or -1 (or sometimes defined negative) if errno is set and there is an error.
     *          This function is useful for wrapping calls to those style functions. It checks if the argument result is negative (so -1 covers that) and
     *          throws and a POSIX (generic_error) SystemErrorException.
     */
    template <typename INT_TYPE>
    INT_TYPE ThrowPOSIXErrNoIfNegative (INT_TYPE returnCode);

    /**
     *  \brief treats sysErr as a platform-defined error number, and throws a SystemErrorException (subclass of @std::system_error) exception with it.
     *
     *  \req sysErr != 0
     *
     *  \note   stdc++ uses 'int' for the type of this error number, but Windows generally defines the type to be
     *          DWORD.
     *
     *  \note   Translates some throws to subclass of SystemErrorException like TimedException or other classes like bad_alloc.
     *
     *   \note  From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf  -
     *          "That object’s category() member shall return std::system_category() for errors originating
     *          from the operating system, or a reference to an implementation"
     *
     *  See:
     *      @see ThrowPOSIXErrNo ();
     */
    [[noreturn]] static void ThrowSystemErrNo (int sysErr);

    /**
     *  \brief Handle UNIX EINTR system call behavior - fairly transparently - just effectively removes them from the set of errors that can be returned
     *
     *  Run the given (argument) call. After each call, invoke Execution::CheckForThreadInterruption ().
     *  If the call returns < 0 and errno == EINTR, repeat the call.
     *  If the result was < 0, but errno != EINTR, then ThrowErrNoIfNegative ();
     *  Then return the result.
     *
     *  \note The only HITCH with respect to automatically handling interuptability is that that its handled by 'restarting' the argument 'call'
     *        That means if it was partially completed, the provider of 'call' must accomodate that fact (use mutable lambda).
     *
     *  This behavior is meant to work with the frequent POSIX API semantics of a return value of < 0
     *  implying an error, and < 0 but errno == EINTR means retry the call. This API also provides a
     *  cancelation point - so it makes otherwise blocking calls (like select, or read) work well with thread
     *  interruption.
     *
     *  \note   ***Cancelation Point***
     */
    template <typename CALL>
    auto Handle_ErrNoResultInterruption (CALL call) -> decltype (call ());

    /**
     *  Check the argument 'return value' from some funciton, and if its null, throw a SystemError exception with
     *  the current errno value.
     *
     *  \note rarely useful, but some POSIX APIs such as getcwd() do return null on error.
     */
    void ThrowPOSIXErrNoIfNull (void* returnValue);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exceptions.inl"

#endif /*_Stroia_Foundation_Execution_Exceptions_h_*/
