/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ViewItemInfo__
#define	__ViewItemInfo__

/*
 * $Header: /fuji/lewis/RCS/ViewItemInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 *	$Log: ViewItemInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  1992/01/31  18:24:50  sterling
 *		Bootstrapped
 *
 *
 *
 */

#include	"Command.hh"

#include	"GroupView.hh"

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "GroupView.hh"
#include "TextEdit.hh"
#include "CSymbolText.hh"
#include "TextView.hh"
#include "NumberText.hh"
#include "PushButton.hh"


class ViewItemInfo_X : public GroupView, public ButtonController, public FocusOwner, public TextController {
	public:
		ViewItemInfo_X ();
		~ViewItemInfo_X ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		LabeledGroup		fQuickHelpGroup;
		FocusOwner			fQuickHelpGroupTabLoop;
		TextEdit			fQuickHelp;
		CSymbolText			fField;
		CSymbolText			fClass;
		LabeledGroup		fMarginGroup;
		FocusOwner			fMarginGroupTabLoop;
		TextView			fMarginHeightLabel;
		TextView			fMarginWidthLabel;
		NumberText			fMarginHeight;
		NumberText			fMarginWidth;
		LabeledGroup		fBorderGroup;
		FocusOwner			fBorderGroupTabLoop;
		TextView			fBorderHeightLabel;
		NumberText			fBorderWidth;
		NumberText			fBorderHeight;
		TextView			fBorderWidthLabel;
		LabeledGroup		fBounds;
		FocusOwner			fBoundsTabLoop;
		TextView			fBoundsWidthLabel;
		TextView			fBoundsHeightLabel;
		TextView			fBoundsLeftLabel;
		NumberText			fWidth;
		NumberText			fHeight;
		TextView			fBoundsTopLabel;
		NumberText			fLeft;
		NumberText			fTop;
		TextView			fFieldLabel;
		TextView			fClassLabel;
		PushButton			fFontButton;
		PushButton			fColor;

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
#include	"ViewItem.hh"

class	ViewItemInfo : public ViewItemInfo_X {
	public:
		ViewItemInfo ();		
		~ViewItemInfo ();

		nonvirtual	NumberText&	GetTop ()
		{
			return (fTop);
		}
		
		nonvirtual	NumberText&	GetLeft ()
		{
			return (fLeft);
		}
		
		nonvirtual	NumberText&	GetHeight ()
		{
			return (fHeight);
		}
		
		nonvirtual	NumberText&	GetWidth ()
		{
			return (fWidth);
		}
		
		nonvirtual	NumberText&	GetBorderWidth ()
		{
			return (fBorderWidth);
		}
		
		nonvirtual	NumberText&	GetBorderHeight ()
		{
			return (fBorderHeight);
		}
		
		nonvirtual	NumberText&	GetMarginHeight ()
		{
			return (fMarginHeight);
		}
		
		nonvirtual	NumberText&	GetMarginWidth ()
		{
			return (fMarginWidth);
		}
		
		nonvirtual	TextEdit&	GetClass ()
		{
			return (fClass);
		}
		
		nonvirtual	TextEdit&	GetField ()
		{
			return (fField);
		}
		
		nonvirtual	TextEdit&	GetQuickHelp ()
		{
			return (fQuickHelp);
		}
				
		nonvirtual	void	SetUpFromView (class ViewItem& item);

	protected:
		override	void	ButtonPressed (AbstractButton* button);
		
	public:	
		Font*		fFont;
		class	ViewItem*	fItem;
};

class	SetItemInfoCommand : public Command {
	public:
		SetItemInfoCommand (class ViewItem& item, ViewItemInfo& info);
		~SetItemInfoCommand ();
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		ViewItem&	fItem;
		
		String	fNewClassName;
		String	fOldClassName;
		String	fNewFieldName;
		String	fOldFieldName;
		Point	fNewOrigin;
		Point	fOldOrigin;
		Point	fNewSize;
		Point	fOldSize;
		Point	fOldBorder;
		Point	fNewBorder;
		Point	fOldMargin;
		Point	fNewMargin;
		String	fNewHelp;
		String	fOldHelp;
		const Font*	fNewFont;
		const Font*	fOldFont;
};
		

#endif	/* __ViewItemInfo__ */



