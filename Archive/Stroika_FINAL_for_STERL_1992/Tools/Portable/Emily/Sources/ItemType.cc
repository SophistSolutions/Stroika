/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ItemType.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ItemType.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 *
 */





#include	"ItemPallet.hh"
#include	"ItemType.hh"
#include	"ViewItem.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, IBProc);
	Implement (Collection, ItemBuilderProc);
	AbMappingImplement (String, IBProc);
	Mapping_HTImplement1 (String, IBProc);
	Mapping_HTImplement2 (String, IBProc);
	Mapping_HTImplement3 (String, IBProc);
#endif


#if	qHPCPlus_DashGWithMappingHashTableBug
Mapping_HashTable(String, ItemBuilderProc)&	ItemType::sItemBuilders = 
	*new Mapping_HashTable(String, ItemBuilderProc) (DefaultStringHashFunction);
#else
Mapping_HashTable(String, ItemBuilderProc)	ItemType::sItemBuilders (DefaultStringHashFunction);
#endif

ItemType::ItemType (CommandNumber palletGroup, const String& itemClass, ItemBuilderProc builder) :
	fPalletGroup (palletGroup),
	fItemClass (itemClass),
	fBuilderProc (builder)
{
	Require (fItemClass != kEmptyString);
	RequireNotNil (fBuilderProc);

	sItemBuilders.Enter (itemClass, builder);
	ItemPallet::AddPalletItem (palletGroup, itemClass);
}

ItemType::~ItemType ()
{
	sItemBuilders.Remove (GetItemBuilder ());
}
		
ItemBuilderProc	ItemType::LookupItemBuilder (const String& itemClass)
{
	ItemBuilderProc	proc = Nil;
	sItemBuilders.Lookup (itemClass, &proc);
	return (proc);
}

CommandNumber	ItemType::GetPalletGroup () const
{
	return (fPalletGroup);
}

String	ItemType::GetItemClass () const
{
	return (fItemClass);
}

ItemBuilderProc	ItemType::GetItemBuilder () const
{
	return (fBuilderProc);
}
		
