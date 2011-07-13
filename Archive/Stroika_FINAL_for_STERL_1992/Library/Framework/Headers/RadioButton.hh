/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__RadioButton__
#define	__RadioButton__

/*
 * $Header: /fuji/lewis/RCS/RadioButton.hh,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *		-	Get rid of typdef RadioBtPtr.
 *
 * Notes:
 *
 * Changes:
 *	$Log: RadioButton.hh,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *		Revision 1.3  1992/07/02  04:37:25  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr and added View::
 *		scope to UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  06:23:50  sterling
 *		RadioButton is a Toggle instead of a Button.
 *		Mix in FocusItem as appropriate instead of assuming we get it from Button.
 *		Renamed CalcDefaultSize to CalcDefaultSize_ (+more?).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.29  1992/06/09  15:01:07  sterling
 *		made portable motif style radiobutton
 *
 *		Revision 1.28  92/05/13  12:17:53  12:17:53  lewis (Lewis Pringle)
 *		STERL: Added overloaded SetLabel() method taking const char*.
 *		Changed default arg to overloading in RadioButton CTOR, and added no-arg case.
 *		
 *		Revision 1.27  92/04/14  07:33:13  07:33:13  lewis (Lewis Pringle)
 *		Added out of line virtual dtor for RadioButton because of qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.26  92/03/26  18:34:29  18:34:29  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.24  1992/03/26  15:17:28  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		oldFocused first argument.
 *
 *		Revision 1.21  1992/03/17  17:03:42  lewis
 *		Override EffectiveFocusChanged () to do XmProcessTraversal for motif - not sure how to do unfocus case??
 *		Also, skip base classes inherited - go directly to FocusItem - since we still have crap in Button.cc.
 *
 *		Revision 1.20  1992/03/09  23:57:17  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.19  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.18  1992/02/21  19:52:51  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.17  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.16  1992/02/03  17:05:33  lewis
 *		Use class rather than #define or typedef for RadioButton.
 *
 *		Revision 1.15  1992/01/23  19:48:43  lewis
 *		Override realize/unrealize to ajdust our oscotnrol directly, so we can reset attributes at that point.
 *		Convert all these routines to public accessor, calls protected pure virtual _ method. Delete 2.0 compat.
 *
 *		Revision 1.14  1992/01/23  08:07:06  lewis
 *		Add label string to motif native radiobutton, since oscontrols no longer keeps track while were un-realized.
 *
 *		Revision 1.10  1992/01/10  08:54:15  lewis
 *		Got rid of bogus portable implementation of motif radiobutton.
 *
 *		Revision 1.9  1992/01/08  06:04:18  lewis
 *		Sterl- changed typdef of RadioButton  to #define cuz of new problem - he
 *		forward declares a class to be class RadioButton ; and that doesn't work
 *		it we make it a typedef.
 *
 *
 *
 */

#include	"Toggle.hh"
#include	"FocusItem.hh"




#if		!qRealTemplatesAvailable
	class	AbstractRadioButton;
	typedef	class	AbstractRadioButton*	RadioBtPtr;
	Declare (Iterator, RadioBtPtr);
	Declare (Collection, RadioBtPtr);
	Declare (AbSequence, RadioBtPtr);
	Declare (Array, RadioBtPtr);
	Declare (Sequence_Array, RadioBtPtr);
	Declare (Sequence, RadioBtPtr);
#endif


class	RadioBank : public ButtonController {
	public:
		RadioBank ();

		virtual	void	AddRadioButton (AbstractRadioButton* button);
		virtual	void	RemoveRadioButton (AbstractRadioButton* button);
		
		virtual	AbstractRadioButton*	GetCurrentButton () const;

		virtual	CollectionSize	GetCurrentIndex () const;
		virtual	void			SetCurrentIndex (CollectionSize index, Panel::UpdateMode updateMode = View::eDelayedUpdate);

	protected:
		override	void	ButtonPressed (AbstractButton* button);

