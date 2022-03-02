/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ScaleItem__
#define	__ScaleItem__

/*
 * $Header: /fuji/lewis/RCS/ScaleItem.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScaleItem.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  92/04/30  14:15:15  14:15:15  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 */

#include	"SliderItem.hh"

class	AbstractScale;
class	ScaleItem : public SliderItem {
	public:
		ScaleItem (ItemType& type);
	
		override	String	GetHeaderFileName ();
		nonvirtual	AbstractScale&	GetScale () const;

	protected:
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
		virtual		void	SetScale (AbstractScale* scale);
	
	private:
		AbstractScale*	fScale;
		Boolean			fDefaultSensitive;
		Coordinate		fDefaultThumbLength;
};

class	ScaleItemType : public ItemType {
	public:
		ScaleItemType ();
		
		static	ScaleItemType&	Get ();
		
	private:		
		static	ViewItem*	ScaleItemBuilder ();
		static	ScaleItemType*	sThis;
};

class	SetScaleInfoCommand : public SetSliderInfoCommand {
	public:
		SetScaleInfoCommand (ScaleItem& item, class ScaleInfo& info);

		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		ScaleItem&		fScale;
		
		Boolean		fNewSensitive;
		Boolean		fOldSensitive;
		Coordinate	fNewThumbLength;
		Coordinate	fOldThumbLength;
};

#endif	/* __ScaleItem__ */
