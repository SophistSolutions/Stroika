/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MapFiles.cc,v 1.6 1992/12/05 17:44:59 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MapFiles.cc,v $
 *		Revision 1.6  1992/12/05  17:44:59  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *
 *
 */
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/MapFiles.cc,v 1.6 1992/12/05 17:44:59 lewis Exp $";

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

#include	<stdlib.h>

#include	"Debug.hh"
#include	"File.hh"
#include	"StreamUtils.hh"

#include	"CPlusLexer.hh"
#include	"GlobalSymbolTranslater.hh"
#include	"IncludeTranslater.hh"
#include	"StringDictionary.hh"
#include	"Translater.hh"









const	String	kUsage	=	"MapFiles "
								"\t[-readDictionary <Includes/GlobalSymbols> <DictFile>]\n"
								"\t[-read <inputFile>]\n"
								"\t[-write <outputFile>]";


class	Options {
	public:
		Options (int argc, const char* argv[]);

		Boolean		fMapIncludes;
		Boolean		fMapGlobalSymbols;
		String		fReadIncludeDictionary;
		String		fReadGlobalSymbolsDictionary;
		String		fReadFile;
		String		fWriteFile;

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
	PathName	srcFile	=	PathComponent (theOptions.fReadFile);
	PathName	dstFile	=	PathComponent (theOptions.fWriteFile);
	PathName	tmpFile	=	PathName::kBad;
	if (theOptions.fMapIncludes and theOptions.fMapGlobalSymbols and (dstFile != PathName::kBad)) {
		tmpFile = FileSystem::Get ().GetTemporaryDirectory () + PathComponent ("fred.tmp");
	}

	if (theOptions.fMapIncludes) {
		IncludeTranslater	xlater	=	IncludeTranslater (includeDictionary, kBadSequenceIndex);
		if (tmpFile == PathName::kBad) {
			xlater.TranslateFile (srcFile, dstFile);
		}
		else {
			xlater.TranslateFile (srcFile, tmpFile);
		}
	}
	if (theOptions.fMapGlobalSymbols) {
		GlobalSymbolTranslater	xlater	=	GlobalSymbolTranslater (globalSymbolDictionary);
		if (tmpFile == PathName::kBad) {
			xlater.TranslateFile (srcFile, dstFile);
		}
		else {
			xlater.TranslateFile (tmpFile, dstFile);
		}
	}

	return (0);
}




Options::Options (int argc, const char* argv[]):
	fMapIncludes (False),
	fMapGlobalSymbols (False),
	fReadIncludeDictionary (""),
	fReadGlobalSymbolsDictionary (""),
	fReadFile (""),
	fWriteFile ("")
{
	const	String	kIncludes		=	toupper ("Includes");
	const	String	kGlobalSymbols	=	toupper ("GlobalSymbols");
	const	String	kDashReadDict	=	toupper ("-readDictionary");
	const	String	kDashRead		=	toupper ("-read");
	const	String	kDashWrite		=	toupper ("-write");

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
				fMapIncludes = True;
			}
			else if (toupper (argv[i]) == kGlobalSymbols) {
				i++;
				fReadGlobalSymbolsDictionary = argv[i];
				fMapGlobalSymbols = True;
			}
			else {
				UsageError ();
			}
		}
		else if (argi == kDashRead) {
			i++;
			if (i >= argc) {
				UsageError ();
			}
			fReadFile = argv[i];
		}
		else if (argi == kDashWrite) {
			i++;
			if (i > argc) {
				UsageError ();
			}
			fWriteFile = argv[i];
		}
		else {
			UsageError ();
		}
	}

#if		qDebug && 0
	cout << "Options are:\n";
	cout << tab << 	"fMapIncludes:" << fMapIncludes << endl;
	cout << tab << 	"fMapGlobalSymbols:" << fMapGlobalSymbols << endl;
	cout << tab << 	"fReadIncludeDictionary:" << fReadIncludeDictionary << endl;
	cout << tab << 	"fReadGlobalSymbolsDictionary:" << fReadGlobalSymbolsDictionary << endl;
	cout << tab << 	"fReadFile:" << fReadFile << endl;
	cout << tab << 	"fWriteFile:" << fWriteFile << endl;
#endif

	if (not fMapIncludes and not fMapGlobalSymbols) {
		cout << "Must specify either -map Includes or -map globalSymbols" << endl;
		UsageError ();
	}
	if (fReadFile == "") {
		cout << "Must specify either -read so we have a file to work with" << endl;
		UsageError ();
	}

}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

