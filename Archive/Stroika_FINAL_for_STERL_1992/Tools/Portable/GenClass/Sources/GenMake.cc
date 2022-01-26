/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GenMake.cc,v 1.25 1992/12/07 03:32:53 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: GenMake.cc,v $
 *		Revision 1.25  1992/12/07  03:32:53  lewis
 *		Renamed Catch/Try to lower case.
 *
 *		Revision 1.24  1992/12/05  03:43:13  lewis
 *		*** empty log message ***
 *
 *		Revision 1.23  1992/12/04  20:19:20  lewis
 *		Wrap name map #defines with #ifndef to work around buggy apple CPP.
 *
 *		Revision 1.22  1992/12/03  19:35:46  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug and
 *		qGCC_StaticInitializerForArrayOfObjectsWithCTOR...
 *		And for UNIX makefiles add '/' separator after dir variable names before
 *		file names.
 *
 *		Revision 1.21  1992/11/25  03:09:45  lewis
 *		Only generate UNIX makefile stuff for UNIX (some stuff was always
 *		generated).
 *		A bunch of changes to mac makefile stuff for recursive make on macs.
 *
 *		Revision 1.20  1992/11/24  15:30:04  lewis
 *		#include <stdlib.h> since we call exit();
 *
 *		Revision 1.19  1992/11/19  22:35:09  lewis
 *		On mac, rename GENDIR->TmplGenDir for consitency with what we did for UNIX.
 *		GenFilePath() now takes care of prepending dirname.
 *		Thus, our file names with the clean and clobber rules get the right prefix
 *		path variable prepended.
 *
 *		Revision 1.18  1992/11/15  16:23:23  lewis
 *		Add new stuff for kdjs changes to how makefiles setup.
 *
 *		Revision 1.17  1992/11/11  04:04:34  lewis
 *		Use qGCC_OperatorNewAndStartPlusPlusBug to mark workarounds for
 *		gcc 2.3.1 bugs.
 *
 *		Revision 1.16  1992/11/05  07:37:22  lewis
 *		Futz with spincursor stuff to make work better under MPW.
 *		And get rid of qPrintDebugInfo.
 *
 *		Revision 1.14  1992/10/28  21:18:09  lewis
 *		generate -fm in call to genClass even under unix - really should
 *		have just done when -short cmd arg given....
 *
 *		Revision 1.13  1992/10/28  01:43:16  lewis
 *		Add -short flag to allow shortened file names even when not needed (eg UNIX).
 *
 *		Revision 1.12  1992/10/22  18:16:57  lewis
 *		if gLinesGetTooLong (happens with AUX) then break up remove list into lots
 *		of lines.
 *
 *		Revision 1.11  1992/10/21  00:45:49  lewis
 *		Fixed rule generated for TemplateClean for UNIX - last hack wasn't quite right.
 *
 *		Revision 1.10  1992/10/20  21:31:25  lewis
 *		Add hack so for UNIX rm line is broken into lots of parts cuz it was
 *		overflowing.
 *
 *		Revision 1.9  1992/10/13  20:21:49  lewis
 *		Broke up call to RM in TemplateClean rule into 2 lines since
 *		was barfing - line too long under AUX.
 *
 *		Revision 1.8  1992/10/10  04:35:13  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/10  03:14:37  lewis
 *		Get rid of declares and implements - use #include instead (genclass).
 *		ALso, define qGenDependencies variable (0) to disable broken
 *		genreation of dependencies in rules to do genclassing.
 *
 *		Revision 1.6  1992/10/09  14:18:45  lewis
 *		Write filename map file even under UNIX.
 *		Fix writing of dependencies not to end with \\NL.
 *		Make TemplatesLib.a etc be double:: dependencies so we could
 *		write default version in makefile and not even build Templates.make
 *		if templates available and not have make rules break.
 *		A few other fixes to these last rules.
 *
 *		Revision 1.4  1992/10/02  23:49:10  lewis
 *		Lots of hacks - got most of the various classes generating properly-
 *		most of the hacks were required to get Mapping to work - some of them
 *		turned out to be unneeded, but I left them in anyhow.
 *
 *		Revision 1.3  1992/10/02  04:17:51  lewis
 *		Lots of stuff. Almost working. Maybe OK. Got about half the files
 *		to genmake, genclass, and compile.
 *
 *		Revision 1.2  1992/09/30  04:15:18  lewis
 *		Got basically working. Still a ways to go. Not yet integrated
 *		with Sterls new GenClass, and dont support shortnames yet.
 *		Support UNIX makefile, and half-assed attempt at Mac Makefiles.
 *
 *		<< Loosely based on code from GenClass.cc >>
 *
 */
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/GenMake.cc,v 1.25 1992/12/07 03:32:53 lewis Exp $";

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

#include	<ctype.h>
#include	<stdlib.h>
#include	<strstream.h>
#include	<fstream.h>

#include	"OSRenamePre.hh"
	#if		qMacOS
		#include	<CursorCtl.h>
	#endif
#include	"OSRenamePost.hh"

#include	"Date.hh"
#include	"Debug.hh"
#include	"File.hh"
#include	"Format.hh"
#include	"Options.hh"
#include	"Sequence.hh"
#include	"StreamUtils.hh"

