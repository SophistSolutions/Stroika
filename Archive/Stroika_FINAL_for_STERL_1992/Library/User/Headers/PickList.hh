/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PickList__
#define	__PickList__

/*
 * $Header: /fuji/lewis/RCS/PickList.hh,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *
 * Design notes:
 *		We use ownership of the list, rather than inheritance, because it would be too easy
 *		for someone to do something undetectibly bad to our list, like SetAt (), and that
 *		would be bad.  This approach is slightly less flexible, but far safer.  Really it is
 *		only a minor inconvienience since most reasoably accessors are provided, and there is
 *		a protected interface giving full access to the list.
 *
 *		We use AbstractPickListItem as the basic entity we store in a picklist rather than view because
 *		we want to assure when they get deleted, however they get deleted we remove them from
 *		our list.  Again, safety, over convienience is chosen.  Again though, it is really
 *		only a minor inconvienience to have to subclass, and own the view you wish to put in
 *		a picklist.
 *
 *		Although there is no known dependence on this, we try to keep the subview list in the
 *		same order as the picklist item list.
 *
 *
 * TODO:
 *		-- needs lots of reworking - need some interface that works withotu subviews - and maybe without
 *		explicit count of subitems (for extenral database?).
 *
 *		-- template based impelemention???
 *
 *		-- dont use EnableView
 *
 *		-- probably do need to be FocusItem - at least under some circumstances???
 *
 *		-- need interface where we can do native/portable - also, probably should move this
 *		   to framework once weve cleaned it up a bit.
 *
 * Changes:
 *	$Log: PickList.hh,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:02:57  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/02  05:09:34  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.12  1992/06/09  15:52:25  sterling
 *		killed AdjustStepandPageSizes
 *
 *		Revision 1.11  1992/03/26  09:24:26  lewis
 *		Got rid of oldLive first argument to EffectiveLiveChanged () method.
 *
 *		Revision 1.10  1992/03/19  20:47:08  sterling
 *		made focusitem usder mouseless interface
 *
 *		Revision 1.9  1992/03/13  16:03:54  lewis
 *		Reorganize and add comments, and #include EnableView since thats been moved to the user layer.
 *
 *		Revision 1.8  1992/03/05  21:46:06  sterling
 *		support for borders
 *
 *		Revision 1.7  1992/02/15  06:21:17  sterling
 *		use EffectiveLiveChanges
 *
 *
 *
 */


#include	"Sequence.hh"

#include	"Button.hh"
#include	"Scroller.hh"
#include	"TextView.hh"

#include	"EnableView.hh"







class	AbstractPickList;
class	AbstractPickListItem;
class	PickListItemContainer;
class	PickListScroller;

#if		!qRealTemplatesAvailable
	typedef	AbstractPickListItem*	PLstItmPtr;
	Declare (Iterator, PLstItmPtr);
	Declare (Collection, PLstItmPtr);
	Declare (AbSequence, PLstItmPtr);
	Declare (Array, PLstItmPtr);
	Declare (Sequence_Array, PLstItmPtr);
	Declare (Sequence, PLstItmPtr);
#endif


#if qMouselessInterface
class	AbstractPickList : public EnableView, public ButtonController, public FocusItem {
#else
class	AbstractPickList : public EnableView, public ButtonController {	
#endif
	public:
		static	const	Boolean	kSelected;
		virtual ~AbstractPickList ();

		override	void	Layout ();
		override	void	Draw (const Region& update);

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		//	Manage list of PickList items.
		// note that in AddItem we return the item we were passed. This is deliberate, cuz we
		// shadow the AddItem routine in subclasses where getting back the AbstractPickListItem
		// can be very useful. Eventually, we will use templates to replace shadowing
		//	virtual	AbstractPickListItem*	AddItem (<T> item, CollectionSize index);
		virtual	AbstractPickListItem*	AddItem (AbstractPickListItem* item, CollectionSize index = kBadSequenceIndex);
		virtual	AbstractPickListItem*	AddItem (AbstractPickListItem* item, AbstractPickListItem* neighbor, AddMode append = eAppend);
		
		virtual	void	RemoveItem (AbstractPickListItem* item);
		
		virtual	void	ReorderItem (AbstractPickListItem* item, CollectionSize index);
		virtual	void	ReorderItem (AbstractPickListItem* item, AbstractPickListItem* neighbor, AddMode append = eAppend);

