/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
 *      Sadly, there is no documentation (I'm aware of) or specification of the character set/code page reported
 *      back by the what () on an exception. It tends to be ascii. Stroika guarantees that all exceptions it throws
 *      will use the current SDK characters (@see SDKString). But - its best to use Characters::ToString () on the
 *      caught exception, since this uses ExceptionStringHelper to properly handle characters the SDK character set might not
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
     *  Stroika's Exception<> class is fully interoperable with the normal C++ exception classes, but its use offers two
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
     *  \brief NestedException contains a new higher level error message (typically based on argument basedOnException)
     *         and preserves the original exception (which you can use to get its message, with Characters::ToString (fBasedOnException)
     * 
     *  This also can be used to wrap third-party libraries exceptions, which don't know about 'DeclareActivity' - and incorporating
     *  that into an error message.
     */
    class NestedException : public RuntimeErrorException<> {
    public:
        /**
         */
        NestedException ()                       = delete;
        NestedException (const NestedException&) = default;
        NestedException (const exception_ptr& basedOnException);
        NestedException (const Characters::String& msg, const exception_ptr& basedOnException);

    public:
        const exception_ptr fBasedOnException;
    };

    /**
     *  Simple wrapper on std::system_error, but adding support for Stroika String, and other utility methods.
     *
     *  \note see https://en.cppreference.com/w/cpp/error/errc for a mapping of errc conditions and ERRNO values.
     *
     *  \note   **Catch `system_error`, not `SystemErrorException`.** Catching the base costs you nothing -
     *          Characters::ToString () still recovers the UNICODE message and the Activity stack (@see Exception<>) -
     *          and it additionally covers what this type does NOT: errors from non-Stroika code (which throws plain
     *          system_error), and Stroika errors built over OTHER system_error subclasses, such as
     *          IO::FileSystem::Exception (an @see Exception<filesystem_error>).
     *
     *  \note   The reverse applies when THROWING: prefer SystemErrorException over a plain std::system_error.
     *          The Activity stack and the UNICODE message are captured when the exception object is CONSTRUCTED
     *          (@see Exception<>), so a plain system_error permanently loses both - the catcher cannot recover
     *          what was never captured. And since SystemErrorException IS a system_error, throwing the
     *          richer type costs nothing. In short: catch broadly (system_error), throw richly
     *          (SystemErrorException, or let @see ThrowError () build it for you).
     *
     *  \note   ***Changed in Stroika v3.0d25*** - this is no longer a template; spell it without the `<>`.
     *          It exists only to add error_code-shaped constructors to Exception<system_error>, and those
     *          constructors are meaningful ONLY for system_error - no other standard system_error subclass has
     *          a constructor taking a bare error_code (filesystem_error, for one, requires a what_arg). The old
     *          `SystemErrorException<BASE_EXCEPTION>` therefore offered a generality it could not deliver.
     *          To mix Stroika's string/Activity support into a DIFFERENT system_error subclass, derive from
     *          @see Exception<THAT_TYPE> directly - which is what IO::FileSystem::Exception now does.
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
     *              Execution::Throw (SystemErrorException (make_error_code (errc::bad_address), kMsgWithUnicode_));
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
     *  \note   The constructors which take no message build one from the error_code, via its category. For a
     *          few codes whose platform wording is poor or actively misleading, Stroika substitutes a clearer
     *          string instead - notably errc::timed_out, which glibc renders "Connection timed out" even for a
     *          mutex or event wait. @see ThrowError for the full rule and rationale.
     *
     *  @see also GetAssociatedErrorCode ()
     */
    class SystemErrorException : public Exception<system_error> {
    private:
        using inherited = Exception<system_error>;

    public:
        /**
         */
        SystemErrorException (error_code errCode);
        SystemErrorException (error_code errCode, const Characters::String& message);
        SystemErrorException (int ev, const error_category& ecat);
        SystemErrorException (int ev, const error_category& ecat, const Characters::String& message);
    };

    /**
     *  \brief treats argument errNo as a `POSIX errno` value, and calls ThrowError () with an error_code constructed from it
     *
     *  \pre errNo != 0
     *
     *  \note   Since this calls @see ThrowError - it will throw a @see SystemErrorException (or a derived type) or bad_alloc.
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
     *  \brief treats argument sysErr as a platform-defined error number, and calls ThrowError () with an error_code constructed from it.
     *
     *  \pre sysErr != 0
     *
     *  \note   Since this calls @see ThrowError - it will throw a @see SystemErrorException (or a derived type) or bad_alloc.
     *
     *  \note   stdc++ uses 'int' for the type of this error number, but Windows generally defines the type to be
     *          DWORD.
     *
     *   \note  From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf  -
     *          "That object’s category() member shall return std::system_category() for errors originating
     *          from the operating system, or a reference to an implementation"
     *
     *  \par Example Usage
     *      \code
     *          #if qStroika_Foundation_Common_Platform_POSIX
     *              ThrowSystemErrNo (errno);
     *          #elif qStroika_Foundation_Common_Platform_Windows
     *              ThrowSystemErrNo (::GetLastError ());      // works with this type of error # - GetLastError () is default if no arg provided
     *              ThrowSystemErrNo (::WSAGetLastError ());   // or this
     *          #endif
     *      \endcode
     *
     *  See:
     *      @see ThrowPOSIXErrNo ();
     *
     *  \note   zero arg versions only defined for POSIX and Windows platforms, and there the default is the obvious value for each
     *          platform - errno and GetLastError(). It is still an assertion (require) error to call these when errno / GetLastError () would return 0.
     */
    [[noreturn]] void ThrowSystemErrNo (int sysErr);
#if qStroika_Foundation_Common_Platform_POSIX or qStroika_Foundation_Common_Platform_Windows
    [[noreturn]] void ThrowSystemErrNo ();
#endif

    /**
     *  \brief throw the given error_code as a Stroika exception, applying the standard type promotions documented below.
     *
     *  \par This throws EITHER a SystemErrorException (a subclass of system_error), OR std::bad_alloc.
     *
     *  Use this - rather than constructing and throwing a SystemErrorException directly - whenever you have an
     *  error_code from ANY error_category: the OS, or a third party library which defines its own category
     *  (libcurl, getaddrinfo/DNS, HRESULT, ...). It is what gives callers one portable story about how errors
     *  surface, independent of where they came from.
     *
     *  \pre ec != error_code{}
     *
     *  \par What a caller must catch
     *      This throws EITHER something derived from std::system_error, OR std::bad_alloc. There is no third
     *      possibility. So a caller which wants to handle everything needs:
     *
     *      \code
     *          try {
     *              ...
     *          }
     *          catch (const system_error& e) {
     *              // everything except memory exhaustion; always test the CONDITION, not the value
     *              if (e.code () == errc::timed_out) { ... }
     *              DbgTrace ("{}"_f, Characters::ToString (e)); // NB: ToString (e), NOT e.what () - see below
     *          }
     *          catch (const bad_alloc&) {
     *              // memory exhaustion ONLY - see below for why this is separate
     *          }
     *      \endcode
     *
     *  \note   **Catch std::system_error, NOT SystemErrorException** - even though today ThrowError () only
     *          ever throws the latter. Catching the base additionally covers errors from non-Stroika code (which
     *          throws plain system_error), and Stroika errors built over other system_error subclasses -
     *          IO::FileSystem::Exception is an Exception<filesystem_error>, and so is NOT a SystemErrorException.
     *          @see the note on SystemErrorException itself, which gives the same advice.
     *
     *          (For reference, the C++ standard defines exactly two subclasses of system_error:
     *          std::ios_base::failure and std::filesystem::filesystem_error. std::future_error is NOT one of
     *          them - it derives from logic_error - and neither are C++20's chrono local-time exceptions.)
     *
     *  \note   **Catching the base costs you nothing - as long as you read the message with
     *          Characters::ToString (e) rather than e.what ().** ToString () does a dynamic_cast to
     *          Execution::ExceptionStringHelper (@see Characters::ToStringDefaults::ToString), and a
     *          dynamic_cast cross-casts, so it finds the Stroika half through ANY base reference -
     *          system_error, or even std::exception. What it returns is As<String> (), which is
     *          GetFullErrorMessage (), so you get the full UNICODE text **with the Activity stack already
     *          merged in** - exactly what you would have got by catching the derived type.
     *
     *          e.what () is the lossy path: it is narrow SDK-charset, so characters the SDK code page cannot
     *          represent do not survive. That is the whole reason Exception<> exists (@see Exception<>).
     *
     *          If you want the activities structurally rather than as message text, dynamic_cast to
     *          Execution::ExceptionStringHelper yourself and call GetActivities ().
     *
     *      The bad_alloc case is the ONLY reason two handlers are needed, and it is deliberate: bad_alloc does
     *      not derive from system_error, because allocation failure is the one error C++ has always expected you
     *      to catch by its own standard type. Folding it in as just another error_code would be more uniform but
     *      would fight decades of convention, so it is called out separately instead.
     *
     *  \par Guaranteed type promotions
     *      The rule is deliberately narrow: **promote only to a type the C++ standard itself already defines for
     *      that condition.** That yields exactly one permanent entry. The second is transitional.
     *
     *      | condition                 | thrown type                   | still a SystemErrorException? |
     *      |---------------------------|-------------------------------|-------------------------------|
     *      | `errc::not_enough_memory` | `std::bad_alloc`              | **no**                        |
     *      | `errc::timed_out`         | `Execution::TimeOutException` | yes - **DEPRECATED**          |
     *
     *      Every other error_code throws a plain SystemErrorException.
     *
     *  \par What is preserved, and what is not
     *      **ThrowError () preserves what the error MEANS. It does not promise to preserve how that meaning is
     *      REPRESENTED.** Where the two conflict it normalizes - sometimes lossily - so the error can be tested
     *      portably. In order of how much is given up:
     *
     *      o   Usually nothing: the error_code is passed through untouched, and only the thrown type is chosen
     *          from it. `e.code ().category ()` still names where the error came from, which is what makes
     *          library-specific diagnosis possible.
     *      o   Windows `WAIT_TIMEOUT` / `ERROR_INTERNET_TIMEOUT` are rewritten to
     *          `make_error_code (errc::timed_out)`, giving up the raw value and the category. MSVC's
     *          system_category does not map either onto that condition, so without the rewrite a genuine
     *          Windows timeout would satisfy no timeout test at all. The raw value survives in the message.
     *      o   `errc::not_enough_memory` becomes `std::bad_alloc`, giving up the error_code entirely - bad_alloc
     *          has no code () - because that is the type C++ has always expected you to catch for allocation
     *          failure. This is also why it needs its own catch handler, and why GetAssociatedErrorCode ()
     *          returns nullopt for it.
     *
     *      So: **test what the error means** - `e.code () == errc::whatever`, or `catch (const bad_alloc&)` -
     *      and none of this normalization is visible to you. Test how it is represented -
     *      `e.code ().value () == SOME_PLATFORM_CONSTANT` - and it is.
     *      @see TranslateException_Impl_ in Exceptions.cpp for the mechanism.
     *
     *      **If you do not want any of that** - if you want the exception object you constructed delivered to
     *      the catcher unchanged - then say so by using @see Throw () instead. That is the whole distinction
     *      between the two: ThrowError () normalizes, possibly lossily, in exchange for portability; Throw ()
     *      is literal.
     *
     *      The narrow rule is what stops the table being arbitrary. The obvious question about any such table is
     *      "why this condition and not permission_denied, or connection_refused?" - and the answer is that C++
     *      defines std::bad_alloc and defines no counterpart for the others. Nothing is invented.
     *
     *  \note   ***DEPRECATED since v3.0d25: the errc::timed_out row.*** @see TimeOutException. It is retained
     *          purely so existing `catch (const TimeOutException&)` clauses keep matching; nothing in Stroika
     *          names that type any more. Write `e.code () == errc::timed_out` instead - that also catches
     *          timeouts raised OUTSIDE Stroika, which catching by type never did (libcurl's
     *          CURLE_OPERATION_TIMEDOUT being the case that forced the issue).
     *
     *  \note   That row and the TimeOutException class must be removed TOGETHER. Dropping the promotion while
     *          leaving the class would leave every such catch clause still compiling, and silently never firing -
     *          strictly worse than the compile error you get when both go.
     *  \par How Stroika treats errc::timed_out, and what its default message says
     *      **Stroika treats errc::timed_out as the generic "the time limit for this operation expired"
     *      condition** - nothing connection-specific. Every Stroika timeout answers to it: a Synchronized<>
     *      lock acquisition, Thread::WaitForDone (), a WaitableEvent, a socket read, a libcurl transfer. That
     *      uniformity is what makes `e.code () == errc::timed_out` the one portable timeout test.
     *
     *      That is the condition's actual meaning, not a Stroika invention. POSIX specifies ETIMEDOUT - which
     *      std::errc::timed_out names - as the result of pthread_mutex_timedlock (), pthread_cond_timedwait (),
     *      sem_timedwait () and mq_timedreceive (), none of which involve a connection.
     *
     *      \note ***Changed in Stroika v3.0d25*** - the DEFAULT message now reflects that. glibc renders
     *            ETIMEDOUT as "Connection timed out" (a BSD-sockets legacy - ETIMEDOUT began life as a
     *            connect () error), while MSVC renders it "timed out": so before v3.0d25 the very same lock
     *            timeout was described differently on each platform, and misleadingly on one. Stroika now
     *            substitutes **"Operation timed out"** when it builds a default message for errc::timed_out in
     *            `generic_category ()` - which is exactly the case where STROIKA decided something timed out,
     *            since make_error_code () always yields that category. A timeout reported by the OS arrives in
     *            `system_category ()` (@see ThrowPOSIXErrNo) and keeps the platform's own wording, which is
     *            accurate there. A message you pass explicitly is never touched.
     *
     *            Nothing portable is being overridden: the standard specifies error_category::message () only
     *            as "a string that describes the error condition", so the text is implementation-defined and
     *            the two platforms already disagreed. @see SystemErrorException, whose constructor is where a
     *            default message actually gets built.
     *
     *  \par Relationship to ThrowPOSIXErrNo () and ThrowSystemErrNo ()
     *      All three funnel through here and behave identically once they do. They differ ONLY in which
     *      error_category the raw number gets tagged with - that is, in how the error_code is built:
     *
     *      | function               | category used                                             |
     *      |------------------------|-----------------------------------------------------------|
     *      | `ThrowError (ec)`      | whatever category you put in the error_code               |
     *      | `ThrowSystemErrNo (n)` | `system_category ()`                                      |
     *      | `ThrowPOSIXErrNo (n)`  | `system_category ()` on POSIX, else `generic_category ()` |
     *
     *      So on a POSIX system ThrowPOSIXErrNo and ThrowSystemErrNo are the same call - errno IS the
     *      platform's native error space there, so one subsumes the other. They diverge only on Windows,
     *      where errno and GetLastError () are distinct numbering spaces and must not be tagged with the
     *      same category. Pick whichever names where your number actually came from.
     *
     *      **The category is not just a label.** It supplies default_error_condition (), which is what every
     *      `e.code () == errc::something` test - and the bad_alloc promotion above - is matched against. So
     *      tagging a number with the wrong category does not merely mislabel the error, it silently changes
     *      which conditions the error appears to satisfy. A Win32 timeout code tagged with system_category ()
     *      compares equal to errc::timed_out; the same number tagged with generic_category () is interpreted
     *      as an errno instead, and matches something else or nothing at all. That - not tidiness - is why
     *      these two functions must stay separate on Windows.
     *
     *  \note   Matching is driven by the CONDITION, not the raw value, so any category which maps its own
     *          numbering onto std::errc (as the LibCurl and DNS categories both do) participates automatically.
     *          That is precisely what error_category::default_error_condition is for - a category author gets
     *          this simply by writing a correct mapping.
     *
     *  \par Example Usage
     *      \code
     *          // wrapping a third party library which has its own error_category
     *          if (status != kThirdPartyOK) [[unlikely]] {
     *              ThrowError (error_code{status, ThirdParty_error_category ()});
     *          }
     *      \endcode
     *
     *  \par Raising a portable condition directly - the std::errc overload
     *      When the error did not come from anywhere with its own numbering - Stroika itself has decided the
     *      operation timed out, say - there is no category to name, and `ThrowError (errc::timed_out)` says so
     *      directly:
     *
     *      \code
     *          ThrowError (errc::timed_out, "Timeout Expired"sv);                      // instead of
     *          ThrowError (make_error_code (errc::timed_out), "Timeout Expired"sv);
     *      \endcode
     *
     *      This is exactly `ThrowError (make_error_code (ec))` and nothing else. It is unambiguous with the
     *      error_code overloads because `is_error_code_enum_v<errc>` is **false** - std::errc has no implicit
     *      conversion to error_code - so overload resolution cannot pick the wrong one.
     *
     *  \note   Use the error_code overload whenever you HAVE a category. std::errc names a *condition*, and
     *          make_error_code () resolves it to `generic_category ()` - it asserts the error originated as a
     *          POSIX errno. That is true when Stroika raises the error itself; it is NOT true of an error handed
     *          back by the OS or a third-party library, and flattening one of those to an errc discards where it
     *          came from. Since `e.code () == errc::whatever` matches either way, nothing is gained by
     *          discarding the category.
     *
     *  See:
     *      @see ThrowPOSIXErrNo ();
     *      @see ThrowSystemErrNo ();
     */
    [[noreturn]] void ThrowError (error_code ec);
    [[noreturn]] void ThrowError (error_code ec, const Characters::String& message);
    [[noreturn]] void ThrowError (errc ec);
    [[noreturn]] void ThrowError (errc ec, const Characters::String& message);

    /**
     *  \brief Handle UNIX EINTR system call behavior - fairly transparently - just effectively removes them from the set of errors that can be returned
     *
     *  Run the given (argument) call. After each call, invoke Thread::CheckForInterruption ().
     *  If the call returns < 0 and errno == EINTR, repeat the call.
     *  If the result was < 0, but errno != EINTR, then ThrowErrNoIfNegative ();
     *  Then return the result.
     *
     *  \note The only HITCH with respect to automatically handling interruptability is that that its handled by 'restarting' the argument 'call'
     *        That means if it was partially completed, the provider of 'call' must accommodate that fact (use mutable lambda).
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