#include	"DoTranslate.hh"
#include	"FileNameUtils.hh"
#include	"InfoDB.hh"




enum MakefileFormat {
	eMacMakefile,
	eUnixMakefile,
	eDosMakefile,
};

struct	ExpandedRecord {
#if		qGCC_NoAutoGenOfDefaultCTORBug
	ExpandedRecord ();
#endif
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStartPlusPlusBug
	// Also GCC bug workaround...
	ExpandedRecord (const ExpandedRecord& from);
	ExpandedRecord& operator=(const ExpandedRecord& rhs);
#endif
	Boolean				fBangDep;
	String				fName;
	Sequence(String)	fArguments;						// actual parameter names to this template i.e. String, int, etc.
	Sequence(String)	fTemplatedArguments;			// substitute real args into argument list ie MapElement(T)->MapElement(String)
	Sequence(String)	fFiles;
};
#if		qGCC_NoAutoGenOfDefaultCTORBug
ExpandedRecord::ExpandedRecord ()
{
}
#endif
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStartPlusPlusBug
ExpandedRecord::ExpandedRecord (const ExpandedRecord& from):fBangDep(from.fBangDep),fName(from.fName),fArguments(from.fArguments),fTemplatedArguments(from.fTemplatedArguments),fFiles(from.fFiles)    {}
ExpandedRecord& ExpandedRecord::operator=(const ExpandedRecord& rhs)
{
	fBangDep = rhs.fBangDep;
	fName = rhs.fName;
	fArguments = rhs.fArguments;
	fTemplatedArguments = rhs.fTemplatedArguments;
	fFiles = rhs.fFiles;
	return *this;
}
#endif
Boolean operator == (ExpandedRecord lhs, ExpandedRecord rhs)
{
	return Boolean ((lhs.fName == rhs.fName) and (lhs.fArguments == rhs.fArguments));
}


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include SetOfExpandedRecord_cc
#endif

static	void	BuildDependencies (const Mapping(String,InfoRecord)& infoRecords,
								   MakefileFormat makefileFormat,
								   istream& buildList, ostream& out);
static	void	BuildDependencies1 (const Mapping(String,InfoRecord)& infoRecords,
								    Set(ExpandedRecord)* impliedRecords,
									String templateName,
									const Sequence(String)& files,
								    const Sequence(String)& args,
									Boolean bangDep, Boolean header);
static	void	GenMakefile (const Mapping(String,InfoRecord)& infoRecords,
							 const Set(ExpandedRecord)& impliedRecords,
							 MakefileFormat makefileFormat,
							 ostream& out);
static	String	GenFileName (const String& collection, const Sequence(String)& arguments);
static	String	GenFilePath (MakefileFormat makefileFormat, const String& collection, const Sequence(String)& arguments, const String& directoryPrefix);
static	void	WriteFileNameTemplateMapFile (const Set(ExpandedRecord)& impliedRecords);
static	Sequence(String)	Snarf (const InfoRecord& i, const ExpandedRecord& e,
								   const Sequence(String) formalArgs);


enum {
	eVerbose,
	eBoilerPlate,
	eHeaderSuffix,
	eSourceSuffix,
	eMakefileFormat,
	eMakefileFormat_Mac,
	eMakefileFormat_Unix,
	eMakefileFormat_Dos,
	eShortFileNames,
};
#if		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
	#if		qRealTemplatesAvailable
	static	Bag<OptionDefinition>	BogusFunction()
	#else
	static	Bag(OptionDefinition)	BogusFunction()
	#endif
{
#endif
#if		!qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
const
#endif
OptionDefinition	kOD[] = {
	OptionDefinition ("Verbose", "v", eVerbose, False),
	OptionDefinition ("BoilerPlate", "b", eBoilerPlate, False),
	OptionDefinition ("HeaderSuffix", "hs", eHeaderSuffix, False),
	OptionDefinition ("SourceSuffix", "ss", eSourceSuffix, False),
	OptionDefinition ("MakefileFormat", "mf", eMakefileFormat, False),
	OptionDefinition ("MakefileFormat", "mfm", eMakefileFormat_Mac, False),
	OptionDefinition ("MakefileFormat", "mfu", eMakefileFormat_Unix, False),
	OptionDefinition ("MakefileFormat", "mfd", eMakefileFormat_Dos, False),
	OptionDefinition ("ShortFileNames", "short", eShortFileNames, False),
};
#if		qGCC_StaticInitializerForArrayOfObjectsWithCTORBug
	#if		qRealTemplatesAvailable
	Bag<OptionDefinition>	tmp	=	Bag<OptionDefinition> (kOD, sizeof (kOD)/sizeof (kOD[0]));
	#else
	Bag(OptionDefinition)	tmp	=	Bag(OptionDefinition) (kOD, sizeof (kOD)/sizeof (kOD[0]));
	#endif
	return (tmp);
}
#endif

