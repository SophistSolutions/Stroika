﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_h_
#define _Stroia_Foundation_Execution_Exceptions_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cerrno>
#include <exception>
#include <optional>
#include <string>
#include <system_error>

#include "Stroika/Foundation/Characters/String.h"

#include "Activity.h"
#include "Throw.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  \note \em Design Note
 *      (essentially) All exceptions thrown by Stroika (except where needed by quirks of underlying library
 *      integrated with) inherit from std::exception, or are Stroika::Foundation::Execution::SilentException.
 *
 *      This means that any code which wishes to report an exception can catch these two types, and use
 *      the 'what()' method to report the text of the exception message.
 *
 *      Sadly, there is no documentation (I'm aware of) or specification of the characterset/code page reported
 *      back by the what () on an exception. It tends to be ascii. Stroika guarantees that all exceptions it throws
 *      will use the current SDK characters (@see SDKString). But - its best to use Characters::ToString () on the
 *      caught exception, since this uses ExceptionStringHelper to properly handle characters the SDK characterset might not
 *      allow representing some unicode characters.
 *
 *  TODO:
 *      @todo   COULD MAYBE add 'suppress-activities' object you can declare and while it exists (it sets private thread local storage flag)
 *              activities are not merged into message. But this can be done other ways too and doesn't address other stages of message
 *              formation so I'm not sure that's worth while.
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
        /**
         *  If the current activities are NOT provided explicitly, they are copied from Execution::CaptureCurrentActivities ().
         */
        ExceptionStringHelper ()                             = delete;
        ExceptionStringHelper (const ExceptionStringHelper&) = default;
        ExceptionStringHelper (const Characters::String& reasonForError);
        ExceptionStringHelper (const Characters::String& reasonForError, const Containers::Stack<Activity<>>& activities);

    public:
        /**
         *  Return error message without added 'activities'.
         */
        nonvirtual Characters::String GetBasicErrorMessage () const;

    public:
        /**
         *  Return error message with added 'activities'
         */
        nonvirtual Characters::String GetFullErrorMessage () const;

    public:
        /**
         *  Return the activity stack from when the exception was thrown. NOTE - see @Activity<>. This has little
         *  todo with the thread runtime stack. It refers to a logical stack of declared Activity<> objects.
         */
        nonvirtual Containers::Stack<Activity<>> GetActivities () const;

    public:
        /**
         * Only implemented for
         *      o   wstring
         *      o   String
         *
         * This returns the message callers should display to represent the error (e.g. in exception::c_str ()).
         * For now, it returns GetFullErrorMessage () - but may someday change.
         */
        template <typename T>
        nonvirtual T As () const;

    protected:
        /**
         *  In order for subclasses to support the c++ exception::c_str () API, we need to convert
         *  the string/message to an SDKChar string, and have that lifetime be very long. So we store it
         *  in a std::string. And this function returns the pointer to that string. This object is
         *  immutable, so that the lifetime of the underlying return const char* is as long as this object.
         */
        nonvirtual const char* _PeekAtNarrowSDKString_ () const;

    private:
        Containers::Stack<Activity<>> fActivities_;
        Characters::String            fRawErrorMessage_;
        Characters::String            fFullErrorMessage_;
        string                        fSDKCharString_; // important declared after others cuz mem-initializer refers back
    };
    template <>
    wstring ExceptionStringHelper::As () const;
    template <>
    Characters::String ExceptionStringHelper::As () const;

    /**
     *  \brief Exception<> is a replacement (subclass) for any std c++ exception class (e.g. the default 'std::exception'), 
     *         which adds UNICODE String support.
     *
     *  Stroika's Exception<> class is fully interoperable with the normal C++ exeption classes, but its use offers two
     *  benefits:
     *      o   It guarantees that UNICODE messages (including things like filenames) are properly preserved in the exception
     *          message, even if the system default code page (locale) does not allow representing those characters.
     *
     *      o   It integrates neatly with the Stroika 'Activity' mechanism, whereby you declare current 'activities' on the stack
     *          and these are automatically integrated into exceptions to provide clearer messages (e.g. instead of
     *          getting the error message 
     *              o   "(errno: 13)" OR 
     *              o   "Permission denied"
     *          Stroika returns something like:
     *              o   "Permission denied {errno: 13} while binding to INADDR_ANY:80, while constructing static content webserver."
     *
     *          @see Activity<>
     *          @see DeclareActivity
     *
     *  \par Example Usage
     *      \code
     *          static constexpr Activity   kBuildingThingy_ {"building thingy"sv };
     *          try {
     *              DeclareActivity declareActivity { &kBuildingThingy_ };
     *              doBuildThing  ();   // throw any exception (that inherits from Exception<>)
     *          }
     *          catch (...) {
     *              String exceptionMsg = Characters::ToString (current_exception ());
     *              Assert (exceptionMsg.Contains (kBuildingThingy_.AsString ());       // exception e while building thingy...
     *          }
     *      \endcode
     */
    template <typename BASE_EXCEPTION = exception>
    class Exception : public ExceptionStringHelper, public BASE_EXCEPTION {
    private:
        static_assert (derived_from<BASE_EXCEPTION, exception>);

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
         *  string argument to a narrow-string (multibyte) in the current locale encoding.
         */
        virtual const char* what () const noexcept override;
    };

    /**
     *  A wrapper on std::runtime_error, which adds Stroika UNICODE string support.
     *
     *  According to https://en.cppreference.com/w/cpp/error/runtime_error
     *      It reports errors that are due to events beyond the scope of the
     *      program and can not be easily predicted.
     *
     *  This isn't super clear, but Stroika interprets this to mean external conditions - files, networks, memory etc - essentially
     *  ALL the things Stroika treats as exceptions.
     */
    template <typename BASE_EXCEPTION = runtime_error>
    class RuntimeErrorException : public Exception<BASE_EXCEPTION> {
    public:
        /**
         */
        RuntimeErrorException (const Characters::String& msg);
    };

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
     *              EXPECT_TRUE (e.code ().value () == make_error_code (errc::bad_address).value ());
     *              EXPECT_TRUE (e.code ().category () == system_category () or e.code ().category () == generic_category ());
     *              Assert (Characters::ToString (e).Contains ("bad address {errno: 14}"));
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
     *
     *  @see also GetAssociatedErrorCode ()
     */
    template <typename BASE_EXCEPTION = system_error>
    class SystemErrorException : public Exception<BASE_EXCEPTION> {
    private:
        static_assert (derived_from<BASE_EXCEPTION, system_error>);

    private:
        using inherited = Exception<BASE_EXCEPTION>;

    public:
        /**
         */
        SystemErrorException (error_code errCode);
        SystemErrorException (error_code errCode, const Characters::String& message);
        SystemErrorException (int ev, const error_category& ecat);
        SystemErrorException (int ev, const error_category& ecat, const Characters::String& message);

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
     *
     *  \note   zero arg versions only defined for POSIX and Windows platforms, and there the default is the obvious value for each
     *          platform - errno and GetLastError(). It is still an assertion (require) error to call these when errno / GetLastError () would return 0.
     */
    [[noreturn]] void ThrowSystemErrNo (int sysErr);
#if qPlatform_POSIX or qPlatform_Windows
    [[noreturn]] void ThrowSystemErrNo ();
#endif

    /**
     *  \brief Handle UNIX EINTR system call behavior - fairly transparently - just effectively removes them from the set of errors that can be returned
     *
     *  Run the given (argument) call. After each call, invoke Thread::CheckForInterruption ().
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
     *  Check the argument 'return value' from some function, and if its null, throw a SystemError exception with
     *  the current errno value.
     *
     *  \note rarely useful, but some POSIX APIs such as getcwd() do return null on error.
     */
    void ThrowPOSIXErrNoIfNull (void* returnValue);

    /**
     *  This checks if the given exception_ptr is of a type that contains an error code, and if so
     *  it extracts the error code, and returns it (else nullopt).
     *
     *  \par Example Usage
     *      \code
     *          try {
     *              /// do something that throws
     *          }
     *          catch (...) {
     *              if (auto err = GetAssociatedErrorCode (current_exception ())) {
     *                  if (*err == errc::no_such_device) {
     *                      // This can happen on Linux when you start before you have a network connection - no problem - just keep trying
     *                      DbgTrace ("Got exception (errno: ENODEV) - while joining multicast group, so try again");
     *                      Execution::Sleep (1);
     *                      goto Again;
     *                  }
     *                  else {
     *                      Execution::ReThrow ();
     *                  }
     *              }
     *          }
     *      \endcode
     */
    optional<error_code> GetAssociatedErrorCode (const exception_ptr& e) noexcept;

    /**
     *  Wrap the the argument function (typically a lambda) in an OPTIONAL of the argument type, and return nullopt - dropping the exception
     *  on the floor.
     * 
     *  @todo When we support std c++23, do likewise for expected!!!
     */
    template <typename F>
    inline auto TranslateExceptionToOptional (F&& f) -> optional<remove_cvref_t<invoke_result_t<F>>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exceptions.inl"

#endif /*_Stroia_Foundation_Execution_Exceptions_h_*/
