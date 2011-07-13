/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Options.cc,v 1.10 1992/12/03 05:59:58 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Options.cc,v $
 *		Revision 1.10  1992/12/03  05:59:58  lewis
 *		Work around qGCC_InserterToSubclassOfOStreamBug
 *
 *		Revision 1.9  1992/11/08  00:33:46  lewis
 *		Work around qGCC_NestedClassWithTypedefParseBug, and copy
 *		fMessage in copy CTOR for OptionParseException, and use fMessage[0] = '\0'
 *		to create empty string - instead of more longwinded code.
 *
 *		Revision 1.8  1992/10/29  22:49:14  lewis
 *		Added utility OptionParser::SnagNextIntArg (),
 *		Fix so should compile with templates.
 *		Make OptionParser (const Bag<OptionDefinition>& optionDefinitions)  take Bag instead
 *		of C style array.
 *
 *		Revision 1.7  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.5  1992/10/09  05:35:31  lewis
 *		Get rid of remainging usage/support for kEmptyString.
 *
 *		Revision 1.4  1992/10/07  23:24:41  sterling
 *		Started getting ready for swithc to GenClass macros.
 *
 *		Revision 1.3  1992/09/29  14:23:01  lewis
 *		Mostly rewritten. Now dont use static fuction procs, use IDs instead.
 *
 *		<Originally developed for Flunger - then moved here>	
 *
 */



#include	"Debug.hh"
#include	"Format.hh"
#include	"Math.hh"
#include	"StreamUtils.hh"

#include	"Options.hh"




#if		!qRealTemplatesAvailable
	#include BagOfOptionDefinition_cc
#endif




/*
 ********************************************************************************
 ********************************* OptionDefinition *****************************
 ********************************************************************************
 */

OptionDefinition::OptionDefinition (const String& optionName, const String& optionAbbreviation, OptionID id, Boolean caseSensative):
	fOptionName (optionName),
	fOptionAbbreviation (optionAbbreviation),
	fID (id),
	fCaseSensative (caseSensative)
{
}




/*
 ********************************************************************************
 ******************************** OptionParseException **************************
 ********************************************************************************
 */
OptionParseException::OptionParseException () :
	Exception ()//,
//	fMessage ()
{
}

OptionParseException::OptionParseException (const Exception& exception) :
	Exception (exception)//,
//	fMessage ()
{
}

#if		qGCC_NestedClassWithTypedefParseBug
OptionParseException::OptionParseException (const OptionParser::OptionParseException& optionParseException) :
#else
OptionParseException::OptionParseException (const OptionParseException& optionParseException) :
#endif
	Exception (optionParseException)//,
//	fMessage (optionParseException.fMessage)
{
	memcpy (fMessage, optionParseException.fMessage, sizeof (fMessage));
}

void	OptionParseException::Raise (const String& message)
{
	message.ToCStringTrunc (fMessage, sizeof (fMessage));
	Exception::Raise ();
}

void	OptionParseException::Raise ()
{
	if (Exception::GetCurrent () != this) {	// if were not the current exception return something
											// indicating unknown error, otherwise preserve our
											// error #
		fMessage[0] = '\0';
	}
	Exception::Raise ();
}

String	OptionParseException::GetMessage () const
{
	return (fMessage);
}

void	OptionParseException::SetMessage (const String& message)
{
	message.ToCStringTrunc (fMessage, sizeof (fMessage));
}




/*
 ********************************************************************************
 ********************************* OptionParser *********************************
 ********************************************************************************
 */
OptionParseException	OptionParser::sBadArgs;


#if		qRealTemplatesAvailable
OptionParser::OptionParser (const Bag<OptionDefinition>& optionDefinitions) :
#else
OptionParser::OptionParser (const Bag(OptionDefinition)& optionDefinitions) :
#endif
#if		qDebug
	fParsing (False),
#endif
	fCurrentIndex (kBadSequenceIndex),
	fCurrentProgramName (""),
	fCurrentArgs (),
	fOptionDefinitions (optionDefinitions),
	fIthParam (0)
{
}

#if		qRealTemplatesAvailable
void	OptionParser::Parse (const Sequence<String>& args)
#else
void	OptionParser::Parse (const Sequence(String)& args)
#endif
{
#if		qDebug
	Require (not fParsing);
	fParsing = True;
#endif
	Parse_ ("", args);
#if		qDebug
	Require (fParsing);
	fParsing = False;
#endif
}

#if		qRealTemplatesAvailable
void	OptionParser::Parse (const String& programName, const Sequence<String>& args)
#else
void	OptionParser::Parse (const String& programName, const Sequence(String)& args)
#endif
{
#if		qDebug
	Require (not fParsing);
	fParsing = True;
#endif
	Parse_ (programName, args);
#if		qDebug
	Require (fParsing);
	fParsing = False;
#endif
}