class	Options : public OptionParser {
	public:
		Options ():
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
			fBoilerplates (PathName::kBad),
			fHeaderSuffix (".hh"),
			fSourceSuffix (".cc"),
			#if		qMacOS
				fMakefileFormat (eMacMakefile),
			#elif	qUnixOS
				fMakefileFormat (eUnixMakefile),
			#elif	qDOS
				fMakefileFormat (eDosMakefile),
			#else	qDOS
				#warning "Not sure what to do here"
				fMakefileFormat (eUnixMakefile),
			#endif
			fInputFile (PathName::kBad),
			fOutputFile (PathName::kBad),
			fFileNameLength (0)
			{
			}

		override	void	ParseOption_ (OptionID optionID)
		{
			switch (optionID) {
				case	eVerbose:				fVerbose = True; break;
				case	eBoilerPlate:			fBoilerplates = SnagNextPathNameArg (); break;
				case	eHeaderSuffix:			fHeaderSuffix = SnagNextArg (); break;
				case	eSourceSuffix:			fSourceSuffix = SnagNextArg (); break;
				case	eMakefileFormat: {
						String	mFormat = SnagNextArg ();
						if (toupper (mFormat) == "MAC") {
							fMakefileFormat = eMacMakefile;
						}
						else if (toupper (mFormat) == "UNIX") {
							fMakefileFormat = eUnixMakefile;
						}
						else if (toupper (mFormat) == "DOS") {
							fMakefileFormat = eDosMakefile;
						}
						else {
							sBadArgs.Raise ("Bad argument to -MakefileFormat");
						}
					}
					break;
				case	eMakefileFormat_Mac:	fMakefileFormat = eMacMakefile; break;
				case	eMakefileFormat_Unix:	fMakefileFormat = eUnixMakefile; break;
				case	eMakefileFormat_Dos:	fMakefileFormat = eDosMakefile; break;
				case	eShortFileNames:		fFileNameLength = strtol (SnagNextArg ()); break;
				default:	RequireNotReached();
			}
		}

		override	void	AddIthParam_ (const String& param, unsigned i)
		{
			switch (i) {
				case	1:	fInputFile = FileSystem::Get ().ParseStringToPathName (param); break;
				case	2:	fOutputFile = FileSystem::Get ().ParseStringToPathName (param); break;
				default:	sBadArgs.Raise ("Unexpected third (non-option) arg");
			}
		}

		Boolean				fVerbose;
		PathName			fBoilerplates;
		String 				fHeaderSuffix;
		String 				fSourceSuffix;
		MakefileFormat		fMakefileFormat;
		PathName			fInputFile;
		PathName			fOutputFile;
		size_t				fFileNameLength;
};


// These are left uninitialzed since we always set them first thing
// in main after parsing our options.
static	Boolean	gVerbose;
static	String	gHeaderSuffix;
static	String	gSourceSuffix;
static	size_t	gFileNameMax;
static	String	gFileNameMapFile	=	"TFileMap.hh";
static	Boolean	gLinesGetTooLong	=	True;

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
		gHeaderSuffix = options.fHeaderSuffix;
		gSourceSuffix = options.fSourceSuffix;
		switch (options.fMakefileFormat) {
			case	eMacMakefile:	gFileNameMax = 31-5; break;	// minus .cc.o
			case	eUnixMakefile:	gFileNameMax = 1024; break;
			case	eDosMakefile:	gFileNameMax = 8; break;
			default:				AssertNotReached ();
		}
		if (options.fFileNameLength != 0) {
			gFileNameMax = options.fFileNameLength;
		}
	}
	catch1 (Options::sBadArgs) {
		cout << Options::sBadArgs.GetMessage () << endl;
		const	String	kUsage	=
			"GenMake \n"
				"\t[-Verbose] {-long}\n"
				"\t-BoilerPlate <boilerplates> {-b}\n"
				"\t[-HeaderSuffix <header suffix>] {-hs}\n"
				"\t[-SourceSuffix <source suffix>] {-ss}\n"
				"\t[-MakefileFormat mac/unix/dos] {-mfm,-mfu,-mfd}\n";
		cout << "Usage: " << kUsage << endl;
		::exit (1);
	}

#if		qMacOS
	::SpinCursor(10);
#endif
	if (gVerbose) {
		cout << "Reading BoilerPlate." << endl;
	}
	Mapping(String,InfoRecord)	infoRecords	= ReadInfoRecords (options.fBoilerplates);
#if		qMacOS
	::SpinCursor(10);
#endif


	AbstractFileSystem::FileDescriptor	inFD	=
		(options.fInputFile == PathName::kBad)?
			0: FileSystem::Get ().Open (options.fInputFile, O_RDONLY);
#if		qMacOS
	AbstractFileSystem::FileDescriptor	outFD	=
		(options.fOutputFile == PathName::kBad)?
			0: FileSystem::Get_MacOS ().Open (options.fOutputFile,
										O_RDWR | O_CREAT | O_TRUNC, 'MPS ', 'TEXT');
#else
	AbstractFileSystem::FileDescriptor	outFD	=
		(options.fOutputFile == PathName::kBad)?
			0: FileSystem::Get ().Open (options.fOutputFile,
										O_RDWR | O_CREAT | O_TRUNC);
#endif

	istream*	in	=	0;
	ifstream ixx(inFD);
	if (inFD==0) in=&cin; else in=&ixx;
	ostream*	out	=	0;
	ofstream oxx(outFD);
	if (outFD==0) out=&cout; else out=&oxx;
	AssertNotNil (in);
	AssertNotNil (out);
	BuildDependencies (infoRecords, options.fMakefileFormat, *in, *out);

	return (0);
}

