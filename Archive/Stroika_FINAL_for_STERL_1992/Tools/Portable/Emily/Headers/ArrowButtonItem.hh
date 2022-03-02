/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ArrowButtonItem__
#define	__ArrowButtonItem__

/*
 * $Header: /fuji/lewis/RCS/ArrowButtonItem.hh,v 1.2 1992/07/16 15:27:55 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ArrowButtonItem.hh,v $
 *		Revision 1.2  1992/07/16  15:27:55  sterling
 *		hi
 *
# Revision 1.1  1992/06/25  10:08:44  sterling
# Initial revision
#
 *
 *
 *
 */

#include	"PixelMap.hh"
#include	"ButtonItem.hh"

class	AbstractArrowButton;
class	ArrowButtonItem : public ButtonItem {
	public:
		ArrowButtonItem (ItemType& type);
	
		nonvirtual	AbstractArrowButton&	GetArrowButton () const;
		override	String	GetHeaderFileName ();
		
	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);
		override	void	SetItemInfo ();
		
		virtual		void	SetArrowButton (AbstractArrowButton* arrow);
	
	private:
		AbstractArrowButton*	fArrowButton;
};

class	ArrowButtonItemType : public ItemType {
	public:
		ArrowButtonItemType ();
		
		static	ArrowButtonItemType&	Get ();
		
	private:		
		static	ViewItem*	ArrowButtonItemBuilder ();
		static	ArrowButtonItemType*	sThis;
};

class	ArrowButtonInfo;
class	SetArrowButtonInfoCommand : public Command {
	public:
		SetArrowButtonInfoCommand (ArrowButtonItem& item, class ArrowButtonInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		ArrowButtonItem&	fItem;
		class Command*		fItemInfoCommand;
	
		AbstractArrowButton::ArrowDirection	fNewDirection;
		AbstractArrowButton::ArrowDirection	fOldDirection;
};

#endif	/* __ArrowButtonItem__ */
