/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Assertions_h_
#define _Stroika_Foundation_Debug_Assertions_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

#include "CompileTimeFlagChecker.h"

/**
 *  \file
 *
 *  \quote
 *      "Truth emerges more readily from error than from confusion." 
 *          - Francis Bacon
 *
 *  TODO:
 *      @todo   Find some way to map Require(), RequireNotNull(), etc to things picked up automatically
 *              by Doxygen. Right now - I must manually map them to \\pre statements in the function header.
 *
 */

namespace Stroika::Foundation::Debug {

    /**
     *  \brief Stroika_Foundation_Debug_Widen is used mostly internally to expand macros like Stroika_Foundation_Debug_Widen ( __FILE__) into UNICODE SAFE strings (so debug messages work properly if run on locale with charset other than matching filesystem)
     *  Idea from https://stackoverflow.com/questions/3291047/how-do-i-print-the-string-which-file-expands-to-correctly
     */
#define Stroika_Foundation_Debug_Widen2_(x) L##x
#define Stroika_Foundation_Debug_Widen(x) Stroika_Foundation_Debug_Widen2_ (x)

#if qDebug || defined(__Doxygen__)
    /**
     *  Note: Some any parameters may be null, if no suitable value is available.
     *
     *  \note   AssertionHandlers shall NOT throw an exception (but I don't think we can declare typedef as noexcept)
     *          The reason for this is so that assertions can be used safely in circumstances that don't expect
     *          exceptions.
     *
     *          Assertion handlers typically just print a debug message, and then exit the program. They are fatal, and must not return/throw.
     */
    using AssertionHandlerType = void (*) (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName,
                                           int lineNum, const wchar_t* functionName) noexcept;

    /**
     *  Stroika makes very heavy use of assertions (to some extent inspired and
     *  patterned after Bertrand Meyers Eiffel assertion mechanism). Assertions are logical
     *  statements about function parameters, results, object states, etc, which are guaranteed
     *  (required) to be held true. These logical assertions serve two important functions:
     *  they <em>document</em> the requirements on input parameters for a function,
     *  and the promises made on function return. And they perform this documentation task <em>
     *  in such a way that the compiler can generate special code (conditionally) to verify that
     *  the assertions hold true</em>.
     *
     *  This latter point about conditional compilation is important. If the macro
     *  preprocessor symbol <code>qDebug</code> is true (non-zero), then this assertion checking is
     *  enabled. If the symbol is false (zero), then the checking is disabled. <b>Of course the
     *  promises must always hold true!</b> But since the checking can significantly slow the code,
     *  it is best to only build with assertions on in certain circumstances (like while developing,
     *  and for much of QA/testing); but for released products it should be disabled so the editor
     *  operates at maximum speed.
     *
     *  Stroika's assertion mechanism is not only present to help debug Stroika itself.
     *  After all, that would have little value to a user of the Stroika class library. It is also
     *  very helpful to a programmer using the class library. This is because nearly all the
     *  parameters passed to Stroika functions are immediately checked for validity, so mistakes
     *  are trapped as early as possible. If you pass bad values to a Stroika function, you will
     *  very likely get a debugger trap at almost exactly the point in your calling code where
     *  the error occurred. This can make debugging code written using Stroika much easier.
     *
     *  Stroika provides four families of 'assertion' macro functions. The are named
     *          <code><em>Assert</em></code>,
     *          <code><em>Require</em></code>,
     *          <code><em>Ensure</em></code>, and
     *          <code><em>Verify</em></code>.
     *
     *  The family you will most often be interested in is <code><em>Require</em></code>s.
     *  This is because these are used to check parameters validity on entry to a function. So
     *  typically when you see a <em>Require</em> fail, you will find that it is the calling
     *  function which is passing invalid arguments to the function which triggered the requirement
     *  failure. The other reason <em>Requires</em> will be of particular interest to programmers
     *  using Stroika is because checking the <em>Require</em> declarations at the beginning of a
     *  function is often very telling about the details of what sort of parameters the function
     *  expects.
     *
     *  Probably the next most common sort of assertion you will see is <em>Ensures</em>.
     *  These state <em>promises</em> about the return values of a function. These should very rarely
     *  be triggered (if they are , they almost always indicate a bug in the function that triggered
     *  the Ensure failure). But the reason they are still of interest to programmers using Stroika
     *  is because they document what can be assumed about return values from a particular function.
     *
     *  Plain <em>Assertions</em> are for assertions which don't fall into any of the above
     *  categories, but are still useful checks to be performed. When an assertion is triggered,
     *  it is almost always diagnostic of a bug in the code which triggered the assertion (or
     *  corrupted data structures). (aside: Assertions logically mean the same thing as Ensures,
     *  except that Ensures only check return values).</p>
     *
     *  <em>Verify</em>s are inspired by the MFC VERIFY() macro, and the particular
     *  idiosyncrasies of the Win32 SDK, though they can be used cross-platform. Many of the Win32 SDK
     *  routines return non-zero on success, and zero on failure. Most sample Win32 code you look at
     *  simply ignores these results. For paranoia sake (which was very helpful in the old moldy win32s
     *  days) I wrap most Win32 SDK calls in a <em><code>Verify</code></em> wrapper. This - when
     *  debugging is enabled - checks the return value, and asserts if there is a problem.
     *  <b>Very Important: Unlike the other flavors of Assertions, Verify always evaluates its
     *  argument!</b>.
     *
     *  This last point is worth repeating, as it is the only source of bugs I've ever seen
     *  introduced from the use of assertions (and I've seen the mistake more than once). <b>All
     *  flavors of assertions (except Verify) do NOT evaluate their arguments if <code>qDebug</code>
     *  is off</b>. This means you <b>cannot</b> count on the arguments to assertions having any
     *  side-effects. Use <em>Verify</em> instead of the other assertion flavors if you want to
     *  only check for compliance if <em><code>qDebug</code></em> is true, but want the side-effect
     *  to happen regardless.
     *
     *  Now when assertions are triggered, just what happens? Here I think there is only one
     *  sensible answer. And that is that the program drops into the debugger. And what happens
     *   after that is undefined. This is Stroika's default behavior.
     *
     *  But there are others who hold the view that triggered assertions should generate
     *  exceptions. This isn't an appropriate forum for explanations of why this is generally
     *  a bad idea. Instead, I simply provide the flexibility to allow those who want todo this
     *  that ability. There are <code>SetAssertionHandler</code> and <code>GetAssertionHandler</code>
     *  functions which allow the programmer to specify an alternate assertion handling scheme. The
     *  only assumption Stroika makes about this scheme is that the assertion handling function not
     *  return (so it must either exit the program, or longjump/throw). Led makes no guarantee that
     *  attempts to throw out past an assertion will succeed.
     *
     *  GetAssertionHandler() never returns nullptr - it always returns some handler.
     *
     */
    AssertionHandlerType GetAssertionHandler ();

