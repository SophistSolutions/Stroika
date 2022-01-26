/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__GlobalSymbolTranslater__
#define	__GlobalSymbolTranslater__

/*
 * $Header: /fuji/lewis/RCS/GlobalSymbolTranslater.hh,v 1.4 1992/10/21 04:58:50 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: GlobalSymbolTranslater.hh,v $
 *		Revision 1.4  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.3  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *
 *
 */
#include	"StringDictionary.hh"
#include	"Translater.hh"




class	GlobalSymbolTranslater : public virtual Translater {
	public:
		GlobalSymbolTranslater (Mapping(String,String)& dictionary);
		GlobalSymbolTranslater (Mapping(String,String)& dictionary,
								const String& prePendage);

	protected:
		override	void	HandleToken (const Token& t);
		nonvirtual	void	_HandleBuildDictionaryToken (const Token& t);
		nonvirtual	void	_HandleTranslateModeToken (const Token& t);

		nonvirtual	Boolean	BuildDictionaryMode () const;

	private:
		Mapping(String,String)&	fIncludeDictionary;
		String			 		fPrependage;
		Boolean					fLookingForTag;
		Boolean					fLookingForEnumTag;
		Boolean					fLookingForEnumerators;
		Boolean					fLookingForEnumeratorsOpenBrace;
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__GlobalSymbolTranslater__*/
