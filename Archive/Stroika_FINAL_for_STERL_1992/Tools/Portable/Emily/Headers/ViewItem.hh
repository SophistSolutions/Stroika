/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ViewItem__
#define	__ViewItem__

/*
 * $Header: /fuji/lewis/RCS/ViewItem.hh,v 1.7 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ViewItem.hh,v $
 *		Revision 1.7  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.11  1992/02/15  04:13:59  sterling
 *		customization stuff
 *
 *
 */

#include	"Debug.hh"

#include	"Command.hh"
#include	"KeyHandler.hh"
#include	"MenuCommandHandler.hh"
#include	"Saveable.hh"
#include	"StreamUtils.hh"
#include	"View.hh"

#include	"ResourceBased.hh"



#if		!qRealTemplatesAvailable
	typedef	class	ViewItem*	ViewItemPtr;
	
	Declare (Iterator, ViewItemPtr);
	Declare (Collection, ViewItemPtr);
	Declare (AbSequence, ViewItemPtr);
	Declare (Array, ViewItemPtr);
	Declare (Sequence_Array, ViewItemPtr);
	Declare (Sequence, ViewItemPtr);
#endif

#if		!qRealTemplatesAvailable
	typedef	class	ItemParams*	ItemParamsPtr;
	
	Declare (Iterator, ItemParamsPtr);
	Declare (Collection, ItemParamsPtr);
	Declare (AbSequence, ItemParamsPtr);
	Declare (Array, ItemParamsPtr);
	Declare (Sequence_Array, ItemParamsPtr);
	Declare (Sequence, ItemParamsPtr);
#endif



#include	"ItemType.hh"
#include	"FocusItem.hh"


class	GroupItem;
class	ItemType;
class	ItemSelector;
class	ViewItem : public View, public FocusOwner, public Saveable {
	protected:
		ViewItem (const ItemType& itemType);
	public:
		~ViewItem ();

		override	Boolean	DoCommand (const CommandSelection& selection);
		override	void	DoSetupMenus ();

		override	void	Layout ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		
		virtual		Boolean	IsButton () = Nil;
		virtual		Boolean	IsSlider () = Nil;
		virtual		Boolean	IsText () = Nil;
		virtual		Boolean	IsFocusItem (CommandNumber gui) = Nil;

#if qUseResourceBased
		virtual		Boolean	IsResourceBased () const;
#endif

		virtual		Point	GetMinSize () const;
		virtual		Point	GetMaxSize () const;

		nonvirtual	GroupItem*	GetGroup () const;
		virtual		void		SetGroup (GroupItem* parent);

		nonvirtual	View&	GetOwnedView () const;
 
		virtual		void	EditModeChanged (Boolean newEditMode);
		virtual		void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);
		virtual		void	LanguageChanged (CommandNumber oldLanguage, CommandNumber newLanguage);
		
		nonvirtual	String	GetBaseClass () const;

		nonvirtual	String	GetFieldName () const;
		nonvirtual	void	SetFieldName (const String& fieldName);

		nonvirtual	String	GetFieldClass () const;
		nonvirtual	void	SetFieldClass (const String& fieldName);
		
		nonvirtual	String	GetHelp () const;
		nonvirtual	void	SetHelp (const String& help);

		nonvirtual	Boolean	GetSelected () const;
		virtual		void	SetSelected (Boolean selected, UpdateMode updateMode = eDelayedUpdate);

		virtual		Boolean	ItemCanBeEnabled () = Nil;

		nonvirtual	Point	GetItemOrigin () const;
		nonvirtual	void	SetItemOrigin (const Point& origin);

		nonvirtual	Point	GetItemSize () const;
		nonvirtual	void	SetItemSize (const Point& size);
	
		nonvirtual	Boolean	GetItemEnabled () const;
		nonvirtual	void	SetItemEnabled (Boolean enabled);

		nonvirtual	Boolean	GetItemVisible () const;
		nonvirtual	void	SetItemVisible (Boolean visible);

		nonvirtual	Boolean	GetConstrainLeft () const;
		nonvirtual	void	SetConstrainLeft (Boolean constrain);

		nonvirtual	Boolean	GetConstrainTop () const;
		nonvirtual	void	SetConstrainTop (Boolean constrain);

		nonvirtual	Boolean	GetConstrainHeight () const;
		nonvirtual	void	SetConstrainHeight (Boolean constrain);

		nonvirtual	Boolean	GetConstrainWidth () const;
		nonvirtual	void	SetConstrainWidth (Boolean constrain);
		
		nonvirtual	Point	GetItemBorder () const;
		nonvirtual	void	SetItemBorder (Point border);

		nonvirtual	Point	GetItemMargin () const;
		nonvirtual	void	SetItemMargin (Point newMargin);

		nonvirtual	Point	GetDefaultBorder () const;
		nonvirtual	void	SetDefaultBorder (Point newBorder);

		nonvirtual	Point	GetDefaultMargin () const;
		nonvirtual	void	SetDefaultMargin (Point newMargin);
		
		nonvirtual	Boolean	UsesBorders ();

		nonvirtual	const Font*	GetItemFont () const;
		nonvirtual	void		SetItemFont (const Font* font);

		virtual		void	DirtyDocument ();

		virtual		Boolean		CanChangeExtent () const;

		// used by GroupItem, normally not overridden
		virtual	GroupItem*	GetMostSelectedGroup ();
		virtual	GroupItem*	GetGroupContaining (const Point& point);
		
		// used to generate correct dependencies
		virtual	String	GetHeaderFileName () = Nil;

		// writes our "MyClass*	fMyFieldName;" (rarely overridden)
		virtual		void	WriteDeclaration (class ostream& to, int tabCount);

		// writes our "#include "MyClass.hh"" (rarely overridden)
		virtual		void	WriteInclude (class ostream& to, int tabCount);
		
		// used by groups, recursively write out all required includes
		virtual		void	WriteIncludes (class ostream& to, int tabCount);

		// calls WriteParameters  (rarely overridden)
		virtual		void	WriteInitializer (class ostream& to, int tabCount, CommandNumber gui);
		
		// writes "fMyFieldName "
		// subclasses should override to write a "(", then
		// fill in any construction arguments needed,
		// followed by a terminating "),"
		virtual		void	WriteBuilder (class ostream& to, int tabCount);

		virtual		void	WriteDestructor (class ostream& to, int tabCount, CommandNumber gui);
		
		// write out any other method calls needed for proper construction, but
		// not passed into constructor. By default sets extent, help, and font
		virtual		void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		
		// utility routine, use when a parameter call requires a font
		nonvirtual	void	WriteFont (class ostream& to, UInt8 tabCount, 
									   const Font* font, const Font* defaultFont,
									   const String& methodName, const String& objectName,
									   Boolean byAddress);

		// used by GroupItem, normally not overridden
		virtual		void	WriteLayout (class ostream& to, int tabCount);
		
		virtual		void	WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		
		// used by GroupItem, normally not overridden
		virtual		void	WriteCustomizations (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		
		virtual		void	AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI);
		virtual		void	RemoveParam (CommandNumber language, CommandNumber gui);
		virtual		void	ApplyParams (CommandNumber language, CommandNumber gui);

		nonvirtual	void	ApplyCurrentParams ();
		nonvirtual	void	DeleteCustomizations ();
		nonvirtual	Boolean	ParamsExist (CommandNumber language, CommandNumber gui) const;
		
		virtual	String	GetSubviewFieldName () const;
		virtual	String	GetFocusItemFieldName () const;
		
		// really only used by group
		virtual	void	ReorderFocusItems ();
		
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

		virtual		void	SetOwnedEnabled (Boolean enabled, UpdateMode updateMode);

		nonvirtual	void	SetOwnedView (View* ownedView);
		virtual		void	SetOwnedView_ (View* ownedView);
		virtual		void	SetSelected_ (Boolean selected, UpdateMode update);
		
		virtual		void	SetItemInfo ();
		virtual		String	CalcFieldName ();
		
		nonvirtual	SequenceIterator(ItemParamsPtr)*	MakeParamsIterator (SequenceDirection d = eSequenceForward) const;

