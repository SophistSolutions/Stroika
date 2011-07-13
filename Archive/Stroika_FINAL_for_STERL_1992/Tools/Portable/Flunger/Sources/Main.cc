/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Main.cc,v 1.1 1992/09/08 18:24:32 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Main.cc,v $
// Revision 1.1  1992/09/08  18:24:32  lewis
// Initial revision
//
 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/Main.cc,v 1.1 1992/09/08 18:24:32 lewis Exp $";
#endif

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#pragma	push
#pragma	force_active	on
#include	"OSInit.hh"
static	OSInit	sOSInit	=	OSInit ();
#pragma	pop

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<CursorCtl.h>
#endif
#include	"OSRenamePost.hh"

#include	"Stroika-Foundation-Globals.hh"

#include	<stdlib.h>

#include	"Bag.hh"
#include	"Debug.hh"
#include	"File.hh"
#include	"StreamUtils.hh" 
#include	"String.hh"

#include	"Flunger.hh"



#if		qMacOS
// For MPW Tool - Only... So we getting spinning beach ball, and more importantly
// interactive behaviour from MPW
#pragma	push
#pragma	force_active on
class	Init {
	public:
		Init ()
			{
				::InitCursorCtl(NULL);
			}
}	aBogusObject;
#pragma pop
#endif






/*
 * Class to parse command line options in a system independent fasion.
 */

	class OptionParser;
#if		!qRealTemplatesAvailable
#if		qMPW_MacroOverflowProblem
#define	OptionDefinition	OpTd
#endif
#endif
	// qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	OptionParser;
	struct	OptionDefinition {
		OptionDefinition ();// Needed cuz we use array of these guys...
		OptionDefinition (const String& optionName, const String& optionAbbreviation,
			void (*actionProc) (OptionParser& theOptionParser, unsigned argc, const char* argv[], unsigned& current),
			Boolean caseSensative);

		String	fOptionName;
		String	fOptionAbbreviation;
		void	(*fActionProc) (OptionParser& theOptionParser, unsigned argc, const char* argv[], unsigned& current);
		Boolean	fCaseSensative;
	};
	Boolean	operator== (const OptionDefinition& lhs, const OptionDefinition& rhs) { return Boolean (rhs.fOptionName == lhs.fOptionName); }

		OptionDefinition::OptionDefinition ():
			fOptionName (kEmptyString),
			fOptionAbbreviation (kEmptyString),
			fActionProc (NULL),
			fCaseSensative (False)
			{
			}
		OptionDefinition::OptionDefinition (const String& optionName, const String& optionAbbreviation,
			void (*actionProc) (OptionParser& theOptionParser, unsigned argc, const char* argv[], unsigned& current),
			Boolean caseSensative):
			fOptionName (optionName),
			fOptionAbbreviation (optionAbbreviation),
			fActionProc (actionProc),
			fCaseSensative (caseSensative)
			{
			}
			

#if		!qRealTemplatesAvailable
#if		qMPW_MacroOverflowProblem
#define	OptionDefinition	OpTd
#endif
	Declare (Iterator, OptionDefinition);
	Declare (Collection, OptionDefinition);
	Declare (AbBag, OptionDefinition);
	Declare (Array, OptionDefinition);
	Declare (Bag_Array, OptionDefinition);
	Declare (Bag, OptionDefinition);
	Implement (Iterator, OptionDefinition);
	Implement (Collection, OptionDefinition);
	Implement (AbBag, OptionDefinition);
	Implement (Array, OptionDefinition);
	Implement (Bag_Array, OptionDefinition);
	Implement (Bag, OptionDefinition);
#endif

class	OptionParser {
	/*
	 * This object is designed to be subclassed, and the subclass will pass in the optionDefinitions
	 * in its CTOR, and provide access to answers after Parse is called.
	 */
	protected:
		OptionParser (const OptionDefinition optionDefinitions[], size_t optionDefinitionCount);


	/*
	 * This is what you call to have the given arguments parsed. Typically as a side
	 * effect, this wills set public fields of some subclass of this object or at least allow
	 * public access to the results.
	 */
	public:
		nonvirtual	void	Parse (unsigned argc, const char* argv[]);


	/*
	 * This function is rarely overriden by may be to modify the behaviour of parsing
	 * command line arguments in case your needs are unusual.
	 */
	protected:
		virtual	void	Parse_ (unsigned argc, const char* argv[]);


	/*
	 * This function is called internally by Parse_() for each arg list item,
	 * and can be overriden to provide unusual behavior for the given option.
	 */
	protected:
		nonvirtual	void	ParseOption (unsigned argc, const char* argv[], unsigned& current);
		virtual		void	ParseOption_ (unsigned argc, const char* argv[], unsigned& current);



	protected:
		nonvirtual	void	AddIthParam (const String& param, unsigned i);
		virtual		void	AddIthParam_ (const String& param, unsigned i);


	protected:
		nonvirtual	void	BadParam (const String& param, unsigned i);
		virtual		void	BadParam_ (const String& param, unsigned i);

	private:
		nonvirtual	void	UsageError ();


