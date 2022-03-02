/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ClassInfo__
#define	__ClassInfo__

/*
 * $Header: /fuji/lewis/RCS/ClassInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ClassInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 */

#include	"Command.hh"

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextView.hh"
#include "GroupView.hh"
#include "TextEdit.hh"
#include "CSymbolText.hh"
#include "CheckBox.hh"
#include "PushButton.hh"
#include "NumberText.hh"


class ClassInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		ClassInfoX ();
		~ClassInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		LabeledGroup		fQuickHelpGroup;
		FocusOwner			fQuickHelpGroupTabLoop;
		TextEdit			fQuickHelp;
		CSymbolText			fClass;
		TextView			fClassLabel;
		TextView			fField1;
		CSymbolText			fBaseClass;
		CheckBox			fAutoSize;
		PushButton			fFontButton;
		LabeledGroup		fScrollBounds;
		FocusOwner			fScrollBoundsTabLoop;
		NumberText			fScrollWidth;
		NumberText			fScrollHeight;
		TextView			fScrollWidthLabel;
		TextView			fScrollHeightLabel;

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
class ClassInfo : public ClassInfoX {
	public:
		ClassInfo (View& view);		
		~ClassInfo ();

		nonvirtual	TextEdit&	GetClassNameField ()
		{
			return (fClass);
		}
		
		nonvirtual	TextEdit&	GetBaseClassNameField ()
		{
			return (fBaseClass);
		}
		
		nonvirtual	NumberText&	GetSizeVField ()
		{
			return (fScrollHeight);
		}
		
		nonvirtual	NumberText&	GetSizeHField ()
		{
			return (fScrollWidth);
		}
		
		nonvirtual	TextEdit&	GetHelpField ()
		{
			return (fQuickHelp);
		}
		
		nonvirtual	CheckBox&	GetAutoSizeField ()
		{
			return (fAutoSize);
		}

	protected:
		override	void	ButtonPressed (AbstractButton* button);

	public:	
		Font*		fFont;
		View&		fView;
};
		

#endif	/* __ClassInfo__ */



