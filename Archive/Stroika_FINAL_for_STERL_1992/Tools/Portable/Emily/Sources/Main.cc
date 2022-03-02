/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Main.cc,v 1.3 1992/09/01 17:25:44 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Main.cc,v $
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.15  1992/02/18  00:38:53  lewis
 *		Use new version stuff.
 *
 *		Revision 1.12  1992/01/27  04:29:14  sterling
 *		support for Grid
 *
 *		Revision 1.8  1992/01/22  06:06:25  lewis
 *		Use Toolkit init.
 *
 *
 *
 */





/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */

#pragma	push
#pragma	force_active	on

#include	"ToolkitInit.hh"
static	ToolkitInit	sInit	=	ToolkitInit ();
#pragma	pop

#include	"Stroika-Globals.hh"		//	To get them inited real early cuz C++ does not guarantee
										//	order of init of static objects across .o files

#include	"ApplicationVersion"
extern	const	Version	kApplicationVersion	=
#if		qMacToolkit
	Version (1);
#else
	Version (kMajorVersion, kMinorVersion, kBugFixVersion, kStage, kStageVersion, kShortVersionString, kLongVersionString);
#endif



#include	"Debug.hh"

#include	"EmilyApplication.hh"



/*
 * Since you cannot count on order of static initialization in C++ we must be very careful
 * with application static objects. Thus when a new type is added, its header must be
 * included by main, and it must have a constructor called, to add it to our item list,
 * as well as to the pallet. Unfortunate, but…
 */
#include	"ArrowButtonItem.hh"
#include	"CheckBoxItem.hh"
#include	"GroupItem.hh"
#include	"LabeledGroupItem.hh"
#include	"PixelMapButtonItem.hh"
#include	"PushButtonItem.hh"
#include	"RadioButtonItem.hh"
#include	"ScaleItem.hh"
#include	"ScrollBarItem.hh"
#include	"ScrollerItem.hh"
#include	"SliderBarItem.hh"

#include	"CSymbolTextItem.hh"
#include	"DateTextItem.hh"
#include	"NumberTextItem.hh"
#include	"TextEditItem.hh"
#include	"TextViewItem.hh"
#include	"PickListItem.hh"
#include	"GridItem.hh"
#include	"SeparatorItem.hh"
#include	"PopUpItem.hh"
#include	"MenuBarItem.hh"
#include	"PixelMapEditorItem.hh"
#include	"SimpleViewItem.hh"
#include	"ViewItemInfoItem.hh"
#include	"BarGraphItem.hh"
#include	"LayerGraphItem.hh"
#include	"LineGraphItem.hh"
#include	"PieGraphItem.hh"


int	main (int argc, const char* argv[])
{
	// add all our types
	new GroupItemType ();
	new ScrollerItemType ();
	new LabeledGroupItemType ();
	
	new ArrowButtonItemType ();
	new CheckBoxItemType ();
	new PixelMapButtonItemType ();
	new PushButtonItemType ();
	new RadioButtonItemType ();
	
	new ScaleItemType ();
	new ScrollBarItemType ();
	new SliderBarItemType ();
	
	new TextEditItemType ();
	new NumberTextItemType ();
	new DateTextItemType ();
	new CSymbolTextItemType ();

	new TextViewItemType ();
	new PickListEditItemType ();
	new GridItemType ();

	new SeparatorItemType ();

	new OptionMenuItemType ();
	new MenuBarItemType ();

	new PixelMapEditorType ();
	new SimpleViewItemType ();
	new ViewItemInfoItemType ();

	new BarGraphItemType ();
#if qMacGDI
	new LayerGraphItemType ();
#endif
	new LineGraphItemType ();
	new PieChartItemType ();
	
	EmilyApplication (argc, argv).Run ();

	return (0);
}




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***
