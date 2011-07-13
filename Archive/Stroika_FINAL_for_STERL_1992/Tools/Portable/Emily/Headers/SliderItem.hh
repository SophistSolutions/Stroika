/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__SliderItem__
#define	__SliderItem__

#include	"ViewItem.hh"

/*
 * $Header: /fuji/lewis/RCS/SliderItem.hh,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderItem.hh,v $
 *		Revision 1.5  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

class	Slider;
class	SliderItem : public ViewItem {
	protected:
		SliderItem (ItemType& type);

	public:
		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();

		nonvirtual	Slider&	GetSlider () const;

		override	String	GetHeaderFileName () = Nil;

	protected:
		override	void	SetOwnedEnabled (Boolean enabled, UpdateMode updateMode);
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
		
		virtual	void	SetSlider (Slider* slider);

	private:
		Slider*	fSlider;
		Real	fDefaultMin;
		Real	fDefaultMax;
		Real	fDefaultValue;
};

class 	ViewItemInfo;
class	SetSliderInfoCommand : public Command {
	public:
		SetSliderInfoCommand (SliderItem& item, Real newMin, Real newMax, Real newValue, ViewItemInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		SliderItem&		fItem;
		class Command*	fItemInfoCommand;
		
		Real	fNewMin;
		Real	fOldMin;
		Real	fNewMax;
		Real	fOldMax;
		Real	fNewValue;
		Real	fOldValue;
};

#endif	/* __SliderItem__ */
