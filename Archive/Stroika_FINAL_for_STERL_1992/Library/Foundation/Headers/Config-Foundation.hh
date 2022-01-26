/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Config_Foundation__
#define	__Config_Foundation__

/*
 * $Header: /fuji/lewis/RCS/Config-Foundation.hh,v 1.52 1992/12/10 05:56:21 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *		-		Rationalize bug names (eg dont call it MPW_CFrontXXXBug if it happens in all cfront 2.1's).
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Config-Foundation.hh,v $
 *		Revision 1.52  1992/12/10  05:56:21  lewis
 *		Added qGCC_OpEqualsNotAutoDefinedSoCanBeExplicitlyCalledBug.
 *
 *		Revision 1.51  1992/12/08  21:16:23  lewis
 *		Cleanup tests for OS macro defines, and add qWinNT support.
 *
 *		Revision 1.50  1992/12/07  09:01:24  lewis
 *		Turned qBCC_LinkerPullsInAllFilesSoWeNeedAllGlobalsBug back on.
 *		No other easy workarounds found yet.
 *
 *		Revision 1.49  1992/12/05  19:11:12  lewis
 *		Comment out qBCC_LinkerPullsInAllFilesSoWeNeedAllGlobalsBug for bcc since
 *		it doesn't seem to be a problem now.
 *		Delete qBorlandCPlus use of qMPW_MacroOverflowProblem since thats not
 *		a problem anymore.
 *		Deleted #if 0'd Real support - just forget about it.
 *
 *		Revision 1.47  1992/12/04  16:10:25  lewis
 *		qCFront_SorryNotImplementedStatementAfterReturnInline
 *
 *		Revision 1.46  1992/12/03  19:17:26  lewis
 *		Note gcc bug still broken.
 *
 *		Revision 1.45  1992/12/03  07:32:24  lewis
 *		Dozens of changes to the G++ bug list, since we got a new compiler.
 *		Lots of messages updated, cleaned up, etc. List of bugs affected:
 *		(I mean value changed - not just comments or retests).
 *		qGCC_WarnPointerArithmaticOverzealousOnDeleteVoidStartBug GONE - was redundent.
 *		qGCC_ConversionOperatorInitializerConfusionBug NEW
 *		qGCC_InserterToSubclassOfOStreamBug MEW
 *		qGCC_SimpleClassStaticInitializerBug NEW.
 *		qGCC_ConversionOperatorCausesHangBug NEW.
 *		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug NEW.
 *		Most of these need further research, and to be posted.
 *
 *		Revision 1.44  1992/11/29  03:48:45  lewis
 *		Update stuff for qSunCPlus.
 *
 *		Revision 1.43  1992/11/26  03:35:29  lewis
 *		Got rid of qBCC_TemplateFriendsNotSoFriendlyBug in association with
 *		gcc. Their templates too broken to even worry about this level
 *		of detail.
 *		Turned it back on for BCC since seemed to help in a few cases where
 *		the inline hack didn't do it all.
 *
 *		Revision 1.42  1992/11/26  02:45:15  lewis
 *		Turn qGCC_StaticInitializerObjectSliceBug back on.
 *
 *		Revision 1.41  1992/11/25  03:22:09  lewis
 *		Add qBCC_LinkerPullsInAllFilesSoWeNeedAllGlobalsBug - not well researched
 *		yet.
 *
 *		Revision 1.40  1992/11/25  02:58:09  lewis
 *		Added new qMPW_CFront_ConversionOperatorInitializerCodeGenBug.
 *
 *		Revision 1.39  1992/11/23  22:49:48  lewis
 *		Added two new bug defines - qGCC_ConversionOperatorInitializerConfusionBug
 *		and qBCC_ConditionalOperatorDemandsTypesExactBug. Either or both could
 *		not be bugs - just my mistake. Very soon well lookup in ARM, and
 *		post if they are bugs.
 *
 *		Revision 1.38  1992/11/21  17:15:17  lewis
 *		qGCC_TemplatesCTORWithArgMustBeInlineBug.
 *
 *		Revision 1.37  1992/11/20  19:17:21  lewis
 *		Marked qBCC_TemplateFriendsNotSoFriendlyBug 0 for gcc.
 *		Added qGCC_WarnPointerArithmaticOverzealousOnDeleteVoidStartBug.
 *
 *		Revision 1.36  1992/11/17  05:32:27  lewis
 *		Get rid of qBCC_InlinesBreakTemplates workaround of #define inline NOTHING.
 *		Instead do this locally around key pieces of container class lib.
 *
 *		Revision 1.35  1992/11/16  04:38:40  lewis
 *		For borland, get rid of qMPW_MacroOverflowProblem.
 *		Add workaround for qBCC_InlinesBreakTemplates.
 *		Add new bugs: qBCC_OpEqualsTemplateNotAutoExpanded,
 *		qBCC_ConditionalOperatorDemandsTypesExactBug, qBCC_InlinesBreakTemplates,
 *		and mark qBCC_TemplateFriendsNotSoFriendlyBug as 0.
 *
 *		Revision 1.34  1992/11/15  03:22:39  lewis
 *		Delete old HP bug workarounds - they were for the 2.1 based compiler.
 *		Mark HP and Sun compilers as CFront30 based.
 *
 *		Revision 1.33  1992/11/13  03:31:35  lewis
 *		Fixed typo in CFront30 bug define support.
 *		Added qBCC_TemplateFriendsNotSoFriendlyBug for gcc - though I'm
 *		not sure I should have - I may be wrong here - its all so broken!!!
 *
 *		Revision 1.32  1992/11/12  08:02:02  lewis
 *		Added qCFront_Aux and qCFront30 support - bugs etc.
 *		Added qBCC_TemplateFriendsNotSoFriendlyBug.
 *		#if 0 all of the Real # support. Wasn't done very well. Not really
 *		needed. Probably better to leave issue alone til really needed (kdj's advice)
 *
 *		Revision 1.31  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.30  1992/11/09  22:34:37  lewis
 *		Add new bug defines for new gcc 2.3.1, and note fixed bugs. Still in
 *		the middle of investigating whats fixed. Had to temporarily hack kMinBigReal
 *		for gcc. Fix soon.
 *		qGCC_ColonColonScopeResolutionOpBrokeIfStructAndFunctAtScope FIXED.
 *		qGCC_OverloadingWithoutExplicitOpEqualsDefine FIXED.
 *		qGCC_InternalError95InInlineExpansionAndScopeResolution FIXED.
 *		qGCC_MysteryOptimizerWithGetAtBug MAYBE FIXED? CHECKING...
 *		qGCC_NestedClassWithTypedefParseBug NEW.
 *
 *		Revision 1.29  1992/11/04  20:08:05  lewis
 *		Added qGCC_MysteryOptimizerWithGetAtBug
 *
 *		Revision 1.28  1992/11/03  17:04:18  lewis
 *		Add qGCC_InternalError95InInlineExpansionAndScopeResolution.
 *
 *		Revision 1.27  1992/10/29  15:53:29  lewis
 *		Add new compiler bug defines: qCFront_InlinesNotFullyImplemented and
 *		qCFront_InlinesOpNewDeleteMustDeclareInline.
 *
 *		Revision 1.26  1992/10/14  14:56:21  lewis
 *		Minor cleanups.
 *
 *		Revision 1.25  1992/10/12  19:35:01  lewis
 *		Neatened bug lists some more. Added new define: qCanAlwaysExplicitlyCallDTOR.
 *
 *		Revision 1.24  1992/10/09  20:19:28  lewis
 *		We still must include <generic.h> with pseudo-templates!
 *
 *		Revision 1.23  1992/10/09  16:58:59  lewis
 *		Got rid of Declare/Implement macros - and no-longer includ<generic.h>
 *
 *		Revision 1.21  1992/09/11  12:50:57  lewis
 *		Added new bugs:
 *			qGCC_PlacementOpNewGeneratesSyntaxError
 *			qGCC_OverloadingWithoutExplicitOpEqualsDefine
 *			qGCC_BadDefaultCopyConstructorGeneration
 *
 *		Revision 1.20  1992/09/05  03:48:07  lewis
 *		Added back Nil #define.
 *
 *		Revision 1.19  1992/09/04  01:08:32  lewis
 *		Added qGCC_LibgPlusPlusIOStreamAttatchMissingBug.
 *
 *		Revision 1.18  1992/09/03  20:45:44  lewis
 *		Added bug defines qGCC_LibgPlusPlusSharpDefineSize_tBug,
 *		qMPW_CFront_SloppyCodeGenWithoutOpNewOverloadForPlacementOp, and
 *		workaround for qGCC_LibgPlusPlusSharpDefineSize_tBug - redefine
 *		it using tmp typedef, and #undef. This is done here, and in a few
 *		other places - as needed.
 *
 *		Revision 1.16  1992/07/21  06:13:10  lewis
 *		Added qConstStaticInitializersAllowedRedundently define. Added
 *		qGCC_StaticInitializerObjectSliceBug - though I'm not too confident I've
 *		characterize dthe bug properly. Got rid of qBorlandCPlus_ShortCharConstantsBug
 *		define and use qLongCharacterConstantsSupported instead.
 *
 *		Revision 1.15  1992/07/17  19:07:13  lewis
 *		Added new define- qStaticDataMembersInTemplatesSupported. GCC does not
 *		and I guessed that BCC will - test soon, and see.
 *
 *		Revision 1.14  1992/07/16  06:29:49  lewis
 *		Added new define: qLongCharacterConstantsSupported.
 *
 *		Revision 1.13  1992/07/16  06:08:17  lewis
 *		Added qTemplatesHasRepository define.
 *
 *		Revision 1.12  1992/07/16  05:57:26  lewis
 *		Under qBorlandCPlus, allow qRealTemplatesAvailable to be defined
 *		externally thru scripts, so we can use macro based templates under
 *		BCC until templates working fully.
 *
 *		Revision 1.11  1992/07/10  22:23:37  lewis
 *		Update comments and still-broken as of's for a couple of mac compiler bugs.
 *
 *		Revision 1.9  1992/07/03  07:08:51  lewis
 *		Added bug qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport.
 *
 *		Revision 1.8  1992/07/03  06:20:59  lewis
 *		Added qGCC_GPlusPlusLib_MissingStrToUL and qGCC_TroubleWithFunctionsReturningFunctionPtrs bug
 *		defines.
 *
 *		Revision 1.7  1992/07/01  07:21:33  lewis
 *		Added qGCC_ColonColonScopeResolutionOpBrokeIfStructAndFunctAtScope bug.
 *
 *		Revision 1.4  1992/06/28  03:57:15  lewis
 *		Hack around gcc 2.2.1 bug with size_t being a macro in some cases - cleanup fixes for gcc soon!.
 *
 *		Revision 1.3  1992/06/25  02:53:46  lewis
 *		Comment on why we include each file we include.
 *		Only include <generic.h> at end if not using templates.
 *		Add qCFront_NestedTypesHalfAssed for CFront21
 *		Add qGCC_DblTooBigBug and conditionally change how we initialize kMaxBigReal - just to avoid warning.
 *
 *		Revision 1.2  1992/06/20  19:22:36  lewis
 *		Work on port to gcc-aux
 *
 *		Revision 1.50  1992/05/23  00:10:37  lewis
 *		Add qHPCPlus_BarfsIfAssertReturnsVoid
 *
 *		Revision 1.49  92/05/18  14:51:11  14:51:11  lewis (Lewis Pringle)
 *		Adjust bit/byte ordering defines.
 *		
 *		Revision 1.47  92/05/14  14:57:49  14:57:49  lewis (Lewis Pringle)
 *		No longer support qCFront20.
 *		Updated dates on some mac compiler bugs.
 *		Set several mac compiler bugs to zero since we no longer have any usages that break.
 *		Try using MSB first instead of LSBFirst for mac. Just a quick test to see if works better -
 *			otherwise more debugging to do.
 *		Major cleanups. Reorgainize, label logical blocks of defines, etc. Reorganize bugs, so common
 *			ones for CFront grouped together (though still with old names - that would invovle chagning lots of
 *			files to fix the bug names).
 *
 *		Revision 1.46  92/05/09  01:28:07  01:28:07  lewis (Lewis Pringle)
 *		Added defines for big/little endian - the stuff needs more work, but its a start.
 *		
 *		Revision 1.45  92/04/16  11:02:25  11:02:25  lewis (Lewis Pringle)
 *		Add more commentary on qMPW_CFRONT_2_1_SingleObjectSubclassThenAddPureVirtualBreaksLoadDump_BUG.
 *		
 *		Revision 1.44  92/04/14  13:00:24  13:00:24  lewis (Lewis Pringle)
 *		Updated date on MPW compiler bug.
 *		
 *		Revision 1.43  92/04/14  07:28:14  07:28:14  lewis (Lewis Pringle)
 *		Added new compiler feature workaround flag - qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.42  92/04/08  15:23:22  15:23:22  lewis (Lewis Pringle)
 *		qMPW_STATIC_CONSTRUCTOR_COMPLEX_INITIALIZER_NYI_BUG
 *		
 *		Revision 1.41  92/04/07  10:01:51  10:01:51  lewis (Lewis Pringle)
 *		Add qMPW_STATIC_CONST_ENUM_MEMBER_NOT_DEFINED_WHEN_DEFINED_BUG
 *		
 *		Revision 1.39  92/03/30  17:24:06  17:24:06  lewis (Lewis Pringle)
 *		Allow for compile with GCC - looked pretty good, except that I couldnt getting libg++ working.
 *		
 *		Revision 1.38  92/03/30  13:49:17  13:49:17  lewis (Lewis Pringle)
 *		Update qBorlandCPlus bugs/features.
 *		
 *		Revision 1.37  92/03/28  03:25:41  03:25:41  lewis (Lewis Pringle)
 *		PC support.
 *		
 *		Revision 1.36  1992/03/26  17:02:51  lewis
 *		Sterl things StreamLib Tromping bug occurs in all 2.1 iostream libs.
 *
 *		Revision 1.34  1992/03/11  22:56:37  lewis
 *		Cleanup old crap, and rationalize names.
 *		__BORLANDC__ is now qBorlandCPlus, all references go qGPlus are gone, and
 *		weve started adding referecnes to qGCC (gcc 2.0).
 *		Added / removed a few q names for compiler features (eg qTemplates...).
 *
 *		Revision 1.33  1992/03/09  15:14:14  lewis
 *		New compiler bug qCFRONT_ARRAYS_IN_INLINE_CAUSE_ERROR_BUG. And noted
 *		mac compiler bug fixed:qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
 *
 *		Revision 1.32  1992/03/05  00:09:37  lewis
 *		Fix Declare2/Implement2 macros for other than MPW_C_Compiler.
 *
 *		Revision 1.31  1992/03/02  22:31:45  lewis
 *		Added compiler bug flag for HP - not well documented - soemthing sterl ran into in Emily with static member functions.
 *
 *		Revision 1.30  1992/02/21  21:10:38  lewis
 *		Made obsolete some old compiler bug workarounds -mostly for g++, and also,
 *		added new macro INHERRIT_FROM_SINGLE_OBJECT to deal with single object stuff
 *		and MPW from in macros.
 *
 *		Revision 1.29  1992/02/18  05:08:19  lewis
 *		Added new library bug define for HP (stream lib).
 *
 *		Revision 1.28  1992/02/14  23:22:37  lewis
 *		Add new compiler bug for hps (from mac list) and neatened both lists.
 *
 *		Revision 1.27  1992/02/12  02:53:15  lewis
 *		Got new ETO for mac, and tested a bunch of compiler bugs, and updated #defines.
 *
 *		Revision 1.26  1992/02/11  00:01:42  lewis
 *		Added qHPCPlus_StaticConstructorReturnBug toist of bugs that breaks
 *		on the mac. Must rename these multiplatform bugs sometime soon.
 *
 *		Revision 1.25  1992/01/31  19:33:32  lewis
 *		add #define inherited inherieted_ for apple compiler, to avoid their interpretion of our nested type
 *		typdefs...
 *
 *		Revision 1.24  1992/01/28  10:38:03  lewis
 *		Added new hp compiler bug.
 *
 *		Revision 1.23  1992/01/28  07:09:23  lewis
 *		Fixed yet another bug with the Align() macro on the HP - pretty scary set of compiler bugs, methinks...
 *
 *		Revision 1.22  1992/01/15  03:44:34  lewis
 *		Added new HP compiler bug with -g
 *
 *		Revision 1.21  1992/01/09  06:34:38  lewis
 *		Add new HP compiler bug having to do with constant expressions involving sizeof, and workaround in Align() macro.
 *
 *		Revision 1.20  1992/01/05  06:24:13  lewis
 *		#defines for mac.
 *
 *
 *
 *
 */

