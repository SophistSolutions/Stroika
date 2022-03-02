/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__FocusItem__
#define	__FocusItem__

/*
 * $Header: /fuji/lewis/RCS/FocusItem.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		A Focus item is a mouse/menu/keyboard handler who can automatically receive event dispatches. There
 *		are two kinds of FocusItems: FocusViews and FocusOwners. FocusViews are visual representations of
 *		FocusItems. They are a common subclass within Stroika (on the Macintosh, TextEdit is a FocusItem,
 *		while under Motif or MS Windows most widgets are FocusItems). Typically a FocusView will display a 
 *		special adornment to cue the application user as to who is the currently focused view. FocusOwners
 *		are the owners of FocusViews. They typically have no visual representation,although the class can
 *		be mixed into a view such as Dialog, that can than automatically support focusing. FocusOwners can
 *		have many owned FocusItems, but only one owned item will be the current focus at a time. FocusOwners
 *		interpret certain key combinations (such as Tab and Arrow keys) to mean advance to the next or
 *		the previous focus.
 *
 *		There is an intermediate class, AbstractFocusOwner, that allows programmers to control the implementation
 *		of the FocusItem list. This can be important for efficiency reasons, but usually you will want to
 *		subclass from FocusOwner, which automatically builds and maintains the focus item list.
 *
 *
 * TODO:
 *
 *		-	Maybe should have some auxiliary functions (static maybe) in FocusItem to say which keystrokes get
 *		interpretted as being part of the mouseless interface, and how they get interpretted???
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: FocusItem.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/07/02  03:55:14  lewis
 *		Renamed Sequence_DoublyLLOfPointers -> SequencePtr.
 *		Added scope to UpdateMode.
 *
 *		Revision 1.18  1992/03/26  15:17:28  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		oldFocused first argument.
 *
 *		Revision 1.17  1992/03/13  16:10:39  lewis
 *		Moved FocusView class to User layer. Consider doing the same to FocusOwner - and also rationalizing
 *		names a bit more!
 *
 *		Revision 1.16  1992/03/09  23:48:42  lewis
 *		Use new HandleKeyStroke / DispatchKeyEvent () interface, and use KeyStrokes rather than KeyBoards in GetTab, etc...
 *
 *		Revision 1.15  1992/03/06  22:16:34  lewis
 *		Cosmetic changes, and comments that I'd like to get rid of FocusView, and what methods are useless and what does what...
 *
 *		Revision 1.13  1992/02/15  04:50:37  sterling
 *		many new features, includng EffectiveFocusChanged
 *
 *		Revision 1.11  1992/01/31  16:23:54  sterling
 *		added grab focus
 *
 *		Revision 1.8  1992/01/08  05:59:12  lewis
 *		Merged some tab-loop related changes for sterl, and delete s
 *		upport for old g++ bugs (gcc 2.0 due soon).
 *
 *
 *
 *
 */

#include	"Collection.hh"
#include	"Exception.hh"
#include	"Sequence.hh"

#include	"EnableItem.hh"
#include	"KeyHandler.hh"
#include	"LiveItem.hh"
#include	"MenuCommandHandler.hh"
#include	"Panel.hh"



 
 
#if		!qRealTemplatesAvailable
	typedef	class	FocusItem*	FocusItemPtr;
	Declare (Iterator, FocusItemPtr);
	Declare (Collection, FocusItemPtr);
	Declare (AbSequence, FocusItemPtr);
	Declare (Array, FocusItemPtr);
	Declare (Sequence_Array, FocusItemPtr);
	Declare (Sequence, FocusItemPtr);
#endif

class	AbstractFocusOwner;



// should be scoped within FocusItem
class	FocusException : public Exception {
	public:
		FocusException ();

		override	void	Raise ();
		nonvirtual	void	Raise (const String& message);
		nonvirtual	String	GetMessage () const;

	private:
		String	fMessage;
};




