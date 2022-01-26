/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PushButton__
#define	__PushButton__

/*
 * $Header: /fuji/lewis/RCS/PushButton.hh,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: PushButton.hh,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:20:01  sterling
 *		default to portable version on mac as bug work-around
 *
 *		Revision 1.3  1992/07/08  02:17:28  lewis
 *		Renamed GUI->UI for all the classes here - added new PushButton_MacUI,
 *		PushButton_MotifUI - which picks the concrete UI class, and then PushButton
 *		comes off of that. New experimantal style - If this goes well, convert the
 *		others.
 *
 *		Revision 1.2  1992/06/25  06:21:37  sterling
 *		Mix in FocusItem as appropriate instead of assuming we get it from Button.
 *		Renamed CalcDefaultSize to CalcDefaultSize_ (+more?).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.30  1992/06/09  15:01:59  sterling
 *		fixed motif portable pushbutton
 *
 *		Revision 1.29  92/05/13  02:07:50  02:07:50  lewis (Lewis Pringle)
 *		STERL: overload SetLabel () to take const char* arg.
 *		Get rid of default arg, and overload more CTOR for PushButton so we can specify no args.
 *		
 *		Revision 1.28  92/04/17  01:17:55  01:17:55  lewis (Lewis Pringle)
 *		Changed how motif native pushbnuttons handles clicks - have the callback just set flag,
 *		and when trackpress returns, then call seton/setoff.
 *		
 *		Revision 1.27  92/04/16  16:58:55  16:58:55  lewis (Lewis Pringle)
 *		Qucikie first cut at portable motif pushbutton - I was so depressed about how bad the widget code was
 *		and how hard it was going to be to get clipping done, that I needed a diversion!
 *		
 *		Revision 1.26  92/04/14  07:34:01  07:34:01  lewis (Lewis Pringle)
 *		Added out of line virtual dtor for PushButton conditioned on qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.25  92/03/26  18:34:29  18:34:29  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.24  1992/03/26  15:17:28  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		oldFocused first argument.
 *
 *		Revision 1.22  1992/03/17  17:03:42  lewis
 *		Override EffectiveFocusChanged () to do XmProcessTraversal for motif - not sure how to do unfocus case??
 *		Also, skip base classes inherited - go directly to FocusItem - since we still have crap in Button.cc.
 *
 *		Revision 1.21  1992/03/09  23:56:55  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.20  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.19  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.18  1992/02/03  17:14:49  lewis
 *		Changed #define/typedef implementation of PushButton to real class.
 *
 *		Revision 1.17  1992/01/27  07:49:06  lewis
 *		Had to get rid of typedef inherited for pushbutton motif gui - I think it is compiler bug, or at least its going
 *		into compat mode when I dont want it to, since its says inherited conflicts with global one, when there was no
 *		global one, just another scoped one....
 *
 *		Revision 1.16  1992/01/23  19:47:47  lewis
 *		Override unrealize to unrealize our subview directly.
 *
 *		Revision 1.15  1992/01/23  08:05:23  lewis
 *		Use nonvirutal public/protected pure virtual _ interface throughout PushButton - though only tested on unix.
 *		Also, start trying typedef local inherited trick.
 *		Also, keep string label in native pushbutton for motif, since we no longer try to do such magic there.
 *
 *		Revision 1.14  1992/01/17  17:49:59  lewis
 *		Delete hack portbale motif implementation, and move pushbutton #defines
 *		to all be togettther ath the bottom.
 *
 *		Revision 1.11  1991/12/27  16:51:24  lewis
 *		Use xt callback.
 *
 *		Revision 1.10  1991/12/18  09:03:38  lewis
 *		Add accessor to osrep for native pushbuttons.
 *
 *
 *
 */

#include	"Button.hh"
#include	"FocusItem.hh"






class	AbstractPushButton : public Button {
	public:
		static	const	Boolean	kIsDefaultButton;
		static	const	String	kOKLabel;
		static	const	String	kCancelLabel;
		static	const	String	kYesLabel;
		static	const	String	kNoLabel;

	protected:
		AbstractPushButton (ButtonController* controller);

