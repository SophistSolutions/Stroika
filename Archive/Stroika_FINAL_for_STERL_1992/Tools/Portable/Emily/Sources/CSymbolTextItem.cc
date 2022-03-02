/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CSymbolTextItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: CSymbolTextItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"

#include	"CSymbolText.hh"

#include	"CommandNumbers.hh"
#include	"CSymbolTextItem.hh"

/*
 ********************************************************************************
 ****************************** CSymbolTextItemType *****************************
 ********************************************************************************
 */
CSymbolTextItemType::CSymbolTextItemType ():
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildTextEdit, "CSymbolText", (ItemBuilderProc)&CSymbolTextItemBuilder)
#else
	ItemType (eBuildTextEdit, "CSymbolText", &CSymbolTextItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
CSymbolTextItemType&	CSymbolTextItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	CSymbolTextItemType::CSymbolTextItemBuilder ()
{
	CSymbolTextItem* item = new CSymbolTextItem (Get ());
	item->SetTextEdit (item->fCSymbolText = new CSymbolText ());
	return (item);
}

CSymbolTextItemType*	CSymbolTextItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** CSymbolTextItem **********************************
 ********************************************************************************
 */
CSymbolTextItem::CSymbolTextItem (ItemType& type) :
	TextEditItem (type),
	fCSymbolText (Nil)
{
}

String	CSymbolTextItem::GetHeaderFileName ()
{
	if ((GetHasVerticalSBar ()) or (GetHasHorizontalSBar ())) {
		return (TextEditItem::GetHeaderFileName ());
	}

	static	const	String kHeaderFileName = "CSymbolText.hh";
	return (kHeaderFileName);
}


