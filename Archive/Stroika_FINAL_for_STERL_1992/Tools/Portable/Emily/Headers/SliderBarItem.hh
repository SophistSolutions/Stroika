/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__SliderBarItem__
#define	__SliderBarItem__

/*
 * $Header: /fuji/lewis/RCS/SliderBarItem.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderBarItem.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

#include	"SliderItem.hh"

class	SliderBar;
class	SliderBarItem : public SliderItem {
	public:
		SliderBarItem (ItemType& type);
	
		override	String		GetHeaderFileName ();
		nonvirtual	SliderBar&	GetSliderBar () const;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
		override	void	WriteBuilder (class ostream& to, int tabCount);
	
		virtual		void	SetSliderBar (SliderBar* sliderBar);

	private:
		SliderBar*	fSliderBar;
		Real		fDefaultTicks;
		Real		fDefaultSubTicks;
};

class	SliderBarItemType : public ItemType {
	public:
		SliderBarItemType ();
		
		static	SliderBarItemType&	Get ();
		
	private:		
		static	ViewItem*	SliderBarItemBuilder ();
		static	SliderBarItemType*	sThis;
};

class	SetSliderBarInfoCommand : public SetSliderInfoCommand {
	public:
		SetSliderBarInfoCommand (SliderBarItem& item, class SliderBarInfo& info);

		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		SliderBarItem&		fSliderBar;
		
		Real	fNewTickSize;
		Real	fOldTickSize;
		Real	fNewSubTickSize;
		Real	fOldSubTickSize;
};

#endif	/* __SliderBarItem__ */
