/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ItemType__
#define	__ItemType__

/*
 * $Header: /fuji/lewis/RCS/ItemType.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ItemType.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.12  1992/03/06  21:53:47  sterling
 *		motif
 *
 *		Revision 1.5  1992/01/15  03:43:50  lewis
 *		Work around HP compiler bug with -g.
 *
 *
 */

#include	"Mapping_HashTable.hh"

#include	"Command.hh"
#include	"String.hh"


class	ViewItem;
typedef	ViewItem* (*ItemBuilderProc) ();

#if		!qRealTemplatesAvailable
	#if qMPW_MacroOverflowProblem
		#define	IBProc	ItemBuilderProc
	#endif
	
	Declare (Iterator, IBProc);
	Declare (Collection, IBProc);
	AbMappingDeclare (String, IBProc);
	Mapping_HTDeclare(String, IBProc);
#endif

class	ItemType {
	public:
		ItemType (CommandNumber palletGroup, const String& itemClass, ItemBuilderProc builder);
		virtual	~ItemType ();
		
		nonvirtual	CommandNumber	GetPalletGroup () const;
		nonvirtual	String			GetItemClass () const;
		nonvirtual	ItemBuilderProc	GetItemBuilder () const;
		
		static	ItemBuilderProc	LookupItemBuilder (const String& itemClass);
		
	private:
		CommandNumber	fPalletGroup;
		String			fItemClass;
		ItemBuilderProc	fBuilderProc;

#if	qHPCPlus_DashGWithMappingHashTableBug
		static		Mapping_HashTable(String, ItemBuilderProc)&	sItemBuilders;
#else
		static		Mapping_HashTable(String, ItemBuilderProc)	sItemBuilders;
#endif
};




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/* __ItemType__ */
