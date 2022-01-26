/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GenClass.cc,v 1.19 1992/12/07 03:23:09 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: GenClass.cc,v $
 *		Revision 1.19  1992/12/07  03:23:09  lewis
 *		Renamed Catch/Try to lower case.
 *
 *		Revision 1.18  1992/12/03  19:34:54  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug and
 *		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug.
 *
 *		Revision 1.17  1992/11/05  07:36:42  lewis
 *		Futz with spincursor stuff to make work better under MPW.
 *
 *		Revision 1.16  1992/11/04  20:08:52  lewis
 *		Work around qGCC_MysteryOptimizerWithGetAtBug
 *
 *		Revision 1.14  1992/10/16  22:58:05  lewis
 *		Minor cleanups including getting rid of GetFileDescriptor proc, and printing messages
 *		on failed opens, and always overwriting file - used to just skip it if it was already there.
 *
 *		Revision 1.13  1992/10/16  20:02:26  lewis
 *		Get rid of obsolete kEmptyString define.
 *
 *		Revision 1.12  1992/10/09  14:17:46  lewis
 *		Surround even sources with #ifndefs.
 *		Delete some commented out code.
 *
 *		Revision 1.10  1992/10/02  23:49:10  lewis
 *		Lots of hacks - got most of the various classes generating properly-
 *		most of the hacks were required to get Mapping to work - some of them
 *		turned out to be unneeded, but I left them in anyhow.
 *
 *		Revision 1.9  1992/10/02  04:17:51  lewis
 *		Lots of work to get working with new GenMake.
 *
 *		Revision 1.8  1992/09/30  04:13:51  lewis
 *		(LGP checked in for STERL) Lots of changes. Made non-recursive-
 *		thats now done by GetMake. Support extra files arg. Support
 *		short names stuff. A few other misc changes to support GenMake.
 *
 *		Revision 1.6  1992/09/28  04:04:01  lewis
 *		Rewrote large parts - broke out info file reading into
 *		seperate module, and build a mapping of info. Broke out
 *		lexical analyzer/parser into separate module. Begin
 *		implementing use of new Options stuff (though havent started
 *		using it yet). All the rewrites introduced one problem:
 *		no macro gens now - should be easy to fix tommorrow, and we
 *		don't count on that yet anyhow.
 *
 *		Revision 1.5  1992/09/25  21:53:44  lewis
 *		Added support for ! dependencies, where we dont gen file, just
 *		include .cc file - useful for things like SharedOfSequenceRepOfInt.
 *		Also, added rcsid to header so I can track binary versions more easily.
 *
 *		Revision 1.4  1992/09/25  21:16:33  lewis
 *		Lots of hacks including verbose mode, moving helper functions
 *		to the bottom of the file as statics, Killed unused routines,
 *		Fixed routine to output #defines of macros to not substiute,
 *		get params right, and wrapped in ifndefs to avoid dups warnings
 *		from cpps.
 *
 *		Revision 1.3  1992/09/25  01:50:12  lewis
 *		STERL(LGP checkedin): added new support for prefix section (rawtext)
 *		in containerinfo file to solve Shared(SequneceRep(T))) problem.
 *		Also added support for autogening macros mappings, I think?
 *
 *		Revision 1.2  1992/09/25  01:34:54  lewis
 *		Lots of changes to port to gcc, and added TypeNameToFileName function.
 *		Handle exceptions better, and print reasonable messages identifying file/line
 *		for problem.
 *
 *
 *
 */
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/GenClass.cc,v 1.19 1992/12/07 03:23:09 lewis Exp $";

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include    "OSInit.hh"
#pragma push
    #pragma force_active    on
    #if     qGCC_SimpleClassStaticInitializerBug
        static  OSInit  sOSInit;
    #else
        static  OSInit  sOSInit =   OSInit ();
    #endif
#pragma pop

#define		qUseAllGlobals		0
#define		qUseFileGlobals		1
#define		qUseDebugGlobals	1
#include	"Stroika-Foundation-Globals.hh"

#include	<stdlib.h>
#include	<ctype.h>
#include	<strstream.h>
#include	<fstream.h>

#include	"OSRenamePre.hh"
#if		qMacOS
	#include	<CursorCtl.h>
#endif
#include	"OSRenamePost.hh"

#include	"Sequence.hh"
#include	"Date.hh"
#include	"Debug.hh"
#include	"Format.hh"
#include	"Memory.hh"
#include	"Options.hh"
#include	"StreamUtils.hh"

#include	"File.hh"

#include	"DoTranslate.hh"
#include	"FileNameUtils.hh"
#include	"InfoDB.hh"




