/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__PixelMapEditorItem__
#define	__PixelMapEditorItem__

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditorItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  92/04/09  23:37:32  23:37:32  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 *
 */

#include	"ViewItem.hh"

class	PixelMapEditorItem : public ViewItem {
	public:
		PixelMapEditorItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		override	void	WriteBuilder (class ostream& to, int tabCount);
		
		override	String	GetHeaderFileName ();
};

class	PixelMapEditorType : public ItemType {
	public:
		PixelMapEditorType ();
		
		static	PixelMapEditorType&	Get ();
		
	private:		
		static	ViewItem*	PixelMapEditorItemBuilder ();
		static	PixelMapEditorType*	sThis;
};

#endif	/* __PixelMapEditorItem__ */
