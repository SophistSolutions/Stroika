/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ReorderFocusItems.hh,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: ReorderFocusItems.hh,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/21  18:33:08  sterling
 *		added
 *
 *		Revision 1.7  92/04/08  17:22:02  17:22:02  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif.
 *		
 *		Revision 1.2  1992/03/06  21:53:47  sterling
 *		motif
 *
 *
 */

#include "PickList.hh"

class	ReorderFocusItem;
class ReorderFIPickList : public StringPickList {
	public:
		ReorderFIPickList (ButtonController* controller = Nil);
		
		override	void	ReorderItem (AbstractPickListItem* item, CollectionSize index);
		override	void	ReorderItem (AbstractPickListItem* item, AbstractPickListItem* neighbor, AddMode append = eAppend);

		nonvirtual	void	SetOwner (ReorderFocusItem* owner);
	
	private:
		ReorderFocusItem*	fOwner;
};

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "View.hh"

#include "PickList.hh"
#include "TextEdit.hh"


class ReorderFocusItemX : public View, public ButtonController, public FocusOwner {
	public:
		ReorderFocusItemX ();
		~ReorderFocusItemX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		ReorderFIPickList	fItems;
		TextEdit			fTitle;

	private:
#if   qMacUI
		nonvirtual void	BuildForMacUI ();
#elif qMotifUI
		nonvirtual void	BuildForMotifUI ();
#else
		nonvirtual void	BuildForUnknownGUI ();
#endif /* GUI */

};



// text past here will be retained: Do not remove or modify this line!!!
#include	"GroupItem.hh"

class	ReorderFocusItem : public ReorderFocusItemX {
	public:
		ReorderFocusItem (GroupItem& group);
		
		nonvirtual	SequenceIterator(ViewItemPtr)*	MakeFocusItemIterator (SequenceDirection d = eSequenceForward) const;
	
		nonvirtual	void	ListOrderChanged (CollectionSize oldIndex, CollectionSize newIndex);
		
	private:
		Sequence(ViewItemPtr)	fFocusItems;
		GroupItem&				fGroup;
};

