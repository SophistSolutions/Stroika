/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	__LedConfig_h__
#define	__LedConfig_h__	1

#include	"../../Foundation/StroikaPreComp.h"

/*
@MODULE:	LedConfig
@DESCRIPTION:
		<p>Support basic configuration defines for the operating system, and compiler, and some basic features
	(like character set).
	</p>
 */


#if		qMacOS
	#if		defined (__cplusplus) && !defined (__SOMIDL__)
		#include	<ConditionalMacros.h>
	#endif
#endif


#if		defined (__GNUC__)
#include	<features.h>
#endif



/*
 *	This Numeric Led version is intended to allow you to conditionally compile code
 *	based on different versions of Led. The Led::kVersion string is a symbolic representation
 *	of this version.
 *
 *	Don't count on the particular hard-wirded version number. Instead - compare as follows:
 *		#if		qLed_FullVersion > MAKE_LED_FULL_VERSION (1, 0, qLed_Version_Stage_Beta, 3, 1)
 *			DOTHIS ();
 *		#else
 *			DOTHAT ();
 *		#endif
 */

// Support declarations for version#
#define	qLed_Version_Stage_Dev				0x1
#define	qLed_Version_Stage_Alpha			0x2
#define	qLed_Version_Stage_Beta				0x3
#define	qLed_Version_Stage_ReleaseCandidate	0x4
#define	qLed_Version_Stage_Release			0x5

#define	MAKE_LED_FULL_VERSION(_Major_,_Minor_,_Stage_,_SubStage_,_FinalBuild_) \
								( (_Major_ << 25) | \
								  (_Minor_ << 17) | \
								  (_Stage_ << 9) | \
								  (_SubStage_ << 1) | \
								  (_FinalBuild_) \
								)

#define	qLed_FullVersion		MAKE_LED_FULL_VERSION (qLed_Version_Major, qLed_Version_Minor, qLed_Version_Stage, qLed_Version_SubStage, qLed_Version_FinalBuild)




// This part is where we actually update the version#
#define	qLed_Version_Major					3
#define	qLed_Version_Minor					1
#define	qLed_Version_MajorMinor				0x31
#define	qLed_Version_Stage					qLed_Version_Stage_Beta
#define	qLed_Version_SubStage				2
#define	qLed_Version_FinalBuild				0

#define	qLed_MajorMinorVersionString		"3.1"
#define	qLed_ShortVersionString				"3.1b2x"






/*
 * OS Defines.
 *		
 *		Be sure OS defines are defined to ZERO if not defined. This is so we can write code
 *	like if (qMacOS) as well as code like #if qMacOS
 */
#ifndef	qMacOS
	#define	qMacOS		defined (macintosh)
#endif
#ifndef	qWindows
	#define	qWindows	(defined (_WIN32) || defined (WIN32))
#endif
#ifndef	qXWindows
	#define	qXWindows	defined (__GNUC__) && !qMacOS
#endif

#if		qMacOS + qWindows + qXWindows > 1
	#error	"Only one of these should be defined"
#endif
#if		!qMacOS && !qWindows && !qXWindows
	#error	"One of these should be defined - right now thats all we support"
#endif











/*
 *	Bug defines for each of the compilers we support. Instead of sprinkling code
 *	that refers to the particular compiler through the code - instead we
 *	refer in the code to specific bugs, and to the specific compilers only here.
 */


#if		defined (__SC__)
	#error	"Compiler currently not supported. See versions of Led before 1.0b26 for SymC++ 703 bug workarounds."
#endif



#if		defined (__MWERKS__)

	#define	__CWPro8_3__VERSION__		0x3003
	#define	__CWPro8_2__VERSION__		0x3002
	#define	__CWPro8__VERSION__			0x3000

	#define	__CWPro7__VERSION__			0x2405

	#define	__CWPro6__VERSION__			0x2400

	// Intermediate version#s only supported in Led development releases - NOT in final Led30 release...
	#define	__CWPro52__VERSION__		0x2301
	#define	__CWPro52_Win32__VERSION__	0x2301

	#define	__CWPro5__VERSION__			0x2300
	#define	__CWPro5_Win32__VERSION__	0x2300

	#define	__CWPro4__VERSION__			0x2200
	#define	__CWPro4_Win32__VERSION__	0x2200

	#if		__MWERKS__ >= __CWPro8_3__VERSION__
		/*
		 *	NOTE: MWERKS CW8 Pro was never very heavily tested, and all these defines really just apply to
		 *	CW8.3.
		 *		The defines for CW7 all worked fine for CW8 and CW8.2 as of Led 3.1a3 (except for the addition
		 *		of qAccessChecksFailFromTemplatesBug).
		 */

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		__MWERKS__ > __CWPro8_3__VERSION__
			#pragma	warn	"Don't know this (newer) version of the MWERKS CW compiler - using last known CW bug defines."
		#endif

		#if		defined (__cplusplus) && !__option(bool)
			#error	"Turn on the 'Enable bool support' checkbox in the 'C++ Language' prefs panel"
		#endif


		// First noticed broken with MWEKRS CW PRo 8.3 - 2003-12-11
		#ifndef	qMixinDisambiguatingNameInBothBug
			#define	qMixinDisambiguatingNameInBothBug								1
		#endif

// Appears still broken in CW8.3 - LGP 2003-01-17
		#ifndef	qNoMacOS68KA4CodeResourceSupport
			#define	qNoMacOS68KA4CodeResourceSupport								1
		#endif
		
		#ifndef	qSDKHasInternetConfig
			#define	qSDKHasInternetConfig											1
		#endif


// Appears still broken in CW8.3 - LGP 2003-01-17
// MAYBE NEW BUG IN CW8??? - LGP 2003-01-17
#ifndef	qAccessChecksFailFromTemplatesBug
	#define	qAccessChecksFailFromTemplatesBug										1
#endif




// First noted broken in CW8.3 - LGP 2003-01-20
// Not REALLY a bug - but a quirk with MSVC70 (and probably mcuh earlier). Way to get cmd line args...
#ifndef	qNo_argc_argv_MacrosSupported
	#define	qNo_argc_argv_MacrosSupported											1
#endif


// Appears FIXED in CW8.3 - LGP 2003-01-17
		//	First found broken in CWPro7 - LGP 2002-11-01
//		#ifndef	qNestedTemplateCTORInTemplateBug
//			#define	qNestedTemplateCTORInTemplateBug								0
//		#endif


// Appears FIXED in CW8.3 - LGP 2003-01-17
		//	Still broken as of CWPro6 - LGP 2001-07-19 (though now fails differently. Generates bogus syntax errors compiling)
		//	Still broken as of CWPro52 - LGP 991129 (fails to compile in MWERKS Compiler)
		//	First noticed CWPro5 - LGP 990715
		//	Problem is basicly
		//	template <class X> class Y { class Z { public Z (); ~Z (); }}}};
		//	Then even though you declare the impelmation of Y<X>::Z::Z and ~Z, you still get linker errors.
		//
		//	NB: The code which fails is the same between MSVC60 and MWERKS CWPro5. Even though it can be reproduced for CW5Pro, its
		//	only done for Led_MFC - so you must test with the cross-compiler!
//		#ifndef	qNestedClassesInTemplateClassesDontExpandCompilerBug
//			#define	qNestedClassesInTemplateClassesDontExpandCompilerBug			1
//		#endif


// Appears still broken in CW8.3 - LGP 2003-01-17
		//	Still broken as of CWPro6 - LGP 2001-07-19
		//	First found broken in CWPro52 - LGP 991224
		#ifndef	qNoSTRNICMP
			#define	qNoSTRNICMP														1
		#endif


// Appears FIXED in CW8.3 - LGP 2003-01-17
		//	First found broken in CWPro6, when targetting Win32.
		//		Strange bug. Not 100% sure its a compiler bug but sure looks like it.
//		#ifndef	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
//			#define	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug					qWindows
//		#endif

// Appears FIXED in CW8.3 - LGP 2003-01-17
		//	Broken in CW7Pro (didnt check earlier compilers - UNSURE now if a real bug - must look at spec) - LGP 2002-09-19
//		#ifndef	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
//			#define	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase		1
//		#endif

// Appears FIXED in CW8.3 - LGP 2003-01-17
		//	Broken in CW7Pro (didnt check earlier compilers - UNSURE now if a real bug - must look at spec) - LGP 2002-09-20
//		#ifndef	qConstNonConstPtrConversionsWithTemplatedMemberFunctionBug
//			#define	qConstNonConstPtrConversionsWithTemplatedMemberFunctionBug		1
//		#endif


// Appears still broken in CW8.3 - LGP 2003-01-17
		//	Broken in CW7Pro (didnt check earlier compilers) - LGP 2002-09-20
		#ifndef	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug
			#define	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug		1
		#endif

