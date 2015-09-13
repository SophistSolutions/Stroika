/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_PP_h__
#define	__Led_PP_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_PP.h,v 2.60 2003/05/15 12:50:28 lewis Exp $
 */


/*
@MODULE:	Led_PP
@DESCRIPTION:
		<p>Led_PP.</p>
 */


/*
 * Changes:
 *	$Log: Led_PP.h,v $
 *	Revision 2.60  2003/05/15 12:50:28  lewis
 *	SPR#1487: Add a 'GetEnabled' method to TextInteractor::CommandUpdater and subclasses (to help with SPR)
 *	
 *	Revision 2.59  2003/04/04 14:41:42  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.58  2003/04/03 22:49:09  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly
 *	MacOS/Linux now)
 *	
 *	Revision 2.57  2003/04/03 21:52:45  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command#
 *	processing support
 *	
 *	Revision 2.56  2003/04/03 16:41:23  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using
 *	templated command classes, just builtin to TextInteractor/WordProcessor (and instead
 *	of template params use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.55  2003/01/29 19:15:07  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.54  2002/05/06 21:33:29  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.53  2001/11/27 00:29:42  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.52  2001/10/17 21:46:41  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.51  2001/09/26 15:41:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.50  2001/09/11 22:29:14  lewis
 *	SPR#1018- preliminary CWPro7 support
 *	
 *	Revision 2.49  2001/08/28 18:43:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2001/07/19 02:21:45  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.47  2001/04/27 15:23:02  lewis
 *	doc comments
 *	
 *	Revision 2.46  2000/10/03 21:50:19  lewis
 *	Lots more work relating to SPR#0839- moved stuff into WordProcessorCommonCommandHelper<> template,
 *	and used that better throughout Led_PP code etc. Broke out WP-specific stuff from Led_PP to
 *	separate file.Lots of cleanups of PP wrapper code
 *	
 *	Revision 2.45  2000/09/30 19:35:01  lewis
 *	Added TextInteractorCommandHelper_DefaultCmdInfo/TextInteractorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>.
 *	Added TextInteractor::OnSelectAllCommand. Moved Led_MFC_TmpCmdUpdater to Led_MFC.h
 *	
 *	Revision 2.44  2000/09/25 21:57:43  lewis
 *	Moved most of the draw/layout/scrollbar and (except D&D) mouse handling code from Led_PP to Led_MacOS
 *	
 *	Revision 2.43  2000/09/25 00:43:33  lewis
 *	Move alot of code from Led_PPView to Led_MacOS_Helper<> template (key handling, tablet
 *	handling). Soon will move alot more
 *	
 *	Revision 2.42  2000/06/15 20:02:59  lewis
 *	SPR#0785
 *	
 *	Revision 2.41  2000/06/12 16:19:07  lewis
 *	support SPR#0780- Led_Tablet_ support
 *	
 *	Revision 2.40  2000/04/16 14:39:36  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.39  2000/04/16 13:58:54  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.38  2000/04/15 14:32:34  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.37  2000/04/14 22:40:20  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.36  2000/04/04 20:49:56  lewis
 *	SPR# 0732- added  Led_Set_OutOfMemoryException_Handler/Led_Set_BadFormatDataException_Handler/
 *	Led_Set_ThrowOSErrException_Handler. And qGenerateStandardPowerPlantExceptions and
 *	qGenerateStandardMFCExceptions defines. The idea is to make it easier to override these
 *	exception throw formats, and get better default behavior for cases other than MFC and PowerPlant.
 *	
 *	Revision 2.35  1999/12/27 23:23:38  lewis
 *	fix some small mac-compatabilty problems with new show/hide invisibles code (SPR#0669)
 *	
 *	Revision 2.34  1999/12/27 17:31:35  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is autohandled
 *	by WordProcessorCommonCommandHelper<> template. And adjusted the characters output (at least for UNICODE).
 *	
 *	Revision 2.33  1999/12/27 16:28:35  lewis
 *	renamed WordProcessorSelectionCommandHelper<> -> WordProcessorCommonCommandHelper<>
 *	
 *	Revision 2.32  1999/12/09 03:20:29  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support
 *	
 *	Revision 2.31  1999/12/08 19:40:21  lewis
 *	finish WordProcessorSelectionCommandHelper_PP(X)<> template. Added ObeyCommand().
 *	
 *	Revision 2.30  1999/12/08 17:39:16  lewis
 *	added  WordProcessorSelectionCommandHelper_PPX<BASECLASS,CMD_INFO,CMD_ENABLER = Led_PP_TmpCmdUpdater> and
 *	WordProcessorSelectionCommandHelper_PP<BASECLASS,CMD_INFO,CMD_ENABLER>
 *	
 *	Revision 2.29  1999/12/07 22:37:30  lewis
 *	add Led_PP_TmpCmdUpdater
 *	
 *	Revision 2.28  1999/11/13 16:32:18  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.27  1999/06/28 14:59:22  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.26  1999/05/03 22:04:55  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.25  1999/04/15 20:34:10  lewis
 *	wrap textAttr_XXX defines with ifndef _H_UTextEdit- tmphack - really should find better org
 *	
 *	Revision 2.24  1999/02/21 21:49:44  lewis
 *	Always keep fAcquireCount, and check if NULL GetMacPort(), and if so, throw NoTabletAvialable,
 *	rather than silently using bad tablet
 *	
 *	Revision 2.23  1998/05/05 00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *	
 *	Revision 2.22  1998/03/04  20:18:18  lewis
 *	*** empty log message ***
 *
 *	Revision 2.21  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/12/24  03:25:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1997/09/29  14:32:27  lewis
 *	Lose qSupportLed21CompatAPI.
 *
 *	Revision 2.18  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.17  1997/07/23  23:03:24  lewis
 *	fixed qSupportLed21CompatAPI support
 *
 *	Revision 2.16  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.15  1997/07/12  20:00:14  lewis
 *	Led_PPView_ renamed Led_PPView.
 *	AutoDoc support.
 *	Command names now strings.
 *
 *	Revision 2.14  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1997/06/18  02:42:27  lewis
 *	Cleanups.
 *
 *	Revision 2.12  1997/03/22  13:49:21  lewis
 *	Use WhileTrackingConstrainSelection () hook instead of old peicemail dbl-click tracking code.
 *
 *	Revision 2.11  1997/03/04  20:04:10  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/01/10  03:03:59  lewis
 *	Major reworking of mixins. Now use templated Led_PPView_X<> to combine iteractor mixins with
 *	a class library wrapper. Makes much easier to support Led_PP together with all the various
 *	cominations of interactors/imagers you now can use.
 *	Old pre-mixed classes supported via qSupportLed21CompatAPI.
 *	Also, new scrolling support. New names, reorganized. Most of the old names still
 *	available via qSupportLed21CompatAPI.
 *	Now horizontal scrolling (partiually) supported.
 *
 *	Revision 2.9  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.8  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1996/09/30  14:14:14  lewis
 *	Lose AdjustBounds() and instead redid the UpdateScrollBars() code, etc...
 *
 *	Revision 2.6  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.5  1996/05/23  19:21:14  lewis
 *	No longer override FocusDraw().
 *	Override HilgihtDropArea() to take into account scrollbar.
 *
 *	Revision 2.4  1996/05/04  19:19:01  lewis
 *	cleanups, and override Refresh to disambiguate names
 *
 *	Revision 2.3  1996/04/18  15:04:12  lewis
 *	drag and drop command name support.
 *
 *	Revision 2.2  1996/03/16  18:38:21  lewis
 *	Added HandleCursorKeyPress () helper function.
 *
 *	Revision 2.1  1996/03/05  18:23:44  lewis
 *	Broken Led_PP into  Led_PPView_ and StandardStyledWordWrappedLed_PPView
 *	and SimpleLed_PPView ().
 *	Support drag drop.
 *
 *
 *
 *
 */
