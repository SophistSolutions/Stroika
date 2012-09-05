/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__TextInteractorMixins_h__
#define	__TextInteractorMixins_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/TextInteractorMixins.h,v 2.50 2003/03/20 15:52:59 lewis Exp $
 */

/*
@MODULE:	TextInteractorMixins
@DESCRIPTION:
		<p>TextInteractorMixins are some templates that help deal with some of the drugery of C++ mixins.</p>

 */


/*
 * Changes:
 *	$Log: TextInteractorMixins.h,v $
 *	Revision 2.50  2003/03/20 15:52:59  lewis
 *	SPR#1360 - added DrawBefore() support
 *	
 *	Revision 2.49  2003/03/05 17:25:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2003/01/29 19:15:11  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.47  2003/01/29 17:59:50  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.46  2002/10/14 23:24:29  lewis
 *	add Invariant() overload so it can be called easily for subclasses
 *	
 *	Revision 2.45  2002/09/19 23:38:21  lewis
 *	Cleanups and qUsingDirectiveSometimesCausesInternalCompilerErrorBug for Borland C++ compat
 *	
 *	Revision 2.44  2002/09/19 01:51:55  lewis
 *	SPR#1104- use using declaration instead of explicit overload (and had do add support for SetWindowRect)
 *	
 *	Revision 2.43  2002/05/06 21:33:39  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.42  2001/11/27 00:29:47  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.41  2001/10/20 13:38:58  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.40  2001/10/17 20:42:55  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.39  2001/09/12 17:23:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.38  2001/09/12 14:53:30  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far). Redo
 *	Led_Region support for MacOS so now more like with PC (no operator conversion to macregion
 *	and auto constructs OSRegion except when called with region arg - in whcih case we dont OWN
 *	region and dont delete it on DTOR
 *	
 *	Revision 2.37  2001/08/29 23:01:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.36  2001/08/28 18:43:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2000/09/08 00:27:39  lewis
 *	SPR#0836 - HookGained/LoseNewTextStore()
 *	
 *	Revision 2.34  2000/04/15 14:32:36  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.33  2000/04/14 22:40:23  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.32  1999/11/29 18:57:09  lewis
 *	overload ScrollSoShowing() to disambigutate mixin for that as well
 *	
 *	Revision 2.31  1999/11/13 16:32:20  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.30  1999/07/12 21:56:19  lewis
 *	add extra scope specifier (TextInteractor::) needed by new mwerks CW5Pro compiler
 *	
 *	Revision 2.29  1999/05/03 22:05:04  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.28  1999/03/06 02:42:15  lewis
 *	add ScrollByIfRoom () to InteractorImagerMixinHelper. Even though not needed by
 *	Led as-is, if you create a class Led_MFC_OptionalWin32SDKMessageMimicHelper
 *	<Led_MFC_MimicMFCAPIHelper<Led_MFC_X<SimpleTextInteractor> > >, you'll get errors
 *	with ambigous method reference in instantialtion. This ambiguity is best cleaned up.
 *	
 *	Revision 2.27  1998/10/30 14:18:12  lewis
 *	new MSVC60 warning support
 *	
 *	Revision 2.26  1998/03/04  20:16:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.25  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.24  1997/09/29  14:54:05  lewis
 *	new template InteractorInteractorMixinHelper.
 *	Revised InteractorImagerMixinHelper<IMAGER> to react to changes in TextImagers
 *	with how we deal with adding UpdateMode
 *	arg to virtuals. Much simpler now (using TmpUpdateMode stuff).
 *
 *	Revision 2.23  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.22  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.21  1997/07/12  20:09:56  lewis
 *	Renamed TextInteractor_ to TextInterctor, etc...
 *	AutoDoc support.
 *
 *	Revision 2.20  1997/06/23  16:12:36  lewis
 *	lose unneeded includ
 *
 *	Revision 2.19  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1997/06/18  02:38:11  lewis
 *	Lose bug workaroudns.
 *	ANd moved StandardStyledWordWrappedTextInteractor, etc, into their own modules.
 *
 *	Revision 2.17  1997/01/20  06:09:47  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1997/01/20  05:22:19  lewis
 *	Mostly just use new eDefaultUpdate UpdateMode instead of eDelayedUpdate.
 *
 *	Revision 2.15  1997/01/10  02:55:37  lewis
 *	Renamed from WordWrappedTextInteractor.
 *	Almost TOTALLY redone using templates to manage the mixins (so we can more easily
 *	and tersely accomodate more mixin images, like new non-wordwrapping ones).
 *	Also, a whole bunch of cleanups to the way the mixins work and what overrides were
 *	really needed. And lots of reworking of scrolling APIs.
 *
 *	Revision 2.14  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.13  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/09/30  14:16:34  lewis
 *	qSupportWholeWindowInvalidOptimization support
 *
 *	Revision 2.11  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.10  1996/05/23  19:31:04  lewis
 *	Led_IncrementalFontSpecification instead of old FontSpecification.
 *
 *	Revision 2.9  1996/05/14  20:23:43  lewis
 *	override HookLosingTextStore/HookGainedNewTextStore to mixin.
 *
 *	Revision 2.8  1996/02/26  18:48:08  lewis
 *	Mostly renamed TextInteracter->TextInterteractor.
 *	Override SetMarkerPositionOfStartOfWindowIfRoom so it can take updatemode
 *	override AboutToUpdateText () to disambiguate base classes (now that base overrides).
 *
 *	Revision 2.7  1996/02/05  04:13:48  lewis
 *	CTOR no longer takes (TextStore_*.
 *	Moved a bunch of stuff to new class StandardStyledTextInteractor.
 *
 *	Revision 2.6  1996/01/22  05:14:20  lewis
 *	Override DidUpdateText ()
 *
 *	Revision 2.5  1995/11/25  00:22:21  lewis
 *	Add support for pasting embedded objects to standardimager mixin. Not
 *	happy about where things are now. Should rethink/re-organize.
 *
 *	Revision 2.4  1995/11/04  23:11:14  lewis
 *	No longer need overrides of MeasureSegmentXXX, CalcSegXX, and DrawSegment in
 *	StyledWordWrappedTExtInteractor...
 *
 *	Revision 2.3  1995/11/02  22:22:15  lewis
 *	Added SharedStyleDatabase* support, and override DidUpdateText
 *	for mixin.
 *
 *	Revision 2.2  1995/10/19  22:11:14  lewis
 *	Added StandardStyledWordWrappedTextInteractor.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.5  1995/06/08  05:11:13  lewis
 *	Code cleanups
 *
 *	Revision 1.4  1995/06/05  21:27:33  lewis
 *	Lose override of SelectWholeLineAfter - See SPR#0320.
 *
 *	Revision 1.3  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.2  1995/05/21  17:02:05  lewis
 *	Got rid of qLotsOfSubclassesFromMixinPointYeildTemporarilyBadVTable since
 *	it must be done in classes decending from concrete TextStore.
 *
 *	Revision 1.1  1995/05/20  04:50:56  lewis
 *	Initial revision
 *
 *
 *
 */