		nonvirtual	CollectionSize	GetItemCount () const;
		nonvirtual	CollectionSize	GetItemIndex (AbstractPickListItem* item);
		nonvirtual	AbstractPickListItem*	GetItemByIndex (CollectionSize index);

		nonvirtual	void	Delete (AbstractPickListItem* item);
		nonvirtual	void	DeleteAll ();

		nonvirtual	SequenceIterator(PLstItmPtr)*	MakeItemIterator (SequenceDirection d = eSequenceForward);

		nonvirtual	void	SelectAll (Boolean selected = True, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Boolean		GetAllowMultipleSelections () const;
		virtual		void		SetAllowMultipleSelections (Boolean allowMultipleSelections);

		nonvirtual	Boolean	GetItemsFloat () const;
		virtual		void	SetItemsFloat (Boolean floating = True);

		nonvirtual	AbstractPickListItem*	GetSelectedItem (UInt32 i = 1);	// nil if past end
		virtual		void					ScrollToSelected ();			// to first, if many...
		nonvirtual	UInt32					CountSelected () const;

		virtual		void	ItemSelected (AbstractPickListItem* item);
		
		nonvirtual	AbstractPickListItem*	PointInWhichItem (const Point& where);

		nonvirtual	PickListItemContainer&						GetScrolledView () const;

#if qMouselessInterface
		override	Boolean	GetLive () const;
#endif

	protected:
		AbstractPickList (ButtonController* controller = Nil);

		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		nonvirtual	Sequence(PLstItmPtr)&	GetPickListItems () const;
		nonvirtual	Scroller&								GetScroller () const;

		virtual		void	LayoutItems ();		// separated out of layout for easy customization

	private:
		Sequence(PLstItmPtr)	fPickListItems;
		Scroller*							fScroller;
		PickListItemContainer*				fScrolledView;
		Boolean								fAllowMultipleSelections;
		Boolean								fItemsFloat;
		UInt16								fSelected;
	friend	class	PickListItemContainer;
	friend	class	PickListScroller;
#if		0
	friend	class	HeirarchicalPickListItem;	// cuz don't have proper SmartIterators
#endif
};




class	AbstractPickListItem : public EnableView {			// SB Scoped in AbstractPickList
	public:
		static	const	Coordinate	kUseViewDefaultHeight;
		virtual ~AbstractPickListItem ();

		override	void	Draw (const Region& update);

		nonvirtual	AbstractPickList&	GetPickList () const;

		virtual	Coordinate	GetLineHeight () const	= Nil;
		virtual	void		SetLineHeight (Coordinate lineHeight, UpdateMode updateMode = eDelayedUpdate) =  Nil;

		nonvirtual	Boolean	GetSelected () const;
		virtual		void	SetSelected (Boolean selected = True, UpdateMode updateMode = eDelayedUpdate);

	protected:
		AbstractPickListItem ();

	private:
		AbstractPickList*	fPickList;			// filled in by AbstractPickList class
		Boolean				fSelected;
		
	friend	class	AbstractPickList;
};


class	PickListItemContainer : public GroupView {		// SB Scoped in AbstractPickList
	public:
		PickListItemContainer (AbstractPickList& pickList);
		~PickListItemContainer ();

		override	void	Layout ();

	private:
		AbstractPickList&	fPickList;
};


class	PickListScroller : public Scroller {		// SB Scoped in AbstractPickList
	public:
		PickListScroller (AbstractPickList& pickList);

	protected:
		override	void	AdjustStepAndPageSizes ();

	private:
		AbstractPickList&	fPickList;
};




/*
 * Use this if you dont want to subclass, and just want to stick any old kind of view
 * into a picklist.
 */
class	PickList : public AbstractPickList {
	public:
		PickList (ButtonController* controller = Nil);

		/*
		 * Manage list of PickList items (same as above - only here for views contained in the items).
		 */
		override	AbstractPickListItem*	AddItem (View* item, CollectionSize index = kBadSequenceIndex);
		override	AbstractPickListItem*	AddItem (View* item, AbstractPickListItem* neighbor, AddMode append = eAppend);
		
		override	void	RemoveItem (View* item);
		
		override	void	ReorderItem (View* item, CollectionSize index);
		override	void	ReorderItem (View* item, AbstractPickListItem* neighbor, AddMode append = eAppend);

		nonvirtual	View*	GetItemByIndex (CollectionSize index);