#include	<LView.h>
#include	<LCommander.h>
#include	<LPeriodical.h>
#include	<LDragAndDrop.h>


#include	"LedSupport.h"


#include	"TextInteractor.h"

#include	"Led_MacOS.h"


#if		qLedUsesNamespaces
namespace	Led {
#endif





#ifndef	_H_UTextEdit
// Must get rid of these to avoid REDECLARE ERROR WITH LTextEdit.h!!!
enum {
	textAttr_MultiStyle		= 0x8000,
	textAttr_Editable		= 0x4000,
	textAttr_Selectable		= 0x2000,
	textAttr_WordWrap		= 0x1000
};
#endif



/*
 *	Note, you can save a small amount of time/space by shutting this compatability switch off.
 *	Turn it on to make life a little easier for people who have PP applications already
 *	using LTextEdit to switch over.
 */
#ifndef	qMimicLTextEditAPI
	#define	qMimicLTextEditAPI	1
#endif






/*
@CONFIGVAR:		qGenerateStandardPowerPlantExceptions
@DESCRIPTION:	
		<p>PowerPlant apps generally throw class LException (used to be ExceptionCode) etc for internal exceptions.
	And there are standard PowerPlant catchers for these types. To make Led utilize the @'Led_Set_OutOfMemoryException_Handler'
	etc mechanism to use the MFC exception types - 	define this to TRUE. To use more standard Standard C++ or your own types -
	you may wish to shut this off.</p>
		<p>Defaults to ON.</p>
 */
#ifndef	qGenerateStandardPowerPlantExceptions
#define	qGenerateStandardPowerPlantExceptions						1
#endif




/*
@CLASS:			Led_PPView
@BASES:			LView, LCommander, LPeriodical, LDragAndDrop, @'Led_MacOS_Helper<BASE_INTERACTOR>'
@DESCRIPTION:
		<p>PowerPlant wrapper class. This provides the basic PowerPlant wrapper support, regardless of the sort of
	TextImager which is being used. See Led_PPView_X&ltChosenInteractor,Traits&gt for more details.</p>
*/
class Led_PPView : public LView, public LCommander, public LPeriodical, public LDragAndDrop, public Led_MacOS_Helper<> {
	protected:
		Led_PPView ();
		Led_PPView (const SPaneInfo& inPaneInfo, const SViewInfo& inViewInfo, UInt16 inTextAttributes, ResIDT inTextTraitsID);
		Led_PPView (LStream *inStream);
	public:
		virtual ~Led_PPView ();