struct	MyParams {
	MyParams (const PathName& destination, const PathName& templates,
			  const Sequence(String)& arguments, const Sequence(String)& includeFiles,
			  const String& collection, 
			  String fileMapping) :
		
		fDestination (destination),
		fTemplates (templates),
		fArguments (arguments),
		fIncludeFiles (includeFiles),
		fCollection (collection),
		fFileMapping (fileMapping)
	{
	}
		
	PathName 			fDestination;
	PathName 			fTemplates;
	Sequence(String)	fArguments;
	Sequence(String)	fIncludeFiles;
	String 				fCollection;
	String				fFileMapping;
};


// This should be done autmatically!!!
ostream& operator << (ostream& out, const Sequence(String)& s)
{
	out << "(";
	Boolean firstTime = True;
	ForEach (String, it, s) {
		if (!firstTime) {
			out << ", ";
			firstTime = True;
		}
		out << it.Current ();
	}
	out << ")";
	return (out);
}


static	void	BuildDestinationFile (const Mapping(String,InfoRecord)& infoRecords, const MyParams& p, Boolean doingHeader);
static	void	BuildDependencies (const Mapping(String,InfoRecord)& infoRecords, const Sequence(InfoDependency)& dependencyInfo, const MyParams& p, const Sequence(String)& argNames, ostream& strm, Boolean processDependency);
static	void	ProcessRenames (const Sequence(RenameRecord)& renameRecords, ostream& to);


// These are left uninitialzed since we always set them first thing
// in main after parsing our options.
static	Boolean	gVerbose;
static	Boolean	gBangMode;
static	Boolean	gDoingHeader;

enum {
	eVerbose,
	eTemplates,
	eDestination,
	eCollectionInfo,
	eADT,
	eArg,
	eFileMapping,
	eIncludeFile,
	eHeader,
	eBangMode,
};
#if		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
	#if		qRealTemplatesAvailable
		static	Bag<OptionDefinition>	BogusFunction()
	#else
		static	Bag(OptionDefinition)	BogusFunction()
	#endif
{
	#if		qRealTemplatesAvailable
		Bag<OptionDefinition>	tmp;
	#else
		Bag(OptionDefinition)	tmp;
	#endif
	tmp.Add (OptionDefinition ("Verbose", "v", eVerbose, False));
	tmp.Add (OptionDefinition ("Header", "h", eHeader, False));
	tmp.Add (OptionDefinition ("Templates", "th", eTemplates, False));
	tmp.Add (OptionDefinition ("Templates", "ts", eTemplates, False));
	tmp.Add (OptionDefinition ("Templates", "t", eTemplates, False));
	tmp.Add (OptionDefinition ("Destination", "dh", eDestination, False));
	tmp.Add (OptionDefinition ("Destination", "ds", eDestination, False));
	tmp.Add (OptionDefinition ("Destination", "d", eDestination, False));
	tmp.Add (OptionDefinition ("CollectionInfo", "b", eCollectionInfo, False));
	tmp.Add (OptionDefinition ("Collection", "c", eADT, False));
	tmp.Add (OptionDefinition ("Arg", "a", eArg, False));
	tmp.Add (OptionDefinition ("FileMapping", "fm", eFileMapping, False));
	tmp.Add (OptionDefinition ("IncludeFile", "i", eIncludeFile, False));
	tmp.Add (OptionDefinition ("BangMode", "z", eBangMode, False));
	return (tmp);
}
#endif	/*qGCC_StaticInitializerForArrayOfObjectsWithCTORBug*/

#if		!qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
const	OptionDefinition	kOD[] = {
	OptionDefinition ("Verbose", "v", eVerbose, False),
	OptionDefinition ("Header", "h", eHeader, False),
	OptionDefinition ("Templates", "th", eTemplates, False),
	OptionDefinition ("Templates", "ts", eTemplates, False),
	OptionDefinition ("Templates", "t", eTemplates, False),
	OptionDefinition ("Destination", "dh", eDestination, False),
	OptionDefinition ("Destination", "ds", eDestination, False),
	OptionDefinition ("Destination", "d", eDestination, False),
	OptionDefinition ("CollectionInfo", "b", eCollectionInfo, False),
	OptionDefinition ("Collection", "c", eADT, False),
	OptionDefinition ("Arg", "a", eArg, False),
	OptionDefinition ("FileMapping", "fm", eFileMapping, False),
	OptionDefinition ("IncludeFile", "i", eIncludeFile, False),
	OptionDefinition ("BangMode", "z", eBangMode, False),
};
#endif	/*!qGCC_StaticInitializerForArrayOfObjectsWithCTORBug*/

