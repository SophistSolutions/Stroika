/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CheckBox__
#define	__CheckBox__

/*
 * $Header: /fuji/lewis/RCS/CheckBox.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *		CheckBox is what you typically will instantiate, and AbstractCheckBox is what you will typically keep
 *		a reference pointer as.
 *
 *
 * Changes:
 *	$Log: CheckBox.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *		Revision 1.2  1992/06/25  05:23:46  sterling
 *		Lots of changes including using new Toggle class and mixing in FocusItem in right places,
 *		rather than inheriting from button.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.30  1992/06/09  15:03:03  sterling
 *		motif style portable checkbox
 *
 *		Revision 1.29  92/05/13  01:59:21  01:59:21  lewis (Lewis Pringle)
 *		STERL: overload SetLabel () to take const char* arg.
 *		Get rid of default arg, and overload more CTOR for CheckBox so we can specify no args.
 *		
 *		Revision 1.28  92/04/14  07:32:26  07:32:26  lewis (Lewis Pringle)
 *		Added virtual dtor for CheckBox conditioned on qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.27  92/03/26  18:32:08  18:32:08  lewis (Lewis Pringle)
 *		Add overrides of EffectiveFontChanged () so we inval layouts, etc, as appropriate.
 *		
 *		Revision 1.26  1992/03/26  15:17:28  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		oldFocused first argument.
 *
 *		Revision 1.25  1992/03/26  09:36:15  lewis
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *
 *		Revision 1.24  1992/03/24  02:28:50  lewis
 *		Made CalcDefaultSize () const.
 *
 *		Revision 1.23  1992/03/17  17:03:42  lewis
 *		Override EffectiveFocusChanged () to do XmProcessTraversal for motif - not sure how to do unfocus case??
 *		Also, skip base classes inherited - go directly to FocusItem - since we still have crap in Button.cc.
 *
 *		Revision 1.22  1992/03/09  23:43:50  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.21  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.20  1992/02/21  19:38:04  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.19  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.18  1992/02/06  20:46:18  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.17  1992/01/31  22:46:25  lewis
 *		Try new style for instantiation class - make real class - all other approaches
 *		seem to fail. A bit more work than #defines or typdefs, but really works in call cases.
 *
 *		Revision 1.16  1992/01/23  19:44:22  lewis
 *		Rename MacUI to MacUI.
 *		Also, fix motif checkboxes native to keep all their state when unrealized, so that on realize they
 *		can adjust themselves correctly. Also, Use public wrappper on protected virtual _ style for attribute accessors.
 *
 *		Revision 1.15  1992/01/23  08:02:46  lewis
 *		Add String to native motif checkbox so we keep track of label, and just set into control.
 *
 *		Revision 1.12  1992/01/10  03:03:45  lewis
 *		Added CalcDefaultSize () override for Motif checkboxes.
 *
 *		Revision 1.10  1992/01/08  05:52:01  lewis
 *		Sterl- changed typdef of CheckBox to #define cuz of new problem - he
 *		forward declares a class to be class CheckBox; and that doesn't work
 *		it we make it a typedef.
 *
 */

#include	"Button.hh"



#include	"Toggle.hh"
#include	"FocusItem.hh"




class	AbstractCheckBox : public Toggle {
	protected:
		AbstractCheckBox (ButtonController* controller);

	public:
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	SetLabel (const char* label, UpdateMode updateMode = eDelayedUpdate);

	protected:
		virtual		String	GetLabel_ () const											=	Nil;
		virtual		void	SetLabel_ (const String& label, UpdateMode updateMode)		=	Nil;
};





class	AbstractCheckBox_MacUI : public AbstractCheckBox {
	protected:
		AbstractCheckBox_MacUI (ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		virtual	Coordinate	GetToggleWidth () const;
		virtual	Coordinate	GetToggleGap () const;
};

class	AbstractCheckBox_MotifUI : public AbstractCheckBox, public FocusItem {
	protected:
		AbstractCheckBox_MotifUI (ButtonController* controller);

	public:
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);
		override	Boolean	GetLive () const;
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	Boolean	GetEffectiveLive () const;		// override and call view version to disambiguate base class versions

