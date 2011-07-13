/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GlobalSymbolTranslater.cc,v 1.4 1992/10/21 04:58:50 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: GlobalSymbolTranslater.cc,v $
 *		Revision 1.4  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.3  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/GlobalSymbolTranslater.cc,v 1.4 1992/10/21 04:58:50 lewis Exp $";
#endif

#include	<ctype.h>

#include	"Debug.hh"
#include	"File.hh"
#include	"StreamUtils.hh"

#include	"CPlusLexer.hh"

#include	"GlobalSymbolTranslater.hh"




/*
 ********************************************************************************
 ***************************** GlobalSymbolTranslater ***************************
 ********************************************************************************
 */

GlobalSymbolTranslater::GlobalSymbolTranslater (Mapping(String,String)& dictionary) :
	Translater (),
	fIncludeDictionary (dictionary),
	fPrependage (""),
	fLookingForTag (False),
	fLookingForEnumTag (False),
	fLookingForEnumerators (False),
	fLookingForEnumeratorsOpenBrace (False)
{
}

GlobalSymbolTranslater::GlobalSymbolTranslater (Mapping(String,String)& dictionary,
												const String& prePendage):
	Translater (),
	fIncludeDictionary (dictionary),
	fPrependage (prePendage),
	fLookingForTag (False),
	fLookingForEnumTag (False),
	fLookingForEnumerators (False),
	fLookingForEnumeratorsOpenBrace (False)
{
}

void	GlobalSymbolTranslater::HandleToken (const Token& t)
{
	if (BuildDictionaryMode ()) {
		_HandleBuildDictionaryToken (t);
	}
	else {
		_HandleTranslateModeToken (t);
	}
}

void	GlobalSymbolTranslater::_HandleBuildDictionaryToken (const Token& t)
{
	Assert (BuildDictionaryMode ());
	if (fLookingForTag) {
		/*
		 * We can be looking for a tag if we've gotten an enum, struct, class keyword and no non-whitespace.
		 * Then the next symbol sb a global symbol name.
		 */
		if (t.fTokenKind == Token::eSymbol) {
			String	mappedTo	=	"";
			if (not fIncludeDictionary.Lookup (t.fText, &mappedTo)) {
				fIncludeDictionary.Enter (t.fText, fPrependage + t.fText);
			}
			fLookingForTag = False;
		}
		else if ((t.fTokenKind != Token::eComment) and (t.fTokenKind != Token::eWhiteSpace)) {
			/*
			 * Something happened, maybe an anonymous struct (no tag?)
			 */
			fLookingForTag = False;
		}
	}
	else {
		if ((t.fTokenKind == Token::eKeyWord) and
				((t.fKeyWord == Token::eClass) or
				 (t.fKeyWord == Token::eStruct) or
				 (t.fKeyWord == Token::eUnion) or
				 (t.fKeyWord == Token::eEnum)
				 )
			) {
			fLookingForTag = True;
		}
	}
}

void	GlobalSymbolTranslater::_HandleTranslateModeToken (const Token& t)
{
	if (t.fTokenKind == Token::eSymbol) {
		String	mappedTo	=	"";
		if (fIncludeDictionary.Lookup (t.fText, &mappedTo)) {
			_GetOut () << mappedTo;
			return;
		}
	}
	Translater::_HandleToken (t);
}

Boolean	GlobalSymbolTranslater::BuildDictionaryMode () const
{
	return (fPrependage != "");
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
