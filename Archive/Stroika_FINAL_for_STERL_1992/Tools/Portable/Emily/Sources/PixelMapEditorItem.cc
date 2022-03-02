/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditorItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 */





#include	"StreamUtils.hh"

#include	"PixelMapEditor.hh"

#include	"PixelMapEditorItem.hh"
#include	"CommandNumbers.hh"





/*
 ********************************************************************************
 ****************************** PixelMapEditorType ****************************
 ********************************************************************************
 */
PixelMapEditorType::PixelMapEditorType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildUser1, "PixelMapEditor", (ItemBuilderProc)&PixelMapEditorItemBuilder)
#else
	ItemType (eBuildUser1, "PixelMapEditor", &PixelMapEditorItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
PixelMapEditorType&	PixelMapEditorType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	PixelMapEditorType::PixelMapEditorItemBuilder ()
{
	return (new PixelMapEditorItem (Get ()));
}

PixelMapEditorType*	PixelMapEditorType::sThis = Nil;





/*
 ********************************************************************************
 ****************************** PixelMapEditorItem ********************************
 ********************************************************************************
 */
PixelMapEditorItem::PixelMapEditorItem (ItemType& type) :
	ViewItem (type)
{
	static	UInt8	kPIXELMAPBUTTON_Data [] = {
		0xFF, 0xFF, 0xD5, 0x55, 0xBF, 0xFF, 0xE0, 0x05, 0xA0, 0x07, 0xE1, 0x05, 0xA2, 0x87, 0xE2, 0x45,
		0xA4, 0xE7, 0xEF, 0xB5, 0xBB, 0x5F, 0xF5, 0xAD, 0xAB, 0x57, 0xFF, 0xFD, 0xAA, 0xAB, 0xFF, 0xFF,
	};

	PixelMapEditor* editor = new PixelMapEditor ();
	editor->SetPixelMap (PixelMapFromData (Point (16, 16), kPIXELMAPBUTTON_Data, sizeof (kPIXELMAPBUTTON_Data)));
	
	SetOwnedView (editor);
}

String	PixelMapEditorItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "PixelMapEditor.hh";
	return (kHeaderFileName);
}

Boolean	PixelMapEditorItem::ItemCanBeEnabled ()
{
	return (False);
}

Boolean	PixelMapEditorItem::IsButton ()
{
	return (False);
}

Boolean	PixelMapEditorItem::IsSlider ()
{
	return (False);
}

Boolean	PixelMapEditorItem::IsText ()
{
	return (False);
}

Boolean	PixelMapEditorItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (True);
}

void	PixelMapEditorItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}