#include	<float.h>		// for floating point limits (eg ULONG_MAX)
#include	<limits.h>		// for integer limits (eg ULONG_MAX)
#include	<stddef.h>		// for size_t, etc type definitions







/*
 ***********************************************************************************************
 ***************************************** Processor defines ***********************************
 ***********************************************************************************************
 */

/*
 *
 * Processor defines:
 *
 *	Supported processor families include:
 *
 *		... Fill in later ...
 *	68K family	mc68000, mc68020
 *
 * 
 * 	386 implies familly 80x86, and at least 386
 * 	for things common to both (most) just use #if i286
 * 	i286, i386			-	for Intel 286, 386 + machines...
 * 
 * 
 * 	 qSparc				-	for Sun Sparc architecture machines
 * 	 qSnake				-	for HP Snake (PaRisc) architecture machines
 */

#if		i386
	#ifndef	i286
		#define	i286	1
	#endif
#endif

// Sanity checks on consistency of compiler args
#if		mc68020 && !mc68000
	#error	"inconsitent flags"
#endif
#if		i386 && !i286
	#error	"inconsitent flags"
#endif
#if		!mc68000 && !i286 && !qSparc && !qSnake
	#error	"Must pick some processor"
#endif












/*
 ***********************************************************************************************
 ************************************ Operating System defines *********************************
 ***********************************************************************************************
 */
/*
 *
 * Operating System defines:
 *
 *
 *	Supported operating systems include:
 *		<qDOS>		Microsofts DOS version 5.0 or greater
 *		<qMacOS>	Apples Mac OS version 6.0.7 or greater
 *		<qWinOS>	Microsofts Windows environment version 3.x
 *		<qWinNT>	Microsofts Windows NT
 *		<qUnixOS>	Any POSIX compliant UNIX (or close)
 *
 *		Sanity checks on consistency of compiler args.  Pick one, and only
 *	one OS.
 */
#if		!qDOS && !qMacOS && !qWinOS && !qWinNT && !qUnixOS
	#error	"Need to compile for some OS"
#endif
#if		(qDOS && qMacOS)	|| (qDOS && qWinOS) || (qDOS && qWinNT) || (qDOS && qUnixOS) || \
		(qMacOS && qWinOS)	|| (qMacOS && qWinNT) || (qMacOS && qUnixOS) || \
		(qWinOS && qWinNT)	|| (qWinOS && qUnixOS) || \
		(qWinNT && qUnixOS)
	#error	"Need to compile for exactly one OS"
#endif











/*
 ***********************************************************************************************
 ************************************ Compilation System defines *******************************
 ***********************************************************************************************
 */
/*
 * Compiler being used - not clear how to parameterize all of these options!!!
 */
