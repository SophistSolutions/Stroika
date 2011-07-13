/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Dialog__
#define	__Dialog__

/*
 * $Header: /fuji/lewis/RCS/Dialog.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Dialog.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/02  03:48:47  lewis
 *		Properly nested enum Updatemode.
 *
 *		Revision 1.2  1992/06/25  05:31:25  sterling
 *		Stop using GroupView as base class of DialogMainView - maybe more (checkedin by LGP).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.19  1992/03/26  18:32:28  lewis
 *		Override EffectiveFontChanged in DialogMainView to inval layout, since it depends on font info.
 *
 *		Revision 1.18  1992/03/26  09:36:15  lewis
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *
 *		Revision 1.17  1992/03/24  02:28:50  lewis
 *		Made CalcDefaultSize () const.
 *
 *		Revision 1.16  1992/03/09  23:48:00  lewis
 *		Use new HandleKeyStroke () interface.
 *
 *		Revision 1.15  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.14  1992/02/15  04:50:37  sterling
 *		used new FocusItem features
 *
 *		Revision 1.13  1992/01/31  16:22:32  sterling
 *		no more backwards compatibility
 *
 *		Revision 1.12  1992/01/19  21:24:39  lewis
 *		Added new ctor to Dialog allowing specification of shell.
 *
 *		Revision 1.9  1992/01/06  09:58:01  lewis
 *		Added backward compatability hack to support dialogeditor generated dialogs
 *		(layout method). Delete when sterls new dialog editor is bootstrapped.
 *
 *		Revision 1.7  1992/01/05  06:12:22  lewis
 *		Moved Alert support to seperate file.
 *		Added support for new window/shell strategy including new PlaceNewWindow
 *		support and using the MainViews.
 *
 *		Revision 1.6  1991/12/27  16:50:26  lewis
 *		Use new Window/Shell support rather than old Window/XGUI/Portable.
 *
 *
 *
 *
 */

// #include	"GroupView.hh" LGP commented out Thursday, June 25, 1992 1:29:17 AM - see if that means we could move
							// group view up to User - ask sterl if he thinks that would be appropriate anyhow...

#include	"Button.hh"
#include	"TextEdit.hh"
#include	"Window.hh"


class	AbstractPushButton;
class	PushButton;
class	Dialog : public Window, public FocusOwner, public ButtonController, public TextController {
	public:
		static	Boolean	kCancel;

		/*
		 * New approved interface for Dialogs. Use this constructor, and optionally pass
		 * along the accepting, and dismissing buttons. Since we are a button controller, if
		 * we get clicks in these, we will automatically call Dismiss (). We make no other assumptions here,
		 * for example neither will be the default button unless the user explicitely calls SetDefaultButton.
		 */
		Dialog (View* mainView, FocusItem* mainFocus, AbstractPushButton* acceptButton = Nil,
				AbstractPushButton* cancelButton = Nil);

		/*
		 * the Dialog will create appropriate buttons, setting the Cancel Button to be the default if
		 * it exists, otherwise the OK Button. The default layout is GUI specific.
		 */
		Dialog (View* mainView, FocusItem* mainFocus, const String& okButtonTitle,
				const String& cancelButtonTitle = kEmptyString);

		~Dialog ();

	protected:
		Dialog (WindowShell* windowShell);
	
	public:
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);

	/*
	 * Disambiguate base classess - pass on to FocusOwner.
	 */
	public:
		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		override	Boolean	DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
											  KeyComposeState& composeState);

		virtual		void	Dismiss (Boolean cancel);		
		virtual		Boolean	Pose ();
		
		nonvirtual	Boolean	GetCancelled () const;

		override	void	ButtonPressed (AbstractButton* button);

		nonvirtual	AbstractPushButton*	GetDefaultButton () const;
		virtual		void				SetDefaultButton (AbstractPushButton* button, Panel::UpdateMode updateMode = View::eDelayedUpdate);

		nonvirtual	AbstractPushButton*	GetCancelButton () const;
		virtual		void				SetCancelButton (AbstractPushButton* button);

		nonvirtual	AbstractPushButton*	GetOKButton () const;
		virtual		void				SetOKButton (AbstractPushButton* button);

		nonvirtual	Boolean		GetDismissed () const;
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode);

	private:
		Boolean				fDismissed;
		Boolean				fCancelled;
		AbstractPushButton*	fDefaultButton;
		AbstractPushButton*	fCancelButton;
		AbstractPushButton*	fOKButton;

		class	DialogMainView*	fDialogMainView;
};

// should be scoped within Dialog
class	DialogMainView : public View {
	public:
		DialogMainView (Dialog& dialog, View& mainView, const String& okButtonTitle, const String& cancelButtonTitle);
		DialogMainView (Dialog& dialog, View& mainView);
		~DialogMainView ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		override	void	EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode);

	private:
		Dialog&			fDialog;
		PushButton*		fOKButton;
		PushButton*		fCancelButton;
		View& 			fMainView;
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Dialog__*/