static	void	BuildDependencies (const Mapping(String,InfoRecord)& infoRecords,
								   MakefileFormat makefileFormat,
								   istream& buildList, ostream& out)
{
#if		qMacOS
	::SpinCursor(10);
#endif
	Set(ExpandedRecord)	impliedRecords;

	/*
	 * Iterate over the line items in the input. Each line is of the form:
	 *		[!]TEMPLATE_NAME	["DEP_FILE_NAME"]* [TEMPLATE_ARG]*
	 * EG:
	 *		Array int
	 *		Sequence_DoubleLinkList "String.hh" String
	 *		!Sequence MyPrivateStruct
	 *
	 * Order of DEP_FILE_NAME and TEMPLATE_ARG can be mixed - we differentiate
	 * based on the occurence of "'s.
	 */
	if (gVerbose) {
		cout << "Building list of dependencies." << endl;
	}
	while (buildList) {
		/*
		 * There was an implementation here that read the stream in one line at a time,
		 * and wrapped each line in a new stream. This silently failed using Apple's (and
		 * thus very likely CFront's) stream implementation. I believe their implementation
		 * does not allow simultaneous reads from two stream (I ran into similar problems
		 * in Emily once).
		 */

		Boolean	bangDep	=	False;
		String	templateName;
		buildList >> templateName;
		if (templateName.GetLength () == 0) {
			break;
		}
		if (templateName[1] == '!') {
			bangDep = True;
			templateName = templateName.SubString (2);
		}
		if (buildList) {
			Sequence(String)	files;
			Sequence(String)	args;
			while ((buildList) and (buildList.peek () != '\n')) {
				String	tmp;
				buildList >> tmp;
				if (tmp != "") {
					if ((tmp[1] == '"') or (tmp[1] == '\'')) {
						files += tmp;
					}
					else {
						args += tmp;
					}
				}
			}
	
			BuildDependencies1 (infoRecords, &impliedRecords, templateName,
								files, args, bangDep, False);
		}
	}

#if		qMacOS
	::SpinCursor(10);
#endif
	WriteFileNameTemplateMapFile (impliedRecords);
#if		qMacOS
	::SpinCursor(10);
#endif
	GenMakefile (infoRecords, impliedRecords, makefileFormat, out);
}

static	void	BuildDependencies1 (const Mapping(String,InfoRecord)& infoRecords,
								    Set(ExpandedRecord)* impliedRecords,
									String templateName,
									const Sequence(String)& files,
								    const Sequence(String)& args,
									Boolean bangDep, Boolean header)
{
#if		qMacOS
	::SpinCursor(10);
#endif
	RequireNotNil (impliedRecords);
	if (gVerbose) {
		cout << tab << "Adding dependencies for template: '"
			 << templateName << "'" << endl;
	}
	InfoRecord		i;
	if (not infoRecords.Lookup (templateName, &i)) {
		Exception ().Raise ();
	}
	AssertNotNil (impliedRecords);
	ExpandedRecord	e;
	e.fName = i.fTemplateName;
	e.fArguments = args;
	e.fFiles = files;
	e.fBangDep = bangDep;

	AssertNotNil (impliedRecords);
	{
		ForEach (ExpandedRecord, aa, *impliedRecords) {
			ExpandedRecord er = aa.Current ();
			if ((er == e) and ((not er.fBangDep) and bangDep)) {
				e.fBangDep = True;
				impliedRecords->Remove (er);
				impliedRecords->Add (e);
				break;
			}
		}
	}

#if		qMacOS
	::SpinCursor(10);
#endif
	AssertNotNil (impliedRecords);
	if (not impliedRecords->Contains (e)) {
		impliedRecords->Add (e);
		{
			ForEach (InfoDependency, it, i.fHeaderDependencies) {
				Boolean currentBangDep = it.Current ().fBangDep;
				BuildDependencies1 (infoRecords, impliedRecords, it.Current ().fName,
									files, Snarf (i, e, it.Current ().fArguments),
									Boolean (bangDep or currentBangDep), True);
			}
		}
		{
			ForEach (InfoDependency, it, i.fSourceDependencies) {
				Boolean currentBangDep = it.Current ().fBangDep;
				BuildDependencies1 (infoRecords, impliedRecords, it.Current ().fName,
									files, Snarf (i, e, it.Current ().fArguments),
									Boolean (bangDep or currentBangDep), False);
			}
		}
	}
}