class	Options : public OptionParser {
	public:
		Options () :
			#if		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
					OptionParser (BogusFunction()),
			#else
				#if		qRealTemplatesAvailable
					OptionParser (Bag<OptionDefinition> (kOD, sizeof (kOD)/sizeof (kOD[0]))),
				#else
					OptionParser (Bag(OptionDefinition) (kOD, sizeof (kOD)/sizeof (kOD[0]))),
				#endif
			#endif
			fVerbose (False),
			fTemplates (PathName::kDot),
			fDestination (PathName::kDot),
			fCollectionInfo (PathName::kBad),
			fArguments (),
			fIncludeFiles (),
			fCollection (""),
			fFileMapping (""),
			fBuildHeader (False),
			fBangMode (False)
			{
			}

		override	void	ParseOption_ (OptionID optionID)
		{
			switch (optionID) {
				case	eVerbose:				fVerbose = True; break;
				case	eHeader:				fBuildHeader = True; break;
				case	eFileMapping:			fFileMapping = SnagNextArg (); break;
				case	eTemplates:				fTemplates = SnagNextPathNameArg (); break;
				case	eDestination:			fDestination = SnagNextPathNameArg (); break;
				case	eCollectionInfo:		fCollectionInfo = SnagNextPathNameArg (); break;
				case	eADT:					fCollection = SnagNextArg (); break;
				case	eArg:					fArguments.Append (SnagNextArg ()); break;
				case	eIncludeFile:			fIncludeFiles.Append (SnagNextArg ()); break;
				case	eBangMode:				fBangMode = True; break;
				default:	RequireNotReached();
			}
		}

		PathName 			fTemplates;
		PathName 			fDestination;
		PathName 			fCollectionInfo;
		String 				fCollection;
		Sequence(String)	fArguments;
		Sequence(String)	fIncludeFiles;
		Boolean				fVerbose;
		String				fFileMapping;
		Boolean				fBuildHeader;
		Boolean				fBangMode;
};

int	main (int argc, char* argv[])
{
#if		qMacOS
	::InitCursorCtl (Nil);

	/*
	 * I have no idea why this is necessary, but the cursor doesn't really spin
	 * unless you give it a good "push" here.
	 */
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);::SpinCursor(10);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
#endif
	Options	options;

	try {
		options.Parse (argc, argv);
		gVerbose = options.fVerbose;
		gBangMode = options.fBangMode;
		gDoingHeader = options.fBuildHeader;
	}
	catch1 (Options::sBadArgs) {
		cout << Options::sBadArgs.GetMessage () << endl;
		const	String	kUsage	=
			"GenClass \n"
				"\t[-dh <destination header filename>]\n"
				"\t[-ds <destination source filename>]\n"
				"\t[-th <template headers directory>]\n"
				"\t[-ts <template sources directory>]\n"
				"\t-b <collection info filename>\n"
				"\t-c <collection name>\n"
				"\t[-a <template argument>]\n"
				"\t[-i <include filename>]\n"
				"\t[-fm]\n"
				"\t[-v]\n";
		cout << "Usage: " << kUsage << endl;
		::exit (1);
	}

#if		qMacOS
	::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);::SpinCursor(1);
#endif
	if (gVerbose) {
		cout << "Reading Info Records." << endl;
	}
	Mapping(String,InfoRecord)	infoRecords;
	{
		try {
			infoRecords = ReadInfoRecords (options.fCollectionInfo);
		}
		catch () {
			cout << "Error reading CollectionInfo file: '" << options.fCollectionInfo << "'" << endl;
			::exit (1);
		}
	}
#if		qMacOS
	::SpinCursor(10);
#endif
	MyParams params = MyParams (options.fDestination, options.fTemplates, 
								options.fArguments, options.fIncludeFiles,
								options.fCollection, options.fFileMapping);

	BuildDestinationFile (infoRecords, params, options.fBuildHeader);

	return (0);
}