// Appears still broken in CW8.3 - LGP 2003-01-17
		//	Broken in CW7Pro (didnt check earlier compilers) - LGP 2002-11-20
		//
		#ifndef	qUsingMemberNameToOverloadInTemplateClassBug
		#define	qUsingMemberNameToOverloadInTemplateClassBug								1
		#endif

	#elif	__MWERKS__ >= __CWPro7__VERSION__
		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		__MWERKS__ > __CWPro7__VERSION__
			#pragma	warn	"Don't know this (newer) version of the MWERKS CW compiler - using last known CW bug defines."
		#endif

		#if		defined (__cplusplus) && !__option(bool)
			#error	"Turn on the 'Enable bool support' checkbox in the 'C++ Language' prefs panel"
		#endif

		#ifndef	qNoMacOS68KA4CodeResourceSupport
			#define	qNoMacOS68KA4CodeResourceSupport								1
		#endif


		//	First found broken in CWPro7 - LGP 2002-11-01
		#ifndef	qNestedTemplateCTORInTemplateBug
			#define	qNestedTemplateCTORInTemplateBug								1
		#endif


		//	Still broken as of CWPro6 - LGP 2001-07-19 (though now fails differently. Generates bogus syntax errors compiling)
		//	Still broken as of CWPro52 - LGP 991129 (fails to compile in MWERKS Compiler)
		//	First noticed CWPro5 - LGP 990715
		//	Problem is basicly
		//	template <class X> class Y { class Z { public Z (); ~Z (); }}}};
		//	Then even though you declare the impelmation of Y<X>::Z::Z and ~Z, you still get linker errors.
		//
		//	NB: The code which fails is the same between MSVC60 and MWERKS CWPro5. Even though it can be reproduced for CW5Pro, its
		//	only done for Led_MFC - so you must test with the cross-compiler!
		#ifndef	qNestedClassesInTemplateClassesDontExpandCompilerBug
			#define	qNestedClassesInTemplateClassesDontExpandCompilerBug			1
		#endif


		//	Still broken as of CWPro6 - LGP 2001-07-19
		//	First found broken in CWPro52 - LGP 991224
		#ifndef	qNoSTRNICMP
			#define	qNoSTRNICMP														1
		#endif


		//	First found broken in CWPro6, when targetting Win32.
		//		Strange bug. Not 100% sure its a compiler bug but sure looks like it.
		#ifndef	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
			#define	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug					qWindows
		#endif

		//	Broken in CW7Pro (didnt check earlier compilers - UNSURE now if a real bug - must look at spec) - LGP 2002-09-19
		#ifndef	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			#define	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase		1
		#endif

		//	Broken in CW7Pro (didnt check earlier compilers - UNSURE now if a real bug - must look at spec) - LGP 2002-09-20
		#ifndef	qConstNonConstPtrConversionsWithTemplatedMemberFunctionBug
			#define	qConstNonConstPtrConversionsWithTemplatedMemberFunctionBug		1
		#endif


		//	Broken in CW7Pro (didnt check earlier compilers) - LGP 2002-09-20
		#ifndef	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug
			#define	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug		1
		#endif

		//	Broken in CW7Pro (didnt check earlier compilers) - LGP 2002-11-20
		//
		#ifndef	qUsingMemberNameToOverloadInTemplateClassBug
		#define	qUsingMemberNameToOverloadInTemplateClassBug								1
		#endif

	#elif		__MWERKS__ >= __CWPro6__VERSION__

		#if		defined (__cplusplus) && !__option(bool)
			#error	"Turn on the 'Enable bool support' checkbox in the 'C++ Language' prefs panel"
		#endif


		//	Still broken as of CWPro6 - LGP 2001-07-19 (though now fails differently. Generates bogus syntax errors compiling)
		//	Still broken as of CWPro52 - LGP 991129 (fails to compile in MWERKS Compiler)
		//	First noticed CWPro5 - LGP 990715
		//	Problem is basicly
		//	template <class X> class Y { class Z { public Z (); ~Z (); }}}};
		//	Then even though you declare the impelmation of Y<X>::Z::Z and ~Z, you still get linker errors.
		//
		//	NB: The code which fails is the same between MSVC60 and MWERKS CWPro5. Even though it can be reproduced for CW5Pro, its
		//	only done for Led_MFC - so you must test with the cross-compiler!
		#ifndef	qNestedClassesInTemplateClassesDontExpandCompilerBug
			#define	qNestedClassesInTemplateClassesDontExpandCompilerBug			1
		#endif


		//	Still broken as of CWPro6 - LGP 2001-07-19
		//	First found broken in CWPro52 - LGP 991224
		#ifndef	qNoSTRNICMP
			#define	qNoSTRNICMP														1
		#endif


		//	First found broken in CWPro6, when targetting Win32.
		//		Strange bug. Not 100% sure its a compiler bug but sure looks like it.
		#ifndef	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
			#define	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug					qWindows
		#endif

	#elif		__MWERKS__ == __CWPro52__VERSION__ || __MWERKS__ == __CWPro52_Win32__VERSION__

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		__MWERKS__ > __CWPro52__VERSION__
			#pragma	warn	"Don't know this version of the MWERKS CW compiler - using last known CW bug defines."
		#endif

		#if		defined (__cplusplus) && !__option(bool)
			#error	"Turn on the 'Enable bool support' checkbox in the 'C++ Language' prefs panel"
		#endif

		//	Still broken as of CWPro52 - LGP 991129 (fails to compile in MWERKS Compiler)
		//	First noticed CWPro5 - LGP 990715
		//	Problem is basicly
		//	template <class X> class Y { class Z { public Z (); ~Z (); }}}};
		//	Then even though you declare the impelmation of Y<X>::Z::Z and ~Z, you still get linker errors.
		//
		//	NB: The code which fails is the same between MSVC60 and MWERKS CWPro5. Even though it can be reproduced for CW5Pro, its
		//	only done for Led_MFC - so you must test with the cross-compiler!
		#ifndef	qNestedClassesInTemplateClassesDontExpandCompilerBug
			#define	qNestedClassesInTemplateClassesDontExpandCompilerBug			1
		#endif

		//	First found broken in CWPro52 - LGP 991224
		#ifndef	qNoSTRNICMP
			#define	qNoSTRNICMP														1
		#endif

	#elif		__MWERKS__ == __CWPro5__VERSION__ || __MWERKS__ == __CWPro5_Win32__VERSION__

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		__MWERKS__ > __CWPro5__VERSION__
			#pragma	warn	"Don't know this version of the MWERKS CW compiler - using last known CW bug defines."
		#endif

		#if		defined (__cplusplus) && !__option(bool)
			#error	"Turn on the 'Enable bool support' checkbox in the 'C++ Language' prefs panel"
		#endif

		//	First noticed CWPro5 - LGP 990715
		//	Problem is basicly
		//	template <class X> class Y { class Z { public Z (); ~Z (); }}}};
		//	Then even though you declare the impelmation of Y<X>::Z::Z and ~Z, you still get linker errors.
		//
		//	NB: The code which fails is the same between MSVC60 and MWERKS CWPro5. Even though it can be reproduced for CW5Pro, its
		//	only done for Led_MFC - so you must test with the cross-compiler!
		#ifndef	qNestedClassesInTemplateClassesDontExpandCompilerBug
			#define	qNestedClassesInTemplateClassesDontExpandCompilerBug			1
		#endif

		//	Still broken as of CWPro5 - LGP 990713 (below sample code fails, but I'm not sure BWA still needed, for reasons I don't understand).
		//	Still broken as of CWPro4 - LGP 990205
		//	Still broken as of CWPro3 - LGP 980502
		//	Still broken as of CWPro2 - LGP 971111
		//	First found in CWPro1 - LGP 970707
		//
		//	You can use this code to test if the bug still exists:
		//
		//	class	foo {
		//		public:
		//			foo (int);
		//	};
		//	vector<foo>	a;

		//	NB: Cannot just turn off to test if fixed cuz
		//	several bugs trigger the same workaround.
		#ifndef	qSTLTemplatesErroniouslyRequireDefCTORCuzOfOverExpanding
			#define	qSTLTemplatesErroniouslyRequireDefCTORCuzOfOverExpanding		1
		#endif

	#elif		__MWERKS__ == __CWPro4__VERSION__ || __MWERKS__ == __CWPro4_Win32__VERSION__

		#if		defined (__cplusplus) && !__option(bool)
			#error	"Turn on the 'Enable bool support' checkbox in the 'C++ Language' prefs panel"
		#endif

		//
		//	try {
		//		void*	p	=	new char [UINT_MAX];
		//	}
		//	catch (...) {
		//		int a = 3; // test breakpoint here
		//	}
		//
		/////////
		/////////STILL UNTESTED WITH CWPRO4
		/////////RETEST WITH CWPro3
		/////////AND WHEN I RETEST - PUT RETEST CODE FRAGMENT HERE SO EASIER TO RETEST!!! IF FAILS THAT IS
		/////////
		//	Still broken as of CWPro2 - LGP 971111
		//	Still broken as of CW11.x (pre-release version after CW11) - LGP 970606
		// First tested in CW9 - LGP 960718
		// Trouble is they add ALLOC_BLOCK_OVERHEAD to requested size which can
		// overflow, and they don't test for overflow. So new char [UINT_MAX] allocates
		// a 5 or 10 byte array, instead of throwing bad_alloc.
		#ifndef	qVERYLargeOpNewFailsToFail
			#define	qVERYLargeOpNewFailsToFail										1
		#endif
		

		//	Still broken as of CWPro4 - LGP 990205
		//	Still broken as of CWPro3 - LGP 980502
		//	Still broken as of CWPro2 - LGP 971111
		//	First found in CWPro1 - LGP 970707
		//
		//	You can use this code to test if the bug still exists:
		//
		//	class	foo {
		//		public:
		//			foo (int);
		//	};
		//	vector<foo>	a;

		//	NB: Cannot just turn off to test if fixed cuz
		//	several bugs trigger the same workaround.
		#ifndef	qSTLTemplatesErroniouslyRequireDefCTORCuzOfOverExpanding
			#define	qSTLTemplatesErroniouslyRequireDefCTORCuzOfOverExpanding		1
		#endif

	#elif	__MWERKS__ < __CWPro4__VERSION__
		
		#error	"No longer supported - see old versions of Led for bug workarounds if you must use an OLD CodeWarrior"

	#else

		#error	"Don't know about this version of the CodeWarrior compiler."

	#endif