static	void	DoDepRule (const Mapping(String,InfoRecord)& infoRecords,
						   const ExpandedRecord& e, Boolean header,
						   MakefileFormat makefileFormat, ostream& out)
{
	String	ruleTerminator		=	"";
	String	lineContinuation	=	"";
	String	tmplOutDir			=	"";
	switch (makefileFormat) {
		case	eMacMakefile:
			{
				ruleTerminator = "\tÄ";
				lineContinuation = "¶";
				tmplOutDir = "{TmplGenDir}";
			}
			break;
		case	eUnixMakefile:
		case	eDosMakefile:
			{
				ruleTerminator = ":";
				lineContinuation = "\\";
				tmplOutDir = "$(TmplGenDir)";
			}
			break;
		default:
			AssertNotReached ();
	}

	String	suffix	=	gSourceSuffix;
	if (header) { suffix = gHeaderSuffix; }	// ?Op broken under CFront 2.1
	out << GenFilePath (makefileFormat, e.fName, e.fArguments, tmplOutDir)
		<< suffix << ruleTerminator;
// These rules are almost right but not quite - they get circular dependencies. Thats
// not the end of the world, but it makes for MAKE warnings, and sometimes redundent
// compiles, and this whole thing doesnt really help unless its 100% accurate, and can
// be counted on, so we disable it for the time being...
#define	qGenDependencies	0
#if		qGenDependencies
	{
		// Output my dependencies so they get built first
		InfoRecord		ir;
		if (not infoRecords.Lookup (e.fName, &ir)) {
			Exception ().Raise ();
		}
		{
			ForEach (InfoDependency, ith, ir.fHeaderDependencies) {
				InfoDependency	iCurrent	=	ith.Current ();
				out << lineContinuation << endl;
				out << tab << tab;
				out << GenFilePath (makefileFormat, iCurrent.fName,
									Snarf (ir, e, iCurrent.fArguments), tmplOutDir)
					<< gHeaderSuffix;
				if (e.fBangDep or iCurrent.fBangDep) {
					out << lineContinuation << endl;
					out << tab << tab;
					out << GenFilePath (makefileFormat, iCurrent.fName,
										Snarf (ir, e, iCurrent.fArguments), tmplOutDir)
						<< gSourceSuffix;
				}
			}
		}
		if (not header) {
			ForEach (InfoDependency, its, ir.fSourceDependencies) {
				InfoDependency	iCurrent	=	its.Current ();
				out << lineContinuation << endl;
				out << tab << tab;;
				out << GenFilePath (makefileFormat, iCurrent.fName,
									Snarf (ir, e, iCurrent.fArguments), tmplOutDir)
					<< gHeaderSuffix;
				if (e.fBangDep or iCurrent.fBangDep) {
					out << lineContinuation << endl;
					out << tab << tab;
					out << GenFilePath (makefileFormat, iCurrent.fName,
										Snarf (ir, e, iCurrent.fArguments), tmplOutDir)
						<< gSourceSuffix << " ";
				}
			}
			// .cc files always depend on their .hh, even if this is not an explicitly
			// given dependency...
			out << tab << tab;
			out << GenFilePath (makefileFormat, e.fName, e.fArguments, tmplOutDir)
				<< gHeaderSuffix;
		}
		out << endl;
	}
#else	/*qGenDependencies*/
	out << endl;
#endif	/*qGenDependencies*/
	{
		switch (makefileFormat) {
			case	eMacMakefile:
				{
					out << tab << "Echo Running GenClass to build "
						<< GenFileName (e.fName, e.fArguments)
						<< suffix << endl;
					out << tab << "{GenClass}";
					if (header) {
						out << " -h -t \"{Stroika_src}\"Library:Foundation:Headers:";
					}
					else {
						out << " -t \"{Stroika_src}\"Library:Foundation:Sources:";
					}
					out << " -b \"{Stroika_src}\"Etc:CollectionInfo";
					out << " -fm " << gFileNameMapFile;
					if (e.fBangDep) {
						out << " -z ";
					}
				}
				break;
			case	eUnixMakefile:
				{
					out << tab << "@echo Running GenClass to build "
						<< GenFileName (e.fName, e.fArguments)
						<< suffix << endl;
					out << tab << "@$(GenClass) -b $(Etc)/CollectionInfo";
					if (header) {
						out << " -h -t $(TemplateHeaders)";
					}
					else {
						out << " -t $(TemplateSources)";
					}
					// really only do this if -short given... but does no harm...
					if (1) {
						out << " -fm " << gFileNameMapFile;
					}
					if (e.fBangDep) {
						out << " -z ";
					}
				}
				break;
			case	eDosMakefile:
				{
					AssertNotImplemented ();
				}
				break;
			default:
				AssertNotReached ();
		}
		out << " -c " << e.fName;
#if		qGCC_BadDefaultCopyConstructorGeneration
		// gross hack - not sure why this might be necessary- I'm afraid its some sort
		// of subtle codegen bug - probably related to, but perhaps not exactly
		// qGCC_BadDefaultCopyConstructorGeneration.
		Sequence(String) XXXX = e.fArguments;
		ForEach (String, itA, XXXX)
#else
		ForEach (String, itA, e.fArguments)
#endif
			{
				out << " -a '" << itA.Current () << "'";
		}

#if		qGCC_BadDefaultCopyConstructorGeneration
		// gross hack - not sure why this might be necessary- I'm afraid its some sort
		// of subtle codegen bug - probably related to, but perhaps not exactly
		// qGCC_BadDefaultCopyConstructorGeneration.
		Sequence(String) YYYY = e.fFiles;
		ForEach (String, itB, YYYY)
#else
		ForEach (String, itB, e.fFiles)
#endif
			{
				out << " -i " << itB.Current ();
		}

		out << " -d " << GenFilePath (makefileFormat, e.fName, e.fArguments, tmplOutDir);
		if (header) {
			out  << gHeaderSuffix << endl;
		}
		else {
			out << gSourceSuffix << endl;
		}
	}
	out << endl;
}