void	OptionParser::Parse (unsigned argc, const char* argv[])
{
#if		qRealTemplatesAvailable
	Sequence<String>	s;
#else
	Sequence(String)	s;
#endif
	for (size_t i = 1; i < argc; i++) {
		s.Append (argv[i]);
	}
	Parse_ (argv[0], s);
}

#if		qRealTemplatesAvailable
void	OptionParser::Parse_ (const String& programName, const Sequence<String>& args)
#else
void	OptionParser::Parse_ (const String& programName, const Sequence(String)& args)
#endif
{
	fCurrentIndex = 1;
	fCurrentProgramName = programName;
	fCurrentArgs = args;
	fIthParam = 0;
	for (; fCurrentIndex <= fCurrentArgs.GetLength (); fCurrentIndex++) {
		ParseArg (fCurrentArgs [fCurrentIndex]);
	}
}

void	OptionParser::ParseArg (const String& param)
{
	Require (fCurrentIndex >= 1);
	Require (fCurrentIndex <= fCurrentArgs.GetLength ());
	ParseArg_ (param);
	Ensure (fCurrentIndex >= 1);
	Require (fCurrentIndex <= fCurrentArgs.GetLength () + 1);
}

void	OptionParser::ParseArg_ (const String& param)
{
	if (param == "") {
		StringStream	sbuf;
#if 	qGCC_InserterToSubclassOfOStreamBug
		((ostream&)sbuf) << "Bad empty argument at position:" << fCurrentIndex << ".";
#else
		sbuf << "Bad empty argument at position:" << fCurrentIndex << ".";
#endif
		sBadArgs.Raise (String (sbuf));
	}

	/*
	 * This part is slightly system dependent. For now we assume all options start with
	 * a '-' which is acceptable on most systems, though really only "Right" under UNIX
	 * and MPW.
	 */
	if (param[1].GetAsciiCode () == '-') {
		String	matchName	=	param.SubString (2);		// skip dash
		ForEach (OptionDefinition, it, fOptionDefinitions) {
			OptionDefinition	o	=	it.Current ();
			if (o.fCaseSensative) {
				if ((matchName == o.fOptionName) or (matchName == o.fOptionAbbreviation)) {
					ParseOption (o.fID);
					return;
				}
			}
			else {
				String	uMatchName	=	toupper (matchName);
				String	optName		=	toupper (o.fOptionName);
				String	optAbbr		=	toupper (o.fOptionAbbreviation);
				if ((uMatchName == optName) or (uMatchName == optAbbr)) {
					ParseOption (o.fID);
					return;
				}
			}
		}
		StringStream	sbuf;
#if 	qGCC_InserterToSubclassOfOStreamBug
		((ostream&)sbuf) << "Unrecognized option argument '" << param << "'.";
#else
		sbuf << "Unrecognized option argument '" << param << "'.";
#endif
		sBadArgs.Raise (String (sbuf));
	}
	else {
		AddIthParam (param, ++fIthParam);
	}
}

void	OptionParser::ParseOption (OptionID optionID)
{
	ParseOption_ (optionID);
}

void	OptionParser::ParseOption_ (OptionID optionID)
{
	StringStream	sbuf;
#if 	qGCC_InserterToSubclassOfOStreamBug
	((ostream&)sbuf) << "Didn't expect option argument '" << GetCurrentArg () << "'.";
#else
	sbuf << "Didn't expect option argument '" << GetCurrentArg () << "'.";
#endif
	sBadArgs.Raise (String (sbuf));
}

void	OptionParser::AddIthParam (const String& param, unsigned i)
{
	Require (i >= 1);
	AddIthParam_ (param, i);
}

void	OptionParser::AddIthParam_ (const String& param, unsigned /*i*/)
{
	StringStream	sbuf;
#if 	qGCC_InserterToSubclassOfOStreamBug
	((ostream&)sbuf)  << "Didn't expect extra argument '" << param << "'.";
#else
	sbuf << "Didn't expect extra argument '" << param << "'.";
#endif
	sBadArgs.Raise (String (sbuf));
}

String	OptionParser::SnagNextArg ()
{
	if (fCurrentIndex >= fCurrentArgs.GetLength ()) {
		sBadArgs.Raise ("Ran out of arguments - was expecting more.");
		AssertNotReached (); return ("");
	}
	else {
		return (fCurrentArgs[++fCurrentIndex]);
	}
}

PathName	OptionParser::SnagNextPathNameArg ()
{
	return (FileSystem::Get ().ParseStringToPathName (SnagNextArg ()));
}

int	OptionParser::SnagNextIntArg ()
{
	return (strtol (SnagNextArg ()));
}







// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