// qMPW_CFront			// 1 iff using Apples MPW's version of CFront
// qMPW_CFront_AppleC	// 1 iff using Apples MPW's version of CFront (WITH MPW C?) (IMPLIES qMPW_CFront TRUE)
// qHPCPlus			-		last tested 05/92
// qSunCPlus		-		last tested 11/91
// <NOT REALLY VALID BY ITSELF - USE qGCC_SYS - eg qGCC_Aux -
//					-	maybe rethink this?? qGCC				-		only partial support right now, since I dont have the compiler
// qGCC_Aux			-		still porting 07/92
// qBorlandCPlus	-		Borland C++ 3.1

// Sanity checks on consistency of compiler args
#if		!qMPW_CFront && !qGCC_Aux && !qBorlandCPlus && !qHPCPlus && !qSunCPlus && !qCFront_Aux
	#error	"Compiler not yet supported"
#endif
#if		qMPW_CFront_AppleC && !qMPW_CFront
	#error	"inconsistent compiler choice flags"
#endif








/*
 * Various compiler specific bug/feature defines.
 */



// CFront is the most common compiler. 
// It can be used in two variations - qCFront21, and qCFront30 You can pick any
// 1 of these to get get some preset flags

// First check that these flags are set for compilers we know use CFront
#if		qMPW_CFront
	#if		!qCFront21
		#error "we only support cfront 21 at this point - no longer support 20!!!"
	#endif
#endif

#if		qHPCPlus || qSunCPlus
	#define	qCFront30	1
#endif

#if		qGCC_Aux
	#define	qGCC		1
#endif

#if		qCFront_Aux
	#define	qCFront30	1
#endif


// Be sure cfront versions are exclusive
#if		(CFront21 && CFront30)
	#error	"Pick 1"
#endif









/*
 ***********************************************************************************************
 ************************************ Common Compiler Bugs *************************************
 ***********************************************************************************************
 */

	/*
	 * Compiler bugs / missing features which may occur on muiltiple systems and compilers.Use these
	 * names in preference to compiler specific names. These flags are all assumed to be off, and
	 * can be turned on, for each individual compiler.
	 *
			qCanFreelyUseVirtualBaseClasses			-	virtual base classes work properly
			qPureVirtualsPropagated					-	dont need to repeat virtual f()=0; for each subclassed abstract
														class w/pure virtuals
			qRealExceptionsAvailable				-	compiler supported exceptions available
			qRealTemplatesAvailable					-	compiler supported templates available
			qTemplatesHasRepository					-	Only makes sense if qRealTemplatesAvailable.
														Means that we can leave body/non-inline functions
														in .cc files with no tricks - compiler will do the
														work of expanding/finding them as necessary. Otherwise
														the need to be in includes, or somehow someone has to
														aragne for their expansion (gcc2.1 and bcc 3.1 work this way-
														only Cfront 3.0 has repository???).
			qStaticDataMembersInTemplatesSupported	-	speaks for iteself - gcc 2.2.1 does - tough in UNIX without
														repository...
			qGeneralInitializerInInitializerListAvailable
													-	is the feature fully supported? (examples)
			qSignedTypeImplemented					-	singned ingnored by most compilers
			qLongLongAvailable						-	only gcc supports this as far as I know - double
														length arithmatic built-in - therefore probably
														better and faster than our class based support.
			qSoleInlineVirtualsNotStripped			-	In case of a simple subclass (eg. PushButton), where
														we do nothing but rename a class, and add a new constructor
														the compiler feels compelled to generate a new vtable, and a new
														virtual dtor, and since these are not stated explicitly, it must
														do this staticly in each .o file that includes the given header.
														This can lead to an explosition of code size.
			qLongCharacterConstantsSupported		-	Support for wide/long character constants available. Not important
														but looks better to say 'TEXT' for file type than 0x38429834,
														or whatever the number might be.
	
			qConstStaticInitializersAllowedRedundently
													-	can we say struct a { static const int x;} ; const int a::x = 1;
														in the header? If we do, then this value can be inlined, and
														therefore much more efficient, but most UNIX systems (eg gcc)
														make this an actaul variable, and generate link errors.
	
			qCanAlwaysExplicitlyCallDTOR			-	Set True if we can call int::~int().
														This ability is important when
														writting generic classes, like dynamic array.
	
			qPlacementOperatorNewProvided			-	Set True if placement operator new is provided in new.h.
	
	 */