	private:
		Sequence(RadioBtPtr)	fRadioButtons;
		AbstractRadioButton*				fCurrentButton;
};




class	AbstractRadioButton : public Toggle {
	protected:
		AbstractRadioButton (ButtonController* controller);

	public:
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	SetLabel (const char* label, UpdateMode updateMode = eDelayedUpdate);

	protected:
		virtual		String	GetLabel_ () const										=	Nil;
		virtual		void	SetLabel_ (const String& label, UpdateMode updateMode)	=	Nil;
};




class	AbstractRadioButton_MacUI : public AbstractRadioButton {
	protected:
		AbstractRadioButton_MacUI (ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		virtual	Coordinate	GetToggleWidth () const;
		virtual	Coordinate	GetToggleGap () const;
};


class	AbstractRadioButton_MotifUI : public AbstractRadioButton, public FocusItem {
	protected:
		AbstractRadioButton_MotifUI (ButtonController* controller);

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

class	AbstractRadioButton_WinUI : public AbstractRadioButton {
	protected:
		AbstractRadioButton_WinUI (ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		virtual	Coordinate	GetToggleWidth () const;
		virtual	Coordinate	GetToggleGap () const;
};


class	RadioButton_MacUI_Portable : public AbstractRadioButton_MacUI {
	public:
		RadioButton_MacUI_Portable (const String& label, ButtonController* controller = Nil);

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



class	RadioButton_WinUI_Portable : public AbstractRadioButton_WinUI {
	public:
		RadioButton_WinUI_Portable (const String& label, ButtonController* controller = Nil);

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

class	RadioButton_MacUI_Native : public AbstractRadioButton_MacUI {
	public:
		RadioButton_MacUI_Native (const String& label, ButtonController* controller = Nil);
		~RadioButton_MacUI_Native ();

	protected:
		override	void	Draw (const Region& update);
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


class	RadioButton_MotifUI_Portable : public AbstractRadioButton_MotifUI {
	public:
		RadioButton_MotifUI_Portable (const String& label, ButtonController* controller = Nil);
		
	protected:
		override	void	Draw (const Region& update);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);

		virtual		void	DrawToggle (Boolean on);
		virtual		Rect	GetBox () const;

	private:
		String		fLabel;
};



#if		qXmToolkit
class	RadioButton_MotifUI_Native : public AbstractRadioButton_MotifUI {
	public:
		RadioButton_MotifUI_Native (const String& label, ButtonController* controller = Nil);
		~RadioButton_MotifUI_Native ();

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
		class	 MotifOSControl;
		MotifOSControl*		fOSControl;
		String				fLabel;
};
#endif	/*qXmToolkit*/




class	RadioButton_MacUI : public
#if		qNative && qMacToolkit
	RadioButton_MacUI_Native
#else
	RadioButton_MacUI_Portable
#endif
	{
		public:
			RadioButton_MacUI ();
			RadioButton_MacUI (const String& label);
			RadioButton_MacUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~RadioButton_MacUI ();
#endif
};




class	RadioButton_MotifUI : public
#if		qNative && qXmToolkit
	RadioButton_MotifUI_Native
#else
	RadioButton_MotifUI_Portable
#endif
	{
		public:
			RadioButton_MotifUI ();
			RadioButton_MotifUI (const String& label);
			RadioButton_MotifUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~RadioButton_MotifUI ();
#endif
};



class	RadioButton_WinUI : public
	RadioButton_WinUI_Portable
	{
		public:
			RadioButton_WinUI ();
			RadioButton_WinUI (const String& label);
			RadioButton_WinUI (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~RadioButton_WinUI ();
#endif
};




class	RadioButton : public
#if		qMacUI
	RadioButton_MacUI
#elif	qMotifUI
	RadioButton_MotifUI
#elif	qWinUI
	RadioButton_WinUI
#endif
	{
		public:
			RadioButton ();
			RadioButton (const String& label);
			RadioButton (const String& label, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~RadioButton ();
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


#endif	/*__RadioButton__*/
