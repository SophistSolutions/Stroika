/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Debug_Assertions_h_
#define	_Stroika_Foundation_Debug_Assertions_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"


namespace	Stroika {
	namespace	Foundation {
		namespace	Debug {

			#if		qDebug
				// Note: Some any paramater could be null, if no suitable value is available
				typedef	void	(*AssertionHandlerType) (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName);

				/*
				@DESCRIPTION:	<p>Led makes very heavy use of assertions (to some extent inspired and patterend after
					Bertrand Meyers Eiffel assertion mechanism). Assertions are logical statements about function parameters,
					results, object states, etc, which are guaranteed (required) to be held true. These logical assertions
					serve two important functions: they <em>document</em> the requirements on input parameters for a function,
					and the promises made on function return. And they perform this documentation task <em>in such a way
					that the compiler can generate special code (conditionally) to verify that the assertions hold true</em>.</p>
						<p>This latter point about conditional compilation is important. If the macro preprocessor
					symbol <code>qDebug</code> is true (non-zero), then this assertion cheching is enabled. If the symbol
					is false (zero), then the checking is disabled. <b>Of course the promises must always hold true!</b> But
					since the checking can significantly slow the code, it is best to only build with assertions on in certain
					circumstances (like while developing, and for much of QA/testing); but for released products it shouldbe
					disabled so the editor operates at maximum speed.</p>
						<p>Led's assertion mechanism is not only present to help debug Led itself. After all, that would
					have little value to a user of the Led class library. It is also very helpful to a programmer using
					the class library. This is because nearly all the parameters passed to Led functions are immediately
					checked for validity, so mistakes are trapped as early as possible. If you pass bad values to a Led
					function, you will very likely get a debugger trap at almost exactly the point in your calling code
					where the error occured. This can make debugging code written using Led much easier.</p>
						<p>Led provides four familes of 'assertion' macro functions. The are named 
							<code><em>Assert</em></code>,
							<code><em>Require</em></code>,
							<code><em>Ensure</em></code>, and
							<code><em>Verify</em></code>.
						</p>
						<p>The familily you will most often be interested in is <code><em>Require</em></code>s.
					This is because these are used to check parameters validity on entry to a function. So typically
					when you see a <em>Require</em> fail, you will find that it is the calling function which is passing
					invalid arguments to the function which triggered the requirement failure. The other reason <em>Requires</em>
					will be of particular interest to programmers using Led is because checking the <em>Require</em> declarations
					at the beggining of a function is often very telling about the details of what sort of parameters the function expects.
						</p>
						<p>Probably the next most common sort of assertion you will see is <em>Ensures</em>. These state <em>promises</em>
					about the return values of a function. These should very rarely be triggered (if they are , they almost always
					indicate a bug in the function that triggered the Ensure failure). But the reason they are still of interest
					to programmers using Led is because they document what can be assumed about return values from a particular function.
						</p>
						<p>Plain <em>Assertions</em> are for assertions which don't fall into any of the above categories,
					but are still useful checks to be performed. When an assertion is triggered, it is almost always diagnostic
					of a bug in the code which triggered the assertion (or corrupted data structures). (asside: Assertions
					logically mean the same thing as Ensures, except that Ensures only check return values).</p>
						</p>
						<p><em>Verify</em>s are inspired by the MFC VERIFY() macro, and the particular idiosyncracies
					of the Win32 SDK, though they can be used cross-platform. Many of the Win32 SDK routines return non-zero
					on success, and zero on failure. Most sample Win32 code you look at simply ignores these results. For
					paranoia sake (which was very helpful in the old moldy win32s days) I wrap most Win32 SDK calls in a
					<em><code>Verify</code></em> wrapper. This - when debugging is enabled - checks the return value,
					and asserts if there is a problem. <b>Very Important: Unlike the other flavors of Assertions, Verify
					always evaluates its argument!</b>.
						</p>
						<p>This last point is worth repeating, as it is the only source of bugs I've ever seen introduced
					from the use of assertions (and I've seen the mistake more than once). <b>All flavors of assertions
					(except Verify) do NOT evaluate their arguments if <code>qDebug</code> is off</b>. This means you <b>cannot</b>
					count on the arguments to assertions having any side-effects. Use <em>Verify</em> instead of the other
					assertion flavors if you want to only check for compliance if <em><code>qDebug</code></em> is true, but want
					the side-effect to happen regardless.</p>
						<p>Now when assertions are triggered, just what happens? Here I think there is only one
					sensible answer. And that is that the program drops into the debugger. And what happens after that
					is undefined. This is Led's default behavior.</p>
						<p>But there are others who hold the view that triggered assertions should
					generate exceptions. This isn't an appropraite forum for explanations of why this is generally
					a bad idea. Instead, I simply provide the flexability to allow those who want todo this
					that ability. There are <code>SetAssertionHandler</code> and <code>GetAssertionHandler</code>
					functions which allow the programmer to specify an alternate assertion handling scheme. The
					only assumption Led mkaes about this scheme is that the assertion handling funciton not return
					(so itmust either exit the program, or longjump/throw). Led makes no gaurantee that attempts
					to throw out past an assertion will succeed.
					
					GetAssertionHandler() never returns NULL - it always returns some handler.
					</p>
				*/
				AssertionHandlerType	GetAssertionHandler ();

