/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ScrollBarItem__
#define	__ScrollBarItem__

/*
 * $Header: /fuji/lewis/RCS/ScrollBarItem.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollBarItem.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

#include	"SliderItem.hh"

class	ScrollBar;
class	ScrollBarItem : public SliderItem {
	public:
		ScrollBarItem (ItemType& type);
	
		override	String	GetHeaderFileName ();
		nonvirtual	AbstractScrollBar&	GetScrollBar () const;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	Layout ();
	
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);
		
		virtual		void	SetScrollBar (AbstractScrollBar* scrollBar);
		
	private:
		AbstractScrollBar*	fScrollBar;
		Real				fDefaultStepSize;
		Real				fDefaultPageSize;
};

class	ScrollBarItemType : public ItemType {
	public:
		ScrollBarItemType ();
		
		static	ScrollBarItemType&	Get ();
		
	private:		
		static	ViewItem*	ScrollBarItemBuilder ();
		static	ScrollBarItemType*	sThis;
};

class	SetScrollBarInfoCommand : public SetSliderInfoCommand {
	public:
		SetScrollBarInfoCommand (ScrollBarItem& item, class ScrollBarInfo& info);

		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		ScrollBarItem&		fScrollBar;
		
		Real	fNewStepSize;
		Real	fOldStepSize;
		Real	fNewPageSize;
		Real	fOldPageSize;
		AbstractScrollBar::Orientation	fOldOrientation;
		AbstractScrollBar::Orientation	fNewOrientation;
};

#endif	/* __ScrollBarItem__ */