#endif






// NB: we check for '&& !defined (__MWERKS__)' because when using MWERKS cross compiler,
// it defines _MSC_VER (in precomp) - presumably to avoid problems with microsofts headers...
// LGP 951228...
#if		defined (_MSC_VER) && !defined (__MWERKS__) && defined (__cplusplus)

	/* _MSC_VER == 1600 - VC++.NET 2010 */
	#define	_MSVC100_VER_	1600

	/* _MSC_VER == 1500 - VC++.NET 2008 */
	#define	_MSVC90_VER_	1500

	/* _MSC_VER == 1400 - VC++.NET 2005 */
	#define	_MSVC80_VER_	1400

	/* _MSC_VER == 1310 - VC++.NET 2003 */
	#define	_MSVC71_VER_	1310

	/* _MSC_VER == 1300 - VC++.NET */
	#define	_MSVC70_VER_	1300

	/* _MSC_VER == 1200 - AKA MSVC 6.0 */
	#define	_MSVC60_VER_	1200

	/* _MSC_VER == 1100 - AKA MSVC 5.0 */
	#define	_MSVC50_VER_	1100

	#if		_MSC_VER > _MSVC80_VER_ || _MSC_VER == _MSVC80_VER_

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		_MSC_VER > _MSVC100_VER_
			#error	"Don't know this version of the MSVC compiler - using last known MSVC bug defines."
		#endif

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 6.0 or later requires you to turn on RTTI"
		#endif



		//	Still broken - by default - for MSVC .Net 2003 programs - LGP 2003-05-09
		//
		//	Still (effectively) broken for MS VC++.NET (MSVC70). They now have a compiler option (/Zc:forLoop) which
		//		lets you make this bug go away, but it is NOT on by default, and they give you no way to check if
		//		it is on (as they do with /Zc:wchar_t, for example). We could FORCE this bugfix on with the new
		//		#pragma conformance () - but that could break OTHER peoples code who assume the opposite behavior!
		//		So - the best thing for now appears to assume they still have this bug broken, and to operate accordingly.
		//
		//	Still broken in MSVC60 - LGP 980923 (AMAZING!AMAZING!AMAZING!)
		//	Still broken in MSVC50 - LGP 970312 (AMAZING!)
		//	Still broken in MSVC 4.2 (LGP 970115)
		//	Still broken in MSVC 4.1 (LGP 960417)
		//	Still broken In MSVC 4.0 (LGP 951125)
		//	MSVC 2.2 and earlier have this problem - LGP 950821
		#ifndef	qNoSupportForNewForLoopScopingRules
			#define	qNoSupportForNewForLoopScopingRules								1
		#endif

		//	Still supported in MSVC 7.1 - LGP 2003-05-08
		//	Broken in MSVC60 - LGP 2002-12-16
		//	Is the Uniscribe SDK (.h file etc) available with this compiler?
		#ifndef	qUniscribeAvailableWithSDK
			#define	qUniscribeAvailableWithSDK										1
		#endif


	#elif		_MSC_VER == _MSVC71_VER_

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		_MSC_VER > _MSVC71_VER_
			#error	"Don't know this version of the MSVC compiler - using last known MSVC bug defines."
		#endif

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 6.0 or later requires you to turn on RTTI"
		#endif

		//	Still broken - by default - for MSVC .Net 2003 programs - LGP 2003-05-09
		//
		//	Still (effectively) broken for MS VC++.NET (MSVC70). They now have a compiler option (/Zc:forLoop) which
		//		lets you make this bug go away, but it is NOT on by default, and they give you no way to check if
		//		it is on (as they do with /Zc:wchar_t, for example). We could FORCE this bugfix on with the new
		//		#pragma conformance () - but that could break OTHER peoples code who assume the opposite behavior!
		//		So - the best thing for now appears to assume they still have this bug broken, and to operate accordingly.
		//
		//	Still broken in MSVC60 - LGP 980923 (AMAZING!AMAZING!AMAZING!)
		//	Still broken in MSVC50 - LGP 970312 (AMAZING!)
		//	Still broken in MSVC 4.2 (LGP 970115)
		//	Still broken in MSVC 4.1 (LGP 960417)
		//	Still broken In MSVC 4.0 (LGP 951125)
		//	MSVC 2.2 and earlier have this problem - LGP 950821
		#ifndef	qNoSupportForNewForLoopScopingRules
			#define	qNoSupportForNewForLoopScopingRules								1
		#endif

		//	Still supported in MSVC 7.1 - LGP 2003-05-08
		//	Broken in MSVC60 - LGP 2002-12-16
		//	Is the Uniscribe SDK (.h file etc) available with this compiler?
		#ifndef	qUniscribeAvailableWithSDK
			#define	qUniscribeAvailableWithSDK										1
		#endif


	#elif		_MSC_VER >= _MSVC70_VER_

		#if		_MSC_VER > _MSVC70_VER_
			#error	"Don't know this version of the MSVC compiler - using MSVC 70 bug defines."
		#endif

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 6.0 or later requires you to turn on RTTI"
		#endif

		//	Broken still but now DOCUMENTED in VC++.NET (VC7) - LGP 2002-09-03:
		//
		//	'function' : member functions of nested classes of a template class cannot be defined outside the class
		//	For inner nested classes inside a template, you must define functions inside the class. Such 
		//	functions automatically become inline functions.
		//	
		//	This error is generated for code allowed by the C++ language, however, not yet supported by Visual C++.
		//	
		//	For example, the following sample generates C3206:
		//	
		// C3206.cpp
		//	template<typename T> class Sequence {
		//	public:
		//	   class Cursor {
   		//	public:
		//	      Cursor(Sequence*);
		//	      // try the following line instead of the preceding line
		//	      // Cursor(Sequence*){}
		//	   };
		//	};
		//	
		//	template<typename T> Sequence<T>::Cursor::Cursor(Sequence<T>*) { }
		//	// C3206, remove previous line
		//	
		//	int main() {
		//	   Sequence<int>::Cursor c(0);
		//	}
		//
		//	Broken  in MSVC60 - LGP 990304
		//	First noticed 990304
		//	Problem is basicly
		//	template <class X> class Y { class Z { public Z (); ~Z (); }}}};
		//	Then even though you declare the impelmation of Y<X>::Z::Z and ~Z, you still get linker errors.
		#ifndef	qNestedClassesInTemplateClassesDontExpandCompilerBug
			#define	qNestedClassesInTemplateClassesDontExpandCompilerBug			1
		#endif


		//	Broken  in MSVC++.Net (MSVC70) - LGP 2002-10-30
		#ifndef	qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug
		#define	qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug						1
		#endif


		//	Still Broken  in MSVC++.Net (MSVC70) - LGP 2002-09-03
		//	Broken  in MSVC70B2 - LGP 2001-07-22
		#ifndef	qTypedefConfusedAccessCheckingCompilerBug
			#define	qTypedefConfusedAccessCheckingCompilerBug						1
		#endif

		//	Still Broken  in MSVC++.Net (MSVC70) - LGP 2002-09-03
		//	Broken  in MSVC70B2 - LGP 2001-07-22
		#ifndef	qTypedefInTemplateToSpecScopeOfNestedTypeCompilerBug
			#define	qTypedefInTemplateToSpecScopeOfNestedTypeCompilerBug			1
		#endif

		//	Still Broken  in MSVC++.Net (MSVC70) - LGP 2002-09-03
		//	Still broken in MSVC60 - LGP 980923
		//	Still broken in MSVC50 - LGP 970312
		//	first noticed - LGP 960509
		#ifndef	qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug
			#define	qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug		1
		#endif

		//	Still (effectively) broken for MS VC++.NET (MSVC70). They now have a compiler option (/Zc:forLoop) which
		//		lets you make this bug go away, but it is NOT on by default, and they give you no way to check if
		//		it is on (as they do with /Zc:wchar_t, for example). We could FORCE this bugfix on with the new
		//		#pragma conformance () - but that could break OTHER peoples code who assume the opposite behavior!
		//		So - the best thing for now appears to assume they still have this bug broken, and to operate accordingly.
		//
		//	Still broken in MSVC60 - LGP 980923 (AMAZING!AMAZING!AMAZING!)
		//	Still broken in MSVC50 - LGP 970312 (AMAZING!)
		//	Still broken in MSVC 4.2 (LGP 970115)
		//	Still broken in MSVC 4.1 (LGP 960417)
		//	Still broken In MSVC 4.0 (LGP 951125)
		//	MSVC 2.2 and earlier have this problem - LGP 950821
		#ifndef	qNoSupportForNewForLoopScopingRules
			#define	qNoSupportForNewForLoopScopingRules								1
		#endif

		//	Still Broken  in MSVC++.Net (MSVC70) - LGP 2002-09-20
		//	Broken in MSVC60 - LGP 2002-09-18 (almost certainly MUCH earlier. I've seen this many times before)
		//	Complex inheritance hierarchies don't interact well with calling methods during construction/destruction
		//	for the MSFT compiler. After a couple level of calls, you get a garbaged 'this' pointer (according to the
		//	debugger, and behavior).
		//
		#ifndef	qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
			#define	qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR		1
		#endif

		//	Still Broken  in MSVC++.Net (MSVC70) - LGP 2002-09-20
		//	Broken in MSVC60 - LGP 2002-09-20
		// I could EASILY have mischaracterised this problem - but it appears to have the nested class
		// stuff with the friend declaraition - LGP 2002-09-20
		#ifndef	qFriendDeclarationsDontWorkWithNestedClassesBug
			#define	qFriendDeclarationsDontWorkWithNestedClassesBug					1
		#endif

		//	Broken in MSVC60 - LGP 2002-12-16
		//	Is the Uniscribe SDK (.h file etc) available with this compiler?
		#ifndef	qUniscribeAvailableWithSDK
			#define	qUniscribeAvailableWithSDK										1
		#endif

	#elif	_MSC_VER >= _MSVC60_VER_

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 6.0 requires you to turn on RTTI"
		#endif

		//	Broken  in MSVC60 - LGP 2004-01-23
		#ifndef	qDeleteOfPointerToConstantDataFailsOverloadError
			#define	qDeleteOfPointerToConstantDataFailsOverloadError				1
		#endif

		//	Broken  in MSVC60 - LGP 2004-02-10
		#ifndef	qFailToCompileLargeDataInitializedArraysBug
		#define	qFailToCompileLargeDataInitializedArraysBug							1
		#endif

		//	Broken  in MSVC60 - LGP 2004-02-10
		#ifndef	qTemplatesWithMixOfNestedClassesWithCommonNamesGetsConfusedBug
		#define	qTemplatesWithMixOfNestedClassesWithCommonNamesGetsConfusedBug		1
		#endif

		//	Broken  in MSVC60 - LGP 2004-02-10
		#ifndef	qDefaultValueInTemplateParemeterUsingTemplatedTypeBug
		#define	qDefaultValueInTemplateParemeterUsingTemplatedTypeBug				1
		#endif

		//	Broken  in MSVC60 - LGP 2004-02-10
		#ifndef	qMemberTemplateNeedsExplicitDeclarationForEachTypeBug
		#define	qMemberTemplateNeedsExplicitDeclarationForEachTypeBug				1
		#endif

		//	Broken  in MSVC60 - LGP 2004-01-23
		#ifndef	qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug
		#define	qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug			1
		#endif

		//	Broken  in MSVC60 - LGP 2003-02-13
		//	(NB: probably same as qTemplatedMemberFunctionsFailBug)
		#ifndef	qTemplatedMemberFunctionsFailWithMFCMessageMaps
			#define	qTemplatedMemberFunctionsFailWithMFCMessageMaps					1
		#endif

		//	Broken  in MSVC60 - LGP 2004-02-10
		//	(NB: probably same as qTemplatedMemberFunctionsFailWithMFCMessageMaps)
		#ifndef	qTemplatedMemberFunctionsFailBug
			#define	qTemplatedMemberFunctionsFailBug								1
		#endif

		//	Broken  in MSVC60 - LGP 2002-10-30
		#ifndef	qNestedTemplateCTORInTemplateBug
			#define	qNestedTemplateCTORInTemplateBug								1
		#endif

		//	Broken  in MSVC60 - LGP 2002-10-30
		#ifndef	qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug
		#define	qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug						1
		#endif

		//	Broken  in MSVC60 - LGP 2002-11-20
		//
		//	#if		qStaticConstInitializerInClassDeclarationNYIBug
		//		enum { kTryToIncorporateInternalizeIntoAdjacentTable	=	true };
		//	#else
		//		static	const	bool	kTryToIncorporateInternalizeIntoAdjacentTable	=	true;
		//	#endif
		#ifndef	qStaticConstInitializerInClassDeclarationNYIBug
		#define	qStaticConstInitializerInClassDeclarationNYIBug						1
		#endif

		//	Broken  in MSVC60 - LGP 2003-04-21
		//
		#ifndef	qCannotStaticlyInitializeConstantsDataMembersInClassDeclaration
		#define	qCannotStaticlyInitializeConstantsDataMembersInClassDeclaration		1
		#endif

		//	Broken  in MSVC60 - LGP 2003-04-21
		//
		#ifndef	qTroubleLookingUpBaseClassInNestedClassDeclaration_VC6
		#define	qTroubleLookingUpBaseClassInNestedClassDeclaration_VC6				1
		#endif

		//	Broken  in MSVC60 - LGP 2002-11-20
		//
		#ifndef	qUsingMemberNameToOverloadInTemplateClassBug
		#define	qUsingMemberNameToOverloadInTemplateClassBug						1
		#endif

		//	Broken  in MSVC60 - LGP 990304
		//	First noticed 990304
		//	Problem is basicly
		//	template <class X> class Y { class Z { public Z (); ~Z (); }}}};
		//	Then even though you declare the impelmation of Y<X>::Z::Z and ~Z, you still get linker errors.
		#ifndef	qNestedClassesInTemplateClassesDontExpandCompilerBug
			#define	qNestedClassesInTemplateClassesDontExpandCompilerBug			1
		#endif

		//	Broken  in MSVC60 - LGP 2002-10-23
		#ifndef	qAutoPtr_NoResetMethodBug
			#define	qAutoPtr_NoResetMethodBug										1
		#endif

		//	Broken  in MSVC60 - LGP 980923
		#ifndef	qNewOfVectorXOfXRefCausesInternalCompilerError
			#define	qNewOfVectorXOfXRefCausesInternalCompilerError					1
		#endif

		//	Still broken in MSVC60 - LGP 980923
		//	Still broken in MSVC50 - LGP 970312
		//	first noticed - LGP 960509
		#ifndef	qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug
			#define	qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug		1
		#endif

		//	Still broken in MSVC60 - LGP 980923 (AMAZING!AMAZING!AMAZING!)
		//	Still broken in MSVC50 - LGP 970312 (AMAZING!)
		//	Still broken in MSVC 4.2 (LGP 970115)
		//	Still broken in MSVC 4.1 (LGP 960417)
		//	Still broken In MSVC 4.0 (LGP 951125)
		//	MSVC 2.2 and earlier have this problem - LGP 950821
		#ifndef	qNoSupportForNewForLoopScopingRules
			#define	qNoSupportForNewForLoopScopingRules								1
		#endif

		//	Broken in MSVC60 - LGP 2000/04/30
		#ifndef	qTroubleAccessingNestedProtectedClassNamesInSubclass
			#define	qTroubleAccessingNestedProtectedClassNamesInSubclass			1
		#endif

		//	Broken in MSVC60 - LGP 2000/05/31
		//	Confusion between two different implemntations in MSVC 60 header files lead to linker errors
		#ifndef	qISSPACE_ETC_FUNCTIONS_LinkerError
			#define	qISSPACE_ETC_FUNCTIONS_LinkerError								1
		#endif

		//	Broken in MSVC60 - LGP 2002-09-18 (almost certainly MUCH earlier. I've seen this many times before)
		//	Complex inheritance hierarchies don't interact well with calling methods during construction/destruction
		//	for the MSFT compiler. After a couple level of calls, you get a garbaged 'this' pointer (according to the
		//	debugger, and behavior).
		//
		#ifndef	qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
			#define	qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR		1
		#endif

		//	Broken in MSVC60 - LGP 2002-09-19
		#ifndef	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			#define	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase		1
		#endif

		//	Broken in MSVC60 - LGP 2003-04-21 (same as qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase ???)
		#ifndef	qAccessCheckAcrossInstancesSometimesWrong
			#define	qAccessCheckAcrossInstancesSometimesWrong							1
		#endif

		//	Broken in MSVC60 - LGP 2002-09-20
		// I could EASILY have mischaracterised this problem - but it appears to have the nested class
		// stuff with the friend declaraition - LGP 2002-09-20
		#ifndef	qFriendDeclarationsDontWorkWithNestedClassesBug
			#define	qFriendDeclarationsDontWorkWithNestedClassesBug					1
		#endif

		//	Broken in MSVC60 - LGP 2002-12-16
		#ifndef	qXOfXRefCTORWithTemplateMemberXOfXRefFailsBug
			#define	qXOfXRefCTORWithTemplateMemberXOfXRefFailsBug					1
		#endif

		//	Broken in MSVC60 - LGP 2002-12-16
		//	Is the Uniscribe SDK (.h file etc) available with this compiler?
		#ifndef	qUniscribeAvailableWithSDK
			#define	qUniscribeAvailableWithSDK										0
		#endif


	#elif	_MSC_VER == _MSVC50_VER_

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 5.0 requires you to turn on RTTI"
		#endif

		//	Still broken in MSVC50 - LGP 970312
		//	first noticed - LGP 960901 - relatively new ansiC++ feature...
		#ifndef	qNoDeclaringVarInsideCondition
			#define	qNoDeclaringVarInsideCondition									1
		#endif

		//	Still broken in MSVC50 - LGP 970312
		//	first noticed - LGP 960509
		#ifndef	qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug
			#define	qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug		1
		#endif

		//	Still broken in MSVC50 - LGP 970312 (AMAZING!)
		//	Still broken in MSVC 4.2 (LGP 970115)
		//	Still broken in MSVC 4.1 (LGP 960417)
		//	Still broken In MSVC 4.0 (LGP 951125)
		//	MSVC 2.2 and earlier have this problem - LGP 950821
		#ifndef	qNoSupportForNewForLoopScopingRules
			#define	qNoSupportForNewForLoopScopingRules								1
		#endif

		//	Broken in MSVC50 - LGP 970707
		//
		//	NB: Cannot just turn off to test if fixed cuz
		//	several bugs trigger the same workaround.
		//
		//	NB:	Also note that some code changed even if this is NOT true (ChunkedArrayTextStore::TextChunk).
		//	Must move the class to be nested again to see if fixed.
		#ifndef	qSTLVectorTemplatesFailToExpandForNestedClasses
			#define	qSTLVectorTemplatesFailToExpandForNestedClasses					1
		#endif

		//	Broken in MSVC50 - LGP 970707
		//
		//	NB: Cannot just turn off to test if fixed cuz
		//	several bugs trigger the same workaround.
		#ifndef	qSTLTemplatesErroniouslyRequireOpLessCuzOfOverExpanding
			#define	qSTLTemplatesErroniouslyRequireOpLessCuzOfOverExpanding			1
		#endif

		//	Broken in MSVC50 - LGP 970707
		//
		//	NB: Cannot just turn off to test if fixed cuz
		//	several bugs trigger the same workaround.
		#ifndef	qSTLTemplatesErroniouslyRequireOpEqualsCuzOfOverExpanding
			#define	qSTLTemplatesErroniouslyRequireOpEqualsCuzOfOverExpanding		1
		#endif

		//	Broken in MSVC50 - LGP 970707
		//
		//	NB: Cannot just turn off to test if fixed cuz
		//	several bugs trigger the same workaround.
		#ifndef	qSTLTemplatesErroniouslyRequireDefCTORCuzOfOverExpanding
			#define	qSTLTemplatesErroniouslyRequireDefCTORCuzOfOverExpanding		1
		#endif

		//	Broken in MSVC60 - LGP 2002-12-16
		//	Is the Uniscribe SDK (.h file etc) available with this compiler?
		#ifndef	qUniscribeAvailableWithSDK
			#define	qUniscribeAvailableWithSDK										0
		#endif

	#elif	_MSC_VER < _MSVC50_VER_

		// Might not be too hard to support, but probably best to upgrade your compiler...
		//
		// See Led version 2.2, or earlier, if you must use such an old compiler.
		// Led 2.1 supported MSVC back (roughly) to version 9 (MSVC20).
		#error	"We don't support such an old compiler."

	#else

		#error	"Unknown compiler version. How could you have missed the above cases?"

	#endif