	private:
		typedef	class	_Ambiguous_	inherited;	// don't accidentally refer to some base-base class. Note that this is ambiguous!


	public:
		override	void	SetupCurrentGrafPort () const;

	public:
		override	void	FinishCreate ();



	// This class builds commands with command names. The UI may wish to change these
	// names (eg. to customize for particular languages, etc)
	// Just patch these strings here, and commands will be created with these names.
	// (These names appear in text of undo menu item)
	public:
		static	const string&	GetDragCommandName ();
		static	void			SetDragCommandName (const string& cmdName);
		static	const string&	GetDropCommandName ();
		static	void			SetDropCommandName (const string& cmdName);
		static	const string&	GetDragNDropCommandName ();
		static	void			SetDragNDropCommandName (const string& cmdName);
	private:
		static	string	sDragCommandName;
		static	string	sDropCommandName;
		static	string	sDragNDropCommandName;



	//TextInteractor overrides
	public:
		override	void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const;

	public:
		override	void	Refresh ();	// disambiguate PowerPlant LView mixin


	// Compat with LTextEdit API - mimic api to make moving code over easier...
	#if		qMimicLTextEditAPI
	public:
		nonvirtual	void		SetTextHandle (Handle inTextH);
		nonvirtual	void		SetTextPtr (Ptr inTextP, SInt32 inTextLen);