	private:
#if		qRealTemplatesAvailable
		Bag<OptionDefinition>		fOptionDefinitions;
#else
		Bag(OptionDefinition)		fOptionDefinitions;
#endif
		unsigned					fIthParam;					//	Ith NON-DASH parameter - usually file name...
};
OptionParser::OptionParser (const OptionDefinition optionDefinitions[], size_t optionDefinitionCount):
	fOptionDefinitions ()
{
	// Change CTOR ARG TO AbBag () when thats available in new release from Sterling...
	for (CollectionSize i = 1; i <= optionDefinitionCount; i++) {
		fOptionDefinitions.Add (optionDefinitions[i-1]);
	}
}

void	OptionParser::Parse (unsigned argc, const char* argv[])
{
	Parse_ (argc, argv);
}

void	OptionParser::Parse_ (unsigned argc, const char* argv[])
{
	fIthParam = 0;
	for (unsigned i = 1; i < argc; i++) {
		ParseOption (argc, argv, i);
	}
}

void	OptionParser::ParseOption (unsigned argc, const char* argv[], unsigned& current)
{
	Require (current >= 1);
	Require (current <= argc);
	ParseOption_ (argc, argv, current);
	Ensure (current >= 1);
	Require (current <= argc);
}

void	OptionParser::ParseOption_ (unsigned argc, const char* argv[], unsigned& current)
{
	/*
	 * This part is slightly system dependent. For now we assume all options start with
	 * a '-' which is acceptable on most systems, though really only "Right" under UNIX
	 * and MPW.
	 */
	String	argi	=	argv [current];
	if (argi[1].GetAsciiCode () == '-') {
		String	matchName	=	argi.SubString (2);		// skip dash
		ForEach (OptionDefinition, it, fOptionDefinitions) {
			OptionDefinition	o	=	it.Current ();
			if (o.fCaseSensative) {
				if ((matchName == o.fOptionName) or (matchName == o.fOptionAbbreviation)) {
					(*o.fActionProc) (*this, argc, argv, current);
					return;
				}
			}
			else {
				String	uMatchName	=	toupper (matchName);
				String	optName		=	toupper (o.fOptionName);
				String	optAbbr		=	toupper (o.fOptionAbbreviation);
				if ((uMatchName == optName) or (uMatchName == optAbbr)) {
					(*o.fActionProc) (*this, argc, argv, current);
					return;
				}
			}
		}
		BadParam (argi, current);
	}
	else {
		AddIthParam (argi, ++fIthParam);
	}
}

void	OptionParser::AddIthParam (const String& param, unsigned i)
{
	Require (i >= 1);
	AddIthParam_ (param, i);
}

void	OptionParser::AddIthParam_ (const String& param, unsigned i)
{
	BadParam (param, i);
}

void	OptionParser::BadParam (const String& param, unsigned i)
{
	Require (i >= 1);
	BadParam_ (param, i);
}

void	OptionParser::BadParam_ (const String& param, unsigned i)
{
	cerr << "Bad #" << i << " parameter: '"<< param << "'" << newline;
}



class	Options : public OptionParser {
	public:
		Options ():
			OptionParser (kOD, sizeof (kOD)/sizeof (kOD[0])),
			fVerbose (False),
	#if 	qPathNameBroke
			fInputFile (kEmptyString),
			fOutputFile (kEmptyString)
	#else
			fInputFile (PathName::kBad),
			fOutputFile (PathName::kBad)
	#endif
			{
			}

		Boolean		fVerbose;
#if 	qPathNameBroke	
		String		fInputFile;
		String		fOutputFile;
#else
		PathName	fInputFile;
		PathName	fOutputFile;
#endif
		override	void	AddIthParam_ (const String& param, unsigned i)
			{
				switch (i) {
		#if 	qPathNameBroke
					case	1:	fInputFile = param; break;
					case	2:	fOutputFile = param; break;
		#else
					case	1:	fInputFile = FileSystem::Get ().ParseStringToPathName (param); break;
					case	2:	fOutputFile = FileSystem::Get ().ParseStringToPathName (param); break;
		#endif
					default:	Exception().Raise ();
				}
			}

		static	void VerboseProc (OptionParser& theOptionParser, unsigned argc, const char* argv[], unsigned& current)
		{
			((Options&)theOptionParser).fVerbose = True;
		}
	private:
		nonvirtual	void	UsageError ();
		static	OptionDefinition	kOD[1];
};
OptionDefinition	Options::kOD[1] = {
	OptionDefinition ("Verbose", "v", Options::VerboseProc, False),
};

#if 0
const	String	Options::kUsage	=
		"PCXLate \n"
			"\t-v\n"
			"\t[-Dictionary <Includes | GlobalSymbols> <dictionaryFile>]\n"
			"\t[-From <inputDirectory>]\n"
			"\t[-To <outputDirectory>]";

void	Options::UsageError ()
{
		cerr << "Usage: " << kUsage << newline;
		::exit (1);
}
#endif




int		main (const int argc, const char* argv[])
{
	Options		theOptions	=	Options ();

	Try {
		theOptions.Parse (argc, argv);
	}
	Catch () {
//		theOptions.UsageError (cerr);
		return (1);
	}

	{
		Try {
			FlungeFile (theOptions.fInputFile, theOptions.fOutputFile, cout, theOptions.fVerbose);
			return (0);
		}
		Catch () {
			return (1);
		}
	}
	AssertNotReached ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

