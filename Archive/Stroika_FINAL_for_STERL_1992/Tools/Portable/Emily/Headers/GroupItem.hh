/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__GroupItem__
#define	__GroupItem__

/*
 * $Header: /fuji/lewis/RCS/GroupItem.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: GroupItem.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.11  1992/03/06  21:53:47  sterling
 *		motif
 *
 *		Revision 1.7  1992/01/27  05:04:01  sterling
 *		fixed groupview dependencies
 *
 *
 */

#include	"FocusItem.hh"
#include	"RadioButton.hh"
#include	"Scroller.hh"
#include	"TextEdit.hh"

#include	"ViewItem.hh"


#if		!qRealTemplatesAvailable
	typedef	class	GroupParam*	GParmPtr;
	#define	GroupParamPtr	GParmPtr
	
	Declare (Iterator, GParmPtr);
	Declare (Collection, GParmPtr);
	Declare (AbSequence, GParmPtr);
	Declare (Array, GParmPtr);
	Declare (Sequence_Array, GParmPtr);
	Declare (Sequence, GParmPtr);
#endif

class	GroupParam : public Saveable {
	public:
		GroupParam (CommandNumber language, CommandNumber gui);
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		Point		fScrollSize;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};



class	GroupItemView;
class 	MagicGroupView;
class	GroupItem : public ViewItem {
	public:
		GroupItem (ItemType& type);
		GroupItem (ItemType& type, Boolean useScrollBars);

		~GroupItem ();

		virtual		void	SetGroupView (Boolean useScrollBars, MagicGroupView* groupView);

		override	Boolean	DoCommand (const CommandSelection& selection);
		override	void	DoSetupMenus ();
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	void	Layout ();
		override	Boolean	DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
							  KeyComposeState& composeState);
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);

		override	Boolean	ItemCanBeEnabled ();
		override	void	SetSelected (Boolean selected, UpdateMode updateMode = eDelayedUpdate);
		override	void	EditModeChanged (Boolean newEditMode);
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);
		override	void	LanguageChanged (CommandNumber oldLanguage, CommandNumber newLanguage);
		
		nonvirtual	GroupItem&	GetMainGroup ();
		
		nonvirtual	void	SelectAllItems (Boolean select, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	DragSelected (const MousePressInfo& mouseInfo);
		
		virtual		Boolean	SubItemsCanChangeExtent () const;
	
		override	GroupItem*	GetMostSelectedGroup ();
		override	GroupItem*	GetGroupContaining (const Point& point);
	
		override	void	WriteDeclaration (class ostream& to, int tabCount);
		override	void	WriteIncludes (class ostream& to, int tabCount);
		override	void	WriteInitializer (class ostream& to, int tabCount, CommandNumber gui);
		override	void	WriteDestructor (class ostream& to, int tabCount, CommandNumber gui);
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteLayout (class ostream& to, int tabCount);
		override	void	WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteCustomizations (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		
		nonvirtual	SequenceIterator(ViewItemPtr)*	MakeItemIterator (SequenceDirection d = eSequenceForward) const;
		nonvirtual	SequenceIterator(ViewItemPtr)*	MakeFocusItemIterator (SequenceDirection d = eSequenceForward) const;

		nonvirtual	Region	CalcSelectedRegion () const;
		
		nonvirtual	void	WriteSelected (class ostream& to);

		virtual		void	SetPastePoint (const Point& point);
		virtual		Point	GetPastePoint () const;
		
		nonvirtual	void	AddRadioButton (AbstractRadioButton* button);
		nonvirtual	void	RemoveRadioButton (AbstractRadioButton* button);		
		nonvirtual	String	GetRadioBankName ();
		
		nonvirtual	void	AddSubItem (ViewItem* item, Boolean prepend = True, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	RemoveSubItem (ViewItem* item, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	ReorderSubItem (ViewItem* item, CollectionSize index = 1);

		nonvirtual	Point	GetScrollSize () const;
		nonvirtual	Point	GetScrollSize (CommandNumber language, CommandNumber gui) const;
		nonvirtual	void	SetScrollSize (const Point& scrollSize);

		virtual	String	GetFieldSizeConstantName () const;
		
		override	String	GetHeaderFileName ();
		override	void	ReorderFocusItems ();

		override	String	GetFocusItemFieldName () const;
		
	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	SetItemInfo ();
		
		override	void	SetItemBorder_ (Point newBorder);
		override	void	SetItemMargin_ (Point newMargin);

		virtual		void	ConstrainItems (const Point& sizeDelta);
	
		virtual		void			EnsureFramed ();
		nonvirtual	CollectionSize	CountSelected () const;
		
		override	void	AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI);
		override	void	RemoveParam (CommandNumber language, CommandNumber gui);
		override	void	ApplyParams (CommandNumber language, CommandNumber gui);
		
		nonvirtual	GroupParam*	FindGroupParam (CommandNumber language, CommandNumber gui) const;
		nonvirtual	GroupParam&	GetCurrentGroupParam () const;

		nonvirtual	GroupItemView&	GetGroupItemView () const;

		nonvirtual	FocusOwner&								GetTabLoop ();
		nonvirtual	String									GetTabLoopName () const;
		nonvirtual	Sequence(ViewItemPtr)&	GetFocusItems ();
		
	private:
		nonvirtual	void	WriteItem (ViewItem& item, class ostream& to, int tabCount);
		
		GroupItemView*							fGroupItemView;
		Sequence(ViewItemPtr)	fItems;
		Sequence(ViewItemPtr)	fFocusItems;
		Point									fOldSize;
		Boolean									fWriteSelectedOnly;
		RadioBank								fRadioBank;
		CollectionSize							fRadioButtonCount;
		FocusOwner								fFocusOwner;
		Sequence(GroupParamPtr)	fGroupParams;

	friend	class	SetGroupInfoCommand;
};

// should be protected
class	MagicGroupView : public View {
	public:
		MagicGroupView ();
		
		virtual	void	AddView (View* subView, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate);
		virtual	void	AddView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate);		
		virtual	void	RemoveView (View* subView, UpdateMode updateMode = eDelayedUpdate);
		virtual	void	ReorderView (View* v, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate);
		virtual	void	ReorderView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate);

		override	void	Draw (const Region& update);
};

class	GroupItemView : public Scroller {
	public:
		GroupItemView (AbstractScrollBar* verticalSlider, AbstractScrollBar* horizontalSlider, MagicGroupView* mainView);
		
		nonvirtual	MagicGroupView&	GetMainView () const;
		nonvirtual	Boolean			HasScrollBars () const;
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		
	private:
		MagicGroupView*	fMainView;
};

/*
 ********************************************************************************
 ********************************* GroupItemType ********************************
 ********************************************************************************
 */
class	GroupItemType : public ItemType {
	public:
		GroupItemType ();
		~GroupItemType ();	// only stuck in to avoid compiler bug with Apple CFront
		
		static	GroupItemType&	Get ();
		
	private:		
		static	ViewItem*	GroupItemBuilder ();
		static	GroupItemType*	sThis;
};


class	SizeItemCommand : public Command {
	public:
		SizeItemCommand (ViewItem& view, const String& name, const Point& newOrigin, const Point& newSize);
		
		override	void	DoIt ();
		override	void	UnDoIt () ;

	private:
		ViewItem&	fView;
		Point		fNewOrigin;
		Point		fOldOrigin;
		Point		fNewSize;
		Point		fOldSize;
};

#endif	/* __GroupItem__ */
