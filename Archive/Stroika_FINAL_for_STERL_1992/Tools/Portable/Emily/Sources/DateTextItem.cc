/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DateTextItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DateTextItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"

#include	"DateText.hh"

#include	"CommandNumbers.hh"
#include	"DateTextItem.hh"

/*
 ********************************************************************************
 ****************************** DateTextItemType ******************************
 ********************************************************************************
 */
DateTextItemType::DateTextItemType () :
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildTextEdit, "DateText", (ItemBuilderProc)&DateTextItemBuilder)
#else
	ItemType (eBuildTextEdit, "DateText", &DateTextItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
DateTextItemType&	DateTextItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	DateTextItemType::DateTextItemBuilder ()
{
	DateTextItem* item = new DateTextItem (Get ());
	item->SetTextEdit (item->fDateText = new DateText ());
	return (item);
}

DateTextItemType*	DateTextItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** DateTextItem **********************************
 ********************************************************************************
 */
DateTextItem::DateTextItem (ItemType& type) :
	TextEditItem (type),
	fDateText (Nil)
{
}

String	DateTextItem::GetHeaderFileName ()
{
	if ((GetHasVerticalSBar ()) or (GetHasHorizontalSBar ())) {
		return (TextEditItem::GetHeaderFileName ());
	}

	static	const	String kHeaderFileName = "DateText.hh";
	return (kHeaderFileName);
}

void	DateTextItem::DoRead_ (class istream& from)
{
	TextEditItem::DoRead_ (from);

	Date	minDate, maxDate;
	from >> minDate >> maxDate;
	fDateText->SetMinDate (minDate);
	fDateText->SetMaxDate (maxDate);
}

void	DateTextItem::DoWrite_ (class ostream& to, int tabCount) const
{
	TextEditItem::DoWrite_ (to, tabCount);
	to << tab (tabCount) << fDateText->GetMinDate () << ' ' << fDateText->GetMaxDate () << newline;
}

void	DateTextItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	TextEditItem::WriteParameters (to, tabCount, language, gui);
	
	static	const	String	argSeperator = ", ";
	if (fDateText->GetMinDate () != DateText::kBadDate) {
		Date::MonthOfYear month;
		Date::DayOfMonth day;
		Date::Year year;
		
		fDateText->GetMinDate ().mdy (month, day, year);
		to << tab (tabCount) << GetFieldName () << ".SetMinDate (Date (" <<
		int (month) << argSeperator << int (day) << argSeperator << int (year) << "));" << newline;
	}
	if (fDateText->GetMaxDate () != DateText::kBadDate) {
		Date::MonthOfYear month;
		Date::DayOfMonth day;
		Date::Year year;
		
		fDateText->GetMaxDate ().mdy (month, day, year);
		to << tab (tabCount) << GetFieldName () << ".SetMaxDate (Date (" <<
		int (month) << argSeperator << int (day) << argSeperator << int (year) << "));" << newline;
	}
}