#endif








#if		defined (__GNUC__)

	#if		__GNUC__ > 3 || __GNUC__ == 3

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		__GNUC__ > 3
			#error	"Don't know this version of the GNUC compiler - using last known GNUC bug defines."
		#endif
// Just started doing VERY minimal testing with gcc on MacOS X 10.2 (gcc 3.1? apx)

		// First noticed broken with GCC 3.2 - 2003-12-11
		#ifndef	qMixinDisambiguatingNameInBothBug
			#define	qMixinDisambiguatingNameInBothBug				1
		#endif

		//g++ libs doesn't seem to have this
		#define	qBitSetTemplateAvailable							0

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 000115
		#ifndef	qNoSTRNICMP
			#define	qNoSTRNICMP										1
		#endif


		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-10-30
		#ifndef	qNestedTemplateCTORInTemplateBug
			#define	qNestedTemplateCTORInTemplateBug				1
		#endif


		//	First found broken in GCC 3.? (RedHat 7.3 - but its NOT broken in GCC 3.2
		//	in RedHat 8) - LGP 2003-06-27
		//	Use __GNUC_PREREQ (from <features.h>) to test if we need this
		//	bug workaround or not... LGP 2003-12-07
		#ifndef	qBasicString_C_STR_BROKEN_forWCHAR_T
			#if		__GNUC_PREREQ (3,2)
				#define	qBasicString_C_STR_BROKEN_forWCHAR_T			0
			#else
				#define	qBasicString_C_STR_BROKEN_forWCHAR_T			1
			#endif
		#endif


		//new - GCC specific bug... with gcc 2.96 (RedHat 7.3) - 2003-04-18
		#ifndef	qTroubleOverloadingXofXRefCTORWithTemplatedMemberCTOR
			#define	qTroubleOverloadingXofXRefCTORWithTemplatedMemberCTOR						1
		#endif

		#ifndef	qSTLwstringBugNeedsTypedef
			#if		qUseGCC295
				#define	qSTLwstringBugNeedsTypedef						0
			#else
				#define	qSTLwstringBugNeedsTypedef						1
			#endif
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-10-23
		#ifndef	qAutoPtrBrokenBug
		#define	qAutoPtrBrokenBug									1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-10-23
		#ifndef	qVirtualBaseMixinCallDuringCTORBug
		#define	qVirtualBaseMixinCallDuringCTORBug					1
		#endif


		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-11-20
		#ifndef	qUsingMemberNameToOverloadInWithUsingBug
			#define	qUsingMemberNameToOverloadInWithUsingBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-11-20
		#ifndef	qUsingMemberNameToOverloadInTemplateClassBug
		#define	qUsingMemberNameToOverloadInTemplateClassBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2001-05-17
		#ifndef	qGCC_OptBugWithLocalClassesScopedInFunction
			#define	qGCC_OptBugWithLocalClassesScopedInFunction		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20 (Thought REALLY doesnt cause crash like
		//	with borland - but still fails to compile.)
		#ifndef	qUsingDirectiveSometimesCausesInternalCompilerErrorBug
			#define	qUsingDirectiveSometimesCausesInternalCompilerErrorBug						1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20
		#ifndef	qGCC_MiscNestedClassNameLookupInTemplateBug
			#define	qGCC_MiscNestedClassNameLookupInTemplateBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20
		#ifndef	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug
			#define	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20
		#ifndef	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			#define	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase					1
		#endif

	#elif		__GNUC__ == 2

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		__GNUC__ > 2
			#error	"Don't know this version of the GNUC compiler - using last known GNUC bug defines."
		#endif

		// First noticed broken with GCC 2.96 - 2004-01-01
		#ifndef	qMixinDisambiguatingNameInBothBug
			#define	qMixinDisambiguatingNameInBothBug				1
		#endif

		// First (and so far only) noted with GCC 2.95, but this MAY also be an issue on the new template processor
		// (currently disabled) for MWERKS 8.3 PRO
		#ifndef	qTemplateProcessorDoesntConsiderBaseClassIfItsTemplatedBug
		#define	qTemplateProcessorDoesntConsiderBaseClassIfItsTemplatedBug	1
		#endif


		//	First found broken in with gcc 2.96 (RedHat 7.3) - LGP 2003-06-27
		#ifndef	qBasicString_C_STR_BROKEN_forWCHAR_T
			#define	qBasicString_C_STR_BROKEN_forWCHAR_T			1
		#endif

		//	First found broken in with gcc 2.96 (RedHat 7.3) - LGP 2003-06-30
		#ifndef	qBasicString_Missing_CompareOverload_T
			#define	qBasicString_Missing_CompareOverload_T			1
		#endif

		// GCC changes alot without changing the compiler version. These changes are reported by derek.shaw@ingeniux.com
		// for GCC 2.95 (I've not tested)
		// define to '1' if you are using this version of GCC
		#ifndef	qUseGCC295
			#define	qUseGCC295	0
		#endif

		//new - GCC specific bug... with gcc 2.96 (RedHat 7.3) - 2003-04-18
		#ifndef	qTroubleOverloadingXofXRefCTORWithTemplatedMemberCTOR
			#define	qTroubleOverloadingXofXRefCTORWithTemplatedMemberCTOR						1
		#endif


		//g++ libs doesn't seem to have this
		#define	qBitSetTemplateAvailable							0

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 000115
		#ifndef	qNoSTRNICMP
			#define	qNoSTRNICMP										1
		#endif


		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-10-30
		#ifndef	qNestedTemplateCTORInTemplateBug
			#define	qNestedTemplateCTORInTemplateBug				1
		#endif


		#ifndef	qSTLwstringBugNeedsTypedef
			#if		qUseGCC295
				#define	qSTLwstringBugNeedsTypedef						0
			#else
				#define	qSTLwstringBugNeedsTypedef						1
			#endif
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-10-23
		#ifndef	qAutoPtrBrokenBug
		#define	qAutoPtrBrokenBug									1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-10-23
		#ifndef	qVirtualBaseMixinCallDuringCTORBug
		#define	qVirtualBaseMixinCallDuringCTORBug					1
		#endif


		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-11-20
		#ifndef	qUsingMemberNameToOverloadInWithUsingBug
			#define	qUsingMemberNameToOverloadInWithUsingBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-11-20
		#ifndef	qUsingMemberNameToOverloadInTemplateClassBug
		#define	qUsingMemberNameToOverloadInTemplateClassBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2001-05-17
		#ifndef	qGCC_OptBugWithLocalClassesScopedInFunction
			#define	qGCC_OptBugWithLocalClassesScopedInFunction		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20 (Thought REALLY doesnt cause crash like
		//	with borland - but still fails to compile.)
		#ifndef	qUsingDirectiveSometimesCausesInternalCompilerErrorBug
			#define	qUsingDirectiveSometimesCausesInternalCompilerErrorBug						1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20
		#ifndef	qGCC_MiscNestedClassNameLookupInTemplateBug
			#define	qGCC_MiscNestedClassNameLookupInTemplateBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20
		#ifndef	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug
			#define	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug		1
		#endif

		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2002-09-20
		#ifndef	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			#define	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase					1
		#endif


		//	First found broken in egcs-2.91.66 (Linux 6.1) - LGP 2003-04-15
		#ifndef	qAccessCheckAcrossInstancesSometimesWrong
			#define	qAccessCheckAcrossInstancesSometimesWrong							1
		#endif

	#else
		// Might not be too hard to support, but probably best to upgrade your compiler...
		#error	"We don't support such an old compiler."
	#endif