/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&& qGCC &&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */
#if		qGCC

	/*
	 * Generic features present/not present. Features defined above.
	 */
	#define	qCanFreelyUseVirtualBaseClasses									1
	#define	qPureVirtualsPropagated											1
	// Allow redefinition from compile line(Config file) since we support both,
	// and convienent to be able to test both with one compiler...
	#ifndef	qRealTemplatesAvailable
		#define	qRealTemplatesAvailable  									1
	#endif

	#define	qCanAlwaysExplicitlyCallDTOR									1

	#define	qTemplatesHasRepository											0
	#define	qStaticDataMembersInTemplatesSupported							0
	#define	qPlacementOperatorNewProvided									1



	/*
	 * GCC specific bugs. Bugs only found in gcc.
	 */


	/*
	 * Enums in enumeration not considered constant - 
	 *		eg enum a { x, y, z=x } gives error saying x not constant...
	 *
	 * (Tuesday, June 30, 1992 12:41:15 AM)
	 *
	 *		Still Broken as of:
	 *			gcc  2.2.1 (aux)- Tuesday, June 30, 1992 12:41:15 AM
	 *			gcc  2.3.1 (aux)- Saturday, November 7, 1992 9:34:57 PM
	 *			gcc  2.3.2 (aux)- Wednesday, December 2, 1992 1:54:46 AM 
	 *
	 *			Posted to bug-g++@prep.ai.mit.edu on 11/09/92
	 */
	#define	qGCC_EnumeratorsNotConstantWhileDefiningEnumeration				1


	/*
	 * Wednesday, July 1, 1992 1:55:12 AM
	 *
	 *		In File.cc, struct stat { ... }; int stat (...); OK, and calls like stat ()
	 *	work fine, but if you call ::stat (args) you get parse error.
	 *
	 *		Originally Broken as of:
	 *			gcc            2.2.1-aux  	- Wednesday, July 1, 1992 1:55:12 AM
	 *
	 *		fixed in:
	 *			gcc 2.3.1 - Saturday, November 7, 1992 7:29:54 PM
	 */
	#define	qGCC_ColonColonScopeResolutionOpBrokeIfStructAndFunctAtScope	0


	/*
	 * Friday, July 3, 1992 1:58:47 AM
	 *
	 *		Originally Broken as of:
	 *			gcc            2.2.1-aux  	- Friday, July 3, 1992 1:58:47 AM
	 *
	 *		This function is listed in my ANSI C book, and is provided by apple, but is missing
	 *	from g++lib 2.1 - so we extern "C" declare it manually, and pull in version from apples
	 *	lib.
	 *
	 *		Oops - mystery apparently not solved yet - making the declaration I need extern "C"
	 *	seems to confuse gcc - For now I just say extern - but if it ever gets called, we wont
	 *	link properly. Must try to fix this - simplify, and report bug.
	 */
	#define	qGCC_GPlusPlusLib_MissingStrToUL								1


	/*
	 * Friday, July 3, 1992 1:58:47 AM
	 *
	 *		Ptr to functions has been a long standing problem with gcc. Seems to work a little
	 *	better than the 1.40 based compiler, but not there yet???
	 *
	 *		From Memory.cc:		extern	Boolean (*GetOutOfMemoryProc ()) (size_t);	
	 * 		(you NEED the extern, or it wont compile)
	 *
	 *		Originally Broken as of:
	 *			gcc		2.2.1-aux  	- Friday, July 3, 1992 1:58:47 AM
	 *
	 *		Still Broken as of:
	 *			gcc		2.3.1 		- Tuesday, November 10, 1992 3:37:00 PM
	 *			gcc		2.3.2 		- Wednesday, December 2, 1992 1:50:07 AM
	 *
	 *			Posted to bug-g++@prep.ai.mit.edu on 11/10/92
	 *
	 */
	#define	qGCC_TroubleWithFunctionsReturningFunctionPtrs					1


	/*
	 * Friday, July 3, 1992 1:58:47 AM
	 *
	 *		Originally Broken as of:
	 *			gcc            2.2.1-aux  	- Friday, July 3, 1992 1:58:47 AM
	 *
	 *		libg++ 2.1 iostreams dont seem to support defining new maniplataors that
	 *	take arguments. I cannot see any reason why I could not just lift the ATT iostream
	 *	version of these macros, but it needs more research, and that may not be legal.
	 *	???
	 *
	 */
	#define	qGCC_libgPlusPlusIostreamsHaveNoManipulatorSupport				1


	/*
	 * Monday, July 20, 1992 1:15:38 PM
	 *
	 *		Causes gcc internal error:
	 *			const	PathComponent	PathName::kDot		=	CurrentDirectoryPathComponent ();
	 *		Works:
	 *			const	PathComponent	PathName::kDot		=	PathComponent (new CurrentDirectoryPathComponentRep ());
	 *
	 *		Originally Broken as of:
	 *			gcc            2.2.1-aux  	- Monday, July 20, 1992 1:15:38 PM
	 *
	 *		fixed in:
	 *			gcc 2.3.1 - Saturday, November 7, 1992 7:29:54 PM
	 *		WHOOPS - not really - still BROKEN - just I lost my testcase...
	 *			gcc 2.3.1 - Wednesday, November 25, 1992 6:13:45 PM
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 2:05:09 AM
	 *			Only now the symtom is:
	 *				bad argument 0 for function `PathComponent::PathComponent (class PathComponentRep*)' (type was class CurrentDirectoryPathComponent*)
	 *				in base initialization for class `PathComponent'
	 */
	#define	qGCC_StaticInitializerObjectSliceBug							1


	/*
	 * Thursday, September 3, 1992 11:31:34 AM
	 *
	 *		Originally Broken as of:
	 *			libg++            2.1-aux  	- Thursday, September 3, 1992 11:31:34 AM
	 *
	 *
	 */
	#define	qGCC_LibgPlusPlusSharpDefineSize_tBug							1


	/*
	 * Thursday, September 3, 1992 11:31:34 AM
	 *
	 *		Originally Broken as of:
	 *			libg++            2.1-aux  	- Thursday, September 3, 1992 11:31:34 AM
	 *
	 *	attach method missing from ifstream, ofstream, and fstream. Use
	 *	rdbuf ().attach () as workaround.
	 */
	#define	qGCC_LibgPlusPlusIOStreamAttatchMissingBug						1


	/*
	 * (LGP Saturday, September 5, 1992 9:17:52 PM)
	 *
	 *	GCC treats call to new with placement as syntax error. Problem in our
	 *	Array(T) template implemenations.
	 *
	 *		Originally Broken as of:
	 *			gcc 2.2.1 - Saturday, September 5, 1992 9:17:52 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on 9/3/92(rougly)
	 *
	 *		fixed in:
	 *			gcc 2.3.1 - Saturday, November 7, 1992 7:29:54 PM (Sort of- I mostly tested with 2.3.1.1,
	 *			which had extra patches from Brendon Kehoe(cygnus)). Test again with release version.
	 *			Also, other related stuff broken with this release which was the
	 *			main source of confusion. See bug: qGCC_OperatorNewAndStarPlusPlusBug.
	 *		DEFINITELY FIXED IN:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 5:25:20 PM
	 */
	#define	qGCC_PlacementOpNewGeneratesSyntaxError							0


	/*
	 * (LGP Saturday, Tuesday, September 8, 1992 9:49:40 PM)
	 *
	 *	GCC fails to do single conversion operator in call to op= when
	 *	it was implicity defined. Either explicity defining the op= method
	 *	or manually calling conversion operator works around the problem.
	 *
	 *		Originally Broken as of:
	 *			gcc 2.2.1 - Tuesday, September 8, 1992 9:49:40 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on 9/8/92
	 *
	 *		fixed in:
	 *			gcc 2.3.1 - Saturday, November 7, 1992 7:29:54 PM
	 */
	#define	qGCC_OverloadingWithoutExplicitOpEqualsDefine					0


	/*
	 * (LGP Saturday, Tuesday, Friday, September 11, 1992 8:16:31 AM)
	 *
	 *	GCC fails to properly generate (and call) an implied copy constructor.
	 *	That is, if you have a class which requires being copied (say string
	 *	for example) and simple declare it as a field of some struct, the compiler
	 *	should automatically generated the right copy CTOR and assignemnt operator
	 *	based on memberwise copy. It seems to not do this however, with gcc 2.2.1.
	 *
	 *	See HashTable TestSuite for example, and workaround.
	 *
	 *		Originally Broken as of:
	 *			gcc 2.2.1 - Friday, September 11, 1992 8:16:31 AM
	 *			Posted to bug-g++@prep.ai.mit.edu on 9/10/92
	 *
	 *	LGP: Saturday, November 7, 1992 10:58:38 PM
	 *	may or may not still be broken - shuttting off workaround causes
	 *	break in options.cc cuz of Array, but may be other troubles - try
	 *	a little later...
	 *
	 *	LGP Tuesday, November 10, 1992 3:48:16 PM
	 *	Try new patch from the net, and see if this is still broken!!!
	 *
	 *
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 2:05:09 AM
	 *
	 * The line (From InfoDB.c in GenMake/Class):
	 *                InfoRecord elt = it.Current ().fElt;
	 * ../Templates/MappingOfString_InfoRecord.cc:194: invalid lvalue in unary `&'
	 * ../Templates/MappingOfString_InfoRecord.cc:194: warning: Internal error: t2 == error_mark_node in `comptypes'
	 *	Reduce to small case, and repost...(Wednesday, December 2, 1992 3:39:22 PM)
	 *	I wonder if workarounds in other files (Array.hh etc) with same name are unnecessary...
	 */
	#define	qGCC_BadDefaultCopyConstructorGeneration						1


	/*
	 * (LGP Saturday, Monday, November 2, 1992 5:07:45 PM)
	 *
	 *	No idea whats going on here.  Bad job describing problem, but since
	 *	I'll have a new gcc any day now not much point in analyzing til I've tried
	 *	new version.
	 *
	 *	See LinkList.hh for example, and workaround.
	 *
	 *		Originally Broken as of:
	 *			gcc 2.2.1 - Saturday, Monday, November 2, 1992 5:07:45 PM
	 *
	 *		fixed in:
	 *			gcc 2.3.1 - Saturday, November 7, 1992 7:29:54 PM
	 */
	#define	qGCC_InternalError95InInlineExpansionAndScopeResolution			0


	/*
	 * (LGP Wednesday, November 4, 1992 3:04:56 PM)
	 *
	 *	No idea whats going on here.  Some bug with optimization. If I compile
	 *	InfoDB.cc with -O, then GetAt(1) crashes in ProcessRenames() in GenClass.cc
	 *	Other operations seem to work fine. For now, just hack around it.
	 *
	 *	See GenClass.cc
	 *
	 *		Originally Broken as of:
	 *			gcc 2.2.1 - Wednesday, November 4, 1992 3:04:56 PM
	 *
	 *		Still Broken as of:
	 *			gcc 2.3.1 - Tuesday, November 10, 1992 3:31:52 PM
	 *			gcc 2.3.2 - Thursday, December 3, 1992 1:11:07 PM
	 */
	#define	qGCC_MysteryOptimizerWithGetAtBug								1


	/*
	 * (LGP Saturday, November 7, 1992 7:29:54 PM)
	 *
	 *	struct  Base { Base (); };
	 *	struct  Enclose {
	 *		struct  Fred : Base {
	 *	       Fred (const Fred& a);
	 *		};
	 *	};
	 *	typedef Enclose::Fred   Fred;
	 *	#if     qCompileFine
	 *	Fred::Fred (const Enclose::Fred& a) :
	 *	#else
	 *	Fred::Fred (const Fred& a) :
	 *	#endif
	 *	        Base ()
	 *	{
	 *	}
	 *
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.1 - Saturday, November 7, 1992 7:29:54 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on 11/7/92
	 *
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 2:05:09 AM
	 *			Well, maybe not broken: gives error message I don't quite understand.
	 *			Needs more analysis. Need this workaround for the time being anyhow,
	 *			but it could be my bug.
	 */
	#define	qGCC_NestedClassWithTypedefParseBug								1


	/*
	 * (LGP Monday, November 9, 1992 10:06:15 PM)
	 *
  	 * void*   x = new char [n];
     * delete (p);
	 * Produces:
	 *		warning: ANSI C forbids taking the sizeof a void type 
	 * With -Wpointer-arith
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.1 - Monday, November 9, 1992 10:06:15 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on 11/08/92
	 *
	 *		Accidentally had two names for this bug- extra name now deleted is:
	 *		qGCC_WarnPointerArithmaticOverzealousOnDeleteVoidStartBug.
	 *
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 3:54:34 PM
	 */
	#define	qGCC_DeleteOfVoidStarProducesWarningBug							1


	/*
	 * (LGP Monday, November 9, 1992 10:06:15 PM)
	 *
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.1 - Monday, November 9, 1992 10:06:15 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on ?????????
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 2:05:09 AM
	 */
	#define	qGCC_OperatorNewAndStarPlusPlusBug								1


	/*
	 * (LGP Monday, November 9, 1992 10:06:15 PM)
	 *
	 *SEE InfoDB.hh or GenMake.cc
	 *		Originally Broken as of:
	 *			gcc 2.3.1 - Monday, November 9, 1992 10:06:15 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on ?????????
	 *
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 3:39:22 PM
	 *
	 *	Without this workaround we get (gcc 2.3.2-Wednesday, December 2, 1992 3:39:22 PM):
	 *		./InfoDB.cc:162: too few arguments for constructor `InfoRecord'
	 *		./InfoDB.cc:162: in base initialization for class `InfoRecord'
	 */
	#define	qGCC_NoAutoGenOfDefaultCTORBug									1


	/*
	 * (LGP Tuesday, November 10, 1992 4:45:18 PM)
	 *
	 * 	template <class T> class Array { stuff deleted; };
	 *
	 * followed by:
	 * 	template <class T> class Array;
	 * causes gcc internal error #124.
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.1 - Tuesday, November 10, 1992 4:45:18 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on 11/10/92
	 */
	#define	qGCC_ReDeclareTemplateClassInternalError124Bug					1


	/*
	 * (LGP Saturday, November 21, 1992 3:14:32 AM)
	 *
	 * This template code fails to compile with g++ 2.3.1.
	 *
	 * Compiled as:
	 *		g++ -c Bug.cc
	 *
	 * Produces:
	 * 	Bug.cc:18: inconsistent return types for method `ArrayNode' in class `ArrayNode<int>'
	 * 	/usr/local/bin/gcc: Internal compiler error: program cc1plus got fatal signal 11
	 *
	 *
	 *	template    <class  T>  class   ArrayNode {
	 *   		public:
	 *        	ArrayNode (int a);
	 *        	~ArrayNode ();
	 *
	 *       	T   fItem;
	 *	};
	 *
	 *	template    <class  T>      ArrayNode<T>::ArrayNode (int a)
	 *	{
	 *	}
	 *	template    <class  T>      ArrayNode<T>::~ArrayNode ()
	 *	{
	 *	}
	 *	ArrayNode<int> f(1);
	 *
	 *		Getting rid of the parameter to ArrayNode::CTOR () makes
	 *	everything compile OK.
	 *
	 *		Getting rid of the DTOR also makes things compile.
	 *	I've tried dozens of different syntaxes to try to get something
	 *	like this to compile. Is there any syntax gcc 2.3.1 will accept
	 *	for a templated class with CTOR (with arguments) and DTOR????
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.1 - November 21, 1992 3:14:32 AM
	 *			Posted to bug-g++@prep.ai.mit.edu on 11/10/92
	 *
	 *			Workaround from hohmann@sc.ZIB-Berlin.DE (Hohmann): November 21, 1992
	 *			Make: ArrayNode (int a); inline implemenation.
	 *
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 5:32:17 PM
	 */
	#define	qGCC_TemplatesCTORWithArgMustBeInlineBug						1


	/*
	 * (LGP Monday, November 9, 1992 10:06:15 PM)
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.1 - Monday, November 23, 1992 5:49:04 PM	-	2.3.1
	 *			INVESTIGATE FURTHER, and REPORT IF IT REALLY IS A BUG!!!
	 *
	 *		Still Broken as of:
	 *			gcc 2.3.2 - Wednesday, December 2, 1992 3:39:22 PM
	 *			with 2.3.2 causes hang... - compiler infinite loop??
	 *
	 * INVESTIGATE FURTHER, and POST!!!
 	 *
	 */
	#define	qGCC_ConversionOperatorInitializerConfusionBug					1


	/*
	 * (LGP Tuesday, December 1, 1992 4:39:08 PM)
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.2 - Tuesday, December 1, 1992 4:39:08 PM
	 *
	 * INVESTIGATE FURTHER, and POST!!!
 	 *
	 */
	#define	qGCC_InserterToSubclassOfOStreamBug								1


	/*
	 * (LGP Tuesday, December 1, 1992 9:56:25 PM)
	 *
	 * Compiled as:
	 *		g++ -c bug.cc
	 *
	 * Produces:
	 *
	 * Compiling the given code with g++ 2.3.2 produces the following
	 * output:
	 *      bug.cc: In function `void  _GLOBAL_$I$f ()':
	 *      bug.cc:15: in base initialization for class `Fred',
	 *      bug.cc:15: invalid initializer to constructor for type `Fred'
	 *
	 * It compiled fine with CFront, BorlandC++, and all previous g++
	 * releases.
	 *
	 *	class   Fred {
	 *		public:
	 *			Fred ();
	 *	};
	 *	
	 *	Fred    f       =       Fred ();        // this fails
	 *	Fred    g;                              // this compiles fine
	 *
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.2 - Tuesday, December 1, 1992 9:56:25 PM
	 *			Posted to bug-g++@prep.ai.mit.edu on 12/01/92
	 */
	#define	qGCC_SimpleClassStaticInitializerBug						1


	/*
	 * (LGP Tuesday, December 1, 1992 9:56:25 PM)
	 *
	 *		Calling function that requires conversion operator to
	 *	invoke causes hang with new version of the compiler.
	 *
	 *		Workaround is to explicitly call cast operator.
	 *
	 *		Originally Broken as of:
	 *			gcc 2.3.2 - Tuesday, December 1, 1992 9:56:25 PM
	 */
	#define	qGCC_ConversionOperatorCausesHangBug						1


	/*
	 * (LGP Monday, November 9, 1992 10:06:15 PM)
	 *
	 *		Originally Broken as of:
	 *			with 2.3.2 
	 *
	 * See GenMake.cc/GenClass.cc...
	 * Probably very closely related to qGCC_ConversionOperatorInitializerConfusionBug or
	 * qGCC_SimpleClassStaticInitializerBug????
	 *
	 * INVESTIGATE FURTHER, and POST!!!
 	 *
	 */
	#define	qGCC_StaticInitializerForArrayOfObjectsWithCTORBug			1


	/*
	 * (LGP Wednesday, December 9, 1992 1:59:46 PM)
	 *
	 *		Originally Broken as of:
	 *			with 2.3.2 
	 *
	 *	See Array.hh and writeup for g++ buglist soon...
 	 *
	 */
	#define	qGCC_OpEqualsNotAutoDefinedSoCanBeExplicitlyCalledBug		1



