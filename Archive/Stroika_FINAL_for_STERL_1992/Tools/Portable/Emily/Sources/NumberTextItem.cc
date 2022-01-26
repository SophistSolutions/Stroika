/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/NumberTextItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: NumberTextItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"

#include	"NumberText.hh"
#include	"CheckBox.hh"

#include	"CommandNumbers.hh"
#include	"NumberTextItem.hh"
#include	"ViewItemInfo.hh"
#include	"NumberTextInfo.hh"




/*
 ********************************************************************************
 ****************************** NumberTextItemType ******************************
 ********************************************************************************
 */
NumberTextItemType::NumberTextItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildTextEdit, "NumberText", (ItemBuilderProc)&NumberTextItemBuilder)
#else
	ItemType (eBuildTextEdit, "NumberText", &NumberTextItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
NumberTextItemType&	NumberTextItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	NumberTextItemType::NumberTextItemBuilder ()
{
	NumberTextItem* item = new NumberTextItem (Get ());
	item->SetTextEdit (item->fNumberText = new NumberText ());
	item->fDefaultValue 	  = item->fNumberText->GetValue ();
	item->fDefaultMinValue    = item->fNumberText->GetMinValue ();
	item->fDefaultMaxValue    = item->fNumberText->GetMaxValue ();
	item->fDefaultPrecision   = item->fNumberText->GetPrecision ();
	return (item);
}

NumberTextItemType*	NumberTextItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** NumberTextItem **********************************
 ********************************************************************************
 */
NumberTextItem::NumberTextItem (ItemType& type) :
	TextEditItem (type),
	fNumberText (Nil)
{
}

String	NumberTextItem::GetHeaderFileName ()
{
	if ((GetHasVerticalSBar ()) or (GetHasHorizontalSBar ())) {
		return (TextEditItem::GetHeaderFileName ());
	}

	static	const	String kHeaderFileName = "NumberText.hh";
	return (kHeaderFileName);
}

NumberText&	NumberTextItem::GetNumberText () const
{
	RequireNotNil (fNumberText);
	return (*fNumberText);
}

void	NumberTextItem::DoRead_ (class istream& from)
{
	TextEditItem::DoRead_ (from);

	Int16	precision;
	Real	min, max, value;
	from >> precision >> ReadReal (min) >> ReadReal (max) >> ReadReal (value);
	GetNumberText ().SetPrecision (precision);
	GetNumberText ().SetMinValue (min);
	GetNumberText ().SetMaxValue (max);
	GetNumberText ().SetValue (value);
SetMultiLine (False);
}

ostream&	space (ostream& out)
{
	return (out << ' ');
}

void	NumberTextItem::DoWrite_ (class ostream& to, int tabCount) const
{
	TextEditItem::DoWrite_ (to, tabCount);

	Int16	precision = GetNumberText ().GetPrecision ();
	Real	min = GetNumberText ().GetMinValue ();
	Real	max = GetNumberText ().GetMaxValue ();
	Real	value = GetNumberText ().GetValue ();
	
	to << tab (tabCount) << precision << space << WriteReal (min) << space << WriteReal (max);
	to << space << WriteReal (value) << newline;
}

void	NumberTextItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	TextEditItem::WriteParameters (to, tabCount, language, gui);

	static	const	String	kBadNumberString = "kBadNumber";
	if (GetNumberText ().GetPrecision () != fDefaultPrecision) {
		to << tab (tabCount) << GetFieldName () << ".SetPrecision (";
		to << GetNumberText ().GetPrecision ();
		to << ");" << newline;
	}
	if ((GetNumberText ().GetMaxValue () != fDefaultMaxValue) and (not ((IsBadNumber (fDefaultMaxValue)) and (IsBadNumber (GetNumberText ().GetMaxValue ()))))) {
		to << tab (tabCount) << GetFieldName () << ".SetMaxValue (";
		if (IsBadNumber (GetNumberText ().GetMaxValue () )) {
			to << kBadNumberString;
		}
		else {
			to << GetNumberText ().GetMaxValue ();
		}
		to << ");" << newline;
	}
	if ((GetNumberText ().GetMinValue () != fDefaultMinValue) and (not ((IsBadNumber (fDefaultMinValue)) and (IsBadNumber (GetNumberText ().GetMinValue ()))))) {
		to << tab (tabCount) << GetFieldName () << ".SetMinValue (";
		if (IsBadNumber (GetNumberText ().GetMinValue () )) {
			to << kBadNumberString;
		}
		else {
			to << GetNumberText ().GetMinValue ();
		}
		to << ");" << newline;
	}
	if ((GetNumberText ().GetValue () != fDefaultValue) and (not ((IsBadNumber (fDefaultValue)) and (IsBadNumber (GetNumberText ().GetValue ()))))) {
		to << tab (tabCount) << GetFieldName () << ".SetValue (";
		if (IsBadNumber (GetNumberText ().GetValue () )) {
			to << kBadNumberString;
		}
		else {
			to << GetNumberText ().GetValue ();
		}
		to << ");" << newline;
	}
}


void	NumberTextItem::SetItemInfo ()
{
	NumberTextInfo info = NumberTextInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetNumberTextInfoCommand (*this, info));
		DirtyDocument ();
	}
}


SetNumberTextInfoCommand::SetNumberTextInfoCommand (NumberTextItem& item, class NumberTextInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewRequireText (info.GetRequireTextField ().GetOn ()),
	fOldRequireText (item.GetNumberText ().GetTextRequired ()),
	fNewPrecision (info.GetPrecisionField ().GetValue ()),
	fOldPrecision (item.GetNumberText ().GetPrecision ()),
	fNewMaximum (info.GetMaxValueField ().GetValue ()),
	fOldMaximum (item.GetNumberText ().GetMaxValue ()),
	fNewMinimum (info.GetMinValueField ().GetValue ()),
	fOldMinimum (item.GetNumberText ().GetMinValue ()),
	fNewDefault (info.GetDefaultField ().GetValue ()),
	fOldDefault (item.GetNumberText ().GetValue ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetNumberTextInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();

	fItem.GetNumberText ().SetTextRequired (fNewRequireText);
	fItem.GetNumberText ().SetPrecision (fNewPrecision);
	fItem.GetNumberText ().SetMaxValue (fNewMaximum);
	fItem.GetNumberText ().SetMinValue (fNewMinimum);
	fItem.GetNumberText ().SetValue (fNewDefault);
	
	Command::DoIt ();
}

void	SetNumberTextInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();

	fItem.GetNumberText ().SetTextRequired (fOldRequireText);
	fItem.GetNumberText ().SetPrecision (fOldPrecision);
	fItem.GetNumberText ().SetMaxValue (fOldMaximum);
	fItem.GetNumberText ().SetMinValue (fOldMinimum);
	fItem.GetNumberText ().SetValue (fOldDefault);
	
	Command::UnDoIt ();
}
