/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__DateTextItem__
#define	__DateTextItem__

/*
 * $Header: /fuji/lewis/RCS/DateTextItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: DateTextItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 *
 */

#include	"TextEditItem.hh"

class	DateText;
class	DateTextItem : public TextEditItem {
	public:
		DateTextItem (ItemType& type);
	
		override	String	GetHeaderFileName ();

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
			
	private:
		DateText*	fDateText;
	
	friend class DateTextItemType;
};

class	DateTextItemType : public ItemType {
	public:
		DateTextItemType ();
		
		static	DateTextItemType&	Get ();
		
	private:		
		static	ViewItem*	DateTextItemBuilder ();
		static	DateTextItemType*	sThis;
};


#endif	/* __DateTextItem__ */