				/*
				@METHOD:		SetAssertionHandler
				@DESCRIPTION:	<p>See @'GetAssertionHandler'. If SetAssertionHandler() is called with nullptr, then this merely selects the default assertion handler.</p>
				*/
				void	SetAssertionHandler (AssertionHandlerType assertionHandler);
	

				namespace	Private {
					void	Debug_Trap_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName);	// don't call directly - implementation detail...
				}


				/*
				@METHOD:		Assert
				@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
				*/
				#define	Assert(c)			{if (!(c)) { Stroika::Foundation::Debug::Private::Debug_Trap_ ("Assert", #c, __FILE__, __LINE__, nullptr); }}
				/*
				@METHOD:		Require
				@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
				*/
				#define	Require(c)			{if (!(c)) { Stroika::Foundation::Debug::Private::Debug_Trap_ ("Require", #c, __FILE__, __LINE__, nullptr); }}
				/*
				@METHOD:		Ensure
				@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
				*/
				#define	Ensure(c)			{if (!(c)) { Stroika::Foundation::Debug::Private::Debug_Trap_ ("Ensure", #c, __FILE__, __LINE__, nullptr); }}
			#else
				#define	Assert(c)
				#define	Require(c)
				#define	Ensure(c)
			#endif

			/*
			@METHOD:		AssertMember
			@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
			*/
			#define	AssertMember(p,c)	Assert (dynamic_cast<c*>(p) != nullptr)
			/*
			@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
			*/
			#define	RequireMember(p,c)	Require (dynamic_cast<c*>(p) != nullptr)
			/*
			@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
			*/
			#define	EnsureMember(p,c)	Ensure (dynamic_cast<c*>(p) != nullptr)

			/*
			@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
			*/
			#define	AssertNotNull(p)		Assert (p!=nullptr)
			/*
			@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
			*/
			#define	RequireNotNull(p)	Require (p!=nullptr)
			/*
			@DESCRIPTION:	<p>See @'GetAssertionHandler'</p>
			*/
			#define	EnsureNotNull(p)		Ensure (p!=nullptr)



			#define	AssertNotReached()		Assert (false)
			#define	RequireNotReached()		Assert (false)
			#define	EnsureNotReached()		Assert (false)

			// Use  this to mark code that is not yet implemented. Using this name for sections of code which fail because of not being implemented
			// makes it easier to search for such code, and when something breaks (esp during porting) - its easier to see why
			#define	AssertNotImplemented()	Assert (false)

			/*
			@DESCRIPTION:	<p>Verify () is an assertion like Assert, except its argument is ALWAYS EVALUATED, even if
				debug is OFF. This is useful for cases where you just want todo an assertion about the result
				of a function, but don't want to keep the result in a temporary just to look at it for this
				one assertion test...</p>
					<p>See @'GetAssertionHandler'</p>
			*/
			#if		qDebug
				#define	Verify(c)		Assert (c)
			#else
				#define	Verify(c)		(c)
			#endif

		}
	}
}
#endif	/*_Stroika_Foundation_Debug_Assertions_h_*/