		virtual	Coordinate	GetToggleWidth () const;
		virtual	Coordinate	GetToggleGap () const;
};

class	AbstractCheckBox_WinUI : public AbstractCheckBox {
	protected:
		AbstractCheckBox_WinUI (ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		virtual	Coordinate	GetToggleWidth () const;
		virtual	Coordinate	GetToggleGap () const;
};



class	CheckBox_MacUI_Portable : public AbstractCheckBox_MacUI {
	public:
		CheckBox_MacUI_Portable (const String& label, ButtonController* controller = Nil);

	protected:
		override	void	Draw (const Region& update);
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		virtual		void	DrawToggle (Boolean on);
		virtual		Rect	GetBox () const;

	private:
		String	fLabel;
};



#if		qMacToolkit
class	CheckBox_MacUI_Native : public AbstractCheckBox_MacUI {
	public:
		CheckBox_MacUI_Native (const String& label, ButtonController* controller = Nil);
		~CheckBox_MacUI_Native ();

		override	void	Draw (const Region& update);

	protected:
		override	void	Layout ();

		override	void	SetOn_ (Boolean on, UpdateMode updateMode);
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode); 
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode); 

	private:
		class	 MacOSControl*	fMacOSControl;
};
#endif	/*qMacToolkit*/







class	CheckBox_MotifUI_Portable : public AbstractCheckBox_MotifUI {
	public:
		CheckBox_MotifUI_Portable (const String& label, ButtonController* controller = Nil);
		
	protected:
		override	void	Draw (const Region& update);
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		virtual		void	DrawToggle (Boolean on);
		virtual		Rect	GetBox () const;

	private:
		String		fLabel;
};


#if		qXmToolkit

class	CheckBox_MotifUI_Native : public AbstractCheckBox_MotifUI {
	public:
		CheckBox_MotifUI_Native (const String& label, ButtonController* controller = Nil);
		~CheckBox_MotifUI_Native ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();

		override	void	EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode);
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		override	void	SetOn_ (Boolean on, UpdateMode updateMode);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		override	void	Realize (osWidget* parent);
		override	void	UnRealize ();

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode); 
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode); 

	private:
		class	MotifOSControl;
		MotifOSControl*		fOSControl;
		String				fLabel;
};

#endif	/*qXmToolkit*/



class	CheckBox_WinUI_Portable : public AbstractCheckBox_WinUI {
	public:
		CheckBox_WinUI_Portable (const String& label, ButtonController* controller = Nil);

	protected:
		override	void	Draw (const Region& update);
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		virtual		void	DrawToggle (Boolean on);
		virtual		Rect	GetBox () const;

	private:
		String	fLabel;
};




class	CheckBox_MacUI : public
#if		qNative && qMacToolkit
	CheckBox_MacUI_Native
#else
	CheckBox_MacUI_Portable
#endif
	{
		public:
			CheckBox_MacUI ();
			CheckBox_MacUI (const String& label);
			CheckBox_MacUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~CheckBox_MacUI ();
#endif
};




class	CheckBox_MotifUI : public
#if		qNative && qXmToolkit
	CheckBox_MotifUI_Native
#else
	CheckBox_MotifUI_Portable
#endif
	{
		public:
			CheckBox_MotifUI ();
			CheckBox_MotifUI (const String& label);
			CheckBox_MotifUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~CheckBox_MotifUI ();
#endif
};



class	CheckBox_WinUI : public
	CheckBox_WinUI_Portable
	{
		public:
			CheckBox_WinUI ();
			CheckBox_WinUI (const String& label);
			CheckBox_WinUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~CheckBox_WinUI ();
#endif
};


class	CheckBox : public
#if		qMacUI
	CheckBox_MacUI
#elif	qMotifUI
	CheckBox_MotifUI
#elif	qWinUI
	CheckBox_WinUI
#endif
	{
		public:
			CheckBox ();
			CheckBox (const String& label);
			CheckBox (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~CheckBox ();
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



#endif	/*__CheckBox__*/

