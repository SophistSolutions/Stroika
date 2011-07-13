/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__CSymbolTextItem__
#define	__CSymbolTextItem__

/*
 * $Header: /fuji/lewis/RCS/CSymbolTextItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: CSymbolTextItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 *
 */

#include	"TextEditItem.hh"

class	CSymbolText;
class	CSymbolTextItem : public TextEditItem {
	public:
		CSymbolTextItem (ItemType& type);
		override	String	GetHeaderFileName ();
			
	private:
		CSymbolText*	fCSymbolText;
	
	friend	class CSymbolTextItemType;
};

class	CSymbolTextItemType : public ItemType {
	public:
		CSymbolTextItemType ();
		
		static	CSymbolTextItemType&	Get ();
		
	private:		
		static	ViewItem*	CSymbolTextItemBuilder ();
		static	CSymbolTextItemType*	sThis;
};


#endif	/* __CSymbolTextItem__ */
