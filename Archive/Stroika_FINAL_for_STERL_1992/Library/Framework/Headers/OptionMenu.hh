/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__OptionMenu__
#define	__OptionMenu__

/*
 * $Header: /fuji/lewis/RCS/OptionMenu.hh,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * 		An AbstractOptionMenuTitle is a MenuTitle, and therefore owns a particular menu.
 *	It maintains a notion of a SINGLE currently selected item, and displays that currently
 *	selected item even when the menu is unarmed. It is very similar to a PopUpMenu, except
 *	that a Popup Menu has no notion of a current item. Also, AbstractOptionMenuTitle is considerably
 *	less flexible that PopupMenu, in that we do not allow changing of the underlying menu, or adding
 *	different kinds of menu items. Also, the menu items cannot have commands. Just indexes into their
 *	names.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *		AbstractOptionMenuButton::SetItem() takes an update mode while the other operations to change
 *	the list of items do not, because it COULD be affecting the current item. This is the only case
 *	where the UpdateMode makes sense, since otherwise the menu need not be updated (we are assuming no
 *	changes while the menu is posted - not necessarily reasobable, but I've never seen it done - and
 *	if it were, I think we would ALWAYS want to do immediate mode update in that case - UpdateModes probably
 *	dont make sense there.
 *
 *
 * Changes:
 *	$Log: OptionMenu.hh,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:26:43  sterling
 *		renamed GUI to UI
 *
 *		Revision 1.3  1992/07/02  23:49:24  lewis
 *		Included conditionally the declaration of all the string variants of containers.
 *
 *		Revision 1.2  1992/06/25  05:57:58  sterling
 *		Mix FocusItem directly into motif OptionMenu since no longer conditionally compiled
 *		into Button class.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.1  92/04/24  09:36:50  09:36:50  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 *	<< Based on PopUpMenu.hh >>
 *
 */

#include	"Sequence_Array.hh"

#include	"Button.hh"
#include	"FocusItem.hh"
#include	"MenuTitle.hh"




#if		!qRealTemplatesAvailable
	// copied from Stroika-Foundation-Globals.hh - see comment there for explanation...
	#include	"Array.hh"
	#include	"Collection.hh"
	#include	"DoubleLinkList.hh"
	#include	"Sequence.hh"
	#include	"Sequence_Array.hh"
	#include	"Sequence.hh"
	#include	"Set_Array.hh"
	
	#ifndef		_ContainersOfStringDeclared_
		#define		_ContainersOfStringDeclared_
		Declare (Iterator, String);
		Declare (Collection, String);
		Declare (AbSequence, String);
		Declare (AbSet, String);
		Declare (Array, String);
		Declare (Sequence_Array, String);
		Declare (Sequence, String);
		Declare (Set_Array, String);
		Declare (Set, String);
	#endif		/*_ContainersOfStringDeclared_*/
#endif		/*!qRealTemplatesAvailable*/




class	AbstractOptionMenuButton : public Button {
	protected:
		AbstractOptionMenuButton ();

	/*
	 * OptionMenuButtons have a label to indicate what sort of options are being presented.
	 */
	public:
		nonvirtual	String		GetLabel () const;
		nonvirtual	void		SetLabel (const String& label, UpdateMode updateMode = eDelayedUpdate);

	/*
	 * OptionMenuButtons have a sequence of items (which are strings). We provide simple, but hopefully adequate
	 * access to these.
	 */
	public:
		nonvirtual	CollectionSize			GetItemCount () const;
		nonvirtual	void					SetItemCount (CollectionSize itemCount);	// remove off end, or add empty strings to end
		nonvirtual	const AbSequence(String)&	GetItems () const;

		nonvirtual	String					GetItem (CollectionSize i) const;
		nonvirtual	void					SetItem (CollectionSize i, const String& item, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	void					AppendItem (const String& item);


	/*
	 * OptionMenuButtons have a notion of the currently selected item. This can be kBadSequenceIndex, to
	 * indicate that none is selected.
	 */
	public:
		nonvirtual	CollectionSize	GetCurrentItem () const;
		nonvirtual	void			SetCurrentItem (CollectionSize i, UpdateMode updateMode = eDelayedUpdate);



	/*
	 * Protected, pure virtual methods to implement the above interface.
	 */
	protected:
		virtual	String	GetLabel_ () const											=	Nil;
		virtual	void	SetLabel_ (const String& label, UpdateMode updateMode)		=	Nil;

		virtual	const AbSequence(String)&	GetItems_ () const							=	Nil;
		virtual	void			SetItemCount_ (CollectionSize itemCount)				=	Nil;
		virtual	void			SetItem_ (CollectionSize i, const String& item, UpdateMode updateMode)	=	Nil;

		virtual	CollectionSize	GetCurrentItem_ () const										=	Nil;
		virtual	void			SetCurrentItem_ (CollectionSize i, UpdateMode updateMode)		=	Nil;
};






class	OptionMenuButton_MacUI : public AbstractOptionMenuButton {
	protected:
		OptionMenuButton_MacUI ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;


