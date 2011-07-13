/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__StringDictionary__
#define	__StringDictionary__

/*
 * $Header: /fuji/lewis/RCS/StringDictionary.hh,v 1.5 1992/10/21 04:58:50 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: StringDictionary.hh,v $
 *		Revision 1.5  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.4  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *		Revision 1.2  1992/07/01  17:44:38  lewis
 *		Renamed Strings.hh String.hh
 *
 *
 */
#include	"Mapping_HashTable.hh"
#include	"String.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	Mapping_HashTableOfString_String_hh
#endif

extern	Mapping(String,String)	ReadMap (const PathName& pathName);
extern	void					WriteMap (const PathName& pathName, const Mapping(String,String)& dictionary);





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__StringDictionary__*/