static	void	BuildDependencies (const Mapping(String,InfoRecord)& infoRecords,
								   const Sequence(InfoDependency)& dependencyInfo, const MyParams& p,
								   const Sequence(String)& argNames, ostream& strm,
								   Boolean processDependency)
{
	ForEach (InfoDependency, it, dependencyInfo) {
#if		qMacOS
		::SpinCursor(2);
#endif
		InfoDependency	curDependencyInfo	=	it.Current ();
		MyParams newParams = p;
		newParams.fArguments.RemoveAll ();
		newParams.fCollection = curDependencyInfo.fName;
		ForEach (String, it2, curDependencyInfo.fArguments) {
			newParams.fArguments.Append (Parameterize (it2.Current (), p.fArguments, argNames));
		}
		if (processDependency) {
			String tmp = TypeNameToFileName (BuildTypeName (newParams.fCollection, newParams.fArguments, False));
			if (not curDependencyInfo.fBangBang) {
				if (not gDoingHeader and (gBangMode or curDependencyInfo.fBangDep)) {
					if (p.fFileMapping != "") {
						strm << "#include " << tmp << "_cc" << endl;
					}
					else {
						strm << "#include " << quote << tmp << ".cc" << quote << endl;
					}
				}
				else {
					strm << "#ifndef\tq" << tmp << "Defined" << endl;
					if (p.fFileMapping != "") {
						strm << tab << "#include " << tmp << "_hh" << endl;
					}
					else {
						strm << tab << "#include " << quote << tmp << ".hh" << quote << endl;
					}
					strm << "#endif" << endl << endl;
				}
			}
		}
	}
}

void	BuildDestinationFile (const Mapping(String,InfoRecord)& infoRecords, const MyParams& p, Boolean doingHeader)
{
#if		qMacOS
	::SpinCursor(10);
#endif

	PathName	templatePathName	=	p.fTemplates;
	PathName	targetPathName		=	p.fDestination;

	AbstractFileSystem::FileDescriptor	fd = 0;
	try {
		#if qMacOS
			fd = FileSystem::Get_MacOS ().Open (targetPathName, O_RDWR | O_CREAT | O_TRUNC, 'MPS ', 'TEXT');
		#else
			fd = FileSystem::Get ().Open (targetPathName, O_RDWR | O_CREAT | O_TRUNC);
		#endif
	}
	catch1 (FileSystem::sAccessPermissionDenied) {
		cout << "Access denied opening " << targetPathName << endl;
		::exit (1);
	}
	catch1 (FileSystem::sException) {
		cout << "File open error #" << FileSystem::sException.GetErrNumber() << " opening file: " << targetPathName << endl;
		::exit (1);
	}
	catch () {
		AssertNotReached ();
	}

	ofstream	strm (fd);
	if (gVerbose) {
		cout << "Translating " << p.fCollection << (doingHeader?" Header ":" Source ") <<
				 "for " << p.fArguments << endl;
	}
#if		qMacOS
	::SpinCursor(10);
#endif

	// write opening comment
	strm << "/*\n * Macro-based template file for " << p.fCollection << "<";
	ForEachT (SequenceIterator, String, it1, p.fArguments) {
		strm << it1.Current ();
		if (it1.CurrentIndex () < p.fArguments.GetLength ()) {
			strm << ", ";
		}
	}
	strm << ">" << "\n *\n * Generated on " << Date () << "\n *\n */\n\n\n" << endl;

	String nameOfFile = TypeNameToFileName (BuildTypeName (p.fCollection, p.fArguments, False));
	if (doingHeader) {
		strm << "#ifndef\tq" << nameOfFile << "HHDefined" << endl;
		strm << "#define\tq" << nameOfFile << "HHDefined\t1\n\n" << endl;
	}
	else {
		strm << "#ifndef\tq" << nameOfFile << "CCDefined" << endl;
		strm << "#define\tq" << nameOfFile << "CCDefined\t1\n\n" << endl;
	}

	// Will need to generate different name for DOS!!!
	strm << "#include\t" << quote << "Config-Foundation.hh" << quote << endl << endl;

	if (p.fFileMapping != "") {
		strm << "#include\t" << quote << p.fFileMapping << quote << endl << endl;
	}
	
	if (doingHeader) {
		strm << "#ifndef\t__" << p.fCollection << "__" << endl;
		strm << "#include " << quote << p.fCollection << ".hh" << quote << endl;
		strm << "#endif\n\n" << endl;
	}

	InfoRecord	thisInfoRecord;
	if (not infoRecords.Lookup (p.fCollection, &thisInfoRecord)) {
		cout << "Could not find " << p.fCollection << " in database." << endl;
		::exit (1);
	}

	if (not doingHeader) {
		if (p.fFileMapping != "") {
			strm << "#include " << nameOfFile << "_hh" << endl << endl;
		}
		else {
			strm << "#include " << quote << nameOfFile << ".hh" << quote << endl << endl;
		}
	}
	
	if (not p.fIncludeFiles.IsEmpty ()) {
		strm << "// Special include files" << endl;
		ForEach (String, it, p.fIncludeFiles) {
			strm << "#include " << quote << it.Current () << quote << endl;
		}
		strm << endl;
	}

	if (doingHeader) {
		ProcessRenames (thisInfoRecord.fRenames, strm);
	}

	if (gVerbose) {
		cout << tab << "Header dependencies" << endl;
	}
#if		qMacOS
	::SpinCursor(10);
#endif

	BuildDependencies (infoRecords, thisInfoRecord.fHeaderDependencies,
					   p, thisInfoRecord.fArguments, strm, doingHeader);		// build header dependencies
	if (gVerbose) {
		cout << tab << "Source dependencies" << endl;
	}
#if		qMacOS
	::SpinCursor(10);
#endif
	BuildDependencies (infoRecords, thisInfoRecord.fSourceDependencies,
					   p, thisInfoRecord.fArguments, strm, not doingHeader);	// build source dependencies

	// process template
	if (gVerbose) {
		cout << tab << "Processing actual template" << endl;
	}
#if		qMacOS
	::SpinCursor(10);
#endif
	{
		AbstractFileSystem::FileDescriptor	templateFD = 0;
		String fileName 	=	p.fCollection + String (doingHeader? ".hh": ".cc");
		PathName	templateFilePath	=	templatePathName + PathComponent (fileName);
		try {
			#if qMacOS
				templateFD = FileSystem::Get_MacOS ().Open (templateFilePath, O_RDONLY, 'MPS ', 'TEXT');
			#else
				templateFD = FileSystem::Get ().Open (templateFilePath, O_RDONLY);
			#endif
		}
		catch1 (FileSystem::sAccessPermissionDenied) {
			cout << "Access denied opening " << templateFilePath << endl;
			::exit (1);
		}
		catch1 (FileSystem::sException) {
			cout << "File open error #" << FileSystem::sException.GetErrNumber() << " opening file: " << templateFilePath << endl;
			::exit (1);
		}
		catch () {
			AssertNotReached ();
		}
		ifstream	from (templateFD);
		strm << endl << endl;
		#if		qMacOS
			::SpinCursor(10);
		#endif
		BuildBoilerplate (fileName, from, strm, p.fArguments, thisInfoRecord.fArguments, False);
		#if		qMacOS
			::SpinCursor(10);
		#endif
		strm << "\n\n#endif\t/* q" << nameOfFile << "Defined" << " */\n" << endl;
		if (gVerbose) {
			cout << "Finished " << p.fCollection << (doingHeader ? " Header " : " Source ") <<
					 "for " << p.fArguments << endl;
		}
#if		qMacOS
		::SpinCursor(10);
#endif
		strm.close ();
	}
	::close (fd);
}

