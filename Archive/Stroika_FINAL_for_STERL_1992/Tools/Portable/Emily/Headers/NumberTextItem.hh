/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__NumberTextItem__
#define	__NumberTextItem__

/*
 * $Header: /fuji/lewis/RCS/NumberTextItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: NumberTextItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 *
 */

#include	"TextEditItem.hh"

class	NumberText;
class	NumberTextItem : public TextEditItem {
	public:
		NumberTextItem (ItemType& type);
	
		nonvirtual	NumberText&	GetNumberText () const;
		override	String	GetHeaderFileName ();
		
	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
			
	private:
		NumberText*	fNumberText;
		Real		fDefaultValue;
		Real		fDefaultMinValue;
		Real		fDefaultMaxValue;
		Int8		fDefaultPrecision;
	
	friend	class NumberTextItemType;
};

class	NumberTextItemType : public ItemType {
	public:
		NumberTextItemType ();
		
		static	NumberTextItemType&	Get ();
		
	private:		
		static	ViewItem*	NumberTextItemBuilder ();
		static	NumberTextItemType*	sThis;
};

class	SetNumberTextInfoCommand : public Command {
	public:
		SetNumberTextInfoCommand (NumberTextItem& item, class NumberTextInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		NumberTextItem&	fItem;
		class Command*	fItemInfoCommand;
		
		Boolean	fNewRequireText;
		Boolean	fOldRequireText;
		Int8	fNewPrecision;
		Int8	fOldPrecision;
		Real	fNewMaximum;
		Real	fOldMaximum;
		Real	fNewMinimum;
		Real	fOldMinimum;
		Real	fNewDefault;
		Real	fOldDefault;
};

#endif	/* __NumberTextItem__ */