#endif	/*qGCC*/






/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&& qCFront21 &&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */
/*
 *		Bugs present in most generic CFront 2.1 implementations - these can
 *	be overridden later. CFront 2.1 compiler bug defines - these can be overriden
 *	by any particular CFront compiler - eg qHPCPlus
 */
#if		qCFront21

	/*
	 * Generic features present/not present. Features defined above.
	 */
	#define	qCanFreelyUseVirtualBaseClasses									0
	#define	qPureVirtualsPropagated											1
	#define	qPlacementOperatorNewProvided									1




	/*
	 * GCC specific bugs. Bugs only found in gcc.
	 */

	/*
	 * CFront internal error when you try to use a class with virtual dtor in ? or && or || expressions.
	 * << CHANGE NAME OF THIS ERROR >>
	 */
	#define	qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG						1			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 * Althogh I've only seen this bug on the HP compiler (its in motif specific code), I am convinced it is
	 * a generic CFront bug, cuz I've seem lots of problems like this. The problem stems from the fact that
	 * theres lots of stuff CFront cannot do inlining, and it sometimes detects this too later and generates
	 * internal errors.
	 *
	 *		Still Broken as of:
	 *			CFront HP (8.05?) - 3/9/92 - LGP
	 */
	#define	qCFRONT_ARRAYS_IN_INLINE_CAUSE_ERROR_BUG						1




	// Seems this is broken in all 2.1 based iostream libs - at least sterl thinks so - LGP March 26, 1992
	#define	qHPCPlus_StreamLibTrompingAtEndBug								1



	#define	qMPW_CFRONT_2_1_FUNNY_STREAM_REAL_BUG							1




	/*
	 * Not sure this is a bug, but it seems that if you have access in a public base class, and then in a subclass
	 * you override, and take away access, you SHOULD STILL REALLY have access. CFront doesnt support
	 * this, but it makes sense, and I bet its the law!!!
	 *
	 * (LGP Thursday, October 17, 1991 4:55:27 AM)
	 *
	 *		Still Broken as of:
	 *			CFront 1.1d16  - Thursday, October 17, 1991 4:55:27 AM
	 */
	#define		qMPW_CFront_AccessShouldBeGenerousBug						1




	/*
	 * Virtual base classes are STILL broken.  Creating several layers of virtual base classes (2 or more)
	 * causes address errors in executing code (ie bad code generation).  Have not followed up fully,
	 * but it is definitely a problem.
	 *
	 * vbase class bug still there as of:
	 *  C                 3.2b6     
	 *  CFront            1.1d13    
	 *
	 *		Still Broken as of:
	 *			CFront 3.2b3    - Tuesday, February 11, 1992 12:45:11 PM
	 *
	 *<CONSIDER Getting rid of this separate bug, and just use qCanFreelyUseVirtualBaseClasses>
	 */
	#define	qMPW_VIRTUAL_BASE_CLASS_BUG										1


	/*
	 * Reference to obj '.' virtual method does not call right method.
	 * Seems only to affect virtual function calls from non-leftmost base classes.
	 *		Still Broken as of:
	 *			CFront 3.2b3    - Tuesday, February 11, 1992 12:45:11 PM
	 */
	#define	qMPW_REFERENCE_DOT_VIRTUAL_BUG									1			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	#define	qCFront_NestedTypesHalfAssed									1


	/*
	 * (LGP Friday, Tuesday, August 18, 1992 4:37:12 PM)
	 *
	 * The code:
	 *		Set_BitString (KeyStrokeModifiers) s;
	 *		const AbSet (KeyStrokeModifiers)& sss= s;
	 *		const AbSet (KeyStrokeModifiers)& sss1 ();
	 *		IndirectIterator(KeyStrokeModifiers) bbfredasssx (sss);
	 *		IndirectIterator(KeyStrokeModifiers) bbfredasssx1 (sss1());
	 * yields # error: cannot make a IndItKeyStrokeModifiers from a const AbSetOfKeyStrokeModifiers 
	 * for the line with the function call sss1 but not the global variable.
	 * Set_BitString (KeyStrokeModifiers) has an operator Iterator<KeyStrokeModifiers>*
	 * and IndirectIterator(KeyStrokeModifiers) has one CTOR with arg Iterator<KeyStrokeModifiers>*,
	 * so BOTH should work. Workaround is to explicily call conversion operator on result of
	 * function call.
	 *
	 *
	 *		Originally Broken as of:
	 *			Apple CFront            3.2  - Tuesday, August 18, 1992 4:37:12 PM (ETO#8)
	 */
	#define	qCFront_ConversionOpOnFunctionResultBroken					1


	/*
	 * (LGP Wednesday, October 28, 1992 2:53:36 PM)
	 *
	 * In BlockAllocated.hh"; line 101 # sorry, not implemented: cannot expand value-returning
	 *		inline BlockAllocatedSequenceRep_ArrayOfString::operator new() with call of
	 *		non-value-returning inline BlockAllocatedSequenceRep_ArrayOfString::SetNextLink()
	 *
	 *		Originally Broken as of:
	 *			Apple CFront            3.2  - Wednesday, October 28, 1992 2:53:36 PM (ETO#8)
	 */
	#define	qCFront_InlinesNotFullyImplemented							1


	/*
	 * (LGP Wednesday, October 28, 1992 2:53:36 PM)
	 *
	 * In File ":Foundation:Headers:Sequence_Array.hh"; line 34 
	 *		# error:  SequenceRep_ArrayOfString::operator new() declared with external
	 *					linkage and called before defined as inline
	 *
	 *		You must declare inline in class declaration for op new / op delete.
	 *
	 *		Originally Broken as of:
	 *			Apple CFront            3.2  - Wednesday, October 28, 1992 2:53:36 PM (ETO#8)
	 */
	#define	qCFront_InlinesOpNewDeleteMustDeclareInline					1