#include	"TextInteractor.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)		//qQuiteAnnoyingDominanceWarnings
#endif


/*
@CLASS:			InteractorImagerMixinHelper<IMAGER>
@BASES:			virtual @'TextInteractor', IMAGER
@DESCRIPTION:	<p>A utility class to facilitate mixing together Interactors with particular
			chosen TextImager subclasses.</p>
				<p>This class is mostly an implementation detail, and shouldn't be of interest to Led users, unless
			they are creating a new TextImager subclass (which isn't also already a subclass of TextInteractor;
			this should be rare).</p>
*/
template	<typename	IMAGER>	class	InteractorImagerMixinHelper : public virtual TextInteractor, public IMAGER {
	private:
		typedef	void*	inherited;	// prevent accidental references to this name in subclasses to base class name

	protected:
		InteractorImagerMixinHelper ();

	/*
	 *	Must combine behaviors of different mixins.
	 */
	public:
		override	void	Draw (const Led_Rect& subsetToDraw, bool printing);
		override	void	AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo);
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();
	protected:
		override	void	HookLosingTextStore ();
		override	void	HookGainedNewTextStore ();

	/*
	 *	Disambiguate overloads than can happen down both base class chains.
	 */
	public:
		#if		qUsingDirectiveSometimesCausesInternalCompilerErrorBug
			nonvirtual	void	SetWindowRect (const Led_Rect& windowRect, TextInteractor::UpdateMode updateMode = TextInteractor::eDefaultUpdate)
				{
					TextInteractor::SetWindowRect (windowRect, updateMode);
				}
			nonvirtual	void	ScrollByIfRoom (long downByRows, UpdateMode updateMode = eDefaultUpdate)
				{
					TextInteractor::ScrollByIfRoom (downByRows, updateMode);
				}
			nonvirtual	void	ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0, UpdateMode updateMode = eDefaultUpdate)
				{
					TextInteractor::ScrollSoShowing (markerPos, andTryToShowMarkerPos, updateMode);
				}
		#else
			using	TextInteractor::SetWindowRect;
			using	TextInteractor::ScrollByIfRoom;
			using	TextInteractor::ScrollSoShowing;
		#endif
};







