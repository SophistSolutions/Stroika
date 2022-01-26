/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__PixelMapButtonItem__
#define	__PixelMapButtonItem__

/*
 * $Header: /fuji/lewis/RCS/PixelMapButtonItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapButtonItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/03/06  21:53:47  sterling
 *		motif
 *
 * 		Revision 1.1  1992/02/03  17:19:27  sterling
 * 		Initial revision
 *
 *
 *
 */

#include	"PixelMap.hh"
#include	"ButtonItem.hh"

class	AbstractPixelMapButton;
class	PixelMapButtonItem : public ButtonItem {
	public:
		PixelMapButtonItem (ItemType& type);
	
		nonvirtual	AbstractPixelMapButton&	GetPixelMapButton () const;
		override	String	GetHeaderFileName ();
		
	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);
	
	private:
		AbstractPixelMapButton*	fPixelMapButton;
};

class	PixelMapButtonItemType : public ItemType {
	public:
		PixelMapButtonItemType ();
		
		static	PixelMapButtonItemType&	Get ();
		
	private:		
		static	ViewItem*	PixelMapButtonItemBuilder ();
		static	PixelMapButtonItemType*	sThis;
};

class	SetPixelMapButtonInfoCommand : public Command {
	public:
		SetPixelMapButtonInfoCommand (PixelMapButtonItem& item, class PixelMapButtonInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		PixelMapButtonItem&	fItem;
		PixelMap			fNewPixelMap;
		PixelMap			fOldPixelMap;
		
		class Command*	fItemInfoCommand;
};

#endif	/* __PixelMapButtonItem__ */