// LGP Sunday, November 22, 1992 10:38:49 PM - maybe this means its not a bug - lookup in ARM!!!
#define	qBCC_ConditionalOperatorDemandsTypesExactBug	1


	/*
	 * (LGP Thursday, December 3, 1992 10:40:21 PM)
	 *
	 * 	Sorry not implemented, statement after return in inline function.
	 *	OK, so you cannot inline it, but why barf??? A warning would have sufficed!!!
	 *
	 *		Originally Broken as of:
	 *			Apple CFront            3.2  - Wednesday, October 28, 1992 2:53:36 PM (ETO#8)
	 */
	#define	qCFront_SorryNotImplementedStatementAfterReturnInline		1


#endif	/*qCFront21*/






/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&& qCFront30 &&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */
/*
 *		Bugs present in most generic CFront 3.0 implementations - these can
 *	be overridden later. CFront 3.0 compiler bug defines - these can be overriden
 *	by any particular CFront compiler - eg qHPCPlus
 */
#if		qCFront30

	/*
	 * Generic features present/not present. Features defined above.
	 */
	#define	qCanFreelyUseVirtualBaseClasses									1
	#define	qPureVirtualsPropagated											1
	// Allow redefinition from compile line(Config file) since we support both,
	// and convienent to be able to test both with one compiler...
	#ifndef	qRealTemplatesAvailable
		#define	qRealTemplatesAvailable  									1
	#endif

	#define	qCanAlwaysExplicitlyCallDTOR									1

	#define	qTemplatesHasRepository											1
	#define	qStaticDataMembersInTemplatesSupported							1
	#define	qPlacementOperatorNewProvided									1


	#define	qCFront_InlinesOpNewDeleteMustDeclareInline						1

#endif	/*qCFront30*/










/*
 ***********************************************************************************************
 ********************************* Vendor Specific Compiler Specific Bugs **********************
 ***********************************************************************************************
 */



/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&& qMPW_CFront &&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */
#if		qMPW_CFront

	/*
	 * Generic features present/not present. Features defined above.
	 */
	#define	qLongCharacterConstantsSupported								1



	#define	qConstStaticInitializersAllowedRedundently						1



	#define	qCanAlwaysExplicitlyCallDTOR									0




	/*
	 * qMPW_CFront specific bugs. Bugs only found in apples CFront implementation.
	 */



	/*
	 * This usually just comes up calling delete (it.Current ()) in iterator
	 */
	#define	qMPW_CFRONT_BUG__PtrExpressionTooCompilcatedInDelete			1		// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 * GUESS: as an attempt to work around awkwardness in semantics of load-dump, apple
	 *	changed default of const from static to extern (trouble they had was construction).
	 * 	This is COUNTER TO DEFN in CFRONT 2.0 manual from ATT.  Bad boys...
	 */
	#define	qMPW_CFRONT_BUG__ConstShouldBeStaticByDefault					1			// Apple CFRONT 1.0b3 (?)


	/*
	 * this bug causes use of load dump to result in 'memory allocation error' from CFRONT on
	 * dump
	 *
	 *		Still Broken as of:
	 *			  Apple CFRONT 1.0 (9/11/90; AT&T 2.0)... (CFront  1.1d16 )
	 *
	 *		Still Broken as of:
	 *			CFront 3.2b3    -	Tuesday, February 11, 1992 12:45:11 PM
	 *			Well, they didnt exactly fix this, but the error message is different now:
	 *			File ":Headers:Stroika-Foundation.hh"; line 60 # internal << CFront Version 3.2b3 (12/6/91; AT&T 2.1.12) ) Apple Computer, Inc. 1989-9 >> error: unknown node type
	 *
	 *		Still Broken as of:
	 *			  CFront 3.2	-	Friday, July 10, 1992 12:01:00 PM) 
	 */
	#define	qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns					1			// CFront 3.2	-	Friday, July 10, 1992 12:01:00 PM) 


	/*
	 * When there is no keyfunction, ie non-inline first virtual function, cfront usually, though
	 * not always forgets to generate a vtable.  Fix is to declare bogus ptbl_hack () funct, and
	 * define it at the bottom of the class declaration.
	 *
	 *	(search for ptbl_hack, and maybe others, in source files for workarounds...)
	 *
	 *		Still Broken as of:
	 *			  Apple CFRONT 1.0 (9/11/90; AT&T 2.0)
	 *
	 *		Still Broken as of:
	 *			CFront 3.2a3 - Monday, October 14, 1991 11:53:00 AM
	 *
	 *		Apparently fixed in:
	 *			 CFront            3.2b3 - Monday, March 9, 1992 10:17:47 AM - LGP
	 *
	 * Look in TextEdit.cc - I think I found a case where it breaks -- LGP March 22, 1992
	 *
	 *		Still Broken as of:
	 *			  CFront 3.2	-	Friday, July 10, 1992 12:01:00 PM) 
	 */
	#define	qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG								1			// CFront 3.2	-	Friday, July 10, 1992 12:01:00 PM) 


	/*
	 * Macro overflow on setquence_array... implementation.
	 * (LGP Saturday, February 2, 1991 12:59:38)
	 *
	 * error: macro expansion overflowed internal buffer size; happened in: doMacroDef at closeParen 
	 *
	 *		Still Broken as of:
	 *			CFront 1.1d16, C 3.2b6 - Sunday, July 21, 1991 8:33:11 PM
	 *
	 *		Still Broken as of:
	 *			CFront 3.2a3 - Monday, October 14, 1991 11:53:00 AM
	 *
	 *		Still Broken as of:
	 *			CFront 3.2b3    - Tuesday, February 11, 1992 12:45:11 PM
	 */
	#define	qMPW_MacroOverflowProblem										1



	/*
	 *
	 * (LGP Tuesday, March 19, 1991 8:14:51 AM)
	 *		Really needed for Try stuff. Can cause bad bugs otherwise....
	 */
	#define	qMPW_NO_VOLATILES_BUG											1			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)
	#if		qMPW_NO_VOLATILES_BUG
		/*
		 * MPW C (3.2b3) doesn't seem to support volitle (or at least that version of CFRONT DOESNT)
		 * We use the word since it should work on other systmes
		 *
		 *	Actually C compiler fine - its CFront that barfs on them - still broken with:
		 *		CFront 1.1d13   (Tuesday, May 21, 1991 3:35:41 AM)
		 *
		 *		Still Broken as of:
		 *			CFront 3.2a3 - Monday, October 14, 1991 11:53:00 AM
		 */
		#define	volatile	
	#endif	/*qMPW_NO_VOLATILES_BUG*/


	/*
	 *	The code:
	 *		(fThumbFirst) ? fThumbGrid : fROMGrid;
	 *	generates an error with "incompatable types", even though both things are declared
	 *	to be pointers to classes with a commom base. Possibly relevant, is that at the point
	 *	where they were declared, the full class hierarchy was not known?
	 * (LGP Tuesday, June 11, 1991)
	 */
	#define	qMPW_QUESTION_MARK_TYPE_PROMO_CFRONT_BUG						1			// Apple CFRONT 1.1d13  


	/*
	 *	typedef	(**MDEFPROC) (short, osMenuInfo**, osRect*, long, short*); was necessary since
	 * doing the type directly for a function retunging this type caused CFront to output a bogus
	 * extern of that function.
	 * (LGP June 29, 1991 12:03:27 PM)
	 */
	#define	qMPW_CFRONT_NEEDS_INTERMEDIATE_TYPEDEF_FOR_FUNCT_RETRN			1			// Apple CFRONT 1.1d13  

	
	/*
	 *	Not quite sure why the Declare2 stuff doesn't work - investigate!!!.
	 * (LGP Saturday, September 14, 1991 12:28:49 AM)
	 *
	 *		Still Broken as of:
	 *			CFront 3.2a3 - Monday, October 14, 1991 11:53:00 AM
	 *			CFront 3.2 - Thursday, May 14, 1992 1:40:57 PM
	 */
	#define	qMPW_APPLE_CPP_BROKEN_FOR_DECLARE2								1			// Apple CFRONT CFront 3.2a3





	/*
	 * Ugg. See Tracker.hh.
	 *
	 * (LGP Friday, November 22, 1991 2:15:54 PM)
	 *
	 *		Still Broken as of:
	 *			CFront 1.1d16  - Tuesday, November 12, 1991 11:17:05 AM
	 */
	#define	qMPW_CLASS_SCOPED_ARRAYOFOBJS_WITH_NO_NOARG_CTOR_BUG			1




	#define		qARRAY_TYPEDEF_IN_STRUCT_BOOM_BUG							1


	/*
	 * Doing local classes with vtables causes crash in CFront routine RealyReallyPrint...
	 *
	 * (LGP Friday, November 22, 1991 2:15:54 PM)
	 *
	 *		Still Broken as of:
	 *			CFront  3.2b3 	-	Saturday, December 28, 1991 12:42:08 AM    
	 */
	#define		q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT			1




	#define		qMPW_CFRONT_2_1_FUNNY_STREAM_TRY_TO_OVERLOAD_BUG			1
	#define		qMPW_CFRONT_2_1_NESTED_TYPEDEFS_BREAK_LOADDUMP_BUG			1




	/*
	 * Load/Dump breaks with SingleObject!!! Simple inheritence followed by subclassing adding
	 * a pure virtual method in a -dumpc file causes bad code gen after the load - makes C compiler
	 * barf.
	 *
	 * In the loaddump file put:
	 *
	 * struct	Base : SingleObject {			// Comment out ": SingleObject" and all is well...
	 * 		Base ();
	 * 
	 * 		virtual	~Base () {}
	 * 
	 * 		virtual	void	f () const;
	 * };
	 * 
	 * struct	Derived : Base {
	 * 		Derived ();
	 * 		virtual	void	g ()	const = 0;
	 * };
	 *
	 * and then
	 * void f () {}
	 * in the file doing the -load - and you get bad code gen in cfront to C.
	 *
	 * << Similar bug with Set(T) - if you defined out-of-line the virtual dtor, you get
	 * 		barf - since Set(T) is a singleobject, and barf looks the same - my guess is same
	 *		bug>>
	 *
	 *
	 * (LGP Friday, December 27, 1991 10:23:58 PM)
	 *
	 *		Still Broken as of:
	 *		   C                 3.2      			Friday, December 27, 1991 10:23:58 PM 
	 *		   CFront            3.2b3     			Friday, December 27, 1991 10:23:58 PM
	 *
	 *		Still Broken as of:
	 *			C                 3.2.2     
	 *			CFront            3.2       
	 *			Thursday, April 16, 1992 11:08:42 AM
	 */
	#define		qMPW_CFRONT_2_1_SingleObjectSubclassThenAddPureVirtualBreaksLoadDump_BUG	1


	// Also breaks on mac - LGP Feb 7, 1992
	#define	qHPCPlus_StaticConstructorReturnBug	1


	//
	// Apple's CFront (really all versions) has builtin support for the inherited keyword, and we
	// want to implement its functionality ourselves using nested typedefs, and so we need to
	// disable their interpretation...
	#define	inherited	inherited_


	/*
	 * (LGP Friday, Tuesday, April 7, 1992 10:06:45 AM)
	 *
	 *		Have a bunch of static members and first set are inintialized with
	 *	constants (eg 1, 2, 3)
	 *
	 *		Then a second set of static const members ininted with THOSE above
	 *	named constats. Externs generated, and code in _sti_ generated to init
	 *	them, but NO Defining implementation, so we get linker sym undefined error.
	 *
	 *		Originally Broken as of:
	 *			CFront            3.2  	- Tuesday, April 7, 1992 10:06:45 AM (ETO#7)
	 */
	#define	qMPW_STATIC_CONST_ENUM_MEMBER_NOT_DEFINED_WHEN_DEFINED_BUG		1


	/*
	 * (LGP Friday, Wednesday, April 8, 1992 3:30:15 PM)
	 *
	 *	Some initializer in QuickHelp.cc generated a sorry not implemetned message from CFront, and had to
	 *	be munged to work.
	 *
	 *		Originally Broken as of:
	 *			CFront            3.2  	- Wednesday, April 8, 1992 3:30:15 PM (ETO#7)
	 */
	#define	qMPW_STATIC_CONSTRUCTOR_COMPLEX_INITIALIZER_NYI_BUG				1




	/*
	 * (LGP Friday, Tuesday, November 24, 1992 9:52:34 AM)
	 *
	 *	Changing the conversion operators in Sequence to return Iterator<T> instead
	 *	of IteratorRep<T>* caused CFront to generate bad code:
	 *	#((*infoRecord)). fArguments)) , ( __op16IteratorOfString__16SequenceOfStringCFv( (struct SequenceOfString *)(& __U414), & __R415) , __R415) ) )
	 *	#                                                                                                                                              ? 
	 *	### Error 198 operand of (cast) must be scalar
	 *	Occasionally. Workaround is explicit cast.
	 *
	 *		Originally Broken as of:
	 *			CFront            3.2  	- Tuesday, November 24, 1992 9:52:34 AM (ETO#8)
	 */
	#define	qMPW_CFront_ConversionOperatorInitializerCodeGenBug				1

