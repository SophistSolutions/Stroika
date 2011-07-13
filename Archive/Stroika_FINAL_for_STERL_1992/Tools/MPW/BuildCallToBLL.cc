/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BuildCallToBLL.cc,v 1.1 1992/06/20 17:39:46 lewis Exp $
 *
 * Description:
 *		BuildCallToBLL <template> <keepSymName> <suffixName> <methods> <methodSuffixes> <classes>
 *
 *		EG:
 *			BuildCallToBLL 'Seq_DLLf' ViewPtr "" 'GetLength RemoveAt' 'Fv CFv' 'ViewPtr ClipCamPtr DocPtr'
 *
 * TODO:
 *
 * Changes:
 *	$Log: BuildCallToBLL.cc,v $
 *		Revision 1.1  1992/06/20  17:39:46  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/06/10  07:10:54  lewis
 *		Reversed args to Tool, and added new suffixName (really suffix for template generated name).
 *		Also, put algorithm to build names into Proc NamesToString () for easier re-use and future
 *		enhancements.
 *
 *		Revision 1.1  1992/06/08  22:02:02  lewis
 *		Initial revision
 *
 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/BuildCallToBLL.cc,v 1.1 1992/06/20 17:39:46 lewis Exp $";
#endif

#include	<ctype.h>
#include	<iostream.h>
#include	<strstream.h>
#include	<stdlib.h>


static	char*		StringSave (const char* s);
static	unsigned	CountNames (const char* s);
static	char**		MakeNameTable (const char* s);
static	char*		NamesToString (const char* methodName, const char* methodSuffixName, const char* className, const char* templateName, const char* suffixName);



int	main (int argc, char* argv[])
{
	if (argc < 6) {
		cerr << "Usage: " << argv[0] << " <template> <keepSymName> <suffixName> <methods> <methodSuffixes> <classes>\n";
		return (1);
	}
	
	char*	templateName	=	argv[1];
	char*	keepSymName		=	argv[2];
	char*	suffixName		=	argv[3];
	char*	methods			=	argv[4];
	char*	methodSuffixes	=	argv[5];
	char*	classes			=	argv[6];



	/*
	 * First parse the space delimited lists into arrays we can directly index.
	 */
	unsigned	nameCount	=	CountNames (methods);

	if (nameCount != CountNames (methodSuffixes)) {
		cerr << "Must have the same # of method suffixes as methods\n";
		cerr << "Usage: " << argv[0] << " <keepSymName> <template> <methods> <methodSuffixes> <classes>\n";
		return (1);
	}

	unsigned	classCount	=	CountNames (classes);


	char**	methodNameTable	=	MakeNameTable (methods);
	char**	methodSuffixTable	=	MakeNameTable (methodSuffixes);
	char**	classNameTable	=	MakeNameTable (classes);

	/*
	 * Then iterate over the method names, and within the method names, the actual class instantiations,
	 * generating the appropriate lib calls.
	 */
	for (int nameIndex = 0; nameIndex < nameCount; nameIndex++) {
		for (int classIndex = 0; classIndex < classCount; classIndex++) {
			if (strcmp (keepSymName, classNameTable[classIndex]) == 0) {
				continue;		// ok to appear redundently, but skip this one...
			}

#if 1
			char*	realSym = NamesToString (methodNameTable [nameIndex], methodSuffixTable[nameIndex],
											classNameTable[classIndex], templateName, suffixName);
			char*	keepSym = NamesToString (methodNameTable [nameIndex], methodSuffixTable[nameIndex],
											keepSymName, templateName, suffixName);

			cout << "\t-dm " << realSym << " ¶\n";
			cout << "\t-rn " << realSym << "=" << keepSym << " ¶\n";
			delete (realSym);
			delete (keepSym);
#else
			cout << "\t-dm " << methodNameTable [nameIndex] << "__" << (strlen (templateName)+strlen(classNameTable[classIndex])) <<
					templateName << classNameTable [classIndex] << methodSuffixTable[nameIndex] << " ¶\n";
			cout << "\t-rn " << methodNameTable [nameIndex] << "__" << (strlen (templateName)+strlen(classNameTable[classIndex])) <<
					templateName << classNameTable [classIndex] << methodSuffixTable[nameIndex] << "=" <<
					methodNameTable [nameIndex] << "__" << (strlen (templateName)+strlen(keepSymName)) << templateName << keepSymName << methodSuffixTable[nameIndex] <<
					" ¶\n";
#endif
		}
	}

	return (0);
}



static	char*	StringSave (const char* s)
{
	char*	buf	=	new char [strlen (s)+1];
	strcpy (buf, s);
	return (buf);
}

static	unsigned	CountNames (const char* s)
{
	unsigned	count		=	0;
	int			partOfWord	=	0;
	for (register const char* p  = s; *p; p++) {
		if (isspace (*p) && partOfWord) {		// we transition from part of word to not...
			partOfWord = 0;
			count ++;
		}
		else if (!isspace (*p) && !partOfWord) {
			partOfWord = 1;						// we transtion from inter word to intra-word.
		}
	}
	if (partOfWord) { // if no trailing spaces - common case!
		count ++;
	}
	return (count);
}

static	char**		MakeNameTable (const char* s)
{
	char**	nameTable	=	new char* [CountNames (s)];
	char*	tmpName		=	new char [strlen (s) + 1];		// thats big enuf for largest word...

	unsigned	count		=	0;		// which one were on...
	int			partOfWord	=	0;
	int			tmpI		=	0;
	for (register const char* p  = s; *p; p++) {
		tmpName [tmpI++] = *p;
		if (isspace (*p) && partOfWord) {		// we transition from part of word to not...
			partOfWord = 0;
			tmpName[tmpI-1] = 0;
			nameTable [count] = StringSave (tmpName);
			tmpI = 0;
			count ++;
		}
		else if (!isspace (*p) && !partOfWord) {
			partOfWord = 1;						// we transtion from inter word to intra-word.
			
		}
		else if (isspace (*p)) {
			tmpI = 0;		// keep ignoring whitespace...
		}
	}
	if (partOfWord) { // if no trailing spaces - common case!
		tmpName[tmpI] = 0;
		nameTable [count] = StringSave (tmpName);
		count ++;
	}

	delete (tmpName);
	return (nameTable);
}

static	char*	NamesToString (const char* methodName, const char* methodSuffixName, const char* className, const char* templateName, const char* suffixName)
{
	strstream	buf;
	buf << methodName << "__" << (strlen (templateName)+strlen(className)+strlen(suffixName)) << templateName << className << suffixName << methodSuffixName;
	return (buf.str ());
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