/*
@CLASS:			InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>
@BASES:			INTERACTOR1, INTERACTOR2
@DESCRIPTION:	<p>A utility class to facilitate mixing together two Interactors.</p>
				<p>This class is mostly an implementation detail, and shouldn't be of interest to Led users, unless
			they are creating a new direct TextImagerInteractor subclass (which should be rare).</p>
*/
template	<typename	INTERACTOR1, typename INTERACTOR2>	class	InteractorInteractorMixinHelper : public INTERACTOR1, public INTERACTOR2 {
	private:
		typedef	void*	inherited;	// prevent accidental references to this name in subclasses to base class name

	protected:
		InteractorInteractorMixinHelper ();

	protected:
		override	void	HookLosingTextStore ();
		override	void	HookGainedNewTextStore ();

	public:
		#if		qUsingDirectiveSometimesCausesInternalCompilerErrorBug
			nonvirtual	void	SetWindowRect (const Led_Rect& windowRect, TextInteractor::UpdateMode updateMode = TextInteractor::eDefaultUpdate)
				{
					TextInteractor::SetWindowRect (windowRect, updateMode);
				}
		#else
			using	INTERACTOR1::SetWindowRect;	// Should both be the same - but arbitrarily pick one
		#endif

		nonvirtual	void	Invariant ()
			{
				INTERACTOR1::Invariant ();
				INTERACTOR2::Invariant ();
			}

	public:
		override	void	DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ();

};








/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	InteractorImagerMixinHelper<IMAGER>
	template	<typename	IMAGER>
		InteractorImagerMixinHelper<IMAGER>::InteractorImagerMixinHelper ():
			TextInteractor (),
			IMAGER ()
		{
		}
	template	<typename	IMAGER>
		void	InteractorImagerMixinHelper<IMAGER>::Draw (const Led_Rect& subsetToDraw, bool printing)
			{
				DrawBefore (subsetToDraw, printing);
				TextInteractor::Draw (subsetToDraw, printing);
				IMAGER::Draw (subsetToDraw, printing);
				DrawAfter (subsetToDraw, printing);
			}
	template	<typename	IMAGER>
		void	InteractorImagerMixinHelper<IMAGER>::AboutToUpdateText (const UpdateInfo& updateInfo)
			{
				IMAGER::AboutToUpdateText (updateInfo);
				TextInteractor::AboutToUpdateText (updateInfo);
			}
	template	<typename	IMAGER>
		void	InteractorImagerMixinHelper<IMAGER>::DidUpdateText (const UpdateInfo& updateInfo) throw ()
			{
				IMAGER::DidUpdateText (updateInfo);
				TextInteractor::DidUpdateText (updateInfo);
			}
	template	<typename	IMAGER>
		void	InteractorImagerMixinHelper<IMAGER>::HookLosingTextStore ()
			{
				/*
				 *	NB: See SPR#0836 - order of Imager/Interactor reversed compared to HookGainedNewTextStore () intentionally (mimic CTOR/DTOR ordering).
				 *	Now DO imager first, then interactor in GAINING text store, and - as always - reverse that order for the LosingTextStore code.
				 */
				TextInteractor::HookLosingTextStore ();
				IMAGER::HookLosingTextStore ();
			}
	template	<typename	IMAGER>
		void	InteractorImagerMixinHelper<IMAGER>::HookGainedNewTextStore ()
			{
				IMAGER::HookGainedNewTextStore ();
				TextInteractor::HookGainedNewTextStore ();
			}





//	class	InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>
	template	<typename	INTERACTOR1, typename INTERACTOR2>
		InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>::InteractorInteractorMixinHelper ():
			INTERACTOR1 (),
			INTERACTOR2 ()
			{
			}
	template	<typename	INTERACTOR1, typename INTERACTOR2>
		void	InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>::HookLosingTextStore ()
			{
				INTERACTOR1::HookLosingTextStore ();
				INTERACTOR2::HookLosingTextStore ();
			}
	template	<typename	INTERACTOR1, typename INTERACTOR2>
		void	InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>::HookGainedNewTextStore ()
			{
				INTERACTOR1::HookGainedNewTextStore ();
				INTERACTOR2::HookGainedNewTextStore ();
			}
	template	<typename	INTERACTOR1, typename INTERACTOR2>
		void	InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ()
			{
				INTERACTOR1::DidUpdateText (updateInfo);
				INTERACTOR2::DidUpdateText (updateInfo);
			}



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif


#if		qLedUsesNamespaces
}
#endif



#endif	/*__TextInteractorMixins_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