#endif	/*qMPW_CFront*/








#if		qMPW_CFront_AppleC


	/*
	 * some kind of bug with pascal procptrs.
	 *
	 *	ProcPtr		actionProc	=	(partCode == inThumb)? ProcPtr (ThumbActionProc): ProcPtr (DragActionProc);
	 *		causes some incorrect compiler diagnostic.
	 *		whereas the below code works fine.
	 *	ProcPtr		actionProc	=	(partCode == inThumb)? ProcPtr ((void*)&ThumbActionProc): ProcPtr (DragActionProc);
	 *
	 *		Still Broken as of:
	 *			CFront 3.2a3 - Monday, October 14, 1991 11:53:00 AM
	 *		Still Broken as of:
	 *			C                 3.2.2     
	 *			CFront            3.2       
	 *			Tuesday, April 14, 1992 1:06:33 PM
	 */
	#define	qMPW_PASCAL_PROC_PTR_BUG										1			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 * Scoped classes only work at outer most scope of the function (at least not in an if block)
	 */
	#define	qMPW_SCOPED_CLASSES_IN_IF_BUG									1			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 * Calling pascal direct (traps) functions with compicated arguments including inlined functions, etc
	 * seems to be generating bad code.  Introducing a temporary solves the problem.  I believe
	 * it has nothing to do with CFRONT, and is a bug with the C Compiler (3.2b1).
	 * EG ::PenSize (short (fred.GetV ()), ..);
	 */
	#define	qMPW_COMPLEX_ARGS_TO_DIRECT_FUN_BUG								1			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 * Somehow doing large complex general initializers seems to screw up with strings.
	 * Instead of creating a table, and instead in a function manually appending these things
	 * seems to work around the bug. Not positive this is a compiler bug, but I am very suspicious.
	 * LGP (Saturday, January 26, 1991)
	 */
	#define	qMPW_LARGE_COMPLEX_GENERAL_STATIC_INITIALIZER_BUG				1			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 * The bug is that doing the trap cache trick for ::NewRgn in Region.cc caused cfront to generate
	 * code the C compiler wouldn't compile.
	 *
	 * (LGP Saturday, February 2, 1991)
	 *
	 *		Still Broken as of:
	 *			CFront 3.2a3 - Monday, October 14, 1991 11:53:00 AM
	 */
	#define	qMPW_NO_ARG_TRAP_CACHE_PASCAL_FUNCT_BUG							1			// BROKEN in C (3.2b6), CFront (1.1d13)  


	/*
	 * Similar to old bug I saw with MPW C in olden days. Make bitfield. Then say a->Get().f, vs.
	 * a->field.f and get bad results thru funct call.
	 *
	 * (LGP Saturday, February 25, 1991)
	 *		Could find no reference to this in code as of May 15, 1992 - LGP
	 */
	#define	qMPW_REF_TO_BITFIELD_ON_FUNC_RET_BUG							0			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 *
	 * (LGP Monday, March 4, 1991 11:24:54 AM)
	 *		Could find no reference to this in code as of May 15, 1992 - LGP
	 */
	#define	qMPW_BITFIELD_STRUCT_RET_IN_QUESTION_OP_BUG						0			// Apple CFRONT 1.0 (9/11/90; AT&T 2.0)


	/*
	 * - See OSInit.cc gPatcher
	 * (LGP Wednesday, June 12, 1991 2:33:20 AM)
	 *		Still Broken as of:
	 *			CFront 3.2b3 - Tuesday, February 11, 1992 9:36:52 PM (ETO#6)
	 */
	#define	qMPW_PRAGMA_FORCE_ACTIVE_BROKE									1


	/*
	 * 
	 *	static	const	CmdNameTablePair	kCommandNames[] = {
	 *		CmdNameTablePair (CommandHandler::eAboutApplication, "About Small"),
	 *	};
	 *
	 * Including inside a function (ie as a local variable) Caused CFront to generate bad code
	 * that the C Compiler could not compile.
	 *
	 * (LGP Monday, December 16, 1991 12:12:53 PM)
	 *
	 *		Still Broken as of:
	 *			CFront 1.1d16, C 3.2b6 - Tuesday, July 16, 1991 12:35:59 AM
	 */
	#define	qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG					1


	/*
	 * (LGP Friday, February 21, 1992 1:23:46 PM)
	 *
	 *	Really this has been broken for almost a year. Trouble is that sometimes the c compiler
	 *	barfs out saying ran of of registers, or registe r lockup, or some such nonsense. Simplifications
	 *	are always possible, but often ugly.
	 *
	 *		Originally Broken as of:
	 *			CFront 3.2b3 - Tuesday, February 11, 1992 9:36:52 PM (ETO#6)
	 */
	#define	qMPW_C_COMPILER_REGISTER_OVERFLOW_BUG							1


	/*
	 * (LGP Friday, Tuesday, September 1, 1992 5:11:06 PM)
	 *
	 *	Workaround for sloppy(inefficent) code gen from CFront under MPW ends up breaking
	 *	gcc 2.2.1. Lovely...
	 *
	 *		Originally Broken as of:
	 *			CFront 3.2 - Tuesday, February 11, 1992 9:36:52 PM (ETO#8)
	 */
	#define	qMPW_CFront_SloppyCodeGenWithoutOpNewOverloadForPlacementOp		1



#endif	/*qMPW_CFront_AppleC*/








/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&& qHPCPlus &&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */

#if		qHPCPlus


#endif	/*qHPCPlus*/






/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&& qSunCPlus &&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */

#if		qSunCPlus

#if		0
	#define	qSun_CFront_FreeBadArg											1
	#define	qSunCFront_StaticFuncsSBPlainTypeBug							1
	#define	qSunCFrontDontLikeZeroSizedStructBug							1
	#define	qSunCFront_OverloadingConfusionBug								1

	// see Test_Strings.cc
	#define		qSunCFrontCannotDeclareConstAutoArrayOfChar_Bug				1
#endif

#endif	/*qSunCPlus*/







/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&& qGCC_Aux &&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */
#if		qGCC_Aux

	/*
	 * Monday, June 22, 1992 2:57:51 PM
	 *
	 *		Originally Broken as of:
	 *			gcc            2.2.1-aux  	- June 22, 1992 2:57:51 PM
	 *
	 *		Below in Config-Foundation.hh, we get warning that LDBL_MAX and DBL_MAX are too big
	 * to fit in a double.  For now, just use some smaller constant and we should be OK.
	 */
	#define	qGCC_DblTooBigBug												1

