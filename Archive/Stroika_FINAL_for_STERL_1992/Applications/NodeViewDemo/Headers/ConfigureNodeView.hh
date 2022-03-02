/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ConfigureNodeView.hh,v 1.3 1992/09/01 17:30:27 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: ConfigureNodeView.hh,v $
 *		Revision 1.3  1992/09/01  17:30:27  sterling
 *		*** empty log message ***
 *
 * Revision 1.1  1992/03/26  18:48:28  sterling
 * Initial revision
 * 
 *
 */

// text before here will be retained: Do not remove or modify this line!!!

#include "Bordered.hh"
#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"


class ConfigureNodeViewX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		ConfigureNodeViewX ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize = kZeroPoint) const;
		override	void	Layout ();

		class CheckBox*			fExpandable;
		class LabeledGroup*		fLineSegmentGroup;
		class FocusOwner*		fLineSegmentGroupTabLoop;
		class NumberText*		fLSVertical;
		class NumberText*		fLSHorizontal;
		class TextView*			fHLineSpacingLabel;
		class TextView*			fVLineSpacingLabel;
		class LabeledGroup*		fDirectionGroup;
		class RadioBank*		fDirectionGroupRadioBank;
		class FocusOwner*		fDirectionGroupTabLoop;
		class RadioButton*		fVertical;
		class RadioButton*		fHorizontal;
		class LabeledGroup*		fSpacingGroup;
		class FocusOwner*		fSpacingGroupTabLoop;
		class NumberText*		fHSpacing;
		class NumberText*		fVSpacing;
		class TextView*			fVSpacingLabel;
		class TextView*			fHSpacingLabel;
		class TextView*			fField1;

	private:
#if   qMacGUI
		nonvirtual void	BuildForMacGUI ();
#else
		nonvirtual void	BuildForMacGUI ();
#endif /* GUI */

		Boolean		fInitialized;
};



// text past here will be retained: Do not remove or modify this line!!!
class BranchNode;

#include "Point.hh"

class ConfigureNodeView : public ConfigureNodeViewX {
	public:
		ConfigureNodeView (BranchNode& nodeView);
		
		Point::Direction	GetDirection () const;
		Point				GetArcLength () const;
		Point				GetSpacing () const;
		Boolean				GetExpandable () const;
};