#if 	qCanFreelyUseVirtualBaseClasses
class	FocusItem : public virtual KeyHandler, public virtual MenuCommandHandler, public virtual LiveItem
#else
class	FocusItem : public KeyHandler, public MenuCommandHandler, public LiveItem
#endif
{
	public:
		static	const	Boolean	kFocused;
		static	const	Boolean	kValidate;
		static	FocusException	sFocusFailed;

	protected:
		FocusItem ();

	public:
		nonvirtual	Boolean	GetFocused () const;
		nonvirtual	void	SetFocused (Boolean focused, Panel::UpdateMode updateMode = Panel::eDelayedUpdate, Boolean validate = True);

		nonvirtual	Boolean	GetEffectiveFocused () const;

		virtual		AbstractFocusOwner*	GetFocusOwner () const;

	protected:
		virtual	Boolean	GetFocused_ () const;
		virtual	void	SetFocused_ (Boolean focused, Panel::UpdateMode updateMode, Boolean validate);
		virtual	void	Validate ();
		virtual	void	EffectiveFocusChanged (Boolean newFocused, Panel::UpdateMode updateMode);

		/*
		 * a FocusView punts on this, while a FocusOwner tries to move on the the next focus. If it 
		 * can it returns True
		 */
		virtual	Boolean	HandleTab (SequenceDirection d, Boolean wrapping);

		virtual	Boolean	CanBeFocused ();
		virtual	void	TabbingFocus (SequenceDirection d, Panel::UpdateMode update);
		virtual	void	GrabFocus (Panel::UpdateMode update, Boolean validate);

	private:
		nonvirtual		void	SetFocusOwner (AbstractFocusOwner* focusOwner);

		Boolean				fFocused;
		AbstractFocusOwner*	fOwner;

		friend	class	AbstractFocusOwner;
};





class	AbstractFocusOwner : public FocusItem {
	public:
		static	const	Boolean	kWrap;

	protected:	
		AbstractFocusOwner ();
	public:
		~AbstractFocusOwner ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	Boolean	DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
											KeyComposeState& composeState);

		nonvirtual	FocusItem*	GetCurrentFocus () const;
		nonvirtual	void		SetCurrentFocus (FocusItem* newFocus, Panel::UpdateMode updateMode = Panel::eDelayedUpdate, Boolean validate = True);
		nonvirtual	void		SetCurrentFocus (SequenceDirection d, Panel::UpdateMode updateMode, Boolean validate = True);

	protected:
		override	void	EffectiveFocusChanged (Boolean newFocused, Panel::UpdateMode updateMode);
		override	Boolean	HandleTab (SequenceDirection d, Boolean wrapping);
		override	void	Validate ();

		override	Boolean	CanBeFocused ();
		override	void	TabbingFocus (SequenceDirection d, Panel::UpdateMode update);

		virtual		void 	SetCurrentFocus_ (FocusItem* newFocus, Panel::UpdateMode update, Boolean validate = True);

		virtual	FocusItem*	CalcNextFocus (SequenceDirection d, Boolean allowWrapAround);

		virtual	Boolean		GetTab (const KeyStroke& keyStroke, SequenceDirection& d, Boolean& wrapping);
		
		nonvirtual	void	SetOwnerOfFocusItem (FocusItem* focus, AbstractFocusOwner* owner);
		
	protected:
		virtual		void	AddFocus (FocusItem* focus, CollectionSize index = eAppend)							= Nil;
		virtual		void	AddFocus (FocusItem* focus, FocusItem* neighborFocus, AddMode addMode = eAppend) = Nil;
		virtual		void	RemoveFocus (FocusItem* focus)						= Nil;
		virtual		void	ReorderFocus (FocusItem* focus, CollectionSize index = 1)	= Nil;
		virtual		void	ReorderFocus (FocusItem* focus, FocusItem* neighborFocus, AddMode addMode = eAppend)	= Nil;

		virtual	SequenceIterator(FocusItemPtr)* MakeFocusIterator (SequenceDirection d = eSequenceForward) = Nil;

		FocusItem*	fFocus;
};




class	FocusOwner : public AbstractFocusOwner {
	public:
		FocusOwner ();
			
		override	void	AddFocus (FocusItem* focus, CollectionSize index = eAppend);
		override	void	AddFocus (FocusItem* focus, FocusItem* neighborFocus, AddMode addMode = eAppend);
		override	void	RemoveFocus (FocusItem* focus);
		override	void	ReorderFocus (FocusItem* focus, CollectionSize index = 1);	// not CollectionSize cuz could be -1
		override	void	ReorderFocus (FocusItem* focus, FocusItem* neighborFocus, AddMode addMode = eAppend);

		override	SequenceIterator(FocusItemPtr)* MakeFocusIterator (SequenceDirection d = eSequenceForward);

		override	Boolean	GetLive () const;

	private:
		Sequence(FocusItemPtr)	fFocusList;
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	Boolean	FocusItem::GetFocused () const	{	return (GetFocused_ ());	}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__FocusItem__*/