#endif




#if     defined (__BCPLUSPLUS__)
	#if	__BCPLUSPLUS__ == 0x0550

		// noticed bug... LGP 2003-03-24 - NOT a problem for NON-UNICODE - but it IS a problem for UNICODE builds...
		#ifndef	qNoSTRNICMP
			#define	qNoSTRNICMP																	1
		#endif

		//new - borland specific bug... LGP 2003-03-21
		#ifndef	qBCCOverloadWIthCallToLowerBoundTemplateBug
		#define	qBCCOverloadWIthCallToLowerBoundTemplateBug										1
		#endif


		// BCC 5.5 bug - LGP 2003-05-09
		#ifndef	qAccessCheckAcrossInstancesSometimesWrong
			#define	qAccessCheckAcrossInstancesSometimesWrong									1
		#endif


		//new - borland specific bug... LGP 2003-03-24
		//	not 100% really a bug with BCC - but an unfortunate design issue with the BCC VCL library
		//	(destroying widgets from static DTOR)
		#ifndef	qBCCStaticVCLDTORLibBug
		#define	qBCCStaticVCLDTORLibBug															1
		#endif

		//new - borland specific bug... LGP 2003-01-23
		#ifndef	qCompilerBuggyOverloadingConstOperators
		#define	qCompilerBuggyOverloadingConstOperators											1
		#endif

		//new - noticed bug... LGP 2003-01-23
		#ifndef	qGCC_OptBugWithLocalClassesScopedInFunction
		#define	qGCC_OptBugWithLocalClassesScopedInFunction										1
		#endif

		//new - borland specific bug... LGP 2002-10-31
		#ifndef	qAutoPtrBrokenBug
		#define	qAutoPtrBrokenBug																1
		#endif

		//new - borland specific bug... LGP 2001-09-26
		#ifndef	qInternalErrorWithStaticRegionDeclaredInFunction
			#define	qInternalErrorWithStaticRegionDeclaredInFunction							1
		#endif

		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qCannotAssignRValueAutoPtrToExistingOneInOneStepBug
			#define	qCannotAssignRValueAutoPtrToExistingOneInOneStepBug							1
		#endif

		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qStaticInitializerOfPairOfStringStringInternalCompilerBug
			#define	qStaticInitializerOfPairOfStringStringInternalCompilerBug					1
		#endif

		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
			#define	qNestedClassesWithoutTypeDefArgToTemplateCompilerBug						1
		#endif

		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
			#define	qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug	1
		#endif

		//new - borland specific bug... LGP 2002-09-19
		#ifndef	qUsingDirectiveSometimesCausesInternalCompilerErrorBug
			#define	qUsingDirectiveSometimesCausesInternalCompilerErrorBug						1
		#endif

		//new -  bug... LGP 2002-09-19
		#ifndef	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			#define	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase					1
		#endif

		//new -  bug... LGP 2002-09-19
		#ifndef	qAccessChecksSometimesBreakForNestedClasses
			#define	qAccessChecksSometimesBreakForNestedClasses									1
		#endif

		//	Broken in Borland C++ 5.5 - LGP 2002-11-21
		//
		#ifndef	qUsingMemberNameToOverloadInTemplateClassBug
		#define	qUsingMemberNameToOverloadInTemplateClassBug									1
		#endif

		//	Broken in Borland C++ 5.5 - LGP 2003-05-20
		//
		#ifndef	qNameLookupInBaseClassWhenItIsNestedSometimesFailsBug
		#define	qNameLookupInBaseClassWhenItIsNestedSometimesFailsBug							1
		#endif

	#elif	__BCPLUSPLUS__ == 0x0540
	
	
		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qCannotAssignRValueAutoPtrToExistingOneInOneStepBug
			#define	qCannotAssignRValueAutoPtrToExistingOneInOneStepBug							1
		#endif

		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qStaticInitializerOfPairOfStringStringInternalCompilerBug
			#define	qStaticInitializerOfPairOfStringStringInternalCompilerBug					1
		#endif

		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
			#define	qNestedClassesWithoutTypeDefArgToTemplateCompilerBug						1
		#endif

		//new - borland specific bug... LGP 2000-12-30
		#ifndef	qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
			#define	qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug	1
		#endif

		//new - borland specific bug...Reported by Claudio Nold for Borland4 LGP 2001-01-04
		#ifndef	qMustUseNoUnderbarForOpenFlagsCompilerBug
			#define	qMustUseNoUnderbarForOpenFlagsCompilerBug									1
		#endif

		//new - borland specific bug...Reported by Claudio Nold for Borland4 LGP 2001-01-04
		#ifndef	qBorlandNameInLocalFunctDeclarationSpaceCompilerBug
			#define	qBorlandNameInLocalFunctDeclarationSpaceCompilerBug							1
		#endif

		//new - borland specific bug...Reported by Claudio Nold for Borland4 LGP 2001-01-04
		#ifndef	qNO_INT_PTR_DefinedCompilerBug
			#define	qNO_INT_PTR_DefinedCompilerBug												1
		#endif

	#elif	__BCPLUSPLUS__ < 0x0550
		#error "older version of Borland C++ compiler - recheck options"
	#elif	__BCPLUSPLUS__ > 0x0550 
		#error "newer version of Borland C++ compiler - recheck options"
	#endif
