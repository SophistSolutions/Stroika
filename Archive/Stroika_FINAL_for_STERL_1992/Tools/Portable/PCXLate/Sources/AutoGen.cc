/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/AutoGen.cc,v 1.8 1992/12/07 03:00:37 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: AutoGen.cc,v $
 *		Revision 1.8  1992/12/07  03:00:37  lewis
 *		Renamed Catch/Try to lower case.
 *
 *		Revision 1.7  1992/12/05  17:44:45  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.6  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.5  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *		Revision 1.4  1992/07/14  20:08:04  lewis
 *		Change for new File support.
 *
 *		Revision 1.3  1992/07/07  01:08:18  lewis
 *		Moved definition of Options class CTOR out of line to end of file.
 *
 *		Revision 1.2  1992/07/06  20:56:20  lewis
 *		Added verbose flag.
 *
 *
 */
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/AutoGen.cc,v 1.8 1992/12/07 03:00:37 lewis Exp $";

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

#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>
#include	<stdlib.h>

#include	"Debug.hh"
#include	"File.hh"
#include	"StreamUtils.hh"

#include	"CPlusLexer.hh"
#include	"GlobalSymbolTranslater.hh"
#include	"IncludeTranslater.hh"
#include	"StringDictionary.hh"
#include	"Translater.hh"



#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SequenceOfString_hh
#endif








const	String	kUsage	=	"AutoGen \n"
								"\t-v"
								"\t[-readDictionary <Includes/GlobalSymbols> <DictFile>]\n"
								"\t[-writeDictionary <Includes/GlobalSymbols> <DictFile>] "
								"\t[<inputFile>]*\n";

class	Options {
	public:
		Options (int argc, const char* argv[]);

		Boolean				fVerbose;
		Boolean				fMapIncludes;
		Boolean				fMapGlobalSymbols;
		String				fReadIncludeDictionary;
		String				fReadGlobalSymbolsDictionary;
		String				fWriteIncludeDictionary;
		String				fWriteGlobalSymbolsDictionary;
		Sequence(String)	fPathNameList;

	private:
		nonvirtual	void	UsageError ()
			{
				cout << "Usage: " << kUsage << endl;
				::exit (1);
			}
};

int		main (int argc, char* argv[])
{
	Options								theOptions 				=	Options (argc, argv);
	Mapping(String,String)	includeDictionary;
	Mapping(String,String)	globalSymbolDictionary;
	if (theOptions.fReadIncludeDictionary != "") {
		includeDictionary = ReadMap (FileSystem::Get().ParseStringToPathName (theOptions.fReadIncludeDictionary));
	}
	if (theOptions.fReadGlobalSymbolsDictionary != "") {
		globalSymbolDictionary = ReadMap (FileSystem::Get().ParseStringToPathName (theOptions.fReadGlobalSymbolsDictionary));
	}
	/*
	 * From the given files, figure out if we need a temp file in-between...
	 */
	ForEach (String, it, theOptions.fPathNameList) {
		if (theOptions.fVerbose) {
			cout << "Translating file: '" << it.Current () << "'" << endl;
		}
		if (theOptions.fMapIncludes) {
			if (theOptions.fVerbose) {
				cout << tab << "...Includes..." << endl;
			}
			IncludeTranslater	xlater	=	IncludeTranslater (includeDictionary, 11);
			try {
				xlater.TranslateFile (PathComponent (it.Current ()), PathName::kBad);
			}
			catch () {
				cout << "error translating file (includes): " << String (it.Current ()) << " exception caught" << endl;
			}
		}
		if (theOptions.fMapGlobalSymbols) {
			if (theOptions.fVerbose) {
				cout << tab << "...Global Symbols..." << endl;
			}
			GlobalSymbolTranslater	xlater	=	GlobalSymbolTranslater (globalSymbolDictionary, "Stroika_");
			try {
				xlater.TranslateFile (PathComponent (it.Current ()), PathName::kBad);
			}
			catch () {
				cout << "error translating file (GlobalSymbols): " << String (it.Current ()) << " exception caught" << endl;
			}
		}
	}

	if (theOptions.fVerbose) {
		cout << "Done with files - outputting dictionaries..." << endl;
	}
	if (theOptions.fWriteIncludeDictionary != "") {
		WriteMap (FileSystem::Get().ParseStringToPathName (theOptions.fWriteIncludeDictionary), includeDictionary);
	}
	if (theOptions.fWriteGlobalSymbolsDictionary != "") {
		WriteMap (FileSystem::Get().ParseStringToPathName (theOptions.fWriteGlobalSymbolsDictionary), globalSymbolDictionary);
	}
	return (0);
}





Options::Options (int argc, const char* argv[]):
	fVerbose (False),
	fMapIncludes (False),
	fMapGlobalSymbols (False),
	fReadIncludeDictionary (""),
	fReadGlobalSymbolsDictionary (""),
	fWriteIncludeDictionary (""),
	fWriteGlobalSymbolsDictionary (""),
	fPathNameList ()
{
	const	String	kVerbose		=	toupper ("-v");
	const	String	kIncludes		=	toupper ("Includes");
	const	String	kGlobalSymbols	=	toupper ("GlobalSymbols");
	const	String	kDashReadDict	=	toupper ("-readDictionary");
	const	String	kDashWriteDict	=	toupper ("-writeDictionary");

	for (int i = 1; i < argc; i++) {
		String	argi	=	toupper (argv[i]);
		if (argi == kDashReadDict) {
			i++;
			if (i+1 >= argc) {
				UsageError ();
			}
			if (toupper (argv[i]) == kIncludes) {
				i++;
				fReadIncludeDictionary = argv[i];
			}
			else if (toupper (argv[i]) == kGlobalSymbols) {
				i++;
				fReadGlobalSymbolsDictionary = argv[i];
			}
			else {
				UsageError ();
			}
		}
		else if (argi == kDashWriteDict) {
			i++;
			if (i+1 >= argc) {
				UsageError ();
			}
			if (toupper (argv[i]) == kIncludes) {
				i++;
				fWriteIncludeDictionary = argv[i];
				fMapIncludes = True;
			}
			else if (toupper (argv[i]) == kGlobalSymbols) {
				i++;
				fWriteGlobalSymbolsDictionary = argv[i];
				fMapGlobalSymbols = True;
			}
			else {
				UsageError ();
			}
		}
		else if (argi == kVerbose) {
			fVerbose = True;
		}
		else {
			fPathNameList.Append (argv[i]);
		}
	}

	if (fVerbose) {
		cout << "Options are:\n";
		cout << tab << 	"fVerbose:" << True << endl;
		cout << tab << 	"fMapIncludes:" << fMapIncludes << endl;
		cout << tab << 	"fMapIncludes:" << fMapIncludes << endl;
		cout << tab << 	"fMapGlobalSymbols:" << fMapGlobalSymbols << endl;
		cout << tab << 	"fReadIncludeDictionary:" << fReadIncludeDictionary << endl;
		cout << tab << 	"fReadGlobalSymbolsDictionary:" << fReadGlobalSymbolsDictionary << endl;
		cout << tab << 	"fWriteIncludeDictionary:" << fWriteIncludeDictionary << endl;
		cout << tab << 	"fWriteGlobalSymbolsDictionary:" << fWriteGlobalSymbolsDictionary << endl;
	}

	if ((fWriteIncludeDictionary == "") and (fWriteGlobalSymbolsDictionary == "")) {
		cout << "Must specify write of either IncludeDictionary or GlobalSymbolsDictionary" << endl;
		UsageError ();
	}
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