	public:
		nonvirtual	void		SetTextTraitsID (ResIDT inTextTraitsID);
		nonvirtual	Boolean		HasAttribute (UInt16 inAttribute);

		// Actually PP doesn't have these methods, but if you will have these attributes, its hard to see why
		// you shouldn't want to be able to change them...
		nonvirtual	void	AddAttributes (UInt16 attributesToAdd);
		nonvirtual	void	SubtractAttributes (UInt16 attributesToSubtract);

	public:
		override	void	DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ();
		virtual		void	UserChangedText ();
	#endif
	
	public:
		override	Boolean		ObeyCommand (CommandT inCommand, void *ioParam);
		override	void		FindCommandStatus (CommandT inCommand, Boolean &outEnabled, Boolean &outUsesMark, UInt16& outMark, Str255 outName);
		override	void		SpendTime (const EventRecord &inMacEvent);
		override	Boolean		HandleKeyPress (const EventRecord& inKeyEvent);

	public:
		override	void	ResizeFrameBy (SInt16 inWidthDelta, SInt16 inHeightDelta, Boolean inRefresh);
		
		override	void	MoveBy (SInt32 inHorizDelta, SInt32 inVertDelta, Boolean inRefresh);
		override	void	ScrollImageBy (SInt32 inLeftDelta, SInt32 inTopDelta, Boolean inRefresh);

		override	void	SelectAll ();
		
		override	void	SavePlace (LStream* outPlace);
		override	void	RestorePlace (LStream* inPlace);

	/*
	 *	Hook into PowerPlant printing support.
	 */
	protected:
		override	void	CountPanels (UInt32& outHorizPanels, UInt32& outVertPanels);
		override	void	PrintPanelSelf (const PanelSpec	&inPanel);


	/*
	 *	Mouse clicks.
	 */
	public:
		override	void	ClickSelf (const SMouseDownEvent &inMouseDown);
	protected:
		virtual		bool	IsADragSelect (const SMouseDownEvent& inMouseDown) const;
		virtual		void	HandleDragSelect (const SMouseDownEvent& inMouseDown);


	/*
	 *	Drag And Drop support.
	 */
	public:
		override	Boolean	ItemIsAcceptable (DragReference inDragRef, ItemReference inItemRef);
		override	void	ReceiveDragItem (DragReference inDragRef, DragAttributes	inDragAttrs, ItemReference inItemRef, Rect& inItemBounds);
		override	void	EnterDropArea (DragReference inDragRef, Boolean inDragHasLeftSender);
		override	void	LeaveDropArea (DragReference inDragRef);
		override	void	InsideDropArea (DragReference inDragRef);
		override	void	HiliteDropArea (DragReference inDragRef);


	protected:
		ResIDT				mTextTraitsID;
		UInt16				mTextAttributes;
	
		override	void		DrawSelf ();
		override	void		HideSelf ();

#if		__PowerPlant__ >= 0x02208000
		// PowerPlant from CW7Pro
		override	void		AdjustMouseSelf (Point inPortPt, const EventRecord &inMacEvent, RgnHandle outMouseRgn);
#else
		override	void		AdjustCursorSelf (Point inPortPt, const EventRecord &inMacEvent);
#endif
		
		override	void		BeTarget ();
		override	void		DontBeTarget ();