static	void	GenMakefile (const Mapping(String,InfoRecord)& infoRecords,
							 const Set(ExpandedRecord)& impliedRecords,
							 MakefileFormat makefileFormat, ostream& out)
{
	if (gVerbose) {
		cout << "Outputting Makefile." << endl;
	}
#if		qMacOS
	::SpinCursor(10);
#endif

	String	ruleTerminator		=	"";
	String	lineContinuation	=	"";
	String	tmplOutDir			=	"";
	String	objOutDir			=	"";
	switch (makefileFormat) {
		case	eMacMakefile:
			{
				ruleTerminator = "\tÄ";
				lineContinuation = "¶";
				tmplOutDir = "{TmplGenDir}";
				objOutDir = "{ObjDir}";
			}
			break;
		case	eUnixMakefile:
		case	eDosMakefile:
			{
				ruleTerminator = ":";
				lineContinuation = "\\";
				tmplOutDir = "$(TmplGenDir)";
			}
			break;
		default:
			AssertNotReached ();
	}

	out << "#	TEMPLATE LIBRARY STUFF" << endl;
	out << "#	Generated on " << Date () << endl;
	switch (makefileFormat) {
		case	eMacMakefile:	out << "#	Makefile for Mac" << endl; break;
		case	eUnixMakefile:	out << "#	Makefile for Unix" << endl; break;
		case	eDosMakefile:	out << "#	Makefile for DOS" << endl; break;
		default:
			AssertNotReached ();
	}
	out << endl;

	if (makefileFormat == eUnixMakefile) {
		out << endl;
		out << ".SUFFIXES" << ruleTerminator << "	.cc .hh" << endl;
		out << endl;
	}

	/*
	 * Output the explicit rules to build the .cc files from the templates.
	 */
	out << endl << "#Explicit rules to build sources - these calls to GenClass also build the header" << endl;
	{
		ForEach (ExpandedRecord, it, impliedRecords) {
#if		qMacOS
			::SpinCursor(1);
#endif
			DoDepRule (infoRecords, it.Current (), True, makefileFormat, out);
			DoDepRule (infoRecords, it.Current (), False, makefileFormat, out);
		}
	}
	out << endl;


	/*
	 * Output the headers list variable.
	 */
	out << endl << endl << "TemplateHeaderList=" << lineContinuation << endl;
	{
		Boolean	onFirst	=	True;
		ForEach (ExpandedRecord, it, impliedRecords) {
#if		qMacOS
			::SpinCursor(1);
#endif
			if (not onFirst) {
				out << lineContinuation << endl;
			}
			onFirst = False;
			out << tab << GenFilePath (makefileFormat, it.Current ().fName, it.Current ().fArguments, tmplOutDir) << gHeaderSuffix << tab;
		}
	}
	out << endl;
	out << endl;


	/*
	 * Output the Sources list variable.
	 */
	out << endl << endl << "TemplateSourceList=" << lineContinuation << endl;
	{
		Boolean	onFirst	=	True;
		ForEach (ExpandedRecord, it, impliedRecords) {
#if		qMacOS
			::SpinCursor(1);
#endif
			if (not onFirst) {
				out << lineContinuation << endl;
			}
			onFirst = False;
			out << tab << GenFilePath (makefileFormat, it.Current ().fName, it.Current ().fArguments, tmplOutDir) << gSourceSuffix << tab;
		}
	}
	out << endl;
	out << endl;


	/*
	 * Output the Objs list variable.
	 */
	out << endl << endl << "TemplateObjects=" << lineContinuation << endl;
	{
		Boolean	onFirst	=	True;
		ForEach (ExpandedRecord, it, impliedRecords) {
#if		qMacOS
			::SpinCursor(1);
#endif
			if (not it.Current().fBangDep) {
				if (not onFirst) {
					out << lineContinuation << endl;
				}
				onFirst = False;
				out << tab << GenFilePath (makefileFormat, it.Current ().fName, it.Current ().fArguments, objOutDir);
				if (makefileFormat == eMacMakefile) {
					out << gSourceSuffix;
				}
				out << ".o	";
			}
		}
	}
	out << endl;
	out << endl;



	/*
	 * Library, Clean and Clobber rules
	 */
#if		qMacOS
	::SpinCursor(10);
#endif
	out << "TemplateClean" << ruleTerminator << endl;
	switch (makefileFormat) {
		case	eMacMakefile:
			out << tab << "{Delete} {DeleteOpts}  {TemplateObjects}" << endl;
			break;
		case	eUnixMakefile:
			out << tab << "@$(RM) -f $(TemplateObjects)" << endl;
			if (gLinesGetTooLong) {
				ForEach (ExpandedRecord, it, impliedRecords) {
					out << tab << "@$(RM) -f " << GenFilePath (makefileFormat, it.Current ().fName, it.Current ().fArguments, tmplOutDir) << gSourceSuffix << endl;
				}
			}
			else {
				out << tab << "@$(RM) -f $(TemplateSourceList)" << endl;
			}
			break;
		case	eDosMakefile:
			out << tab << "@$(RM) $(TemplateObjects)" << endl;
#if 0
			out << tab << "@$(RM) $(TemplateSourceList)" << endl;
#endif
			break;
		default:				AssertNotReached ();
	}
	out << endl;
#if		qMacOS
	::SpinCursor(10);
#endif
	out << "TemplateClobber" << ruleTerminator << "	TemplateClean" << endl;
	switch (makefileFormat) {
		case	eMacMakefile:
			out << tab << "{Delete} {DeleteOpts}  {TemplateHeaderList}" << "\"{TmplGenDir}\"" << "TemplatesLib.o" << endl;
			break;
		case	eUnixMakefile:
			if (gLinesGetTooLong) {
				ForEach (ExpandedRecord, it, impliedRecords) {
					out << tab << "@$(RM) -f " << GenFilePath (makefileFormat, it.Current ().fName, it.Current ().fArguments, tmplOutDir) << gHeaderSuffix << endl;
				}
				out << tab << "@$(RM) -f TemplatesLib.a" << endl;
			}
			else {
				out << tab << "@$(RM) -f $(TemplateHeaderList) TemplatesLib.a" << endl;
			}
			break;
		case	eDosMakefile:
			out << tab << "@$(RM) $(TemplateHeaderList) TmplLib.a" << endl;
			break;
		default:				AssertNotReached ();
	}
	out << endl;
#if		qMacOS
	::SpinCursor(10);
#endif
	switch (makefileFormat) {
		case	eMacMakefile:
			out << "\"{ObjDir}\"" << "TemplatesLib.o	Ä	{TemplateHeaderList}	{TemplateSourceList}	{TemplateObjects} " << endl;
			out << tab << "echo Making library TemplatesLib.o" << endl;
			out << tab << "{Lib} {LibFlags} -o " << "\"{ObjDir}\"" << "TemplatesLib.o {TemplateObjects}" << endl;	
			break;
		case	eUnixMakefile:
			out << "TemplatesLib.a:	 $(TemplateHeaderList)	$(TemplateSourceList)	$(TemplateObjects)" << endl;
			out << tab << "@echo Making library TemplatesLib.a" << endl;
			out << tab << "@$(RM) -f TemplatesLib.a" << endl;
			out << tab << "@$(AR) cq TemplatesLib.a $(TemplateObjects)" << endl;
			out << tab << "@$(RANLIB) TemplatesLib.a" << endl;
			break;
		case	eDosMakefile:
			out << "TmplLib.a:	$(TemplateHeaderList)	$(TemplateSourceList)	$(TemplateObjects)" << endl;
			out << tab << "@echo Making library TmplLib.a" << endl;
			out << tab << "@$(RM) -f TmplLib.a" << endl;
			out << tab << "@$(AR) cq TmplLib.a $(TemplateObjects)" << endl;
			break;
		default:				AssertNotReached ();
	}
	out << endl;

	switch (makefileFormat) {
		case	eMacMakefile:
			out <<	endl;
			out <<	"\"{ObjDir}\"	Ä	\"{TmplGenDir}\"" << endl;
			out << 	endl;
			out <<	".cc.o		Ä		.cc" << endl;
			out <<	tab << "Echo Compiling \"{default}.cc\"" << endl;
			out <<	tab << "# If loaddump is one by default, then use the \"{LoadStatement}\"" << endl;
			out <<	tab << "if (\"{UseLoadDump}\") " << endl;
			out <<	tab << "Set LDF \"{LoadStatement}\"" << endl;
			out <<	tab << "End" << endl;
			out <<	tab << "# If loaddump is off, or there was no \"{LoadStatement}\", then use the CPPDefines" << endl;
			out <<	tab << "if (Not \"{LDF}\") " << endl;
			out <<	tab << tab << "Set LDF  \"{CPPDefines}\"" << endl;
			out <<	tab << "End" << endl;
			out <<	tab << "if (\"{ModelFar}\" == \"0\") " << endl;
			out <<	tab << tab << "Set ModelFarFlags \"-s {default}\"" << endl;
			out <<	tab << "End" << endl;
			out <<	tab << "if (\"{UseTemplates}\" == \"0\") " << endl;
			out <<	tab << tab << "Set ExtraIncludes \"{TmplGenDir}\";" << endl;
			out <<	tab << "Else" << endl;
			out <<	tab << tab << "Set ExtraIncludes \"\";" << endl;
			out <<	tab << "End" << endl;
			out <<	tab << "{CPlus} ¶" << endl;
			out <<	tab <<tab <<  "{LDF}  ¶" << endl;
			out <<	tab << tab << "{CFlags} ¶" << endl;
			out <<	tab << tab << "{COptions} ¶" << endl;
			out <<	tab << tab << "{ModelFarFlags} ¶" << endl;
			out <<	tab << tab << "-i \"{ExtraIncludes}\" ¶" << endl;
			out <<	tab << tab << "-o \"{targDir}{default}.cc.o\" ¶" << endl;
			out <<	tab << tab << "\"{depDir}{default}.cc\"" << endl;
			break;
		case	eUnixMakefile:
			out << "TmpLinks:" << endl
				<<	"	@for i in $(TemplateObjects) ; \\" << endl
				<<	"		do \\" << endl
				<<	"		$(RM) -f `basename $$i .o`.cc;\\" << endl
				<<	"		ln -s $(TmplGenDir)/`basename $$i .o`.cc `basename $$i .o`.cc;\\" << endl
				<<	"	done" << endl;

				out <<	".cc.o:" << endl;
				out	<<	"	@echo \"Compiling $<\"" << endl;
				out	<<	"	@if [ \"$$CPlusPlusSystem\" = \"cfront\" ] ; then \\" << endl;
				out	<<	"		rm -f $*.C; \\" << endl;
				out	<<	"		ln -s $< $*.C; \\" << endl;
				out	<<	"		if [ $$UseTemplates = 1 ] ; then \\" << endl;
				out	<<	"			$(CPlusPlus) $(CPlusFlags) -c $(Includes) $*.C -o $@; \\" << endl;
				out	<<	"		else \\" << endl;
				out	<<	"			$(CPlusPlus) $(CPlusFlags) -c $(TemplateIncludes) $(Includes) $*.C -o $@; \\" << endl;
				out	<<	"		fi; \\" << endl;
				out	<<	"		rm -f $*.C; \\" << endl;
				out	<<	"	else \\" << endl;
				out	<<	"		if [ $$UseTemplates = 1 ] ; then \\" << endl;
				out	<<	"			$(CPlusPlus) $(CPlusFlags) -c $(Includes) $< -o $@; \\" << endl;
				out	<<	"		else \\" << endl;
				out	<<	"			$(CPlusPlus) $(CPlusFlags) -c $(TemplateIncludes) $(Includes) $< -o $@; \\" << endl;
				out	<<	"		fi; \\" << endl;
				out	<<	"	fi;" << endl;
			break;
		case	eDosMakefile:
			break;
		default:				AssertNotReached ();
	}
		

	out << "#END of TEMPLATE LIBRARY STUFF" << endl;
}

