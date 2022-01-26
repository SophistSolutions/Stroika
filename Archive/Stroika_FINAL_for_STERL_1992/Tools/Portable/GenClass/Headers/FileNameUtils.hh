/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__FileNameUtils__
#define	__FileNameUtils__

/*
 * $Header: /fuji/lewis/RCS/FileNameUtils.hh,v 1.3 1992/10/10 04:33:34 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: FileNameUtils.hh,v $
 *		Revision 1.3  1992/10/10  04:33:34  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
# Revision 1.2  1992/10/10  03:13:33  lewis
# Include SequenceOfString_hh
#
# Revision 1.1  1992/10/02  04:17:27  lewis
# Initial revision
#
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Sequence.hh"

#if		!qRealTemplatesAvailable
#include	"TFileMap.hh"
#include	SequenceOfString_hh
#endif

class	String;
extern	String	BuildTypeName (const String& collection, const Sequence(String)& arguments, Boolean asTemplate);
extern	String	TypeNameToFileName (const String& typeName);
extern	String	ShortenFileName (const String& fileName, size_t toLength);




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

#endif	/*__FileNameUtils__*/