    /**
     */
    AssertionHandlerType GetDefaultAssertionHandler ();

    /**
     *  See @'GetAssertionHandler'. If SetAssertionHandler() is called with nullptr,
     *  then this merely selects the default assertion handler.
     */
    void SetAssertionHandler (AssertionHandlerType assertionHandler);

    /**
     */
    AssertionHandlerType GetWeakAssertionHandler ();

    /**
    */
    AssertionHandlerType GetDefaultWeakAssertionHandler ();

    /**
     *  See @'GetAssertionHandler'. If SetAssertionHandler() is called with nullptr,
     *  then this merely selects the default assertion handler.
     */
    void SetWeakAssertionHandler (AssertionHandlerType assertionHandler);

    namespace Private_ {
        // don't call directly - implementation detail...
        // NOTE - take strings as wchar_t so no lose UNICODE, but functionName as char* since I cannot figure out so far how to get UNICODE filename
        [[noreturn]] void Assertion_Failure_Handler_ (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName,
                                                      int lineNum, const char* functionName) noexcept;
        // don't call directly - implementation detail...
        // NOTE - take strings as wchar_t so no lose UNICODE, but functionName as char* since I cannot figure out so far how to get UNICODE filename
        void Weak_Assertion_Failure_Handler_ (const wchar_t* assertCategory, const wchar_t* assertionText, const wchar_t* fileName,
                                              int lineNum, const char* functionName) noexcept;