	private:
		nonvirtual	void	InitTextEdit (ResIDT inTextTraitsID);
};






/*
@CLASS:			Led_PPView_Traits_Default
@DESCRIPTION:
		<p>Specifier class, used as default value with @'Led_PPView_X' template.</p>
*/
struct	Led_PPView_Traits_Default {
	enum { class_ID = 0 };
};
/*
@CLASS:			Led_PPView_X
@BASES:			ChosenInteractor, @'Led_PPView'
@DESCRIPTION:
		<p>Utility template to mix together Led_PPView an a <code>ChosenInteractor</code> which already has support
	for a particular TextImager mixed in.</p>
*/
template	<typename	ChosenInteractor, typename Traits = Led_PPView_Traits_Default>	class	Led_PPView_X :
	public ChosenInteractor, public Led_PPView
{
	public:
		enum { class_ID = Traits::class_ID };

		static Led_PPView_X*	CreateLedPPViewStream (LStream* inStream);

		Led_PPView_X ();
		Led_PPView_X (const SPaneInfo& inPaneInfo, const SViewInfo& inViewInfo, UInt16 inTextAttributes, ResIDT inTextTraitsID);
		Led_PPView_X (LStream* inStream);

	// Disambiguate conflicting method overrides/names
	public:
		#if		qMimicLTextEditAPI
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();
		#endif
};




/*
@CLASS:			PP_CommandNumberMapping
@DESCRIPTION:	<p></p>
*/
class	PP_CommandNumberMapping : public CommandNumberMapping<CommandT> {
	protected:
		PP_CommandNumberMapping ();
};




/*
@CLASS:			Led_PP_TmpCmdUpdater
@DESCRIPTION:
	<p>Helper class, used with @'Led_PPView'.</p>
*/
class	Led_PP_TmpCmdUpdater : public TextInteractor::CommandUpdater {
	public:
		Led_PP_TmpCmdUpdater (CommandNumber inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, unsigned char* outName);

	public:
		override	CommandNumber	GetCmdID () const;
		override	bool			GetEnabled () const;
		override	void			SetEnabled (bool enabled);
		override	void			SetChecked (bool checked);
		override	void			SetText (const Led_SDK_Char* text);

	private:
		CommandNumber 	fCommand;
		Boolean& 		fEnabled;
		Boolean& 		fUsesMark;
		UInt16&			fMark;
		unsigned char*	fName;
};










/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	Led_PPView_X
	template	<typename	ChosenInteractor, typename Traits>
		Led_PPView_X<ChosenInteractor,Traits>*	Led_PPView_X<ChosenInteractor,Traits>::CreateLedPPViewStream (LStream* inStream)
			{
				return (new Led_PPView_X<ChosenInteractor,Traits> (inStream));
			}

	template	<typename	ChosenInteractor, typename Traits>
		Led_PPView_X<ChosenInteractor,Traits>::Led_PPView_X ():
			ChosenInteractor (),
			Led_PPView ()
			{
			}

	template	<typename	ChosenInteractor, typename Traits>
		Led_PPView_X<ChosenInteractor,Traits>::Led_PPView_X (const SPaneInfo& inPaneInfo, const SViewInfo& inViewInfo, UInt16 inTextAttributes, ResIDT inTextTraitsID):
			ChosenInteractor (),
			Led_PPView ()
			{
			}

	template	<typename	ChosenInteractor, typename Traits>
		Led_PPView_X<ChosenInteractor,Traits>::Led_PPView_X (LStream* inStream):
			ChosenInteractor (),
			Led_PPView (inStream)
			{
			}
	#if		qMimicLTextEditAPI
	template	<typename	ChosenInteractor, typename Traits>
		void	Led_PPView_X<ChosenInteractor,Traits>::DidUpdateText (const UpdateInfo& updateInfo) throw ()
			{
				ChosenInteractor::DidUpdateText (updateInfo);
				UserChangedText ();
			}
	#endif





//	class	PP_CommandNumberMapping
	inline	PP_CommandNumberMapping::PP_CommandNumberMapping ()
		{
		}





//	class	Led_PP_TmpCmdUpdater
	inline	Led_PP_TmpCmdUpdater::Led_PP_TmpCmdUpdater (CommandNumber inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, unsigned char* outName):
				fCommand (inCommand),
				fEnabled (outEnabled),
				fUsesMark (outUsesMark),
				fMark (outMark),
				fName (outName)
			{
			}





#if		qLedUsesNamespaces
}
#endif


#endif	/*__Led_PP_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

