/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SliderBarItem.cc,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderBarItem.cc,v $
 *		Revision 1.5  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"SliderBar.hh"

#include	"SliderBarItem.hh"
#include	"SliderBarInfo.hh"
#include	"CommandNumbers.hh"






/*
 ********************************************************************************
 ****************************** SliderBarItemType *******************************
 ********************************************************************************
 */
SliderBarItemType::SliderBarItemType () :
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildSlider, "SliderBar", (ItemBuilderProc)&SliderBarItemBuilder)
#else
	ItemType (eBuildSlider, "SliderBar", &SliderBarItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
SliderBarItemType&	SliderBarItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	SliderBarItemType::SliderBarItemBuilder ()
{
	return (new SliderBarItem (Get ()));
}

SliderBarItemType*	SliderBarItemType::sThis = Nil;






/*
 ********************************************************************************
 ****************************** SliderBarItem ***********************************
 ********************************************************************************
 */
SliderBarItem::SliderBarItem (ItemType& type) :
	SliderItem (type),
	fSliderBar (Nil),
	fDefaultTicks (0),
	fDefaultSubTicks (0)
{
	SetSliderBar (new SliderBar ());
}

SliderBar&	SliderBarItem::GetSliderBar () const
{
	AssertNotNil (fSliderBar);
	return (*fSliderBar);
}

void	SliderBarItem::SetSliderBar (SliderBar* sliderBar)
{
	SetSlider (sliderBar);
	fSliderBar = sliderBar;
	if (fSliderBar != Nil) {
		fDefaultTicks = GetSliderBar ().GetTickSize ();
		fDefaultSubTicks = GetSliderBar ().GetSubTickSize ();
	}
}

String	SliderBarItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "SliderBar.hh";
	return (kHeaderFileName);
}

void	SliderBarItem::WriteBuilder (class ostream& to, int tabCount)
{
	SliderItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	SliderBarItem::DoRead_ (class istream& from)
{
	SliderItem::DoRead_ (from);
	Real	ticks;
	Real	subTicks;
	from >> ReadReal (ticks) >> ReadReal (subTicks);
	GetSliderBar ().SetTickSize (ticks, eNoUpdate);
	GetSliderBar ().SetSubTickSize (subTicks, eNoUpdate);
}

void	SliderBarItem::DoWrite_ (class ostream& to, int tabCount) const
{
	SliderItem::DoWrite_ (to, tabCount);
	to << tab (tabCount) << WriteReal (GetSliderBar ().GetTickSize ()) << ' ' << WriteReal (GetSliderBar ().GetSubTickSize ());
	to << newline;
}

void	SliderBarItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	SliderItem::WriteParameters (to, tabCount, language, gui);
	if (fDefaultTicks != GetSliderBar ().GetTickSize ()) {
		to << tab (tabCount) << GetFieldName () << ".SetTickSize (" << 
			GetSliderBar ().GetTickSize () << ", eNoUpdate);" << newline;
	}
	if (fDefaultSubTicks != GetSliderBar ().GetSubTickSize ()) {
		to << tab (tabCount) << GetFieldName () << ".SetSubTickSize (" << 
			GetSliderBar ().GetSubTickSize () << ", eNoUpdate);" << newline;
	}
}

void	SliderBarItem::SetItemInfo ()
{
	SliderBarInfo info = SliderBarInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetSliderBarInfoCommand (*this, info));
		DirtyDocument ();
	}
}



SetSliderBarInfoCommand::SetSliderBarInfoCommand (SliderBarItem& item, class SliderBarInfo& info) :
	SetSliderInfoCommand (
		item, 
		info.GetMinValueField ().GetValue (),
		info.GetMaxValueField ().GetValue (),
		info.GetValueField ().GetValue (),
		info.GetViewItemInfo ()),
	fSliderBar (item),
	fNewTickSize (info.GetTicks ()),
	fOldTickSize (item.GetSliderBar ().GetTickSize ()),
	fNewSubTickSize (info.GetSubTicks ()),
	fOldSubTickSize (item.GetSliderBar ().GetSubTickSize ())
{
}
		
void	SetSliderBarInfoCommand::DoIt ()
{
	fSliderBar.GetSliderBar ().SetTickSize (fNewTickSize);
	fSliderBar.GetSliderBar ().SetSubTickSize (fNewSubTickSize);
	SetSliderInfoCommand::DoIt ();
}

void	SetSliderBarInfoCommand::UnDoIt ()
{
	fSliderBar.GetSliderBar ().SetTickSize (fOldTickSize);
	fSliderBar.GetSliderBar ().SetSubTickSize (fOldSubTickSize);
	SetSliderInfoCommand::UnDoIt ();
}