#endif




/*
 * Check and warn if using an unknown compiler.
 */
#if		defined (__cplusplus)

	#if		!defined (__BCPLUSPLUS__) && !defined (__MWERKS__) && !defined (_MSC_VER) && !defined (__GNUC__)
			#error	"We don't support this unknown compiler."
	#endif

#endif




/// MAY THIS A COMMENTED DEFINE - AND FIXUP THE ABOVE __GNUC__ STUFF LIKE OTHER COMPILER SETTINGS - LGP 991214
#ifndef	qBitSetTemplateAvailable
#define	qBitSetTemplateAvailable	1
#endif















/*
 *	Do things like memcpy () get inlined by the compiler, or should we do so ourselves
 *	manually for cases that matter.
 */
#ifndef	qComplilerDoesNotInlineIntrinsics
	#define	qComplilerDoesNotInlineIntrinsics	0
#endif







/*
 *	Not a bug to have this - I think - but doesn't appear part of any ansi spec???
 */
#ifndef	qHasIsAscii
	#define	qHasIsAscii							0
#endif









/*
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 **************** COMMOM configuration variables ***************
 */


/*
@CONFIGVAR:		qDebug
@DESCRIPTION:	<p>Be sure debug #defines setup properly. These are just some sanity checks.
	In Led - we use qDebug. Just make sure other defines are setup consistently
	with that define.</p>
		<p>The default value of the qDebug variable is !defined (NDEBUG)</p>
		<p>But typically, it is set externally via prefix files (Precomp.pch, MSVC settings dialog, or some such).</p>
		<p>See also @'qHeavyDebugging'</p>
 */
#if		defined (__cplusplus)
	#ifndef	qDebug
		// for some reason, directly defining this as qDebug=!defined(NDEBUG) didn't work on msvc
		// LGP 070404
		#if		defined(NDEBUG)
			#define	qDebug	0
		#else
			#define	qDebug	1
		#endif
	#endif
	#ifndef	qDebug
		#error	"This must be defined either 0 or 1, depending on if you are building a debug version or not"
	#endif
	#if		qDebug && defined (NDEBUG)
		#error	"qDebug and NDEBUG must agree"
	#endif
	#if		!qDebug && !defined (NDEBUG)
		#error	"qDebug and NDEBUG must agree"
	#endif
	
	#if		defined (_MSC_VER)
		#if		qDebug && !defined (_DEBUG)
			#error	"qDebug and _DEBUG must agree"
		#endif
		#if		!qDebug && defined (_DEBUG)
			#error	"qDebug and _DEBUG must agree"
		#endif
	#endif
#endif




/*
@CONFIGVAR:		qHeavyDebugging
@DESCRIPTION:	<p>Led contains lots of internal debugging code which is mainly intended to find bugs
	in Led itself. Its less valuable for finding bugs in your programs' usage of Led. Much of that debugging
	code makes Led quite slow (for larger documents).</p>
		<p>The Led sample applications are all built with qHeavyDebugging when qDebug is on. But Led defaults
	to having this value false, so that your applications won't be needlessly slow.</p>
		<p>If you run into some subtle bug, or if you aren't worried about the speed of Led with large documents
when debugging is ON, then you may want to turn this flag ON.</p>
		<p>See also @'qDebug'</p>
 */