#endif	/*qGCC_Aux*/








/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&& qCFront_Aux &&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */
#if		qCFront_Aux


#endif	/*qCFront_Aux*/








/*
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
		&&&&&&&&&&&&&&& qBorlandCPlus &&&&&&&&&&&
		&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */
#if		qBorlandCPlus
	#define	qCanFreelyUseVirtualBaseClasses									1
	#define	qPureVirtualsPropagated											1

	// Allow redefinition from compile line(Config file) since we support both,
	// and convienent to be able to test both with one compiler...
	#ifndef	qRealTemplatesAvailable
		#define	qRealTemplatesAvailable   									1
	#endif
	#define	qTemplatesHasRepository											0
	#define	qStaticDataMembersInTemplatesSupported							1
	#define	qPlacementOperatorNewProvided									0


	/*
	 * Wednesday, November 11, 1992 12:06:27 PM
	 *
	 *		Page 285 of  Stroustrup "C++: The Programming Language, Second Edition"
	 *	has as an example of template friend usage:
	 *
	 * template<class K, class V> class Map;
	 * template<class K, class V> class Mapiter;
	 * template<class K, class V> class Link {
	 *		friend	class	Map<K,V>;
	 *		...
	 *
	 * Borland C++ does not seem to accept any form of templated friend classes.
	 *
	 *		Originally Broken as of:
	 *			BorlandC++            3.1  	- Wednesday, November 11, 1992 12:06:27 PM
	 *		FIXED as of:
	 *			BorlandC++            3.1  	- Wednesday, November 11, 1992 12:06:27 PM
	 *
	 *	This may never have really been broken. The real problems appears to have
	 *	been use of inlines elsewhere causing BCC to get confused???
	 *
	 *	On second thoguht, getting rid of inlines helped alot, but there are still
	 *	a few cases where this is needed too.
	 *
	 */
	#define	qBCC_TemplateFriendsNotSoFriendlyBug							1


	/*
	 * Sunday, November 15, 1992 11:10:47 PM
	 *
	 *		Not really sure whats going on. Three days of spooky bugs, compiler
	 *	crashes, and rediculous messages, I discovered that nearly all the problems
	 *	went away if I #define inline  to nothing. Hardly a solution, but a
	 *	workaround for the time being. I can investigate whats really going
	 *	on later.
	 *
	 *		Originally Broken as of:
	 *			BorlandC++            3.1  	- Sunday, November 15, 1992 11:10:47 PM
	 *
	 */
	#define	qBCC_InlinesBreakTemplates										1


	/*
	 * Sunday, November 15, 1992 11:10:47 PM
	 *
	 *		In a conditional operator expression (?:) BCC requires that the
	 *	type of the two possible values be the same type. I believe the ARM
	 *	says the requirement SB something like unifyable to a unique common
	 *	type - anyway, it fails for two different subclasses of iterator.
	 *	Easy to work around - use ifs.
	 *
	 *		Originally Broken as of:
	 *			BorlandC++            3.1  	- Sunday, November 15, 1992 11:10:47 PM
	 *
	 */
	#define	qBCC_ConditionalOperatorDemandsTypesExactBug					1


	/*
	 * Sunday, November 15, 1992 11:10:47 PM
	 *
	 *		template for operator== in shared not autmatically expanded when used -
	 *	we get an error message saying that == op n two structs (shared(StringRep))
	 *	are not defined. Workaround is to just manually instantiate.
	 *
	 *		Originally Broken as of:
	 *			BorlandC++            3.1  	- Sunday, November 15, 1992 11:10:47 PM
	 *
	 */
	#define	qBCC_OpEqualsTemplateNotAutoExpanded							1


// Document better when understood...
#define	qBCC_LinkerPullsInAllFilesSoWeNeedAllGlobalsBug						1

#endif	/*qBorlandCPlus*/














/*
 ***********************************************************************************************
 ********************************** Compiler/System Bug Workarounds ****************************
 ***********************************************************************************************
 */

#if		!qSignedTypeImplemented
	#define	signed
#endif



/*
 * Hack so we can do single object stuff inside of macros...
 * Really we shjouldnt have to do this - apple should have used #pragma singleObject!!!
 */
#if		qMPW_CFront
	#define	INHERRIT_FROM_SINGLE_OBJECT	: private SingleObject
#else
	#define	INHERRIT_FROM_SINGLE_OBJECT
#endif	/*qMPW_CFront*/



#if		qGCC_LibgPlusPlusSharpDefineSize_tBug
	#ifdef	size_t
		typedef	size_t	size_t_unhack;
	#undef	size_t
		typedef	size_t_unhack	size_t;
	#endif
#endif	/*qGCC_LibgPlusPlusSharpDefineSize_tBug*/











/*
 ***********************************************************************************************
 *************** Compiler/System Dependent type/size/align/etc Definitions *********************
 ***********************************************************************************************
 */


/*
 * Bit/Byte orderings.
 *
 *	qLeastSignicicantBitFirst
 *		This is not detectable from C, but is important when communicating
 *	with the outside world, thru files, or setting bits in hardware (like
 *	video hardware, for example). This means that the bit numbering from the
 *	shift operator cooresponds to the bit-ordering defined by the hardware.
 *	Otherwise, the numbering is reversed.
 *
 *	qLeastSignicicantByteFirst
 *		This simply means that the lowest order byte of a word (UInt16/short)
 *	is at the lowest address. This can be easily checked from C, by putting
 *	a known bit pattern into a short, and checking if the low order byte value
 *	is gotten by taking the address of the short, casting it to a char*,
 *	and looking at it that way.
 *
 *		THERE IS POSSIBLY ANOTHER SUCH qVariable we need for machines
 *	like PDP-11 where diff between as short/long???? Talk to KDJ about how
 *	to phrase this???
 */
#if		mc68000
	#define	qLeastSignicicantBitFirst	0
	#define	qLeastSignicicantByteFirst	0
#elif	i286
	#define	qLeastSignicicantBitFirst	1
	#define	qLeastSignicicantByteFirst	1
#elif	qSparc
	// guess same as 68K...
	#define	qLeastSignicicantBitFirst	0
	#define	qLeastSignicicantByteFirst	0
#elif	qSnake
	#define	qLeastSignicicantBitFirst	0
	#define	qLeastSignicicantByteFirst	0
#else
	#error	"No Machine defined"
#endif	/*qSnake*/




/*
 * Machine independent Type Declarations.
 */




enum	Boolean { False, True };


/*
 * Machine independent Fixed Point Numeric Types.
 */
#if		mc68000 || i286 || qSparc || qSnake
	typedef	unsigned	char	UInt8;
	typedef	char				Int8;
	const	UInt8	kMinUInt8	=	0;
	const	UInt8	kMaxUInt8	=	UCHAR_MAX;
	const	Int8	kMinInt8	=	SCHAR_MIN;
	const	Int8	kMaxInt8	=	SCHAR_MAX;

	typedef	unsigned	short	UInt16;
	typedef	short				Int16;
	const	UInt16	kMinUInt16	=	0;
	const	UInt16	kMaxUInt16	=	USHRT_MAX;
	const	Int16	kMinInt16	=	SHRT_MIN;
	const	Int16	kMaxInt16	=	SHRT_MAX;

	typedef	unsigned	long	UInt32;
	typedef	long				Int32;
	const	UInt32	kMinUInt32	=	0;
	const	UInt32	kMaxUInt32	=	ULONG_MAX;
	const	Int32	kMinInt32	=	LONG_MIN;
	const	Int32	kMaxInt32	=	LONG_MAX;
#else
	#error	"No Machine Defined"
#endif

#if		mc68000 || qSparc || qSnake
	const	size_t	kMinSize_T	=	size_t (kMinUInt32);
	const	size_t	kMaxSize_T	=	size_t (kMaxUInt32);
#elif	i286
	const	size_t	kMinSize_T	=	size_t (kMinUInt16);
	const	size_t	kMaxSize_T	=	size_t (kMaxUInt16);
#endif	/* MACHINE */




#if		!qRealTemplatesAvailable
	#include	<generic.h>

	// hack so we have a name we can use in macros for void*
	// Dont use this name outside macro / template name hacks...
	typedef	void*	VoidStar;
#endif



/*
 * Alignment worries.
 * 	AlignTo(S,T)	aligns the size to the type T.
 *			Eg. AlignTo(strlen(p)+1, int) rounds up the length
 *			to be int aligned.
 *	Align(S)	aligns the size to something always safe for that machine.
 *
 * When using gcc/g++ consider usign __alignof () builtin. Also, not sure
 * if desgin of how I'm using these macros is right...
 *
 */
#define	AlignSize(S,T)			((((S) + sizeof (T) - 1)/sizeof (T)) * sizeof (T))
#if		mc68000
	#define	Align(S)			(((S)&1)? ((S)+1): (S))
#elif	i286
	#define	Align(S)			(S)
#elif	qSparc || qSnake
	#define	Align(S)			AlignSize((S),double)
#else
	#error "No alignment macro!"
#endif

#if		qHPCPlus_ConstantExpressionWithSizeofBug
	#undef	Align
	#define	Align(S)			((((S)+7)>>3)<<3)
#endif







/*
 ***********************************************************************************************
 ****************************************** Syntactic sugar ************************************
 ***********************************************************************************************
 */

#define	nonvirtual	
#define	override	virtual

#define	and	&&
#define	or	||
#define	not	(Boolean)!

/*
 * We use this Nil instead of the C NULL because NULL may be pre-defined for C to be
 * (void*)0, or 0L, which is very bad for C++ ((void*)0 not assignable to char* for example).
 */
#define	Nil	0





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Config_Foundation__*/

