/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItView__
#define	__LedLineItView__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Headers/LedLineItView.h,v 2.22 2003/04/04 14:47:50 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItView.h,v $
 *	Revision 2.22  2003/04/04 14:47:50  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.21  2003/04/03 16:54:37  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command classes, just builtin to TextInteractor/WordProcessor (and instead of template params use new TextInteractor/WordProcessor::DialogSupport etc)
 *	
 *	Revision 2.20  2002/05/06 21:31:27  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.19  2001/11/27 00:28:27  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.18  2001/08/30 01:01:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.17  2001/07/19 21:06:20  lewis
 *	SPR#0960- CW6Pro support
 *	
 *	Revision 2.16  2000/10/19 15:51:54  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 2.15  2000/10/03 21:52:01  lewis
 *	Lots more work relating to SPR#0839. Eliminated a little duplicated code (undo stuff
 *	now handled genericly via TextInteractorCommandHelper<> template
 *	
 *	Revision 2.14  1999/12/25 00:08:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.13  1999/12/24 23:46:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  1999/12/24 23:23:53  lewis
 *	add PreLim (qSupportSyntaxColoring) support.
 *	
 *	Revision 2.11  1999/12/21 21:05:49  lewis
 *	react to change in OnTypedNormalCharacter API
 *	
 *	Revision 2.10  1998/10/30 15:09:55  lewis
 *	use mutable - instead of const_cast.
 *	
 *	Revision 2.9  1998/03/04  20:31:54  lewis
 *	override DidUpdateText()
 *
 *	Revision 2.8  1997/12/24  04:52:10  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/12/24  04:00:09  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1997/07/27  16:03:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.5  1997/06/24  03:39:25  lewis
 *	No more DynamiclyAdjustLayoutWidth () - instead override ComputeMaxHScrollPos ()
 *	and keep fLayoutWidth; ourselves (see spr#0450).
 *
 *	Revision 2.4  1997/06/18  03:34:51  lewis
 *	new tabstop Led changes support.
 *	ShiftLeft/ShiftRight commands
 *
 *	Revision 2.3  1997/03/23  00:57:12  lewis
 *	Lose syntax coloring stuff
 *
 *	Revision 2.2  1997/03/04  20:27:02  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/01/20  05:45:18  lewis
 *	overridUpdateScrollbars() to adjust hscrollbar as we do vscrolling
 *	override TabletChangedMetrics () to adjust tabstops (measured in pixels) as font changes.
 *
 *	Revision 2.0  1997/01/10  03:51:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#include	"LedLineItConfig.h"

#include	"Led_PP.h"
#include	"SimpleTextInteractor.h"
#include	"SyntaxColoring.h"




#if		qSupportSyntaxColoring
	struct	BASECLASS : public Led_PPView_X<SimpleTextInteractor>, public StyledTextImager {\
		private:
			typedef	Led_PPView_X<SimpleTextInteractor>	inherited;
		protected:
			override	Led_Distance	MeasureSegmentHeight (size_t from, size_t to) const
				{
					return inherited::MeasureSegmentHeight (from, to);
				}
			override	Led_Distance	MeasureSegmentBaseLine (size_t from, size_t to) const
				{
					return inherited::MeasureSegmentBaseLine (from, to);
				}
	};
#else
	typedef	Led_PPView_X<SimpleTextInteractor>	BASECLASS;
#endif



class	LedLineItView : public BASECLASS {
	private:
		typedef	BASECLASS	inherited;

	public:
	 	LedLineItView ();
	 	~LedLineItView ();

#if		qSupportSyntaxColoring
	protected:
		override	void	HookLosingTextStore ();
		override	void	HookGainedNewTextStore ();
#endif
	
	public:
		override	void	UpdateScrollBars ();

	public:
		override	Led_Distance	ComputeMaxHScrollPos () const;
	private:
		mutable	Led_Distance	fLayoutWidth;

	public:
		override	void	OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

	protected:
		override	const TabStopList&	GetTabStopList (size_t containingPos) const;
	protected:
		override	void	TabletChangedMetrics ();
	private:
		SimpleTabStopList	fTabStopList;

	public:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	public:
		override	void	FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName);
		override	Boolean	ObeyCommand (CommandT inCommand, void* ioParam);
	protected:
		nonvirtual	bool	FindCommandStatus_Font (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName);
		nonvirtual	bool	ObeyCommand_Font (CommandT inCommand, void* ioParam);
	private:
		nonvirtual	Led_Distance	PickOtherFontHeight (Led_Distance origHeight);	// zero means default(in)/unchanged(out)

	public:
		nonvirtual	void	OnGotoLineCommand ();
		nonvirtual	void	OnShiftLeftCommand ();
		nonvirtual	void	OnShiftRightCommand ();
		nonvirtual	void	OnShiftNCommand (bool shiftRight);

	protected:
		override	SearchParameters	GetSearchParameters () const;
		override	void				SetSearchParameters (const SearchParameters& sp);

#if		qSupportSyntaxColoring
	private:
		#if		qSupportOnlyMarkersWhichOverlapVisibleRegion
			WindowedSyntaxColoringMarkerOwner*	fSyntaxColoringMarkerOwner;
		#else
			SimpleSyntaxColoringMarkerOwner*	fSyntaxColoringMarkerOwner;
		#endif
#endif
};

#endif	/*__LedLineItView__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