#ifndef	qHeavyDebugging
#define	qHeavyDebugging		0
#endif








/*
@CONFIGVAR:		qNestedTablesSupported
@DESCRIPTION:	<p>This define controls whether or not the Led WordProcessor/StyledTextIO class support nested tables.
			This will <em>not</em> be supported for Led 3.1, but probably will be in a release thereafter.
			There is SOME code available for both cases, but just a little (as of 2003-04-12).</p>
 */
#ifndef	qNestedTablesSupported
#define	qNestedTablesSupported	0
#endif





/*
@CONFIGVAR:		qSupportLed30CompatAPI
@DESCRIPTION:	<p>A way to incrementally, semi-backward-compatably implement API changes. Also, this
	serves as documentation.</p>
		<p>Any methods etc marked with @'qSupportLed30CompatAPI' will be supported (assuming this flag
	is set true) for Led 3.1, but probably NOT thereafter. You can get your programs compiling
	with this set true. But you should try getting stuff compiling with this
	set OFF soon thereafter. Using the NEW apis will (hopefully make your code clearer), but at
	any rate, make it more easy to migrate to the NEXT Led release.</p>
		<p>Default is OFF</p>
 */
#ifndef	qSupportLed30CompatAPI
	// ship with off, but test with it both ways!!!
	//#define	qSupportLed30CompatAPI				1
	#define	qSupportLed30CompatAPI						0
#endif






/*
 *	-----------------------        Character set Settings/Specifications         ---------------------------
 */



#if		qWindows
	/*
	 *	Some of Microsoft's headers use _UNICODE and some use UNICODE. If users set one or the other, treat that as having
	 *	set both.
	 */
	#ifdef _UNICODE
		#ifndef UNICODE
		#define UNICODE         // UNICODE is used by Windows headers
		#endif
	#endif

	#ifdef UNICODE
		#ifndef _UNICODE
		#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
		#endif
	#endif
#endif




#if		qSupportLed30CompatAPI
/*
@CONFIGVAR:		qLed_CharacterSet
@DESCRIPTION:	<p>OBSOLETE</p>
				<p>As of Led 3.1a4 - you must directly define @'qSingleByteCharacters' or
			@'qMultiByteCharacters' or @'qWideCharacters'.
				</P>
		<p>It was a difficult choice deciding how to deal with character set issues.
	There are four major character sets I was immediately concerned with supporting,
	and, of course, hundreds more that I'd like to be able to support in the future.
	And then there is the question of supporting different character sets at the same
	time.</p>
		<p>Since my immediate contractual obligations only require support for SJIS, and
	possibly ASCII, and or Extended ASCII, and not in any mixed form, I've allowed myself
	the luxury of not worrying beyond that for now. Also - I've taken the approach of
	building the choice of character set in at the compilation level. This is a
	simplification since it allows me to make type choices (as WinNT does for UNICODE)
	depending on character set choices. The difference probably doesn't matter for
	different single byte character sets.</p>
		<p>Anyhow - for the time being, the character set defined at compile time is
	given by qLed_CharacterSet.</p>
		<p><code>qLed_CharacterSet</code> can be any one of the following:
		<ul><code>
			<li>qASCII_CharacterSet</li><br>
			<li>qExtendedASCII_CharacterSet</li><br>
			<li>qSJIS_CharacterSet</li><br>
			<li>qUNICODE_CharacterSet</li><br>
		</ul></code>
		</p>
		<p>Depending on which value is specified, Led will automatically define one of:
		<ul><code>
			<li>qSingleByteCharacters</li><br>
			<li>qMultiByteCharacters</li><br>
			<li>qWideCharacters</li><br>
		</ul></code>
		and will use that value internally.
		</p>
		<p><b>WARNING</b>All this character set support is subject to change, and is being re-thought for future
	Led releases. I probably will go in the direction of just supporting ASCII, or UNICODE. And not all the various
	MBYTE character sets (mainly cuz then no good way to mix charactersets within a document). -- LGP 970722</p>
 */
	#define	qASCII_CharacterSet			1
	#define	qExtendedASCII_CharacterSet	2
	#define	qUNICODE_CharacterSet		3
	#define	qSJIS_CharacterSet			4

	#ifndef	qLed_CharacterSet
		#ifdef	_UNICODE
			#define	qLed_CharacterSet		qUNICODE_CharacterSet
		#else
			#define	qLed_CharacterSet		qExtendedASCII_CharacterSet
		#endif
	#endif


	// DON'T Edit THESE directly - change the characterset, and allow
	// these to be derivitive...
	#if		qLed_CharacterSet == qASCII_CharacterSet
		#define	qSingleByteCharacters	1
		#define	qMultiByteCharacters	0
		#define	qWideCharacters			0
	#elif	qLed_CharacterSet == qExtendedASCII_CharacterSet
		#define	qSingleByteCharacters	1
		#define	qMultiByteCharacters	0
		#define	qWideCharacters			0
	#elif	qLed_CharacterSet == qUNICODE_CharacterSet
		#define	qSingleByteCharacters	0
		#define	qMultiByteCharacters	0
		#define	qWideCharacters			1
	#else
		#error	"Unknown character set"
	#endif

#else

	#if		defined (qLed_CharacterSet)
		#error	"This is OBSOLETE - use qSingleByteCharacters/qMultiByteCharacters/qWideCharacters"
	#endif

#endif






/*
@CONFIGVAR:		qSingleByteCharacters
@DESCRIPTION:	<p>Define this if you will only be editing 8-bit ascii or extended ASCII characters, and don't
			want to deal with UNICODE.</p>
				<p>See also @'qMultiByteCharacters' and @'qWideCharacters'. Note that
			@'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
				<p>Defaults OFF</p>
 */
#ifndef	qSingleByteCharacters
#define	qSingleByteCharacters			0
#endif


/*
@CONFIGVAR:		qMultiByteCharacters
@DESCRIPTION:	<p>Define this if you will only be editing multibyte text (e.g. SJIS) and don't
			want to deal with UNICODE.</p>
				<p>See also @'qMultiByteCharacters' and @'qSingleByteCharacters'. Note that
			@'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
				<p>Note that this is really only partially still supported, and may not be supported in the future (as of Led 3.1a4).
			To make this define work - you must implement yourself @'Led_IsLeadByte' and @'Led_IsValidSecondByte' and perhaps
			a few other small matters. The problem with doing this is that its not clear WHICH mutibyte code set you are compiling for.
			Led's multibyte support is designed to only work for a particular multibyte code set at a time (though this could
			be a global variable - perhaps - rather than a compile-time constant). Anyhow - if @'qSingleByteCharacters' isn't good
			enough - we strongly recomend using UNICODE (@'qWideCharacters').</p>
				<p>Defaults OFF</p>
 */
#ifndef	qMultiByteCharacters
#define	qMultiByteCharacters			0
#endif


/*
@CONFIGVAR:		qWideCharacters
@DESCRIPTION:	<p>Define this to work with the widest variety of characters - UNICODE. This is the preferred way
			to build Led, and as a result - is now the default (as of Led 3.1a4).</p>
				<p>See also @'qMultiByteCharacters' and @'qSingleByteCharacters'. Note that
			@'qWideCharacters', @'qMultiByteCharacters', and @'qSingleByteCharacters' are mutually exclusive.</p>
 				<p>Defaults TRUE (if @'qMultiByteCharacters', and @'qSingleByteCharacters' are false).</p>
*/
#ifndef	qWideCharacters
	#if		qMultiByteCharacters || qSingleByteCharacters
		#define	qWideCharacters			0
	#else
		#define	qWideCharacters			1
	#endif
#endif





// Be sure ONLY ONE of these flags set
#if		qSingleByteCharacters && (qMultiByteCharacters || qWideCharacters)
	#error	"Only one can be set at a time"
#endif
#if		qMultiByteCharacters && (qSingleByteCharacters || qWideCharacters)
	#error	"Only one can be set at a time"
#endif
#if		qWideCharacters && (qSingleByteCharacters || qSingleByteCharacters)
	#error	"Only one can be set at a time"
#endif
#if		!qSingleByteCharacters && !qMultiByteCharacters && !qWideCharacters
	#error	"At least one should be set"
#endif







/*
@CONFIGVAR:		qSDK_UNICODE
@DESCRIPTION:	<p>Roughly this corresponds to the MSDEV -DUNICODE -D_UNICODE define. But can
	be used for other things as well on other platforms? Maybe?</p>
 */
#ifndef	qSDK_UNICODE
	#ifdef	_UNICODE
		#define	qSDK_UNICODE		1
	#else
		#define	qSDK_UNICODE		0
	#endif
#endif





/*
@CONFIGVAR:		qProvideIMESupport
@DESCRIPTION:	<p>Do we need to explictly support the IME?</p>
		<p>NB: qProvideIMESupport used to be called qRequireIMESupport, but now we
	simply provide it, and it essentailly becomes a no-op if you've no IME installed.</p>
		<p>This is ON by default (for windows). It degrades gracefully if no IME found.</p>
 */
#ifndef	qProvideIMESupport
	#if		qWindows
		#define	qProvideIMESupport						1
	#else
		#define	qProvideIMESupport						0
	#endif