	/*
	 * When a new value is set to the popup, should we automatically resize ourselves to our CalcDefaultSize()
	 * to make sure the text of the new item fits.
	 */
	public:
		nonvirtual	Boolean	GetAutoResize () const;
		nonvirtual	void	SetAutoResize (Boolean autoResize);


	/*
	 * The appropraite spacing between the label, and the current item being displayed.
	 * << SHOULD THIS BE MOVED UP TO AbstractOptionMenuButton?? >>
	 */
	protected:
		nonvirtual	Coordinate	GetItemOffset () const;

	protected:
		virtual	Boolean	GetAutoResize_ () const						=	Nil;
		virtual	void	SetAutoResize_ (Boolean autoResize)			=	Nil;

};




class	OptionMenuButton_MacUI_Portable : public OptionMenuButton_MacUI {
	public:
		OptionMenuButton_MacUI_Portable (const String& label);
		~OptionMenuButton_MacUI_Portable ();

	public:
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	void	Draw (const Region& update);
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

	protected:
		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);
		override	const AbSequence(String)&	GetItems_ () const;
		override	void			SetItemCount_ (CollectionSize itemCount);
		override	void			SetItem_ (CollectionSize i, const String& item, UpdateMode updateMode);
		override	CollectionSize	GetCurrentItem_ () const;
		override	void			SetCurrentItem_ (CollectionSize i, UpdateMode updateMode);
		override	Boolean	GetAutoResize_ () const;
		override	void	SetAutoResize_ (Boolean autoResize);

	private:
		String					fLabel;
		Sequence_Array(String)	fItems;
		CollectionSize			fCurrentItem;
		Boolean					fAutoResize;
		class	PopUpMenuTitle;
		PopUpMenuTitle*			fPopUp;

		nonvirtual	Rect		CalcMenuRect () const;
		nonvirtual	void		DrawPopUpBox ();
		nonvirtual	String		GetCurrentItemText () const;
};




class	OptionMenuButton_MotifUI : public AbstractOptionMenuButton, public FocusItem {
	protected:
		OptionMenuButton_MotifUI ();

	public:
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		
		nonvirtual	Coordinate	GetItemOffset () const;

};



#if		qXmToolkit
class	OptionMenuButton_MotifUI_Native : public OptionMenuButton_MotifUI {
	public:
		OptionMenuButton_MotifUI_Native (const String& label);
		~OptionMenuButton_MotifUI_Native ();

	public:
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	void	Layout ();

		override	String	GetLabel_ () const;
		override	void	SetLabel_ (const String& label, UpdateMode updateMode);
		override	const AbSequence(String)&	GetItems_ () const;
		override	void			SetItemCount_ (CollectionSize itemCount);
		override	void			SetItem_ (CollectionSize i, const String& item, UpdateMode updateMode);
		override	CollectionSize	GetCurrentItem_ () const;
		override	void			SetCurrentItem_ (CollectionSize i, UpdateMode updateMode);

		// View/Widget support
		override	void	Realize (osWidget* parent);
		override	void	UnRealize ();

	private:
		String						fLabel;
		Sequence_Array(String)		fItems;
		CollectionSize				fCurrentItem;
		class	MotifOSControl;
		MotifOSControl*	fOptionMenu;
		nonvirtual	void			RebuildOSMenu ();			// ok to call if not realized
		nonvirtual	CollectionSize	GetOSSelectedItem () const;	// illegal to call if not realized
};
#endif





class	OptionMenuButton : public
#if		qMacUI
	OptionMenuButton_MacUI_Portable
#elif	qMotifUI
#if		qXmToolkit
	OptionMenuButton_MotifUI_Native
#else
	OptionMenuButton_MacUI_Portable
#endif
#elif	qWinUI
	OptionMenuButton_MacUI_Portable
#endif	/*GUI*/
{
	public:
		OptionMenuButton (const String& label = kEmptyString, ButtonController* controller = Nil);
};





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__OptionMenu__*/