		nonvirtual	AbstractPickListItem*	GetItemFromView (View* view);
};


// Just put any view in here, and you can avoid subclassing AbstractPickListItem...
class	PickListItem : public AbstractPickListItem {		// SB scoped in PickList
	public:
		PickListItem (View* itsView);
		virtual ~PickListItem ();

		override	void	Layout ();
		override	void	Draw (const Region& update);
		override	void	SetSelected (Boolean selected = True, UpdateMode updateMode = eDelayedUpdate);

		override	Coordinate	GetLineHeight () const;
		override	void		SetLineHeight (Coordinate lineHeight, UpdateMode updateMode = eDelayedUpdate);

	private:
		Coordinate	fLineHeight;
		View*		fItsView;
		View*		fMagicView;		// for handling grayout, and selection - since
									// sad order of draw - us then children...
	friend	class	MyMagicView;
	friend	class	PickList;	// peeks at fItsView;
};



class StringPickListItem;
class	StringPickList : public AbstractPickList {
	public:
		StringPickList (ButtonController* controller = Nil);

		nonvirtual	String	GetSelectedString (UInt32 i = 1);	// kEmptyString if past end

		override	StringPickListItem*	AddItem (String s, CollectionSize index = kBadSequenceIndex);
		override	StringPickListItem*	AddItem (String s, AbstractPickListItem* neighbor, AddMode append = eAppend);

		nonvirtual	String				GetStringByIndex (CollectionSize index);
		nonvirtual	StringPickListItem*	StringToItem (const String& s, CollectionSize startIndex = 1);
};



class	StringPickListItem : public AbstractPickListItem {		// SB scoped in StringPickList
	public:
		StringPickListItem (const String& text);
		~StringPickListItem ();

		nonvirtual	String	GetText () const;
		virtual		void	SetText (const String& text, UpdateMode updateMode = eDelayedUpdate);

		override	void	Draw (const Region& update);

		override	Coordinate	GetLineHeight () const;
		override	void		SetLineHeight (Coordinate lineHeight, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Coordinate	GetLeftMargin () const;
		nonvirtual	void		SetLeftMargin (Coordinate margin, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	StringPickList&	GetPickList () const;
	private:
		String		fText;
		Coordinate	fLineHeight;
		Coordinate	fLeftMargin;
};

class	PixelMapStringPickListItem : public StringPickListItem {
	public:
		PixelMapStringPickListItem (const String& text, const PixelMap* icon = Nil);
		~PixelMapStringPickListItem ();
		
		override	void	Draw (const Region& update);

		override	Coordinate	GetLineHeight () const;
		
		nonvirtual	PixelMap	GetPixelMap () const;
		virtual		void		SetPixelMap (const PixelMap* pixelMap, UpdateMode updateMode = eDelayedUpdate);

	private:
		PixelMap*	fPixelMap;
};

#if 0
// doesn't work cuz sizeof (SubItemIterator) != sizeof (SequenceIterator(PLstItmPtr))
class	HeirarchicalPickListItem;
class	SubItemIterator : public SequencePtrIterator(PLstItmPtr) {
	public:
		SubItemIterator (const SequencePtr(PLstItmPtr)& iterateOver, HeirarchicalPickListItem& item);

		override	Boolean	Done ();
		override	void	Next ();
	private:
		HeirarchicalPickListItem&	fItem;
		CollectionSize				fIndex;
		CollectionSize				fStartIndex;
};
#endif

class	HeirarchicalPickListItem : public StringPickListItem {
	public:
		HeirarchicalPickListItem (const String& text);
		
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		virtual		HeirarchicalPickListItem*	AddSubItem (HeirarchicalPickListItem* item, CollectionSize index = kBadSequenceIndex);
		virtual		HeirarchicalPickListItem*	AddSubItem (const String& s, CollectionSize index = kBadSequenceIndex);
		virtual		HeirarchicalPickListItem*	AddSubItem (const String& s, HeirarchicalPickListItem* neighbor, AddMode append = eAppend);
		virtual		void						RemoveSubItem (const String& s);
		
		nonvirtual	Boolean	GetExpanded () const;
		virtual		void	SetExpanded (Boolean expanded, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	HeirarchicalPickListItem*	GetParentItem () const;
		nonvirtual	UInt16						GetSubItemCount () const;
#if 0		
		virtual	SequenceIterator(PLstItmPtr)*	MakeSubItemIterator ();
#endif
	private:
		Boolean						fExpanded;
		HeirarchicalPickListItem*	fParentItem;
		UInt16						fSubItemCount;
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __PickList__ */