        /**
         * Private implementation utility macro
         *      \note these are ASCII - not wchar_t for now, since not clear how to get UNICODE here? --LGP 2024-04-01
         */
#if !defined(__Doxygen__)
#if qCompilerAndStdLib_Support__PRETTY_FUNCTION__
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ __PRETTY_FUNCTION__
#elif qCompilerAndStdLib_Support__func__
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ __func__
#elif qCompilerAndStdLib_Support__FUNCTION__
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ __FUNCTION__
#else
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ L""
#endif
#endif
    }
#endif

/**
 *  \def AssertExpression(c)
 *
 *  Like Assert(), but without [[assume]] support, and in the form of an expression (since https://en.cppreference.com/w/cpp/language/attributes/assume - can only be applied to a statement)
 */
#if qDebug
#define AssertExpression(c)                                                                                                                \
    (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Assert", Stroika_Foundation_Debug_Widen (#c),           \
                                                                                 Stroika_Foundation_Debug_Widen (__FILE__), __LINE__,      \
                                                                                 ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_),                 \
               false))
#else
#define AssertExpression (c) ((void)0)
#endif

    /**
     *  \def Assert(c)
     *
     *  \note   logically
     *          if (!(c)) {
     *              Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Assert", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_); }
     *          }
     *
     *  \note As of C++23, Stroika uses the [[assume(X)]] attribute in the case of qDebug false. This means that - though the arguments will not be evaluated in a release
     *        build, they must be syntactic (new requirement in Stroika v3.0).
     * 
     *  @see GetAssertionHandler
     *
     *  \hideinitializer
     */
#if qDebug
#define Assert(c) AssertExpression (c)
#else
#define Assert (c) _ASSUME_ATTRIBUTE_ (c)
#endif

    /**
     *  \def RequireExpression(c) - alias for AssertExpression(), but with a different message: used at the start of a function to declare calling conventions - expected arguments to the function
     *  in debug builds (qDebug true) - check the given requirement and trigger GetAssertionHandler () if false (terminates program); 
     *
     *  result is EXPRESSION;
     */
#if qDebug
#define RequireExpression(c)                                                                                                               \
    (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Require", Stroika_Foundation_Debug_Widen (#c),          \
                                                                                 Stroika_Foundation_Debug_Widen (__FILE__), __LINE__,      \
                                                                                 ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_),                 \
               false))
#else
#define RequireExpression(c) ((void)0)
#endif

/**
 *  \def Require(c) - alias for Assert(), but with a different message upon failure, and used to declare an assertion about the incoming contract - arguments to a function.
 *
 *  \note As of C++23, Stroika uses the [[assume(X)]] attribute in the case of qDebug false. This means that - though the arguments will not be evaluated in a release
 *        build, they must be syntactic (new requirement in Stroika v3.0).
 */
#if qDebug
#define Require(c) RequireExpression (c)
#else
#define Require(c) _ASSUME_ATTRIBUTE_ (c)
#endif

/**
 *  \def EnsureExpression(c) - alias for AssertExpression(), but with a different message, and used to declare an assertion promised about the state at the end of a function.
 */
#if qDebug
#define EnsureExpression(c)                                                                                                                \
    (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Ensure", Stroika_Foundation_Debug_Widen (#c),           \
                                                                                 Stroika_Foundation_Debug_Widen (__FILE__), __LINE__,      \
                                                                                 ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_),                 \
               false))
#else
#define EnsureExpression(c) ((void)0)
#endif

/**
 *  \def Ensure(c) - alias for Assert(), but with a different message upon failure, and used to declare an assertion promised about the state at the end of a function.
 *
 *  \note As of C++23, Stroika uses the [[assume(X)]] attribute in the case of qDebug false. This means that - though the arguments will not be evaluated in a release
 *        build, they must be syntactic (new requirement in Stroika v3.0).
 */
#if qDebug
#define Ensure(c) EnsureExpression (c)
#else
#define Ensure(c) _ASSUME_ATTRIBUTE_ (c)
#endif

/**
 *  \def AssertMember(p,c)
 *
 *  Simple wrapper on Assert () - checking p is a member of class c (with dynamic_cast)
 */
#define AssertMember(p, c) Assert (dynamic_cast<const c*> (p) != nullptr)

/**
 *  \def EnsureMember(p,c)
 *
 *  Simple wrapper on Ensure () - checking p is a member of class c (with dynamic_cast)
 */
#define EnsureMember(p, c) Ensure (dynamic_cast<const c*> (p) != nullptr)

/**
 *  \def RequireMember(p,c)
 *
 *  @see GetAssertionHandler
 */
#define RequireMember(p, c) Require (dynamic_cast<const c*> (p) != nullptr)

/**
 *  \def AssertNotNull(p)
 *
 *  @see GetAssertionHandler
 */
#define AssertNotNull(p) Assert (p != nullptr)

/**
 *  \def EnsureNotNull(p)
 *
 *  @see GetAssertionHandler
 */
#define EnsureNotNull(p) Ensure (p != nullptr)

/**
 *  \def RequireNotNull(p)
 *
 *  @see GetAssertionHandler
 */
#define RequireNotNull(p) Require (p != nullptr)

/**
 *  \def AssertNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#if qDebug
#define AssertNotReached()                                                                                                                  \
    Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Assert", L"Not Reached", Stroika_Foundation_Debug_Widen (__FILE__), \
                                                                      __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)
#elif __cpp_lib_unreachable < 202202
#define AssertNotReached()
#else
#define AssertNotReached() unreachable ()
#endif

/**
 *  \def EnsureNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#if qDebug
#define EnsureNotReached()                                                                                                                  \
    Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Ensure", L"Not Reached", Stroika_Foundation_Debug_Widen (__FILE__), \
                                                                      __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)
#elif __cpp_lib_unreachable < 202202
#define EnsureNotReached()
#else
#define EnsureNotReached() unreachable ()
#endif

/**
 *  \def RequireNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#if qDebug
#define RequireNotReached()                                                                                                                  \
    Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Require", L"Not Reached", Stroika_Foundation_Debug_Widen (__FILE__), \
                                                                      __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)
#elif __cpp_lib_unreachable < 202202
#define RequireNotReached()
#else
#define RequireNotReached() unreachable ()
#endif

/**
 *  \def AssertNotImplemented()
 *
 *  Use  this to mark code that is not yet implemented. Using this name for sections of code which fail because of not being implemented
 *  makes it easier to search for such code, and when something breaks (esp during porting) - its easier to see why
 *
 *  \hideinitializer
 */
#if qDebug
#define AssertNotImplemented()                                                                                                                  \
    Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ (L"Assert", L"Not Implemented", Stroika_Foundation_Debug_Widen (__FILE__), \
                                                                      __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)
#else
#define AssertNotImplemented()
#endif

/**
 *  \def Verify(c)
 *
 *  Verify () is an 'assertion' like Assert, except its argument is ALWAYS
 *  EVALUATED, even if debug is OFF. This is useful for cases where you just want
 *  todo an assertion about the result of a function, but don't want to keep the
 *  result in a temporary just to look at it for this one assertion test...
 *
 *  @see GetAssertionHandler
 */
#if qDebug
#define Verify(c) Assert (c)
#else
#define Verify(c) ((void)(c))
#endif

/**
 *  \brief  A WeakAssert() is for things that aren't guaranteed to be true, but are overwhelmingly likely to be true. Use this so you see debug logs of rare events you way want to dig into, but don't want to fail/crash the program just because it fails.
 *
 *  \def WeakAssert(c)
 *
 *  \note   logically
 *          if (!(c)) {
 *              Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ (L"Assert", Stroika_Foundation_Debug_Widen (#c), Stroika_Foundation_Debug_Widen ( __FILE__), __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_); }
 *          }
 *          But using funny !! and || syntax to allow use in expressions
 *
 *  @see GetWeakAssertionHandler
 *
 *  \hideinitializer
 */
#if qDebug
#define WeakAssert(c)                                                                                                                      \
    (!!(c) || (Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ (L"WeakAssert", Stroika_Foundation_Debug_Widen (#c),  \
                                                                                      Stroika_Foundation_Debug_Widen (__FILE__), __LINE__, \
                                                                                      ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_),            \
               false))
#else
#define WeakAssert (c) ((void)0)
#endif

/**
 *  \def WeakAssertMember(p,c)
 *
 *  @see AssertMember
 */
#define WeakAssertMember(p, c) WeakAssert (dynamic_cast<const c*> (p) != nullptr)

/**
 *  \def WeakAssertNotNull(p)
 *
 *  @see GetAssertionHandler
 */
#define WeakAssertNotNull(p) WeakAssert (p != nullptr)

/**
 *  \def WeakAssertNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#if qDebug
#define WeakAssertNotReached()                                                                                                             \
    Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ (                                                                \
        L"WeakAssert", L"Not Reached", Stroika_Foundation_Debug_Widen (__FILE__), __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)
#else
#define WeakAssertNotReached()
#endif

/**
 *  \def WeakAssertNotImplemented()
 *
 *  Use  this to mark code that is not yet implemented. Using this name for sections of code which fail because of not being implemented
 *  makes it easier to search for such code, and when something breaks (esp during porting) - its easier to see why
 *
 *  \hideinitializer
 */
#if qDebug
#define WeakAssertNotImplemented()                                                                                                         \
    Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ (                                                                \
        L"WeakAssert", L"Not Implemented", Stroika_Foundation_Debug_Widen (__FILE__), __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)
#else
#define WeakAssertNotImplemented()
#endif

/**
 *  \def WeakVerify(c)
 *
 *  Verify () is an assertion like Assert, except its argument is ALWAYS
 *  EVALUATED, even if debug is OFF. This is useful for cases where you just want
 *  todo an assertion about the result of a function, but don't want to keep the
 *  result in a temporary just to look at it for this one assertion test...
 *
 *  @see GetAssertionHandler
 */
#if qDebug
#define WeakVerify(c) WeakAssert (c)
#else
#define WeakVerify(c) ((void)(c))
#endif

    /// ---------------------  DEPRECATED CODE      ------------------------
    [[deprecated ("Since Stroika v3.0d6 - use the wchar_t overload")]] void
    SetWeakAssertionHandler (void (*legacyHandler) (const char* assertCategory, const char* assertionText, const char* fileName,
                                                    int lineNum, const char* functionName) noexcept);
    [[deprecated ("Since Stroika v3.0d6 - use the wchar_t overload")]] void
    SetAssertionHandler (void (*legacyHandler) (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum,
                                                const char* functionName) noexcept);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qDebug, qDebug);

#endif /*_Stroika_Foundation_Debug_Assertions_h_*/
