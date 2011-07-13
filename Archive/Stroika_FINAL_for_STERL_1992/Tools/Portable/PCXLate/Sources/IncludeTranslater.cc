/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/IncludeTranslater.cc,v 1.4 1992/10/21 04:58:50 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: IncludeTranslater.cc,v $
 *		Revision 1.4  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.3  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *		Revision 1.2  1992/05/21  17:34:10  lewis
 *		React to change in definition of String::SubString().
 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/IncludeTranslater.cc,v 1.4 1992/10/21 04:58:50 lewis Exp $";
#endif

#include	<ctype.h>

#include	"Debug.hh"
#include	"File.hh"
#include	"StreamUtils.hh"

#include	"CPlusLexer.hh"

#include	"IncludeTranslater.hh"




Mapping(String,String)* fred = 0;// hack test for syntax error....


/*
 ********************************************************************************
 ******************************** IncludeTranslater *****************************
 ********************************************************************************
 */

IncludeTranslater::IncludeTranslater (Mapping(String,String)& dictionary, size_t autoGenIfLongerThan):
	Translater (),
	fIncludeDictionary (dictionary),
	fAutoGenIfLongerThan (autoGenIfLongerThan)
{
}

void	IncludeTranslater::HandleToken (const Token& t)
{
	if (not _HandleToken (t)) {
		Translater::_HandleToken (t);
	}
}

Boolean	IncludeTranslater::_HandleToken (const Token& t)
{
	if (t.fTokenKind == Token::ePreProcessorDirective) {
		return (TranslatePreProcDirective (t.fText));
	}
	else {
		return (False);
	}
}

String	IncludeTranslater::_AutoGenName (const String& oldName, size_t maxLength)
{
	Require (maxLength > 2);
	Require (oldName.GetLength () >= maxLength);

	String	newName	=	oldName;
	newName.SetLength (maxLength);

	// This could fail to gen a unique name, but not likely...
	if (GenNameUsed (newName)) {
		for (char i = 'a'; i <= 'z'; i++) {
			newName.SetCharAt (i, maxLength);
			if (not GenNameUsed (newName)) {
				break;
			}
		}
		if (i > 'z') {
			cout << "Warning: failed to generate a unique name mapping for " << oldName << endl;
		}
	}
	return (newName);
}

Boolean	IncludeTranslater::GenNameUsed (const String& name)
{
	ForEach (MapElement(String,String), it, fIncludeDictionary) {
		if (it.Current ().fElt == name) {
			return (True);
		}
	}
	return (False);
}

Boolean	IncludeTranslater::TranslatePreProcDirective (const String& text)
{
	String	includePrefix;
	String	includeFileName;
	String	includeSuffix;
	if (ParseIncludeDirective (text, includePrefix, includeFileName, includeSuffix)) {
		TranslateIncludeDirective (includePrefix, includeFileName, includeSuffix);
		return (True);
	}
	else {
		return (False);
	}
}

void	IncludeTranslater::TranslateIncludeDirective (const String& includePrefix, const String& includeFileName, const String& includeSuffix)
{
	_GetOut () << includePrefix;
	String	mappedTo	=	"";
	if (fIncludeDictionary.Lookup (includeFileName, &mappedTo)) {
		_GetOut () << mappedTo;
	}
	else {
		if ((fAutoGenIfLongerThan != kBadSequenceIndex) and (includeFileName.GetLength () > fAutoGenIfLongerThan)) {
			mappedTo = _AutoGenName (includeFileName, fAutoGenIfLongerThan);
			fIncludeDictionary.Enter (includeFileName, mappedTo);
			_GetOut () << mappedTo;
		}
		else {
			_GetOut () << includeFileName;
		}
	}
	_GetOut () << includeSuffix;
}

Boolean	IncludeTranslater::ParseIncludeDirective (const String& inputStr, String& includePrefix, String& includeFileName, String& includeSuffix)
{
	size_t	i		=	1;

	// Look for #
	if ((i = inputStr.IndexOf ('#')) == kBadSequenceIndex) {
		return (False);
	}

	// Skip whitespace
	for (; i <= inputStr.GetLength () and isspace (inputStr[i].GetAsciiCode ()); i++)
		;

	const	String	kInclude	=	String ("include");
	if (inputStr.GetLength () <= i + kInclude.GetLength ()) {
		return (False);
	}
	if (inputStr.SubString (i+1, kInclude.GetLength ()) != kInclude) {
		return (False);
	}
	i += kInclude.GetLength ()+1;

	// Skip whitespace
	for (; i <= inputStr.GetLength () and isspace (inputStr[i].GetAsciiCode ()); i++)
		;

	if ((inputStr [i] != '<') and(inputStr [i] != '\"')) {
		return (False);
	}
	Character		delimiter			=	(inputStr [i] == '<')? '>': '\"';
	size_t	endOfIncludePrefix	=	i;
	i++;		// skip prefix

	for (; i <= inputStr.GetLength () and inputStr[i] != delimiter; i++)
		;

	if (i > inputStr.GetLength ()) {
		return (False);
	}
	Assert (inputStr[i] == delimiter);


	/*
	 * We win!!!
	 */
	includePrefix = inputStr.SubString (1, endOfIncludePrefix);
	includeFileName = inputStr.SubString (endOfIncludePrefix+1, i - endOfIncludePrefix - 1);
	includeSuffix = inputStr.SubString (i);
	return (True);
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

