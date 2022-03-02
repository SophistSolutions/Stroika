/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FileNameUtils.cc,v 1.3 1992/10/10 04:36:18 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: FileNameUtils.cc,v $
 *		Revision 1.3  1992/10/10  04:36:18  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
// Revision 1.2  1992/10/08  02:10:18  sterling
// Lots of cleanups - use (conditionally) GenClass stuff to bootstrap.
// Got rid of some obsolete parts (like RawText - toiugh still supported
// in file format). Got basically all the Foundation, this guy and testsuites
// working on mac.
//
// Revision 1.1  1992/10/02  04:17:51  lewis
// Initial revision
//
 *		<Copied From GenClass.cc>
 *
 */
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/FileNameUtils.cc,v 1.3 1992/10/10 04:36:18 lewis Exp $";

#include	<iostream.h>

#include	"Debug.hh"
#include	"Format.hh"
#include	"Mapping_HashTable.hh"
#include	"Sequence.hh"

#include	"FileNameUtils.hh"

#ifdef qGlarf

#if		!qRealTemplatesAvailable
		Declare (Iterator, String);
		Declare (Sequence, String);
#endif

#else
#include	"TFileMap.hh"
#include SequenceOfString_hh
#endif



String	BuildTypeName (const String& collection, const Sequence(String)& arguments, Boolean asTemplate)
{
	String	tmp = collection;
	tmp += (asTemplate) ? '<' : '(';
	ForEachT (SequenceIterator, String, it, arguments) {
		tmp += it.Current ();
		if (it.CurrentIndex () < arguments.GetLength ()) {
			tmp += ',';
		}
	}
	tmp += (asTemplate) ? '>' : ')';
	return (tmp);
}

String	TypeNameToFileName (const String& typeName)
{
	String	result	=	"";
	for (size_t i = 1; i <= typeName.GetLength (); i++) {
		switch (typeName[i].GetAsciiCode ()) {
			case	'(':	result += "Of"; 		break;
			case	')':							break;
			case	',':	result += "_";			break;
			default:		result += typeName[i];	break;
		}
	}
	return (result);
}

String	ShortenFileName (const String& fileName, size_t toLength)
{
	if (fileName.GetLength () <= toLength) {
		return (fileName);
	}

	unsigned long	hashVal	=	DefaultStringHashFunction (fileName);

	String	hashString = ultostring (hashVal, 16);
	if (hashString.GetLength () > toLength) {
		hashString.SetLength (toLength);
	}

	size_t fileNameLength = toLength;
	if (fileNameLength > hashString.GetLength ()) {
		fileNameLength -= hashString.GetLength ();
	}
	else {
		fileNameLength = 0;
	}
	return (fileName.SubString (1, fileNameLength) + hashString);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

