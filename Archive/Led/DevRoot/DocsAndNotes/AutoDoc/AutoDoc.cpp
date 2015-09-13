/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/DocsAndNotes/AutoDoc/AutoDoc.cpp,v 2.20 2003/05/08 20:38:09 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: AutoDoc.cpp,v $
 *	Revision 2.20  2003/05/08 20:38:09  lewis
 *	support MSVC71
 *	
 *	Revision 2.19  2002/09/04 03:42:44  lewis
 *	support msvc70 (.net)
 *	
 *	Revision 2.18  2001/11/27 00:36:34  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.17  2001/11/07 21:18:11  lewis
 *	use 3.0 instead of 2.3 in a couple places and normalize spaces around / for various doc titles
 *	
 *	Revision 2.16  2001/10/20 13:38:28  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  2001/10/20 11:45:29  lewis
 *	added a small hack to support overloads - munge their names slightly
 *	
 *	Revision 2.14  2001/10/19 22:21:00  lewis
 *	added heading to docfiles (with icon and saying Led 3.0 classlib docs. And made Modules page have 3 columns (so fits in a single page)
 *	
 *	Revision 2.13  2001/10/18 13:09:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2001/10/18 12:29:14  lewis
 *	<STYLE> sections need to have their data wrapped in HTML comment
 *	
 *	Revision 2.11  2001/10/17 20:59:26  lewis
 *	lose GlobalFunction prefix on non-nested function declarations
 *	
 *	Revision 2.10  2001/10/17 16:25:09  lewis
 *	SPR#1066- MAJOR cleanup of the AutoDoc generated HTML
 *	
 *	Revision 2.9  2001/10/16 15:36:56  lewis
 *	update docs version# to 3.0
 *	
 *	Revision 2.8  2001/08/30 01:10:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.7  1999/02/05 14:27:40  lewis
 *	fix autodoc to build with msvc60
 *	
 *	Revision 2.6  1998/04/09 01:23:58  lewis
 *	Fix bug with name conflct between CLASS namespace and MODULE namespace. Solve problem by manlging module names.
 *	
 *	Revision 2.5  1998/03/04  20:38:08  lewis
 *	version 2.3, and support for per-module docs.
 *
 *	Revision 2.4  1997/07/27  15:44:09  lewis
 *	work around netscape bug with &lt - wrap in space
 *
 *	Revision 2.3  1997/07/23  23:27:42  lewis
 *	Lots of fixes, includign extenrally specifying gDateLabel.
 *	Use tables for method lists.
 *	DOnt gen tables / sections if they are empty.
 *	Config vars better supported.
 *	more cleanups/fixes
 *
 *	Revision 2.2  1997/07/15  05:41:21  lewis
 *	Fixed bug with &gt etc in generated <a name=>.
 *	Added support for @'xxx'.
 *	Added super minimal kConfigVar support.
 *
 *	Revision 2.1  1997/07/14  15:06:39  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 *
 *
 */

/* _MSC_VER == 1100 - AKA MSVC 5.0 */
#define	_MSVC50_VER_	1100
/* _MSC_VER == 1200 - AKA MSVC 6.0 */
#define	_MSVC60_VER_	1200
/* _MSC_VER == 1300 - AKA MSVC 7.0 AKA VS.NET */
#define	_MSVC70_VER_	1300
/* _MSC_VER == 1310 - AKA MSVC 7.1 AKA VS.NET 2003 */
#define	_MSVC71_VER_	1310

#if		_MSC_VER ==_MSVC50_VER_ || _MSC_VER ==_MSVC60_VER_
	#pragma	warning (4 : 4786)
#endif


#include	<assert.h>

#include	<algorithm>
#include	<cctype>
#include	<iostream>
#include	<fstream>
#include	<map>
#include	<string>
#include	<vector>



#if		_MSC_VER >=_MSVC50_VER_ || defined (MSIPL_USING_NAMESPACE)
using	namespace	std;
#endif

#if		__MWERKS__ == 0x1800
	#define	qNeedSTLAllocatorBWA				1
#endif

#if		_MSC_VER ==_MSVC50_VER_ || _MSC_VER ==_MSVC60_VER_ || _MSC_VER ==_MSVC70_VER_ || _MSC_VER ==_MSVC71_VER_
	#define	qNoSupportForNewForLoopScopingRules	1

	#define	and	&&
	#define	not	!
	#define	or	||
#endif




#if		qNeedSTLAllocatorBWA
	#define	STLAllocator_BWA(T)	,allocator< T >
#else
	#define	STLAllocator_BWA(T)
#endif



const	char	kCommentStartLine[]	=	"/*";
const	char	kCommentEndLine[]	=	"*/";
const	char	kClassTag[]			=	"@CLASS:";
const	char	kConfigVar[]		=	"@CONFIGVAR:";
const	char	kBases[]			=	"@BASES:";
const	char	kMethodTag[]		=	"@METHOD:";
const	char	kAccessTag[]		=	"@ACCESS:";
const	char	kModuleTag[]		=	"@MODULE:";
const	char	kDescriptionTag[]	=	"@DESCRIPTION:";


const	char	kMainTableWidth[]	=	"90%";
//const	char	kMainTableWidth[]	=	"600";

const	char	kSTYLEInfo[]	=	"<Style type=\"text/css\">\n"
									"	<!--\n"
					//				"	H1 {color: blue}\n"
									"	P { font-face: Verdana, Arial, Helvetica; text-indent: 3em}\n"
									"	P.MethodHeader {; text-indent: 0em; font-weight: bold; color: blue}\n"
									"	P.MethodDeclaration {; text-indent: -2em; margin-left: 5em; margin-right: 5em;}\n"
									"	H3.PageHeading {text-align: center; font-size: 18pt; }\n"
									"	Span.ModuleTitleDeclaration { color: blue; }\n"
									"	Span.ClassNameDeclaration { color: blue; }\n"
									"	-->\n"
									"</Style>\n";




string	gDateLabel	=	"2001-10-17";

//const	char	kFontDeclarationForMainText[]	=	"<FONT FACE=Verdana,Arial,Helvetica SIZE=2>\n";



typedef	ifstream::pos_type	pos_type;


inline	string	MangleModuleName (string mn)
{
	return "$" + mn + "$";
}

inline	string	UnMangleModuleName (string mn)
{
	assert (mn.length () > 2);
	return mn.substr (1, mn.length () - 2);
}



typedef	vector<string STLAllocator_BWA(string)>	StringVector;
struct	Options {
	StringVector	fInputFiles;
};



template	<class	T>	inline	T	SkipSpace (T i)
	{
	    while (*i != '\0' and isspace (*i)) {
	        ++i;
		}
		return i;
	}
template	<class	T>	inline	T	SkipNONSpace (T i)
	{
	    while (*i != '\0' and !isspace (*i)) {
	        ++i;
		}
		return i;
	}
inline	string	EncodeBadHTMLChars (const string& in)
	{
		string	result;
		result.reserve (in.length ());
		for (string::const_iterator i = in.begin (); i != in.end (); ++i) {
			char	c	=	*i;
			/*
			 *	Normally I would replace '<' with "&lt". But i had to stick in the space after
			 *	to work around a Netscape 4.0 bug ("&ltT&gt" displayed as &ltT>").
			 *
			 *	But then with one extra space, MSIE looked funny. With the two extra spaces, niether
			 *	looks too bad.
			 */
			if (c == '<') {
				result.append (" &lt ");
			}
			else if (c == '>') {
				result.append (" &gt ");
			}
			else {
				result.append (&c, 1);
			}
		}
		return result;
	}
inline	string	NoralizeBadHTMLCharsForARefs (string in)
	{
		in = EncodeBadHTMLChars (in);
		string	result;
		result.reserve (in.length ());
		for (string::const_iterator i = in.begin (); i != in.end (); ++i) {
			char	c	=	*i;
			if (c == ':') {
				c = '_';
			}
			if (c == '&') {
				c = '_';
			}
			if (c == ' ') {
				c = '_';
			}
			result.append (&c, 1);
		}
		return result;
	}




struct	ClassInfo {
	ClassInfo ():
		fOnlyConfigVar (false),
		fReallyModuleDoc (false)
		{
		}
	bool			fOnlyConfigVar;
	bool			fReallyModuleDoc;
	string			fFullyQualifiedClassName;
	string			fAccess;	// e.g public, private, protected (defaults to public)
	string			fModule;
	string			fDocString;
	string			fBaseClasses;
	StringVector	fMethods;		// of form 'CLASSNAME::NAME'; a method record will contain
									// multiple overloads

	inline	string	GetAccess () const
		{
			if (fAccess.empty ()) {
				return "public";
			}
			else {
				return fAccess;
			}
		}
};

struct	MethodInfo {
	string	fFullyQualifiedMethodName;
	string	fAccess;	// e.g public, private, protected (defaults to public)
	string	fModule;
	string	fDocString;
	string	fDeclarationLine;
	inline	string	GetClassName () const
		{
			size_t	i	=	fFullyQualifiedMethodName.rfind (":");
			if (i == string::npos) {
				return string ();
			}
			else {
				--i;
				return fFullyQualifiedMethodName.substr (0, i);
			}
		}
	inline	string	GetMemberName () const
		{
			size_t	i	=	fFullyQualifiedMethodName.rfind (":");
			if (i == string::npos) {
				return fFullyQualifiedMethodName;
			}
			else {
				++i;
				return fFullyQualifiedMethodName.substr (i);
			}
		}
	inline	string	GetAccess () const
		{
			if (fAccess.empty ()) {
				return "public";
			}
			else {
				return fAccess;
			}
		}
};


typedef	map<string, ClassInfo, less<string> STLAllocator_BWA(ClassInfo) >	ClassInfoMap;
typedef	map<string, MethodInfo, less<string> STLAllocator_BWA(MethodInfo) >	MethodInfoMap;


static	Options			ParseCommandLine (int argc, const char** argv);
static	void			BadOption (const string& argi);
static	void			PrintHelpInfo ();
static	void			ReadFile (ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap, const string& inFileName);
static	void			ReadComment (ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap, const string& moduleName, ifstream& srcFile);
static	string			ReadDescription (ifstream& srcFile, const char* initialText);
static	void			WriteDocString (ofstream& outFile, const ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap, const string& docString);
static	void			WriteDocFileForModule (ofstream& outFile, const string& moduleName, ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap);
static	void			WriteDocFileForClass (ofstream& outFile, const ClassInfo& classInfo, ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap);
static	string			NormalizeSpaces (const string& in);
static	void			WriteDocFileForMethod (ofstream& outFile, MethodInfo& methodInfo, const ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap);
static	string			ExtractModuleNameFromFileName (const string& fileName);
static	StringVector	ExtractModuleList (ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap);
static	void			WriteIndexFile (const StringVector& moduleList);

int	main (int argc, const char** argv)
{
	#if		!defined (qArgcHack) && defined (__MWERKS__)
		// Cuz mac doesn't have a way to pass command line args. Really use this from windows!
		#define	qArgcHack	1
	#endif
	#if		qArgcHack
		const	char*	_fake_argv_[]	=	{
			"Marker.h",
			"TextStore.h",
			"TextStore.cpp",
		};
		argc = sizeof (_fake_argv_)/sizeof (_fake_argv_[0]) + 1;
		argv = _fake_argv_;
	#endif

	Options	o	=	ParseCommandLine (argc, argv);

	ClassInfoMap	classInfoMap;
	MethodInfoMap	methodInfoMap;

	for (StringVector::const_iterator i = o.fInputFiles.begin (); i != o.fInputFiles.end (); ++i) {
		ReadFile (classInfoMap, methodInfoMap, *i);
	}

	StringVector	moduleList	=	ExtractModuleList (classInfoMap, methodInfoMap);
	WriteIndexFile (moduleList);
	#if		qNoSupportForNewForLoopScopingRules
		{
	#endif
	for (StringVector::const_iterator i = moduleList.begin (); i != moduleList.end (); ++i) {
		string		htmlFileName	=	UnMangleModuleName (*i) + ".html";
		ofstream	out (htmlFileName.c_str ());
		WriteDocFileForModule (out, *i, classInfoMap, methodInfoMap);
	}
	#if		qNoSupportForNewForLoopScopingRules
		}
	#endif

	return 0;
}


static	void	ReadFile (ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap, const string& inFileName)
{
	string		moduleName	=	ExtractModuleNameFromFileName (inFileName);
	ifstream	srcFile (inFileName.c_str ());

	char	lineBuf[4*1024];
	while (srcFile.getline (lineBuf, sizeof (lineBuf))) {
		const char *pc = SkipSpace (lineBuf);
		if (::strcmp (pc, kCommentStartLine) == 0) {
			ReadComment (classInfoMap, methodInfoMap, moduleName, srcFile);
		}
	}
}


static	void	ReadComment (ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap, const string& moduleName, ifstream& srcFile)
{
	bool	isConfigVarInfo	=	false;
	bool	isClassInfo		=	false;
	bool	isMethodInfo	=	false;

	ClassInfo	classInfo;
	classInfo.fModule = moduleName;
	MethodInfo	methodInfo;
	methodInfo.fModule = moduleName;

	char	lineBuf[4*1024];
	while (srcFile.getline (lineBuf, sizeof (lineBuf))) {
		const char *pc = SkipSpace (lineBuf);
		if (::strncmp (pc, kClassTag, ::strlen (kClassTag)) == 0) {
			isClassInfo = true;
			pc += ::strlen (kClassTag);
			pc = SkipSpace (pc);
			classInfo.fFullyQualifiedClassName = EncodeBadHTMLChars (pc);
		}
		else if (::strncmp (pc, kConfigVar, ::strlen (kConfigVar)) == 0) {
			isConfigVarInfo = true;
			pc += ::strlen (kConfigVar);
			pc = SkipSpace (pc);
			classInfo.fFullyQualifiedClassName = EncodeBadHTMLChars (pc);
			classInfo.fOnlyConfigVar = true;
		}
		else if (::strncmp (pc, kBases, ::strlen (kBases)) == 0) {
			pc += ::strlen (kBases);
			pc = SkipSpace (pc);
			classInfo.fBaseClasses = EncodeBadHTMLChars (pc);
		}
		else if (::strncmp (pc, kMethodTag, ::strlen (kMethodTag)) == 0) {
			isMethodInfo = true;
			pc += ::strlen (kMethodTag);
			pc = SkipSpace (pc);
			methodInfo.fFullyQualifiedMethodName = EncodeBadHTMLChars (pc);
		}
		else if (::strncmp (pc, kAccessTag, ::strlen (kAccessTag)) == 0) {
			pc += ::strlen (kAccessTag);
			pc = SkipSpace (pc);
			classInfo.fAccess = EncodeBadHTMLChars (pc);
			methodInfo.fAccess = EncodeBadHTMLChars (pc);
		}
		else if (::strncmp (pc, kModuleTag, ::strlen (kModuleTag)) == 0) {
			isClassInfo = true;
			pc += ::strlen (kModuleTag);
			pc = SkipSpace (pc);
//			classInfo.fFullyQualifiedClassName = EncodeBadHTMLChars (pc);
			classInfo.fFullyQualifiedClassName = MangleModuleName (pc);
			classInfo.fReallyModuleDoc = true;
		}
		else if (::strncmp (pc, kDescriptionTag, ::strlen (kDescriptionTag)) == 0) {
			string	t	=	ReadDescription (srcFile, pc + ::strlen (kDescriptionTag));
			if (isClassInfo or isConfigVarInfo) {
				classInfo.fDocString = t;
			}
			else if (isMethodInfo) {
				methodInfo.fDocString = t;
			}
			break;
		}
		else if (::strcmp (pc, kCommentEndLine) == 0) {
			break;
		}
	}

	// Line right after contains declaration
// TODO: may want to embellish this to handle multiline declarations - serach for line with "{" at start!
	if (isMethodInfo and srcFile.getline (lineBuf, sizeof (lineBuf))) {
		const char *pc = SkipSpace (lineBuf);
		const	char	kInline[]	=	"inline";
		if (strncmp (pc, kInline, ::strlen (kInline)) == 0) {
			pc = SkipSpace (pc + ::strlen (kInline));
		}
		methodInfo.fDeclarationLine = pc;
	}

	if (isClassInfo or isConfigVarInfo) {
		classInfoMap.insert (ClassInfoMap::value_type (classInfo.fFullyQualifiedClassName, classInfo));
	}
	if (isMethodInfo) {
		// Handle overloaded functions (hack - but effective for now - LGP 2001-10-20)
		int	count	=	0;
		string	newName	=	methodInfo.fFullyQualifiedMethodName;
		while (methodInfoMap.find (newName) != methodInfoMap.end ()) {
			count++;
			char	buf[1024];
			sprintf (buf, " (overload %d)", count);
			newName = methodInfo.fFullyQualifiedMethodName + buf;
	methodInfo.fFullyQualifiedMethodName = newName;
		}
		methodInfoMap.insert (MethodInfoMap::value_type (methodInfo.fFullyQualifiedMethodName, methodInfo));
	}
}

static	string	ReadDescription (ifstream& srcFile, const char* initialText)
{
	string	result	=	initialText;
	result += "\n";

	char	lineBuf[4*1024];
	while (srcFile.getline (lineBuf, sizeof (lineBuf))) {
		const char *pc = SkipSpace (lineBuf);
		if (::strcmp (pc, kCommentEndLine) == 0) {
			break;
		}
		result += lineBuf;
		result += "\n";
	}
	return result;
}

static	void	WriteDocFileForModule (ofstream& outFile, const string& moduleName, ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap)
{
	const	char	kDocFileHeader[]	=	"<!DOCTYPE HTML SYSTEM \"html.dtd\">\n"
											"<HTML>\n"
											"<HEAD>\n"
											"	<TITLE>Led Class Library 3.0 / ReferenceManual</TITLE>\n"
											"<!-- WARNING: Machine-generated file, from Led sources. Don't edit directly.-->\n"
											"</HEAD>\n"
											"<Body>\n"
											"<h3 class=\"PageHeading\"><Img Width=\"32\" Height=\"32\" Src=\"../Common/LedClassLibIcon.gif\"><u>Led 3.0 Class Library Documentation</u></h3>"
											"\n";
										;

	const	char	kDocFileTrailer1[]	=	"<hr>\n"
											"<a href=\"http://www.sophists.com/Led/\"><img src=\"../Common/TinyHouse.gif\" WIDTH=\"23\" HEIGHT=\"21\"> Return to Led  Page </a>\n"
											" "
											"<a href=\"../index.html\"><img src=\"../Common/TinyHouse.gif\" WIDTH=\"23\" HEIGHT=\"21\"> Return to Led ClassLib Documentation Index</a>\n"
											" "
											"<a href=\"index.html\"><img src=\"../Common/TinyHouse.gif\" WIDTH=\"23\" HEIGHT=\"21\"> Return to Led Reference Manual Index</a>\n"
										;
	const	char	kDocFileTrailer2[]	=	"</BODY>\n"
											"</HTML>\n"
										;

	outFile << kDocFileHeader;
	outFile << kSTYLEInfo;
//	outFile << kFontDeclarationForMainText;

	outFile << "<Center><h2>Module <em><Span Class=\"ModuleTitleDeclaration\">" << UnMangleModuleName (moduleName) << "</Span></em></h2></Center>" << endl;
	outFile << "<hr>" << endl;

	/*
	 *	Write the classes/global functions index table.
	 */
	string	moduleDoc;
	{
		outFile << "<h3>Classes and Global Functions Index</h3>" << endl;
		outFile << "<ul>" << endl;
		for (ClassInfoMap::const_iterator i = classInfoMap.begin (); i != classInfoMap.end (); ++i) {
			ClassInfo	ci	=	(*i).second;
			if (ci.fModule == moduleName) {
				if (ci.fReallyModuleDoc) {
					moduleDoc = ci.fDocString;
				}
				else {
					outFile << "<li><code><a href=\"#" << NoralizeBadHTMLCharsForARefs (ci.fFullyQualifiedClassName) << "\">" << ci.fFullyQualifiedClassName << "</a></code></li><br>" << endl;
				}
			}
		}
		#if		qNoSupportForNewForLoopScopingRules
			{
		#endif
		for (MethodInfoMap::const_iterator i = methodInfoMap.begin (); i != methodInfoMap.end (); ++i) {
			MethodInfo	mi	=	(*i).second;
			if (mi.fModule == moduleName and mi.GetClassName ().empty ()) {
				outFile << "<li><code><a href=\"#" << NoralizeBadHTMLCharsForARefs (mi.fFullyQualifiedMethodName) << "\">" << mi.fFullyQualifiedMethodName << "</a></code></li><br>" << endl;
			}
		}
		#if		qNoSupportForNewForLoopScopingRules
			}
		#endif
		outFile << "</ul></h3>" << endl;
	}

	if (not moduleDoc.empty ()) {
		outFile << "<h3>Module Description:</h3>" << endl;
		WriteDocString (outFile, classInfoMap, methodInfoMap, moduleDoc);
	}

	/*
	 *	Write the classes.
	 */
	for (ClassInfoMap::const_iterator i = classInfoMap.begin (); i != classInfoMap.end (); ++i) {
		ClassInfo	ci	=	(*i).second;
		if (ci.fModule == moduleName and not ci.fReallyModuleDoc) {
			outFile << "<hr>" << endl;
			WriteDocFileForClass (outFile, ci, classInfoMap, methodInfoMap);
		}
	}


	/*
	 *	Write the global functions.
	 */
	#if		qNoSupportForNewForLoopScopingRules
		{
	#endif
	for (MethodInfoMap::const_iterator i = methodInfoMap.begin (); i != methodInfoMap.end (); ++i) {
		MethodInfo	mi	=	(*i).second;
		if (mi.fModule == moduleName and mi.GetClassName ().empty ()) {
			outFile << "<hr>" << endl;
			WriteDocFileForMethod (outFile, mi, classInfoMap, methodInfoMap);
		}
	}
	#if		qNoSupportForNewForLoopScopingRules
		}
	#endif

	outFile << kDocFileTrailer1;
	outFile << 	"<br><i>Last Updated " << gDateLabel << "</i>\n";
	outFile << kDocFileTrailer2;
}

static	void	WriteDocFileForClass (ofstream& outFile, const ClassInfo& classInfo, ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap)
{
	outFile << "<h4><a name=\"" << NoralizeBadHTMLCharsForARefs (classInfo.fFullyQualifiedClassName);
	if (classInfo.fOnlyConfigVar) {
		outFile << "\">Config Variable:";
	}
	else {
		outFile << "\">Class:";
	}
	outFile << " <em><Span Class=\"ClassNameDeclaration\"><code>" << classInfo.fFullyQualifiedClassName;
	outFile << "\t[" << classInfo.GetAccess () << "]" << endl;
	outFile << "</code></em></Span></a></h4>";
	if (not classInfo.fBaseClasses.empty ()) {
		outFile << "<h5>Base Classes: <code>";
		WriteDocString (outFile, classInfoMap, methodInfoMap, classInfo.fBaseClasses);
		outFile << "</code></h5>" << endl;
	}

	outFile << "<h5>Description:</h5>" << endl;
	WriteDocString (outFile, classInfoMap, methodInfoMap, classInfo.fDocString);


	/*
	 *	Write the index, with names of members and links to later with more content
	 */
	int	memberCount	=	0;
	outFile << "<Center>";
// Leave this part out - doesn't server much purpose
	{
		for (MethodInfoMap::const_iterator i = methodInfoMap.begin (); i != methodInfoMap.end (); ++i) {
			MethodInfo	mi	=	(*i).second;
			if (mi.GetClassName () == classInfo.fFullyQualifiedClassName) {
#if 0
				if (memberCount == 0) {
	//				outFile << "<h5>Members:</h5>" << endl;
	//				outFile << "<ul>" << endl;
					outFile << "<p>" << endl;
					outFile << "<Table Width=\"" << kMainTableWidth << "\" Border=\"1\" Frame=\"box\" Rules=\"Groups\">" << endl;
					outFile << "<THead>" << endl;
					outFile << "<Tr>" << endl;
					outFile << "<Th>Members" << endl;
					outFile << "<TBody>" << endl;
				}
#endif
				memberCount++;
#if 0
//				outFile << "<li><code><a href=#" << NoralizeBadHTMLCharsForARefs (mi.fFullyQualifiedMethodName) << ">" << mi.GetMemberName () << "</a></code></li><br>" << endl;
				outFile << "<Tr><Td><code>";
				outFile << "<a href=\"#" << NoralizeBadHTMLCharsForARefs (mi.fFullyQualifiedMethodName) << "\">" << mi.GetMemberName ();
		//		outFile << "\t[" << mi.GetAccess () << "]";
				outFile << "</a></code>" << endl;
#endif
			}
		}
		if (memberCount != 0) {
	//		outFile << "</ul>" << endl;
#if 0
			outFile << "</Table>" << endl;
#endif
		}
	}

	/*
	 *	Write the member details.
	 */
	if (memberCount != 0) {
		//outFile << "<h5>Member Details:</h5>" << endl;
//		outFile << "<h5>Member Details:</h5>" << endl;
		outFile << "<Table Width=\"" << kMainTableWidth << "\" Border=\"1\" Frame=\"box\">" << endl;
		outFile << "<THead>" << endl;
		outFile << "<Tr>" << endl;
		outFile << "<Th>Member Details" << endl;
		outFile << "<TBody align=\"left\">" << endl;
		for (MethodInfoMap::const_iterator i = methodInfoMap.begin (); i != methodInfoMap.end (); ++i) {
			MethodInfo	mi	=	(*i).second;
			if (mi.GetClassName () == classInfo.fFullyQualifiedClassName) {
outFile << "<tr><td>" << endl;
				WriteDocFileForMethod (outFile, mi, classInfoMap, methodInfoMap);
			}
		}
		outFile << "</Table>" << endl;
	}
	outFile << "</Center>";
}

static	void	WriteDocString (ofstream& outFile, const ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap, const string& docString)
{
	// Write the docString out to outFile. But in doing so, scan it for tokens of the form:
	//	@'XXXX'. And when found, strip all but the XXXX and lookup XXXX in the method and class
	// tables. If found, wrap XXXX with a a/href to that name. Handy for cross-indexing things
	// which are related
	const char*	start	=	&*docString.begin ();
	const char*	end		=	&*docString.end ();
	for (const char* p = start; p != end; ++p) {
		if (*p == '@' and p+1 != end and *(p+1) == '\'') {
			// Probably one of our special tokens, but beware, and don't crash on syntax errors!
			// Scan for end quote
			const char*	startQuote	=	p + 2;
			const char* endQuote 	= startQuote;
			for (; endQuote != end and *endQuote != '\''; ++endQuote)
				;
			if (endQuote != end) {
				string							potentialStringToCheck	=	string (startQuote, endQuote-startQuote);
    			
				bool	didAHRef	=	false;

    			// First try method table
    			MethodInfoMap::const_iterator	mii	=	methodInfoMap.find (EncodeBadHTMLChars (potentialStringToCheck));
    			if (mii != methodInfoMap.end ()) {
    				MethodInfo	mi	=	(*mii).second;
					outFile << "<code><a href=\"" << UnMangleModuleName (mi.fModule) << ".html#" << NoralizeBadHTMLCharsForARefs (mi.fFullyQualifiedMethodName) << "\">";
					didAHRef = true;
    			}

    			// Then try class table
				if (not didAHRef) {
    				ClassInfoMap::const_iterator	cii	=	classInfoMap.find (EncodeBadHTMLChars (potentialStringToCheck));
	    			if (cii != classInfoMap.end ()) {
	    				ClassInfo	ci	=	(*cii).second;
						outFile << "<code><a href=\"" << UnMangleModuleName (ci.fModule) << ".html#" << NoralizeBadHTMLCharsForARefs (ci.fFullyQualifiedClassName) << "\">";
						didAHRef = true;
	    			}
				}
				outFile << potentialStringToCheck;
				if (didAHRef) {
					outFile << "</a></code>";
				}
				p = endQuote;	// so skipped in outer loop
				continue;
			}
		}
		outFile << *p;
	}
}

static	string	NormalizeSpaces (const string& in)
{
	string	out;
	for (string::const_iterator i = in.begin (); i != in.end ();) {
		if (isspace (*i)) {
			out += " ";
			while (isspace (*i) and i != in.end ()) {
				++i;
			}
		}
		else {
			out += *i;
			++i;
		}
	}
	return out;
}

static	void	WriteDocFileForMethod (ofstream& outFile, MethodInfo& methodInfo, const ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap)
{
#if 0
	if (methodInfo.GetClassName ().empty ()) {
		outFile << "<h4><a name=\"" << NoralizeBadHTMLCharsForARefs (methodInfo.fFullyQualifiedMethodName) << "\">GlobalFunction: <em><code>" << methodInfo.fFullyQualifiedMethodName << "</code></em></a></h4>";
	}
#endif

	
//	outFile << "<p style=\"text-indent: 0em\"><b><code><a name=\"" << NoralizeBadHTMLCharsForARefs (methodInfo.fFullyQualifiedMethodName) << "\">" << methodInfo.fFullyQualifiedMethodName;
	outFile << "<p class=\"MethodHeader\"><b><code><a name=\"" << NoralizeBadHTMLCharsForARefs (methodInfo.fFullyQualifiedMethodName) << "\">" << methodInfo.fFullyQualifiedMethodName;
		outFile << "\t[" << methodInfo.GetAccess () << "]" << endl;
		outFile << "</a></code></b></p>" << endl;
//	outFile << "<code><pre>" << "\t" << NormalizeSpaces (methodInfo.fDeclarationLine) << "</pre></code><br>" << endl;
//	outFile << "<p style=\"text-indent: 3em\"><code>" << "\t" << NormalizeSpaces (methodInfo.fDeclarationLine) << "</code></p>" << endl;
	outFile << "<p class=\"MethodDeclaration\"><code>" << "\t" << EncodeBadHTMLChars (NormalizeSpaces (methodInfo.fDeclarationLine)) << "</code></p>" << endl;
//	outFile << kFontDeclarationForMainText << endl;
	WriteDocString (outFile, classInfoMap, methodInfoMap, methodInfo.fDocString);
//	outFile << "</font>" << endl;
	outFile << endl;
}

static	Options	ParseCommandLine (int argc, const char** argv)
{
	Options	o;

	for (size_t i = 1; i < argc; ++i) {
		const	char*	argi	=	argv[i];
		if ((*argi) == '-' or (*argi == '/')) {
			switch (argi[1]) {
				case	'd': {
					if (string (argi+1) == "date") {
						i++;
						argi = argv[i];
						if (i < argc) {
							gDateLabel = argi;
						}
						else {
							BadOption ("-date missing argument");
						}
					}
					else {
						BadOption (argi);
					}
				}
				break;
				case	'h': {
					if (string (argi+1) == "help") {
						PrintHelpInfo ();
					}
					else {
						BadOption (argi);
					}
				}
				break;
				default: {
					BadOption (argi);
				}
				break;
			}
		}
		else {
			o.fInputFiles.push_back (argi);
		}
	}

	return o;
}

static	void	BadOption (const string& argi)
{
	cerr << "Bad Option: '" << argi << "'" << endl;
	cerr << "\ttry -help for more information" << endl;
	::exit (1);
}

static	void	PrintHelpInfo ()
{
	cerr << "Usage: AutoDoc [options]* [inputFiles]*" << endl;
	cerr << "\toptions are:" << endl;
	cerr << "\t\tNoneYet" << endl;
	::exit (0);
}

static	string	ExtractModuleNameFromFileName (const string& fileName)
{
	string	result	=	fileName;

	// strip stuff after .
	if (result.rfind (".") != string::npos) {
		result = result.substr (0, result.rfind ("."));
	}

	// strip stuff before first \/etc...
	if (result.rfind ("/") != string::npos) {
		result = result.substr (result.rfind ("/")+1);
	}
	if (result.rfind ("\\") != string::npos) {
		result = result.substr (result.rfind ("\\")+1);
	}
	return MangleModuleName (result);
}

static	StringVector	ExtractModuleList (ClassInfoMap& classInfoMap, MethodInfoMap& methodInfoMap)
{
	StringVector	modules;
	for (ClassInfoMap::const_iterator i = classInfoMap.begin (); i != classInfoMap.end (); ++i) {
		modules.push_back ((*i).second.fModule);
	}
	#if		qNoSupportForNewForLoopScopingRules
		{
	#endif
	for (ClassInfoMap::const_iterator i = classInfoMap.begin (); i != classInfoMap.end (); ++i) {
		modules.push_back ((*i).second.fModule);
	}
	#if		qNoSupportForNewForLoopScopingRules
		}
	#endif
	sort (modules.begin (), modules.end ());
	StringVector::iterator rest = unique (modules.begin (), modules.end ());
	modules.erase (rest, modules.end ());	// remove the extra entries
	return modules;
}

static	void	WriteIndexFile (const StringVector& moduleList)
{
	const	char	kIndexFileHeader1[]	=	"<!DOCTYPE HTML SYSTEM \"html.dtd\">\n"
											"<HTML>\n"
											"<HEAD>\n"
											"	<TITLE>Led Class Library 3.0/ReferenceManual</TITLE>\n"
											"<!-- WARNING: Machine-generated file, from Led sources. Don't edit directly.-->\n"
											"</HEAD>\n"
											"\n"
										;
//											"<FONT FACE=Verdana,Arial,Helvetica SIZE=2>\n"
	const	char	kIndexFileHeader2[]	=	"<Body>\n"
											"<h3 class=\"PageHeading\"><Img Width=\"32\" Height=\"32\" Src=\"../Common/LedClassLibIcon.gif\"><u>Led 3.0 Class Library Documentation</u></h3>"
											"\n"
//											"<FONT FACE=Verdana,Arial,Helvetica SIZE=2>\n"
										;

	const	char	kIndexFileTrailer1[]	=	"<hr>\n"
												"<a href=\"http://www.sophists.com/Led/\"><img src=../Common/TinyHouse.gif WIDTH=\"23\" HEIGHT=\"21\"> Return to Led  Page </a>\n"
												"<a href=\"../index.html\"><img src=\"../Common/TinyHouse.gif\" WIDTH=\"23\" HEIGHT=\"21\"> Return to Led Class Library Documentation Index</a>\n"
											;
	const	char	kIndexFileTrailer2[]	=	"</BODY>\n"
												"</HTML>\n"
											;

	ofstream	outFile ("index.html");

	outFile << kIndexFileHeader1;
	outFile << kSTYLEInfo;
	outFile << kIndexFileHeader2;

	outFile << "<Center><h2>Modules</h2><hr></Center>" << endl;

	outFile << "<ul>" << endl;
	outFile << "<table>" << endl;
	for (StringVector::const_iterator i = moduleList.begin (); i != moduleList.end (); ++i) {
		string		htmlFileName	=	UnMangleModuleName (*i) + ".html";
		outFile << "<tr>" << endl;
		outFile << "<td>" << endl;
		outFile << "<li><a href=\"" << htmlFileName << "\">" << UnMangleModuleName (*i) << "</a></li><br>" << endl;
		outFile << "</td>" << endl;
		if (i+1 < moduleList.end ()) {
			++i;
			htmlFileName	=	UnMangleModuleName (*i) + ".html";
			outFile << "<td>" << endl;
			outFile << "<li><a href=\"" << htmlFileName << "\">" << UnMangleModuleName (*i) << "</a></li><br>" << endl;
			outFile << "</td>" << endl;
		}
		if (i+1 < moduleList.end ()) {
			++i;
			htmlFileName	=	UnMangleModuleName (*i) + ".html";
			outFile << "<td>" << endl;
			outFile << "<li><a href=\"" << htmlFileName << "\">" << UnMangleModuleName (*i) << "</a></li><br>" << endl;
			outFile << "</td>" << endl;
		}
	}
	outFile << "</ul>" << endl;
	outFile << "</table>" << endl;

	outFile << kIndexFileTrailer1;
	outFile << 	"<br><i>Last Updated " << gDateLabel << "</i>\n";
	outFile << kIndexFileTrailer2;
}



