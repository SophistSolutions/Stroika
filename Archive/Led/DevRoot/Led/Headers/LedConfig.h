/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedConfig_h__
#define	__LedConfig_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/LedConfig.h,v 2.254 2004/02/26 03:23:29 lewis Exp $
 */


/*
@MODULE:	LedConfig
@DESCRIPTION:
		<p>Support basic configuration defines for the operating system, and compiler, and some basic features
	(like character set).
	</p>
 */



/*
 * Changes:
 *	$Log: LedConfig.h,v $
 *	Revision 2.254  2004/02/26 03:23:29  lewis
 *	3.1b2x
 *	
 *	Revision 2.253  2004/02/12 21:12:07  lewis
 *	3.1b1
 *	
 *	Revision 2.252  2004/02/11 01:05:11  lewis
 *	SPR#1635: qFailToCompileLargeDataInitializedArraysBug, qTemplatesWithMixOfNestedClassesWithCommonNamesGetsConfusedBug, qDefaultValueInTemplateParemeterUsingTemplatedTypeBug, qTemplatedMemberFunctionsFailBug bug defines for MSVC60
 *	
 *	Revision 2.251  2004/02/10 19:42:36  lewis
 *	SPR#1635: qNoOpKeywordSupported is false for MSVC6
 *	
 *	Revision 2.250  2004/01/23 21:43:44  lewis
 *	Define qDeleteOfPointerToConstantDataFailsOverloadError and qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug
 *	for MSVC6
 *	
 *	Revision 2.249  2004/01/23 03:00:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.248  2004/01/06 01:29:17  lewis
 *	3.1a9x
 *	
 *	Revision 2.247  2004/01/02 01:40:17  lewis
 *	added qMixinDisambiguatingNameInBothBug define for GCC2
 *	
 *	Revision 2.246  2004/01/01 15:07:09  lewis
 *	3.1a8
 *	
 *	Revision 2.245  2003/12/12 02:39:07  lewis
 *	qMixinDisambiguatingNameInBothBug defined for GCC 3.2
 *	
 *	Revision 2.244  2003/12/12 01:41:51  lewis
 *	qMixinDisambiguatingNameInBothBug BWA for MWERS/MACOS
 *	
 *	Revision 2.243  2003/12/08 02:34:19  lewis
 *	include <features.h> for GCC and then can use __GNUC_PREREQ macro to test intra-major version comparisons
 *	(so can set qBasicString_C_STR_BROKEN_forWCHAR_T flag appropraitely)
 *	
 *	Revision 2.242  2003/06/30 14:28:29  lewis
 *	qBasicString_Missing_CompareOverload_T for GCC 2.96
 *	
 *	Revision 2.241  2003/06/30 04:28:22  lewis
 *	fix qBasicString_C_STR_BROKEN_forWCHAR_T to be in gcc-2 category
 *	
 *	Revision 2.240  2003/06/29 15:31:41  lewis
 *	qBasicString_C_STR_BROKEN_forWCHAR_T
 *	
 *	Revision 2.239  2003/06/10 14:24:54  lewis
 *	3.1a8x
 *	
 *	Revision 2.238  2003/06/05 18:53:46  lewis
 *	3.1a7
 *	
 *	Revision 2.237  2003/05/20 23:40:26  lewis
 *	qNameLookupInBaseClassWhenItIsNestedSometimesFailsBug
 *	
 *	Revision 2.236  2003/05/09 20:58:26  lewis
 *	add qAccessCheckAcrossInstancesSometimesWrong for BCC 5.5, and cleanup defines (tested) for MSVC71
 *	
 *	Revision 2.235  2003/05/09 20:13:00  lewis
 *	Led 3.1a7x
 *	
 *	Revision 2.234  2003/05/08 20:40:05  lewis
 *	fix compiler defs from _MSVC_80 to _MSVC_71 (really part of a6)
 *	
 *	Revision 2.233  2003/05/08 19:12:46  lewis
 *	Led 3.1a6
 *	
 *	Revision 2.232  2003/04/21 16:21:10  lewis
 *	 qAccessCheckAcrossInstancesSometimesWrong for VC6
 *	
 *	Revision 2.231  2003/04/21 16:17:25  lewis
 *	qCannotStaticlyInitializeConstantsDataMembersInClassDeclaration/qTroubleLookingUpBaseClassInNestedClassDeclaration_VC6
 *	for MSVC60 compat
 *	
 *	Revision 2.230  2003/04/18 22:55:13  lewis
 *	misc cleanups to get things compiling with gcc 2.96 on Linux RedHat 7.3
 *	
 *	Revision 2.229  2003/04/18 22:42:55  lewis
 *	Minor tweeks for MSVC80 (VC++ .Net 2003) compatability
 *	
 *	Revision 2.228  2003/04/18 21:02:57  lewis
 *	misc cleanups to get things compiling with gcc 2.96 on Linux RedHat 7.3
 *	
 *	Revision 2.227  2003/04/15 22:57:53  lewis
 *	qAccessCheckAcrossInstancesSometimesWrong GCC/Linux BWA
 *	
 *	Revision 2.226  2003/04/13 23:08:08  lewis
 *	SPR#1427: add qNestedTablesSupported flag and assure we dont get nested tables and handle paste
 *	of tables inside table cells gracefull
 *	
 *	Revision 2.225  2003/03/28 12:36:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.224  2003/03/27 17:06:07  lewis
 *	3.1a5
 *	
 *	Revision 2.223  2003/03/24 14:32:48  lewis
 *	Added qBCCStaticVCLDTORLibBug BCC bug workaround
 *	
 *	Revision 2.222  2003/03/24 14:11:21  lewis
 *	Led 3.1a5x and added qBCCOverloadWIthCallToLowerBoundTemplateBug define
 *	
 *	Revision 2.221  2003/03/21 17:15:43  lewis
 *	3.1a4
 *	
 *	Revision 2.220  2003/03/11 02:23:57  lewis
 *	SPR#1288 - added qLed_MajorMinorVersionString define and MakeSophistsAppNameVersionURL () helper
 *	
 *	Revision 2.219  2003/02/24 23:14:32  lewis
 *	define qNoSTRNICMP for borland C++
 *	
 *	Revision 2.218  2003/02/14 20:40:13  lewis
 *	SPR#1303 - qLed_CharacterSet is now obsolete. Instead - use qSingleByteCharacters, qMultiByteCharacters,
 *	or qWideCharacters
 *	
 *	Revision 2.217  2003/02/13 22:58:09  lewis
 *	SPR#1297- qTemplatedMemberFunctionsFailWithMFCMessageMaps to get MFC code compiling on MSVC6 again
 *	
 *	Revision 2.216  2003/02/03 21:02:36  lewis
 *	qTemplateProcessorDoesntConsiderBaseClassIfItsTemplatedBug BWA
 *	
 *	Revision 2.215  2003/02/03 13:53:28  lewis
 *	rewrite #define for qProvideIMESupport cuz qLedCheckCompilerFlagsConsistency causes finicky compiler
 *	behavior on details of define format (SPR#1291)
 *	
 *	Revision 2.214  2003/01/28 22:45:58  lewis
 *	3.1a4x
 *	
 *	Revision 2.213  2003/01/23 22:37:14  lewis
 *	3.1a3
 *	
 *	Revision 2.212  2003/01/23 21:45:10  lewis
 *	added qCompilerBuggyOverloadingConstOperators/qGCC_OptBugWithLocalClassesScopedInFunction for
 *	BCC bug workarounds
 *	
 *	Revision 2.211  2003/01/22 22:43:52  lewis
 *	SPR#1257- added qSDKHasInternetConfig for MWERKS CW8 (could be around earlier too). And turn
 *	qUseInternetConfig back on for CARBON
 *	
 *	Revision 2.210  2003/01/21 22:49:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.209  2003/01/21 16:45:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.208  2003/01/20 23:50:19  lewis
 *	VERY prelim __GNUC__ version 3 support (so I can work with interface builder on MacOS X). Also -
 *	get rid of qNamespaceSupport being set to 1 in EVERY compiler section - instead default to one and
 *	only set to zero for one place we need it set to zero
 *	
 *	Revision 2.207  2003/01/20 16:32:06  lewis
 *	qNo_argc_argv_MacrosSupported for MWERKS CW8.3 cross compiling for PC
 *	
 *	Revision 2.206  2003/01/20 15:25:01  lewis
 *	turn qUseActiveXToOpenURLs by default - even for MWERKS compiler
 *	
 *	Revision 2.205  2003/01/20 14:44:52  lewis
 *	preliminary bug defines for CWPro 8 and 8.3
 *	
 *	Revision 2.204  2003/01/17 20:22:11  lewis
 *	small changes to GCC defines for GCC295 on MacOS (SPR#1248)
 *	
 *	Revision 2.203  2003/01/17 19:28:52  lewis
 *	get compiling with CW8Pro for MacOS
 *	
 *	Revision 2.202  2002/12/19 16:48:55  lewis
 *	SPR#1215 -  Added EmitTraceMessage/LedDebugTrace/qNoOpKeywordSupported/qDefaultTracingOn
 *	support. Really only works (well) for MS-Windows, but could pretty easily get
 *	working elsewhere - if there is any need.
 *	
 *	Revision 2.201  2002/12/16 22:11:01  lewis
 *	qUniscribeAvailableWithSDK define set for MSVC7 but not earlier compilers. SPR#1209 hack to
 *	Led_RefCntPtr<> doesnt work if qXOfXRefCTORWithTemplateMemberXOfXRefFailsBug (MSVC6)
 *	
 *	Revision 2.200  2002/12/03 02:36:25  lewis
 *	3.1a3x
 *	
 *	Revision 2.199  2002/11/22 01:55:27  lewis
 *	qUsingMemberNameToOverloadInTemplateClassBug for BC++ 5.5
 *	
 *	Revision 2.198  2002/11/21 16:17:51  lewis
 *	3.1a2
 *	
 *	Revision 2.197  2002/11/21 02:07:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.196  2002/11/21 01:57:37  lewis
 *	qUsingMemberNameToOverloadInTemplateClassBug for GCC
 *	
 *	Revision 2.195  2002/11/20 20:06:25  lewis
 *	define qUsingMemberNameToOverloadInWithUsingBug for GCC
 *	
 *	Revision 2.194  2002/11/20 18:52:11  lewis
 *	qUsingMemberNameToOverloadInTemplateClassBug for MWERKS
 *	
 *	Revision 2.193  2002/11/20 17:35:48  lewis
 *	Added qStaticConstInitializerInClassDeclarationNYIBug and qUsingMemberNameToOverloadInTemplateClassBug
 *	bug workarounds
 *	
 *	Revision 2.192  2002/11/07 03:12:05  lewis
 *	3.1a2x
 *	
 *	Revision 2.191  2002/11/02 01:21:03  lewis
 *	version 3.1a1 (instead of d1)
 *	
 *	Revision 2.190  2002/11/01 15:46:32  lewis
 *	Added qNestedTemplateCTORInTemplateBug bug define for MWERKS CWPro7
 *	
 *	Revision 2.189  2002/10/31 14:46:39  lewis
 *	qAutoPtrBrokenBug for BCC
 *	
 *	Revision 2.188  2002/10/31 04:21:28  lewis
 *	qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug for MSVC
 *	
 *	Revision 2.187  2002/10/31 03:24:42  lewis
 *	Work around qNestedTemplateCTORInTemplateBug for GCC and MSVC60
 *	
 *	Revision 2.186  2002/10/30 15:16:46  lewis
 *	Add qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug
 *	
 *	Revision 2.185  2002/10/24 01:57:03  lewis
 *	GCC bug workaround - qVirtualBaseMixinCallDuringCTORBug
 *	
 *	Revision 2.184  2002/10/23 20:01:18  lewis
 *	SPR#1144 - qAutoPtrBrokenBug workaround
 *	
 *	Revision 2.183  2002/10/23 18:00:21  lewis
 *	small tweeks to get working on MSVC60
 *	
 *	Revision 2.182  2002/09/20 18:02:05  lewis
 *	qFriendDeclarationsDontWorkWithNestedClassesBug for MSVC70
 *	
 *	Revision 2.181  2002/09/20 15:05:30  lewis
 *	Add lots of bug defines to get things working on all compiler platforms (
 *	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug, qFriendDeclarationsDontWorkWithNestedClassesBug,
 *	qGCC_MiscNestedClassNameLookupInTemplateBug, qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug, 
 *	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase) and deleted obsolete (wasnt really a bug)
 *	qTypedefsOfPrivateAccessNamesNotAllowedInSourceFile.
 *	
 *	Revision 2.180  2002/09/20 13:03:23  lewis
 *	added qUsingDirectiveSometimesCausesInternalCompilerErrorBug for GCC
 *	
 *	Revision 2.179  2002/09/20 12:45:21  lewis
 *	qConstNonConstPtrConversionsWithTemplatedMemberFunctionBug workaround for MWERKS CWPro7 compiler
 *	
 *	Revision 2.178  2002/09/20 00:26:33  lewis
 *	qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase for MWERKS
 *	
 *	Revision 2.177  2002/09/19 23:37:07  lewis
 *	qUsingDirectiveSometimesCausesInternalCompilerErrorBug/qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase/
 *	qAccessChecksSometimesBreakForNestedClasses to get compiling with BorlandC++
 *	
 *	Revision 2.176  2002/09/19 15:30:32  lewis
 *	get working on VC++6 - added qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
 *	
 *	Revision 2.175  2002/09/19 14:11:26  lewis
 *	Removed qSupportLed23CompatAPI and replaced it with qSupportLed23CompatAPI
 *	
 *	Revision 2.174  2002/09/19 01:48:32  lewis
 *	SPR#1103- qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR workaround added to SimpleLed
 *	to work around crasher bug
 *	
 *	Revision 2.173  2002/09/09 02:59:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.172  2002/09/04 01:50:55  lewis
 *	3.1d1x, lose qCannotExpandTemplateOfTypeWithNoExternalLinkage as it appears now (so many
 *	compilers say so) to NOT be a compiler bug, but rather an unfortunate language definition
 *	choice. Document qKeywordsForAndOrNotBroken.
 *	
 *	Revision 2.171  2002/05/06 21:33:24  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.170  2002/05/03 13:37:54  lewis
 *	3.0.1
 *	
 *	Revision 2.169  2002/04/02 01:04:36  lewis
 *	3.0.1x
 *	
 *	Revision 2.168  2001/11/27 00:29:38  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.167  2001/11/26 23:53:56  lewis
 *	Led 3.0 (final)
 *	
 *	Revision 2.166  2001/11/09 21:11:47  lewis
 *	3.0c2
 *	
 *	Revision 2.165  2001/10/17 20:42:48  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.164  2001/10/17 00:29:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.163  2001/09/28 14:03:31  lewis
 *	3.0b2x
 *	
 *	Revision 2.162  2001/09/27 01:56:13  lewis
 *	3.0c1
 *	
 *	Revision 2.161  2001/09/26 22:12:30  lewis
 *	SPR#1049- fix qInternalErrorWithStaticRegionDeclaredInFunction
 *	
 *	Revision 2.160  2001/09/11 22:29:13  lewis
 *	SPR#1018- preliminary CWPro7 support
 *	
 *	Revision 2.159  2001/08/28 18:43:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.158  2001/08/28 18:22:59  lewis
 *	3.0c1x
 *	
 *	Revision 2.157  2001/08/27 23:26:08  lewis
 *	3.0b9
 *	
 *	Revision 2.156  2001/07/31 15:29:18  lewis
 *	MSVC70 (really only tested againsted MSVC70B2) compiler support
 *	
 *	Revision 2.155  2001/07/19 23:43:39  lewis
 *	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug support
 *	
 *	Revision 2.154  2001/07/19 21:05:19  lewis
 *	fix some small sytnax quirks only complained about by MWERKS compiler
 *	
 *	Revision 2.153  2001/07/19 02:20:49  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.152  2001/06/27 13:24:27  lewis
 *	3.0b9x
 *	
 *	Revision 2.151  2001/06/05 15:46:12  lewis
 *	3.0b8
 *	
 *	Revision 2.150  2001/05/29 20:20:53  lewis
 *	3.0b8x
 *	
 *	Revision 2.149  2001/05/27 02:23:42  lewis
 *	Led 3.0b7
 *	
 *	Revision 2.148  2001/05/18 20:56:57  lewis
 *	Added qGCC_OptBugWithLocalClassesScopedInFunction bug define. Added new
 *	qUseSystemNetscapeOpenURLs define and X-Windows specific config vars section of this file
 *	
 *	Revision 2.147  2001/05/15 16:39:38  lewis
 *	lose qGCC_gettimeofdayBug- my mistake
 *	
 *	Revision 2.146  2001/05/12 22:50:56  lewis
 *	SPR#0917- new TextImager::G/SetImageUsingOffscreenBitmaps () method to replace
 *	obsolete qUseOffscreenBitmapsToReduceFlicker
 *	
 *	Revision 2.145  2001/05/11 23:36:29  lewis
 *	can now turn on qUseOffscreenBitmapsToReduceFlicker for xwindows. Not fully implemented yet -
 *	but at least it now compiles on X
 *	
 *	Revision 2.144  2001/05/10 21:55:40  lewis
 *	3.0b7x
 *	
 *	Revision 2.143  2001/05/10 00:24:16  lewis
 *	3.0b6
 *	
 *	Revision 2.142  2001/05/02 18:20:30  lewis
 *	qGCC_gettimeofdayBug
 *	
 *	Revision 2.141  2001/04/23 15:57:16  lewis
 *	Lose qDoTrapCaching- flag - since 68K mac so old/unimportant
 *	
 *	Revision 2.140  2001/04/20 19:10:39  lewis
 *	3.0b6x
 *	
 *	Revision 2.139  2001/04/19 16:13:20  lewis
 *	Led 3.0b5
 *	
 *	Revision 2.138  2001/02/05 17:17:02  lewis
 *	3.0b5x
 *	
 *	Revision 2.137  2001/01/20 15:30:10  lewis
 *	Led 3.0b4
 *	
 *	Revision 2.136  2001/01/20 01:58:22  lewis
 *	GCC qSTLwstringBugNeedsTypedef bug workaround
 *	
 *	Revision 2.135  2001/01/08 18:08:54  lewis
 *	default qWindows to defined (_WIN32) || defined (WIN32)
 *	
 *	Revision 2.134  2001/01/04 14:27:49  lewis
 *	a few small changes to be compatable with an older version of Borland C++-
 *	requested by a customer - he proovded the patches- Cladio Nold
 *	
 *	Revision 2.133  2001/01/03 16:00:02  lewis
 *	qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
 *	
 *	Revision 2.132  2001/01/03 15:29:21  lewis
 *	qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
 *	
 *	Revision 2.131  2001/01/03 14:46:42  Lewis
 *	add #error if unkown compiler.  Added support for Borland C++(preliminary)
 *	
 *	Revision 2.130  2000/11/06 22:48:29  lewis
 *	3.0b4x
 *	
 *	Revision 2.129  2000/10/22 17:38:15  lewis
 *	3.0b3
 *	
 *	Revision 2.128  2000/09/14 19:04:03  lewis
 *	Led 3.0b3x
 *	
 *	Revision 2.127  2000/09/09 14:46:46  lewis
 *	3.0b2
 *	
 *	Revision 2.126  2000/06/23 21:38:25  lewis
 *	Led 3.0b2x
 *	
 *	Revision 2.125  2000/06/17 13:08:22  lewis
 *	3.0b1
 *	
 *	Revision 2.124  2000/06/17 06:03:48  lewis
 *	enough has changed - call this b1x - not d7x
 *	
 *	Revision 2.123  2000/06/17 05:08:27  lewis
 *	SPR#0793- Lose qUseSystemWordBreakRoutine define - and replace it with new TextBreaks_System class.
 *	
 *	Revision 2.122  2000/05/31 15:02:59  lewis
 *	Add qISSPACE_ETC_FUNCTIONS_LinkerError
 *	
 *	Revision 2.121  2000/05/08 15:33:26  lewis
 *	Led 3.0d7x
 *	
 *	Revision 2.120  2000/05/01 21:12:13  lewis
 *	Led 3.0d6
 *	
 *	Revision 2.119  2000/04/30 14:53:33  lewis
 *	added qTroubleAccessingNestedProtectedClassNamesInSubclass
 *	
 *	Revision 2.118  2000/04/15 14:32:33  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.117  2000/04/14 22:40:19  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.116  2000/04/14 19:11:32  lewis
 *	added qLedUsesNamespaces - for now defaulting OFF
 *	
 *	Revision 2.115  2000/04/08 03:00:35  lewis
 *	3.0d6x
 *	
 *	Revision 2.114  2000/04/04 22:09:18  lewis
 *	Led 3.0d5
 *	
 *	Revision 2.113  2000/04/03 18:28:25  lewis
 *	Make qUseSpyglassDDESDIToOpenURLs default to OFF
 *	
 *	Revision 2.112  2000/03/11 22:26:48  lewis
 *	Led 3.0d5x
 *	
 *	Revision 2.111  2000/01/22 23:02:27  lewis
 *	3.0d4
 *	
 *	Revision 2.110  2000/01/15 16:57:44  lewis
 *	GCC bug define updates
 *	
 *	Revision 2.109  1999/12/29 15:17:17  lewis
 *	3.0d3
 *	
 *	Revision 2.108  1999/12/29 03:12:31  lewis
 *	SPR#0628- Lose #define qUseGDIScrollbitsForScrolling. Instead have methods
 *	G/SetUseBitmapScrollingOptimization ()
 *	
 *	Revision 2.107  1999/12/24 23:45:15  lewis
 *	define bug-define qNoSTRNICMP- and BWA version of qNoSTRNICMP
 *	
 *	Revision 2.106  1999/12/15 17:53:47  lewis
 *	3.0d3x
 *	
 *	Revision 2.105  1999/12/15 03:23:01  lewis
 *	Led 3.0d2
 *	
 *	Revision 2.104  1999/12/14 21:40:34  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.103  1999/12/14 21:23:07  lewis
 *	cannot #define qSDK_UNICODE            defined (_UNICODE) - must do ifdef cases
 *	cuz otherwise using qSDK_UNICODE define in C++ code (not macro code) fails
 *	
 *	Revision 2.102  1999/12/14 18:00:47  lewis
 *	Add new qHeavyDebugging define - so Led (by default) isn't so slow for users in
 *	DEBUG mode. And add preliminary qXWindows/__GNUC__ support
 *	
 *	Revision 2.101  1999/12/11 21:41:58  lewis
 *	qWorkAroundWin95BrokenUNICODESupport - change default value
 *	
 *	Revision 2.100  1999/12/09 03:18:00  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support
 *	
 *	Revision 2.99  1999/11/30 20:02:05  lewis
 *	3.0d2x
 *	
 *	Revision 2.98  1999/11/29 23:38:54  lewis
 *	3.0d1
 *	
 *	Revision 2.97  1999/11/29 20:45:22  lewis
 *	Added qPeekAtQuickDrawGlobals define (defaults off for PPC -spr#0634).
 *	Preliminary __CWPro52__VERSION__ support.
 *	
 *	Revision 2.96  1999/11/13 22:32:47  lewis
 *	lose qSupportLed22CompatAPI - but add qSupportLed23CompatAPI
 *	
 *	Revision 2.95  1999/11/13 17:08:51  lewis
 *	version 3.0d1x
 *	
 *	Revision 2.94  1999/11/13 16:32:17  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.93  1999/08/28 14:39:08  lewis
 *	Led 2.3 RELEASE VERSION
 *	
 *	Revision 2.92  1999/08/28 02:10:28  lewis
 *	Led 2.3c3x
 *	
 *	Revision 2.91  1999/07/22 13:26:19  lewis
 *	2.3c2
 *	
 *	Revision 2.90  1999/07/16 22:24:08  lewis
 *	qNestedClassesInTemplateClassesDontExpandCompilerBug needed for MWERKS as well, when cross
 *	compiling for windows (though differnt behavior - fails to compile)
 *	
 *	Revision 2.89  1999/07/13 22:39:08  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds
 *	that were only needed for these old compilers
 *	
 *	Revision 2.88  1999/07/12 21:56:38  lewis
 *	first cut at new CW5 Pro compiler flag support
 *	
 *	Revision 2.87  1999/07/01 22:53:44  lewis
 *	2.3c2x
 *	
 *	Revision 2.86  1999/06/30 14:06:19  lewis
 *	2.3c1
 *	
 *	Revision 2.85  1999/06/14 21:11:50  lewis
 *	2.3c1x
 *	
 *	Revision 2.84  1999/05/03 22:32:32  lewis
 *	2.3b10
 *	
 *	Revision 2.83  1999/05/03 22:04:50  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.82  1999/05/03 21:41:17  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.81  1999/04/28 16:59:28  lewis
 *	2.3b9
 *	
 *	Revision 2.80  1999/04/01 03:03:15  lewis
 *	2.3b9x
 *	
 *	Revision 2.79  1999/03/31 21:52:08  lewis
 *	2.3b8
 *	
 *	Revision 2.78  1999/03/11 03:01:14  lewis
 *	2.3b8x
 *	
 *	Revision 2.77  1999/03/10 21:11:22  lewis
 *	2.3b7
 *	
 *	Revision 2.76  1999/03/06 02:42:44  lewis
 *	added qNestedClassesInTemplateClassesDontExpandCompilerBug
 *	
 *	Revision 2.75  1999/02/23 15:56:55  lewis
 *	2.3b7x
 *	
 *	Revision 2.74  1999/02/22 13:18:34  lewis
 *	2.3b6
 *	
 *	Revision 2.73  1999/02/08 20:53:29  lewis
 *	2.3b6x
 *	
 *	Revision 2.72  1999/02/06 15:08:59  lewis
 *	Release 2.3b5
 *	
 *	Revision 2.71  1999/02/05 23:52:20  lewis
 *	support CW4Pro and lose unneeded qBrokenSTLTemplEvalWAR
 *	
 *	Revision 2.70  1998/10/31 20:28:47  lewis
 *	change version to 2.3b5x
 *	
 *	Revision 2.69  1998/10/30 14:11:15  lewis
 *	lots of changes for MSVC60
 *	and 2.3b4.
 *	
 *	Revision 2.68  1998/07/24  00:55:24  lewis
 *	2.3b3.
 *
 *	Revision 2.67  1998/06/03  01:29:16  lewis
 *	qWorkAroundWin95BrokenUNICODESupport and 2.3b2.
 *
 *	Revision 2.66  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.65  1998/04/25  01:17:31  lewis
 *	qProvideIMESupport defauls on.
 *	Fixed / implemented UNICODE support.
 *	2.3b1
 *
 *	Revision 2.64  1998/04/09  01:43:55  lewis
 *	2.3a3
 *
 *	Revision 2.63  1998/04/08  01:10:34  lewis
 *	*** empty log message ***
 *
 *	Revision 2.62  1998/03/04  20:19:18  lewis
 *	*** empty log message ***
 *
 *	Revision 2.61  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.60  1997/12/24  03:21:22  lewis
 *	Various changes for updated/new compilers.
 *	2.3a1.
 *
 *	Revision 2.59  1997/10/01  01:54:29  lewis
 *	Led 2.3d2
 *
 *	Revision 2.58  1997/09/29  14:29:10  lewis
 *	Lose qLedFirstIndex support, up version#, and lose old compiler support.
 *
 *	Revision 2.57  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.56  1997/07/27  15:32:31  lewis
 *	*** empty log message ***
 *
 *	Revision 2.55  1997/07/27  15:00:21  lewis
 *	Edit docs.
 *	2.2 final release
 *
 *	Revision 2.54  1997/07/23  23:02:29  lewis
 *	Lots of AutoDoc changes (docs).
 *	Release 2.2rc1
 *
 *	Revision 2.53  1997/07/15  05:24:31  lewis
 *	Turn back on blockallocation (was off by accident, cuz of futzing earlier).
 *	2.2b4.
 *	AutoDoc stuff.
 *
 *	Revision 2.52  1997/07/12  19:57:04  lewis
 *	new compiler bug defines for using STL/vector class.
 *	qLedFirstIndex = 0 now!
 *	2.2b3.
 *
 *	Revision 2.51  1997/06/28  17:12:15  lewis
 *	2.2b2
 *
 *	Revision 2.50  1997/06/24  03:26:25  lewis
 *	2.2b1
 *
 *	Revision 2.49  1997/06/23  16:10:12  lewis
 *	Fix __option(bool) test for CWPro1/Windows & Led 2.2b1x
 *
 *	Revision 2.48  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.47  1997/06/18  03:49:15  lewis
 *	*** empty log message ***
 *
 *	Revision 2.46  1997/06/18  02:33:38  lewis
 *	Lots of compiler bug workarounds etc changes. Support new CWPro1. And MSVC50. Lose most
 *	old compilers support (back to CW11, and MSVC42).
 *
 *	Revision 2.45  1997/03/22  13:51:39  lewis
 *	Lose of cleanups to config define organization.
 *	Added MSVC50 support.
 *	Lose all the old versions of MSVC support before 42 (still retain 42 for a while).
 *	Lost some old MWERKS version support defines?
 *	more...
 *	2.2a1.
 *
 *	Revision 2.44  1997/03/04  20:02:27  lewis
 *	New CW release (bug defines).
 *	2.2d3.
 *
 *	Revision 2.43  1997/01/20  05:18:34  lewis
 *	define qNoANSIStringClassSupportSoFakeWithMFCCString 0 for MSVC4.2
 *	Version 2.2d2.
 *
 *	Revision 2.42  1997/01/10  02:48:53  lewis
 *	Introduced qSupportLed21CompatAPI define (default off).
 *	A couple cleanups, new compiler bug defines.
 *	Led 2.2d1.
 *
 *	Revision 2.41  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.40  1996/12/13  17:51:56  lewis
 *	<======= Led 2.1 Final ========>
 *
 *	Revision 2.39  1996/12/05  21:45:43  lewis
 *	2.1rc1
 *
 *	Revision 2.38  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.37  1996/10/31  00:07:50  lewis
 *	2.1b2.
 *
 *	Revision 2.36  1996/10/15  18:58:31  lewis
 *	add qCPlusPlusRuntimeLibraryLacksStringEraseMethod define (in b1).
 *
 *	Revision 2.35  1996/10/15  18:22:11  lewis
 *	Add qUseActiveXToOpenURLs define.
 *	2.1b1.
 *
 *	Revision 2.34  1996/10/04  16:33:07  lewis
 *	qExplicitCastToBoolOfExpressionWithBitwiseAndFails added
 *
 *	Revision 2.33  1996/09/30  14:10:26  lewis
 *	2.1d2.
 *	Also some compile bug define changes for CW10, and redid them
 *	slightly for MSVC so default for new compilers
 *	is to use old defines, with no changes.
 *
 *	Revision 2.32  1996/09/03  15:00:07  lewis
 *	Got rid of CW7/CW8 bug workarounds.
 *	Added support for (prerelease of) CW10.
 *	Redid CW10 bug workaround support so by default it WILL work with new compilers,
 *	as long as the same old bug defines continue to work.
 *	Up version to 2.1d1.
 *
 *	Revision 2.31  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.30  1996/08/05  06:01:17  lewis
 *	!!!!!!!!!! 2.0 Final Release Version !!!!!!!!!!
 *
 *	Revision 2.29  1996/07/24  21:57:26  lewis
 *	Led 2.0rc4.
 *
 *	Revision 2.28  1996/07/19  16:32:34  lewis
 *	new qVERYLargeOpNewFailsToFail workaround
 *	2.0rc3.
 *
 *	Revision 2.27  1996/07/03  00:51:28  lewis
 *	Add qNoANSIStringClassSupportSoFakeWithMFCCString fir MSVC.
 *	Up version to 2.0rc2.
 *
 *	Revision 2.26  1996/06/01  02:00:56  lewis
 *	Lose qNoRTTISupportBug support.
 *	Rename qRequireIMESupport --> qProvideIMESupport.
 *	Version 2.0rc1.
 *
 *	Revision 2.25  1996/05/23  19:16:24  lewis
 *	shutoff qDoubleClickSelectsSpaceAfterWord by default on PC
 *	also.
 *	Protect #include wtih && && !defined (__SOMIDL__)
 *	cuz som IDL compiler defines C++ but then barfs on it. Sigh.
 *	Release b10.
 *
 *	Revision 2.24  1996/05/14  20:18:03  lewis
 *	2.0b9.
 *	Added define qUseOffscreenBitmapsToReduceFlicker.
 *	Added whole bunch of other bug defines, and updated bug defines for CW9
 *	pre-release.
 *
 *	Revision 2.23  1996/05/04  19:14:27  lewis
 *	Added qUseSpyglassDDESDIToOpenURLs.
 *	Support MWERKS 0x900 compiler (CW9- prerelease)
 *	No longer include <afxwin.h>
 *	define qMacOS/qWinOS to zero by default (so can write code like
 *	if (qMacOS) { etc...
 *
 *	Revision 2.22  1996/04/18  15:02:18  lewis
 *	Support MSVC 4.1.
 *	Release 2.0b7.
 *
 *	Revision 2.21  1996/03/16  18:32:18  lewis
 *	re-organized defines a little. Support new mwkers compiler (pre-cw9 beta).
 *	qUseMacTmpMemForAllocs.
 *	Release 2.0b6.
 *
 *	Revision 2.20  1996/03/05  16:54:08  lewis
 *	Lose unneeded qNoMinMaxAbsBug.
 *	Add define for qUseInternetConfig (on for mac by default except CFM68K)
 *	Version 2.0b5.
 *
 *	Revision 2.19  1996/03/04  07:39:27  lewis
 *	2.0b4
 *
 *	Revision 2.18  1996/02/26  18:32:56  lewis
 *	Moved a bunch of bug defines to Led_CLASSLIB headers intead of being here.
 *	No more qTCL / qMFC. Now all these (at least most) of these dependencies
 *	in Led_CLASSLIB modules.
 *	Support MWERKS 801.
 *	Led 2.0b3.
 *
 *	Revision 2.17  1996/02/05  04:07:24  lewis
 *	use qTCl intead if defined(__TCL__)
 *	new qInheritedSomeSortOfReservedWordMistakenly bug.
 *	2.0b2.
 *
 *	Revision 2.16  1996/01/22  05:06:15  lewis
 *	2.0b1
 *
 *	Revision 2.15  1996/01/11  08:11:28  lewis
 *	Added qDoubleClickSelectsSpaceAfterWord support/define.
 *	qAllowRowsThatAreLongerThan255 now defaults to TRUE - cuz comes up often
 *	with embedded objects.
 *	2.0a8.
 *
 *	Revision 2.14  1996/01/03  23:46:53  lewis
 *	2.0a7.
 *	And lots of bug-define changes, for MSVC 4.0, and new MWKERS CW8
 *	compiler.
 *
 *	Revision 2.13  1995/12/16  05:01:19  lewis
 *	2.0d6
 *
 *	Revision 2.12  1995/12/15  02:12:26  lewis
 *	2.0a5
 *
 *	Revision 2.11  1995/12/13  05:51:11  lewis
 *	Lose qUseLocalBoolFlagToKeepTrackOfShowHideCaretForNTJCaretBug.
 *	2.0a4.
 *
 *	Revision 2.10  1995/12/09  05:32:43  lewis
 *	Led 2.0a3.
 *
 *	Revision 2.9  1995/12/08  07:06:42  lewis
 *	2.0a3x
 *
 *	Revision 2.8  1995/12/06  01:24:04  lewis
 *	Support for _MSC_VER==1000 (MSVC 4.0).
 *	Up version to 2.0a2.
 *
 *	Revision 2.7  1995/11/25  00:15:18  lewis
 *	Support version 701 of metrowerks compiler.
 *	trap cache depends on GENERATINGCFM instead of powerpc.
 *	Version 2.0a1
 *
 *	Revision 2.6  1995/11/04  23:06:05  lewis
 *	MWKERS has qMightSlopMSVC20DTOROfSimpleTypeBug too (at least for T=const char*
 *	and 2.0d6.
 *
 *	Revision 2.5  1995/11/02  21:45:42  lewis
 *	2.0d6x
 *
 *	Revision 2.4  1995/10/19  22:01:25  lewis
 *	New windows compiler bug workarounds - esp for MSVC 2.2
 *	Lose old qSupportWindowsSystemsWithNoMeasureText (win32s).
 *	Lose qWin32s_EM_Messages_Broken.
 *	Lose CW6 compatability.
 *	Version 2.0d5.
 *
 *	Revision 2.3  1995/10/09  22:12:48  lewis
 *	2.0d4
 *
 *	Revision 2.2  1995/09/08  15:20:29  lewis
 *	2.0d3.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.43  1995/06/19  13:43:54  lewis
 *	Led 1.0.1
 *
 *	Revision 1.42  1995/06/12  03:07:22  lewis
 *	Release 1.0 FINAL!!!
 *
 *	Revision 1.41  1995/06/08  05:09:45  lewis
 *	Led 1.0c4.
 *
 *	Revision 1.40  1995/06/05  21:57:53  lewis
 *	1.0c3
 *
 *	Revision 1.39  1995/06/05  21:27:48  lewis
 *	Led 1.0c3(develop).
 *
 *	Revision 1.38  1995/06/04  04:24:42  lewis
 *	Embellish version # support to support release candidates (SPR#0310).
 *	Led 1.0c2
 *
 *	Revision 1.37  1995/06/02  06:15:58  lewis
 *	Add define for qCodeWarrierFailureToSubclassOrOpEqualsRGBColor bug.
 *	Led 1.0RC1
 *
 *	Revision 1.36  1995/05/31  07:33:42  lewis
 *	1.0b39.
 *
 *	Revision 1.35  1995/05/29  23:44:47  lewis
 *	Changed emacs-mode string to be c++ from Text
 *
 *	Revision 1.34  1995/05/29  23:34:17  lewis
 *	1.0b38.
 *
 *	Revision 1.33  1995/05/26  06:40:27  lewis
 *	Led 1.0b37
 *
 *	Revision 1.32  1995/05/25  09:44:20  lewis
 *	Added qCanUseAlloca define.
 *	1.0b36.
 *
 *	Revision 1.31  1995/05/24  07:14:40  lewis
 *	Added qUseLocalBoolFlagToKeepTrackOfShowHideCaretForNTJCaretBug workaround.
 *	1.0b35.
 *
 *	Revision 1.30  1995/05/21  17:00:47  lewis
 *	Added qLedFirstIndex define - SPR0272
 *	1.0b34.
 *
 *	Revision 1.29  1995/05/20  04:40:57  lewis
 *	Updated status of MWEKRS compiler bugs
 *	1.0b34 - development
 *	Added new config variable - qAllowRowsThatAreLongerThan255.
 *
 *	Revision 1.28  1995/05/18  08:49:25  lewis
 *	Worked around qMSVC_LocalClassesCannotHaveAnyFunctionMembersBug.
 *	Still 1.0b33.
 *
 *	Revision 1.27  1995/05/18  07:59:31  lewis
 *	For windows add qSupportFunnyMSPageUpDownAdjustSelectionBehavior and
 *	default to true. SPR # 0248.
 *	Recieved CW6 and got rid of old bug workarounds for CW5, and updated
 *	most CW bug defines (tested most of them again) - SPR 0249.
 *	Led 1.0b33.
 *
 *	Revision 1.26  1995/05/16  06:37:11  lewis
 *	Led 1.0b32.
 *
 *	Revision 1.25  1995/05/15  02:54:25  lewis
 *	Led 1.0b31.
 *
 *	Revision 1.24  1995/05/12  20:59:36  lewis
 *	Define qComplilerDoesNotInlineIntrinsics for MWKERS compiler.
 *	Add new define qMakeWindowsScrollbarAppearOnlyIfNeeded for PC (default false).
 *	Up version to 1.0b30.
 *
 *	Revision 1.23  1995/05/09  23:18:12  lewis
 *	1.0b29.
 *
 *	Revision 1.22  1995/05/09  23:17:32  lewis
 *	Support for qSupportWindowsSystemsWithNoMeasureText workaround for
 *	win32s bug. See SPR 0238.
 *
 *	Revision 1.21  1995/05/08  03:02:19  lewis
 *	Led 1.0b28.
 *
 *	Revision 1.20  1995/05/06  19:19:17  lewis
 *	Cleaned up MSVC bug defines.
 *	Deleted obsolete Symantec C++ bug defines.
 *	Added configuration variable qPeekForMoreCharsOnUserTyping (SPR#0231).
 *	Led Relase 1.0b27.
 *
 *	Revision 1.19  1995/05/05  19:40:13  lewis
 *	Version 1.0b26
 *	And added new compiler bug: qTemplateExpansionAssertFailureInLed_SmallStackBufferOfShortBug.
 *
 *	Revision 1.18  1995/05/03  19:07:35  lewis
 *	Update mwerks compiler bug flags.
 *	Led release 1.0b25
 *
 *	Revision 1.17  1995/04/25  02:53:47  lewis
 *	1.0b24
 *
 *	Revision 1.16  1995/04/20  06:42:10  lewis
 *	Up Led version to 1.0b23.
 *	Also update MWERKS bug defines for new compiler (1.2.2b3 - 950419).
 *
 *	Revision 1.15  1995/04/18  00:41:48  lewis
 *	1.0b22
 *
 *	Revision 1.14  1995/04/18  00:06:37  lewis
 *	Comment on qLotsOfSubclassesFromMixinPointYeildTemporarilyBadVTable
 *
 *	Revision 1.13  1995/04/16  19:20:38  lewis
 *	Led 1.0b22x
 *
 *	Revision 1.12  1995/04/11  02:55:56  lewis
 *	Led 1.0b21
 *
 *	Revision 1.11  1995/03/29  22:44:02  lewis
 *	1.0b20.
 *
 *	Revision 1.10  1995/03/29  21:01:12  lewis
 *	Version 1.0b20(develop).
 *
 *	Revision 1.9  1995/03/24  01:26:24  lewis
 *	Led 1.0b19.
 *
 *	Revision 1.8  1995/03/23  03:42:35  lewis
 *	1.0b18.
 *
 *	Revision 1.7  1995/03/17  02:57:15  lewis
 *	Version 1.0b17.
 *
 *	Revision 1.6  1995/03/15  01:03:02  lewis
 *	Upped version to 1.0b16.
 *
 *	Revision 1.5  1995/03/13  03:15:10  lewis
 *	Upped version to 1.0b15, and fixed MAKE_LED_VERSION macro bug.
 *
 *	Revision 1.4  1995/03/06  21:27:02  lewis
 *	Release 1.0b14.
 *
 *	Revision 1.3  1995/03/03  15:49:26  lewis
 *	Added new support for #defines for version# - qLed_Version etc...
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *
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

	/* _MSC_VER == 1310 - VC++.NET 2003 */
	#define	_MSVC71_VER_	1310

	/* _MSC_VER == 1300 - VC++.NET */
	#define	_MSVC70_VER_	1300

	/* _MSC_VER == 1200 - AKA MSVC 6.0 */
	#define	_MSVC60_VER_	1200

	/* _MSC_VER == 1100 - AKA MSVC 5.0 */
	#define	_MSVC50_VER_	1100

	#if		_MSC_VER > _MSVC71_VER_ || _MSC_VER == _MSVC71_VER_

		/*
		 *	Above, we assume that any new compiler versions will have the same
		 *	bugs/quirks as the last we've tried. Simply issue a warning if a user tries
		 *	to compile with a newer compiler than we support.
		 */
		#if		_MSC_VER > _MSVC71_VER_
			//#error	"Don't know this version of the MSVC compiler - using last known MSVC bug defines."
		#endif

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 6.0 or later requires you to turn on RTTI"
		#endif

		// Verified still broken in VC++.Net 2003 (VC7.1) - LGP 2003-05-09
		#ifndef	qKeywordsForAndOrNotBroken
			#define	qKeywordsForAndOrNotBroken										1
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

		//	Still supported in MSVC 7.1 - LGP 2003-05-08
		//	This feature is at least supported in MSVC70 - LGP 2002-12-19
		#ifndef	qNoOpKeywordSupported
			#define	qNoOpKeywordSupported											1
		#endif

	#elif		_MSC_VER >= _MSVC70_VER_

		#if		_MSC_VER > _MSVC70_VER_
			#error	"Don't know this version of the MSVC compiler - using MSVC 70 bug defines."
		#endif

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 6.0 or later requires you to turn on RTTI"
		#endif

		#ifndef	qKeywordsForAndOrNotBroken
			#define	qKeywordsForAndOrNotBroken										1
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

		//	This feature is at least supported in MSVC70 - LGP 2002-12-19
		#ifndef	qNoOpKeywordSupported
			#define	qNoOpKeywordSupported											1
		#endif

	#elif	_MSC_VER >= _MSVC60_VER_

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 6.0 requires you to turn on RTTI"
		#endif

		#ifndef	qKeywordsForAndOrNotBroken
			#define	qKeywordsForAndOrNotBroken										1
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

		//	This feature is NOT supported in MSVC60 - LGP 2004-02-10
		#ifndef	qNoOpKeywordSupported
			#define	qNoOpKeywordSupported											0
		#endif

	#elif	_MSC_VER == _MSVC50_VER_

		// MSVC OPTIONALLY supports RTTI - be sure option turned on (Led requires RTTI)!
		#ifndef	_CPPRTTI
			#error	"MSVC 5.0 requires you to turn on RTTI"
		#endif

		#ifndef	qKeywordsForAndOrNotBroken
			#define	qKeywordsForAndOrNotBroken										1
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

		// There is some - but its buggy. Try again later - LGP 2001-01-19
		#ifndef	qNamespaceSupport
			#if		qUseGCC295
				#define	qNamespaceSupport								1
			#else
				#define	qNamespaceSupport								0
			#endif
		#endif

		#ifndef	qKeywordsForAndOrNotBroken
			#if		qUseGCC295
				#define	qKeywordsForAndOrNotBroken							0
			#else
				#define	qKeywordsForAndOrNotBroken							1
			#endif
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

		#ifndef	qKeywordsForAndOrNotBroken
			#define	qKeywordsForAndOrNotBroken													1
		#endif

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
	
		#ifndef	qKeywordsForAndOrNotBroken
			#define	qKeywordsForAndOrNotBroken													1
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
 *	I recall reading a LONG time ago that these were going to be included in STD C++ as keywords. Maybe this
 * is wrong (and so they name of the #define is then wrong, and I should just #define these myself always). Anyhow - for now
 * it appears basically nobdoy supports them, but treat it as a compiler bug til I read otherwise - LGP 2002-09-03
 *	manually for cases that matter.
 */
#ifndef	qKeywordsForAndOrNotBroken
	#define	qKeywordsForAndOrNotBroken			0
#endif






/*
 *	Do things like memcpy () get inlined by the compiler, or should we do so ourselves
 *	manually for cases that matter.
 */
#ifndef	qComplilerDoesNotInlineIntrinsics
	#define	qComplilerDoesNotInlineIntrinsics	0
#endif




/*
 *	Can we use namespaces? Does the compiler support them? Default TRUE (except if not in C++ compiler).
 */
#ifndef	qNamespaceSupport
	#if		defined (__cplusplus)
		#define	qNamespaceSupport					1
	#else
		#define	qNamespaceSupport					0
	#endif
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
@CONFIGVAR:		qDefaultTracingOn
@DESCRIPTION:	<p>Led contains a limited amount of pre-built tracing code. This could be expanded someday,
			depending on how useful people find it. This defaults to being on only for Windows and if @'qDebug' is
			on (windows only cuz thats the only place I've implemented the trace message emitter so far).</p>
				<p>See also @'qDebug' and @'LedDebugTrace'</p>
 */
#ifndef	qDefaultTracingOn
#define	qDefaultTracingOn	qDebug && qWindows
#endif




/*
@CONFIGVAR:		qNoOpKeywordSupported
@DESCRIPTION:	<p>MSVC-specific hack - but a useful one. Allows for trace macro calls (see @'qDefaultTracingOn'). I don't
			know of any compilers which support this other than MSVC.</p>
 */
#ifndef	qNoOpKeywordSupported
#define	qNoOpKeywordSupported	0
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





/*
@CONFIGVAR:		qLedUsesNamespaces
@DESCRIPTION:	<p>If defined true then define all Led code in the namespace 'Led'.</p>
		<p>This can prevent Led's names from conflicting with the names of variables, and functions
	etc in other libraries you maybe integrating with. It allows Led to use shorter, more
	common names, without worries of creating such conflicts (not really taken advantage
	of as of Led 3.0d6, 2000/04/14, howevever).</p>
		<p>Default is TRUE iff @'qNamespaceSupport'</p>
 */
#ifndef	qLedUsesNamespaces
	#define	qLedUsesNamespaces					qNamespaceSupport
#endif



#if		qLedUsesNamespaces
namespace	Led	{}			// Be sure namespace is defined for later use.
#endif







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

