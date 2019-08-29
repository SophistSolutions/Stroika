/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Assertions_h_
#define _Stroika_Foundation_Debug_Assertions_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
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

#if qDebug || defined(__Doxygen__)
    /**
     *  Note: Some any paramaters may be null, if no suitable value is available.
     *
     *  \note   AssertionHandlers shall NOT throw an exception (but I don't think we can declare typedef as noexcept)
     *          The reason for this is so that assertions can be used safely in circumstances that don't expect
     *          exceptions.
     *
     *          Assertion handlers typically just print a debug message, and then exit the program. They are fatal, and must not return/throw.
     */
    using AssertionHandlerType = void (*) (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept;

    /**
     *  Note: Some any paramaters may be null, if no suitable value is available.
     *
     *  \note   AssertionHandlers shall NOT throw an exception (but I don't think we can declare typedef as noexcept)
     *          The reason for this is so that assertions can be used safely in circumstances that don't expect
     *          exceptions.
     *
     *          Weak assertion handlers typically just print a debug message. They are NOT fatal, and allow the program to continue.
     */
    using WeakAssertionHandlerType = void (*) (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept;

    /**
     *  Stroika makes very heavy use of assertions (to some extent inspired and
     *  patternend after Bertrand Meyers Eiffel assertion mechanism). Assertions are logical
     *  statements about function parameters, results, object states, etc, which are guaranteed
     *  (required) to be held true. These logical assertions serve two important functions:
     *  they <em>document</em> the requirements on input parameters for a function,
     *  and the promises made on function return. And they perform this documentation task <em>
     *  in such a way that the compiler can generate special code (conditionally) to verify that
     *  the assertions hold true</em>.
     *
     *  This latter point about conditional compilation is important. If the macro
     *  preprocessor symbol <code>qDebug</code> is true (non-zero), then this assertion cheching is
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
     *  Stroika provides four familes of 'assertion' macro functions. The are named
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
     *  using Stroika is because checking the <em>Require</em> declarations at the beggining of a
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
     *  corrupted data structures). (asside: Assertions logically mean the same thing as Ensures,
     *  except that Ensures only check return values).</p>
     *
     *  <em>Verify</em>s are inspired by the MFC VERIFY() macro, and the particular
     *  idiosyncracies of the Win32 SDK, though they can be used cross-platform. Many of the Win32 SDK
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
     *  exceptions. This isn't an appropraite forum for explanations of why this is generally
     *  a bad idea. Instead, I simply provide the flexability to allow those who want todo this
     *  that ability. There are <code>SetAssertionHandler</code> and <code>GetAssertionHandler</code>
     *  functions which allow the programmer to specify an alternate assertion handling scheme. The
     *  only assumption Stroika makes about this scheme is that the assertion handling funciton not
     *  return (so itmust either exit the program, or longjump/throw). Led makes no gaurantee that
     *  attemptsto throw out past an assertion will succeed.
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
    WeakAssertionHandlerType GetWeakAssertionHandler ();

    /**
    */
    WeakAssertionHandlerType GetDefaultWeakAssertionHandler ();

    /**
     *  See @'GetAssertionHandler'. If SetAssertionHandler() is called with nullptr,
     *  then this merely selects the default assertion handler.
     */
    void SetWeakAssertionHandler (WeakAssertionHandlerType assertionHandler);

    namespace Private_ {
        void              Weak_Assertion_Failure_Handler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept; // don't call directly - implementation detail...
        [[noreturn]] void Assertion_Failure_Handler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName) noexcept;      // don't call directly - implementation detail...
    }

/**
 * Private implementation utility macro
 */
#if !defined(__Doxygen__)
#if qCompilerAndStdLib_Support__PRETTY_FUNCTION__
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ __PRETTY_FUNCTION__
#elif qCompilerAndStdLib_Support__func__
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ __func__
#elif qCompilerAndStdLib_Support__FUNCTION__
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ __FUNCTION__
#else
#define ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_ ""
#endif
#endif

/**
 *  \def Assert(c)
 *
 *  \note   logically
 *          if (!(c)) {
 *              Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Assert", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_); }
 *          }
 *          But using funny !! and || syntax to allow use in expressions
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#define Assert(c) (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Assert", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_), false))

/**
 *  \def Ensure(c)
 *
 *  @see GetAssertionHandler
 *  @see Assert
 *
 *  \hideinitializer
 */
#define Ensure(c) (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Ensure", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_), false))

/**
 *  \def Require(c)
 *
 *  @see GetAssertionHandler
 *  @see Assert
 *
 *  \hideinitializer
 */
#define Require(c) (!!(c) || (Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Require", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_), false))

/**
 *  \brief  A WeakAssert() is for things that arent guaranteed to be true, but are overwhelmingly likely to be true. Use this so you see debug logs of rare events you way want to dig into, but don't want to fail/crash the program just because it fails.
 *
 *  \def WeakAssert(c)
 *
 *  \note   logically
 *          if (!(c)) {
 *              Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ ("Assert", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_); }
 *          }
 *          But using funny !! and || syntax to allow use in expressions
 *
 *  @see GetWeakAssertionHandler
 *
 *  \hideinitializer
 */
#define WeakAssert(c) (!!(c) || (Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ ("WeakAssert", #c, __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_), false))

#else

#define WeakAssert(c) ((void)0)
#define Assert(c) ((void)0)
#define Require(c) ((void)0)
#define Ensure(c) ((void)0)

#endif

/**
 *  \def AssertMember(p,c)
 *
 *  @see GetAssertionHandler
 */
#define AssertMember(p, c) Assert (dynamic_cast<const c*> (p) != nullptr)

/**
 *  \def EnsureMember(p,c)
 *
 *  @see GetAssertionHandler
 */
#define EnsureMember(p, c) Ensure (dynamic_cast<const c*> (p) != nullptr)

/**
 *  \def RequireMember(p,c)
 *
 *  @see GetAssertionHandler
 */
#define RequireMember(p, c) Require (dynamic_cast<const c*> (p) != nullptr)

/**
 *  \def WeakAssertMember(p,c)
 *
 *  @see AssertMember
 */
#define WeakAssertMember(p, c) WeakAssert (dynamic_cast<const c*> (p) != nullptr)

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
 *  \def WeakAssertNotNull(p)
 *
 *  @see GetAssertionHandler
 */
#define WeakAssertNotNull(p) WeakAssert (p != nullptr)

#if qDebug

/**
 *  \def AssertNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#define AssertNotReached() Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Assert", "Not Reached", __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)

/**
 *  \def EnsureNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#define EnsureNotReached() Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Ensure", "Not Reached", __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)

/**
 *  \def RequireNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#define RequireNotReached() Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Require", "Not Reached", __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)

/**
 *  \def RequireNotReached(p)
 *
 *  @see GetAssertionHandler
 *
 *  \hideinitializer
 */
#define WeakAsserteNotReached() Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ ("WeakAssert", "Not Reached", __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)

/**
 *  \def AssertNotImplemented()
 *
 *  Use  this to mark code that is not yet implemented. Using this name for sections of code which fail because of not being implemented
 *  makes it easier to search for such code, and when something breaks (esp during porting) - its easier to see why
 *
 *  \hideinitializer
 */
#define AssertNotImplemented() Stroika::Foundation::Debug::Private_::Assertion_Failure_Handler_ ("Assert", "Not Implemented", __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)

/**
 *  \def AssertNotImplemented()
 *
 *  Use  this to mark code that is not yet implemented. Using this name for sections of code which fail because of not being implemented
 *  makes it easier to search for such code, and when something breaks (esp during porting) - its easier to see why
 *
 *  \hideinitializer
 */
#define WeakAssertNotImplemented() Stroika::Foundation::Debug::Private_::Weak_Assertion_Failure_Handler_ ("WeakAssert", "Not Implemented", __FILE__, __LINE__, ASSERT_PRIVATE_ENCLOSING_FUNCTION_NAME_)

#else

#define AssertNotReached()
#define EnsureNotReached()
#define RequireNotReached()
#define WeakAsserteNotReached()

#define AssertNotImplemented()
#define WeakAssertNotImplemented()

#endif

/**
 *  \def Verify(c)
 *
 *  Verify () is an assertion like Assert, except its argument is ALWAYS
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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::Debug, qDebug, qDebug);

#endif /*_Stroika_Foundation_Debug_Assertions_h_*/
