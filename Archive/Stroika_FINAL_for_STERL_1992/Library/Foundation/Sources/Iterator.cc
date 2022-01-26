/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Iterator__cc
#define	__Iterator__cc

/*
 * $Header: /fuji/lewis/RCS/Iterator.cc,v 1.7 1992/11/20 19:19:24 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Iterator.cc,v $
 *		Revision 1.7  1992/11/20  19:19:24  lewis
 *		Move IteratorRep<T>::~IteratorRep () to be inline.
 *
 *		Revision 1.5  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.4  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.3  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.2  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		
 *
 */



#include	"Debug.hh"

#include	"Iterator.hh"


#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