// needed to support old param support (used in pushbutton, checkbox, and radiobutton items)
virtual		void		OldParamReadHack (class istream& from);

		nonvirtual	CommandNumber	GetBaseLanguage () const;
		nonvirtual	CommandNumber	GetBaseGUI () const;
				
		virtual	void	SetItemBorder_ (Point newBorder);
		virtual	void	SetItemMargin_ (Point newMargin);
		
		nonvirtual	void	SetDefaultsFromView (View& v);

		nonvirtual	ItemParams*		FindParams (CommandNumber language, CommandNumber gui) const;

		virtual		void			AutoCustomize (CommandNumber language, CommandNumber gui);
		
		nonvirtual	void	DeclareField (class ostream& to, int tabCount, const String& className, const String& fieldName);
		
		static	const	String	kEmptyInitializer;
		static	const	String	kFieldAccessor;
		
	private:
		nonvirtual	void			CopyParams (ItemParams* oldParams, ItemParams* newParams);
		nonvirtual	ItemParams&		GetCurrentParams () const;
		
		nonvirtual	ItemSelector&	GetSelector () const;
		
		View*			fOwnedView;
		GroupItem*		fGroup;
		const ItemType&	fItemType;
		ItemSelector*	fSelector;
		const Font*		fDefaultFont;
		
		Boolean			fSelected;
		String			fFieldName;
		String			fFieldClass;
		Boolean			fFieldNameTemporary;
		
		Boolean		fConstrainLeft;
		Boolean		fConstrainTop;
		Boolean		fConstrainHeight;
		Boolean		fConstrainWidth;

		CommandNumber	fBaseLanguage;
		CommandNumber	fBaseGUI;
		
		Sequence(ItemParamsPtr)	fParams;
		class ItemParams*		fCurrentParams;
		
	friend	class	ViewInfo;
};

class	ItemParams : public Saveable {
	public:
		ItemParams (CommandNumber language, CommandNumber gui);
		virtual	~ItemParams ();
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		Point		fOrigin;
		Point		fSize;
		Point		fBorder;
		Point		fMargin;
		Point		fDefaultBorder;
		Point		fDefaultMargin;
		Boolean		fEnabled;
		Boolean		fVisible;

		Font*		fFont;
		
		String		fHelp;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};


// utility functions
String	GetBooleanAsText (Boolean b);

#include "TextView.hh"
String	GetJustificationAsText (AbstractTextView::Justification just);

// InLines
inline	CommandNumber	ViewItem::GetBaseLanguage () const
{
	return (fBaseLanguage);
}

inline	CommandNumber	ViewItem::GetBaseGUI () const
{
	return (fBaseGUI);
}

#endif	/* __ViewItem__ */