	public:
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	SetLabel (const char* label, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Boolean	GetIsDefault () const;
		nonvirtual	void	SetIsDefault (Boolean isDefault = kIsDefaultButton, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	void	Flash ();

	protected:
		virtual	String	GetLabel_ () const											=	Nil;
		virtual	void	SetLabel_ (const String& label, UpdateMode updateMode)		=	Nil;

		virtual	Boolean	GetIsDefault_ () const										=	Nil;
		virtual	void	SetIsDefault_ (Boolean isDefault, UpdateMode updateMode)	=	Nil;

		virtual		void	Flash_ ()												=	Nil;
};




class	AbstractPushButton_MacUI : public AbstractPushButton {
	protected:
		AbstractPushButton_MacUI (ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		override	Boolean	GetIsDefault_ () const;
		override	void	SetIsDefault_ (Boolean isDefault, UpdateMode updateMode);

		override	void	SetSize_ (const Point& newSize, UpdateMode updateMode);
		virtual		void	RebuildAdornment_ (Boolean isDefault, UpdateMode updateMode);

	private:
		Adornment*	fDefaultButtonAdornment;
};




class	AbstractPushButton_MotifUI : public AbstractPushButton, public FocusItem {
	protected:
		AbstractPushButton_MotifUI (ButtonController* controller);

	public:
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);
		override	Boolean	GetLive () const;
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	Boolean	GetEffectiveLive () const;		// override and call view version to disambiguate base class versions

		override	Boolean	GetIsDefault_ () const;
		override	void	SetIsDefault_ (Boolean isDefault, UpdateMode updateMode);

		override	void	SetSize_ (const Point& newSize, UpdateMode updateMode);
		virtual		void	RebuildAdornment_ (Boolean isDefault, UpdateMode updateMode);

	private:
		Adornment*	fDefaultButtonAdornment;
};


class	AbstractPushButton_WinUI : public AbstractPushButton, public FocusItem {
	protected:
		AbstractPushButton_WinUI (ButtonController* controller);

	public:
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);
		override	Boolean	GetLive () const;
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	Boolean	GetEffectiveLive () const;		// override and call view version to disambiguate base class versions

		override	Boolean	GetIsDefault_ () const;
		override	void	SetIsDefault_ (Boolean isDefault, UpdateMode updateMode);

		override	void	SetSize_ (const Point& newSize, UpdateMode updateMode);
		virtual		void	RebuildAdornment_ (Boolean isDefault, UpdateMode updateMode);

	private:
		Adornment*	fDefaultButtonAdornment;
};



class	PushButton_MacUI_Portable : public AbstractPushButton_MacUI {
	public:
		PushButton_MacUI_Portable (const String& label, ButtonController* controller = Nil);
		
	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	Draw (const Region& update);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		override	void	Flash_ ();

	private:
		String	fLabel;
};




class	PushButton_MotifUI_Portable : public AbstractPushButton_MotifUI {
	public:
		PushButton_MotifUI_Portable (const String& label, ButtonController* controller = Nil);
		
	protected:
		override	void	Draw (const Region& update);
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		override	void	Flash_ ();
		virtual		void	DrawToggle (Boolean on);
		
	private:
		String		fLabel;
};


class	PushButton_WinUI_Portable : public AbstractPushButton_WinUI {
	public:
		PushButton_WinUI_Portable (const String& label, ButtonController* controller = Nil);
		
	protected:
		override	void	Draw (const Region& update);
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		override	void	Flash_ ();
		virtual		void	DrawToggle (Boolean on);
		
	private:
		String		fLabel;
};


#if		qMacToolkit
class	MacOSControl;
struct	osControlRecord;
class	PushButton_MacUI_Native : public AbstractPushButton_MacUI {
	public:
		PushButton_MacUI_Native (const String& label, ButtonController* controller = Nil);
		~PushButton_MacUI_Native ();

		nonvirtual	osControlRecord**	GetOSRepresentation () const;

	protected:
		override	void	Draw (const Region& update);
		override	void	Layout ();

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode update);
		
		override	void	Flash_ ();

	private:
		MacOSControl*	fMacOSControl;
};
#endif	/*qMacToolkit*/


#if		qXmToolkit
class	MotifOSControl;
struct	osWidget;
class	PushButton_MotifUI_Native : public AbstractPushButton_MotifUI {
	public:
		PushButton_MotifUI_Native (const String& label, ButtonController* controller = Nil);
		~PushButton_MotifUI_Native ();
		
		nonvirtual	osWidget*	GetOSRepresentation () const;
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	void	Layout ();
		
		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		override	void	EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode);
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode update);

		override	void	Flash_ ();

		override	void	Realize (osWidget* parent);
		override	void	UnRealize ();

		override	void	SetBorder_ (const Point& border, UpdateMode update); 
		override	void	SetMargin_ (const Point& margin, UpdateMode update); 

	private:
		String			fLabel;
		MotifOSControl*	fOSControl;
		Boolean			fGotClick;

		static	void	ActivateCallBack (osWidget* w, char* client_data, char* call_data);
};
#endif	/*qXmToolkit*/




class	PushButton_MacUI : public
// temporarily disable native version due to system 7 bug with setting windows backPixPat
#if		qNative && qMacToolkit && 0
	PushButton_MacUI_Native
#else
	PushButton_MacUI_Portable
#endif
	{
		public:
			PushButton_MacUI ();
			PushButton_MacUI (const String& label);
			PushButton_MacUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~PushButton_MacUI ();
#endif
};




class	PushButton_MotifUI : public
#if		qNative && qXmToolkit
	PushButton_MotifUI_Native
#else
	PushButton_MotifUI_Portable
#endif
	{
		public:
			PushButton_MotifUI ();
			PushButton_MotifUI (const String& label);
			PushButton_MotifUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~PushButton_MotifUI ();
#endif
};

class	PushButton_WinUI : public
	PushButton_WinUI_Portable
	{
		public:
			PushButton_WinUI ();
			PushButton_WinUI (const String& label);
			PushButton_WinUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~PushButton_WinUI ();
#endif
};




class	PushButton : public
#if		qMacUI
	PushButton_MacUI
#elif	qMotifUI
	PushButton_MotifUI
#elif	qWinUI
	PushButton_WinUI
#endif
{
	public:
		PushButton ();
		PushButton (const String& label);
		PushButton (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PushButton ();
#endif
};






/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__PushButton__*/