static	Sequence(String)	Snarf (const InfoRecord& i, const ExpandedRecord& e,
								   const Sequence(String) formalArgs)
{
	Sequence(String)	hack	=	formalArgs;	// avoid gcc barfing...
	Sequence(String)	newArgs;
	ForEach (String, it, hack) {
		newArgs.Append (Parameterize (it.Current (), e.fArguments, i.fArguments));
	}
	return (newArgs);
}

static	String	GenFileName (const String& collection, const Sequence(String)& arguments)
{
	return (ShortenFileName (TypeNameToFileName (BuildTypeName (collection, arguments, False)), gFileNameMax));
}

static	String	GenFilePath (MakefileFormat makefileFormat, const String& collection, const Sequence(String)& arguments, const String& directoryPrefix)
{
	if (directoryPrefix == "") {
		return (GenFileName (collection, arguments));
	}
	else {
		if (makefileFormat == eUnixMakefile) {
			return (directoryPrefix + String ("/") + GenFileName (collection, arguments));
		}
		else {
			return (directoryPrefix + GenFileName (collection, arguments));
		}
	}
}

static	void	WriteFileNameTemplateMapFile (const Set(ExpandedRecord)& impliedRecords)
{
	if (gVerbose) {
		cout << "Outputting Template name map file." << endl;
	}
#if		qMacOS
	AbstractFileSystem::FileDescriptor	outFD	=
		FileSystem::Get_MacOS ().Open (FileSystem::Get().ParseStringToPathName (gFileNameMapFile), O_RDWR | O_CREAT | O_TRUNC, 'MPS ', 'TEXT');
#else
	AbstractFileSystem::FileDescriptor	outFD	=
		FileSystem::Get ().Open (FileSystem::Get().ParseStringToPathName (gFileNameMapFile), O_RDWR | O_CREAT | O_TRUNC);
#endif

	ofstream	out (outFD);
	ForEach (ExpandedRecord, it, impliedRecords) {
		/*
		 * Wrap defines in #ifndef since Apples CFront barfs with internal preprocessor
		 * error after too many redefines to same value.
		 */
		out << "#ifndef\t" << TypeNameToFileName (BuildTypeName (it.Current ().fName, it.Current ().fArguments, False))
			<< "_hh\t" << endl;
		out << "\t#define\t";
		out << TypeNameToFileName (BuildTypeName (it.Current ().fName, it.Current ().fArguments, False))
			<< "_hh\t" << "\"" << GenFileName (it.Current ().fName, it.Current ().fArguments)
			<< gHeaderSuffix << "\"" << endl;
		out << "#endif" << endl;
		out << "#ifndef\t" << TypeNameToFileName (BuildTypeName (it.Current ().fName, it.Current ().fArguments, False))
			<< "_cc\t" << endl;
		out << "\t#define\t";
		out << TypeNameToFileName (BuildTypeName (it.Current ().fName, it.Current ().fArguments, False))
			<< "_cc\t" << "\"" << GenFileName (it.Current ().fName, it.Current ().fArguments)
			<< gSourceSuffix << "\"" << endl;
		out << "#endif";
		out << endl;
	}
	out.close ();
	::close (outFD);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