#endif





/*
@CONFIGVAR:		qWorkAroundWin95BrokenUNICODESupport
@DESCRIPTION:	<p>Win95 doesn't completely support UNICODE. In particular, the clipboard/Drag&Drop stuff
	does't work with UNICODE. This workaround is on by default, and really only does anything if qWideCharacters
	is set on (depends on character set you've defined). This is here mostly for documentation of why I'm doing
	these workarounds (so they can be lifted in the future), and so folks can conidtionally compile them out
	as Win95 fades into history.</p>
		<p>This is ON by default (for windows, and if _UNICODE not defined - cuz there is no point - _UNICODE doesn't work on Win95)</p>
 */
#ifndef	qWorkAroundWin95BrokenUNICODESupport
	#define	qWorkAroundWin95BrokenUNICODESupport		(qWindows && !defined (_UNICODE))
#endif





/*
@CONFIGVAR:		qAllowBlockAllocation
@DESCRIPTION:	<p>Allow use of block-allocation. The main reason to
	disable it indescriminantly is for debugging purposes (looking for
	memory leaks). But others may have other reasons.</p>
		<p>Defaults to true.</p>
 */
#ifndef	qAllowBlockAllocation
	#define	qAllowBlockAllocation						1
#endif





/*
@CONFIGVAR:		qPeekForMoreCharsOnUserTyping
@DESCRIPTION:	<p>A trick to get better (worse?) interactivity in typing.</p>
		<p>Defaults to true.</p>
 */
#ifndef	qPeekForMoreCharsOnUserTyping
	#define	qPeekForMoreCharsOnUserTyping				1
#endif





/*
@CONFIGVAR:		qAllowRowsThatAreLongerThan255
@DESCRIPTION:	<p>This define allows control over whether we perform a packing optimization to save memory.
	If we do (qAllowRowsThatAreLongerThan255 FALSE) then we asssert were are never
	given a row > 255 pixels. Similarly, we disalow rows of more than 255 characters.
	If qAllowRowsThatAreLongerThan255 - fine - we just don't optimize packing.</p>
		<p>For now, not fully implemented. Only for RowHeights, not for RowStarts.</p>
		<p>See SPR#0257</p>
 */
#ifndef	qAllowRowsThatAreLongerThan255
	#define	qAllowRowsThatAreLongerThan255				1
#endif





/*
@CONFIGVAR:		qDoubleClickSelectsSpaceAfterWord
@DESCRIPTION:	<p>qDoubleClickSelectsSpaceAfterWord is a very silly idea, imho, but it is a standard UI feature
	on MS Windows editors, and not uncommon on the Mac. As far as I can tell, the idea
	is just to select a word, plus all trailing whitespace on double clicks. The idea is
	that this somehow makes cut/paste work more easily. So it is sometimes refered
	to as smart cut and paste or intelligent cut and paste.</p>
		<p>By default I USED TO enable this only for windows.
	Now it is always OFF by default, since I think its too stupid, and annoying.
	Someday soon I'll implement better smart-cut/paste like Style(on Mac) does.</p>
 */
#ifndef	qDoubleClickSelectsSpaceAfterWord
	#if		qWindows
		#define	qDoubleClickSelectsSpaceAfterWord		0
	#else
		#define	qDoubleClickSelectsSpaceAfterWord		0
	#endif
#endif





/*
@CONFIGVAR:		qDynamiclyChooseAutoScrollIncrement
@DESCRIPTION:	<p>On slower computers, this can make scrolling appear a bit faster. At some risk to
	predictablity...</p>
 */
#ifndef	qDynamiclyChooseAutoScrollIncrement
	#define	qDynamiclyChooseAutoScrollIncrement			1
#endif





/*
@CONFIGVAR:		qUseOffscreenBitmapsToReduceFlicker
@DESCRIPTION:	<p>Led already has very little flicker. This is because we are very careful to
	draw as little as possible, and to draw quickly. But some cases still exist.
	Like large pictures being drawn are flicker, cuz we must erase the bounds and then
	draw the picture.</p>
		<p>Using this gets rid of these few cases of flicker, but at a small performance cost
	in overall draw speed.</p>
		<P> THIS FLAG IS NOW OBSOLETE (as of Led 3.0b6). Its still supported for backwards compatability.
	But now all it does is set the default VALUE for the ImageUsingOffscreenBitmaps property.</p>
		<p>Instead of using this, use @'TextImager::SetImageUsingOffscreenBitmaps' ().</p>
 */
#ifndef	qUseOffscreenBitmapsToReduceFlicker
	#define	qUseOffscreenBitmapsToReduceFlicker			1
#endif





/*
@CONFIGVAR:		qSilenceAnnoyingCompilerWarnings
@DESCRIPTION:	<p>Replaces 'qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings', 'qQuiteAnnoyingDominanceWarnings',
	'qQuiteAnnoyingDebugSymbolTruncationWarnings'</p>
 */
#ifndef	qSilenceAnnoyingCompilerWarnings
	#define	qSilenceAnnoyingCompilerWarnings			1
#endif





namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {
		}
	}
}
// Be sure namespace is defined for later use.







/*
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 **************** MacOS Specific configuration variables ***************
 */
#if		qMacOS

	#ifndef	TARGET_CARBON
		#define	TARGET_CARBON	0
	#endif


	/*
	@CONFIGVAR:		qPeekAtQuickDrawGlobals
	@DESCRIPTION:	<p><b>MAC ONLY</b></p>
	 */
	#if		defined (__cplusplus)
		#ifndef	qPeekAtQuickDrawGlobals
			#if		TARGET_CARBON
				#define	qPeekAtQuickDrawGlobals	0
			#else
				#if		GENERATINGCFM
					#define	qPeekAtQuickDrawGlobals	0
				#else
					#define	qPeekAtQuickDrawGlobals	1
				#endif
			#endif
		#endif
	#endif





	/*
	@CONFIGVAR:		qUseMacTmpMemForAllocs
	@DESCRIPTION:	<p><b>MAC ONLY</b></p>
			<p>Use of TmpMemory on the mac allows for the editor to run in a small partition
		but to use all available system resources (memory) when the user requests it
		for a large clipbaord operation, or opening a large file, or whatever.</p>
			<p>I believe it generally makes sense to leave this on.</p>
			<p>LGP - 960314</p>
	 */
	#if		!defined (qUseMacTmpMemForAllocs)
		#define	qUseMacTmpMemForAllocs	1
	#endif





	/*
	@CONFIGVAR:		qUseInternetConfig
	@DESCRIPTION:	<p><b>MAC ONLY</b></p>
			<p>For URL support, and any other places where it is appropriate, should we try
		to use Internet Config (IC) when available? Mainly used right now by
		URL embeddings. Default to TRUE on mac. Not avialable elsewhere.</p>
	 */
	#ifndef	qUseInternetConfig
		#if		TARGET_CARBON
			#define	qUseInternetConfig		qSDKHasInternetConfig
		#else
			// disable it by default if using CFM68K - since right now I have no CFM68K lib for it
			#if		defined(__CFM68K__)
				#define	qUseInternetConfig	0
			#else
				#define	qUseInternetConfig	1
			#endif
		#endif
	#endif

#endif	/*qMacOS*/











/*
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 **************** Windows Specific configuration variables **************
 */
#if		qWindows


	/*
	@CONFIGVAR:		qUseQuicktimeForWindows
	@DESCRIPTION:	<p><b>Win32 ONLY</b></p>
			<p>When we attempt to draw Macintosh Picture objects, should we try using QuickTime For Windows?
		If not, the rectangle where the picture should be will be left blank.</p>
	 */
	#ifndef	qUseQuicktimeForWindows
		#define	qUseQuicktimeForWindows			0
	#endif





	/*
	@CONFIGVAR:		qUseSpyglassDDESDIToOpenURLs
	@DESCRIPTION:	<p><b>Win32 ONLY</b></p>
			<p>For supporting openening URLs, use DDE to an application with Spyglass SDI/DDE.
		This takes over DDE processing for your app (if you care, this could probably
		be fixed).</p>
			<p>Detaults OFF now (as of Led 3.0d5, 2000/04/03) - because this may fail when Led embedded in OCX, and
		seems of only very marginal value anyhow. The ActiveX stuff SHOULD work, by now!</p>
	 */
	#ifndef	qUseSpyglassDDESDIToOpenURLs
		#define	qUseSpyglassDDESDIToOpenURLs	0
	#endif	





	/*
	@CONFIGVAR:		qUseActiveXToOpenURLs
	@DESCRIPTION:	<p><b>Win32 ONLY</b></p>
			<p>For supporting openening URLs.</p>
	 */
	#ifndef	qUseActiveXToOpenURLs
		#define	qUseActiveXToOpenURLs			1
	#endif

#endif	/*qWindows*/








/*
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 *************** X-Windows Specific configuration variables *************
 */
#if		qXWindows

	/*
	@CONFIGVAR:		qUseSystemNetscapeOpenURLs
	@DESCRIPTION:	<p><b>X-Windows ONLY</b></p>
			<p>For supporting openening URLs.</p>
	 */
	#ifndef	qUseSystemNetscapeOpenURLs
		#define	qUseSystemNetscapeOpenURLs			1
	#endif

#endif	/*qWindows*/




#endif	/*__LedConfig_h__*/


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