static	void	ProcessRenames (const Sequence(RenameRecord)& renameRecords, ostream& to)
{
#if		qMacOS
	::SpinCursor(10);
#endif
	if (renameRecords.IsEmpty ()) {
		return;
	}
#if		qGCC_MysteryOptimizerWithGetAtBug
	RenameRecord	rr;
	{ ForEach (RenameRecord, i, renameRecords) { rr = i.Current (); } }
#else
	RenameRecord	rr = renameRecords.GetAt (1);
#endif

	String	firstRenameSourceString = 	rr.fTemplateName;	
	to << "#ifndef\t" << firstRenameSourceString << endl;
	to << "\t#if\tqRealTemplatesAvailable" << endl;

	ForEach (RenameRecord, it, renameRecords) {
		String sourceName   = BuildTypeName (it.Current ().fTemplateName, it.Current ().fArguments, False);
		String templateName = BuildTypeName (it.Current ().fShortName,    it.Current ().fArguments, True);

		to << "\t\t#define\t" << sourceName << tab << templateName << endl;
	}
	to << "\t#else" << endl;
	
#if		qMacOS
	::SpinCursor(10);
#endif
	ForEach (RenameRecord, it1, renameRecords) {
		Sequence(String) args = it1.Current ().fArguments;

		String sourceName = BuildTypeName (it1.Current ().fTemplateName, args, False);
		// hack, add short name as first argument, so the name2 works correctly
		String	shortName = it1.Current ().fShortName;
		args.Prepend (shortName);
		String mappedName =  BuildTypeName (String ("name") + ltostring (args.GetLength ()), args, False);
		args.RemoveAt (1);

		to << "\t\t#define\t" << sourceName << tab << mappedName << endl;
	}
	to << "\t#endif\t/*qRealTemplatesAvailable*/" << endl;

	to << "#endif\t/*" << firstRenameSourceString << "*/" << endl;
	to << endl << endl;
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

