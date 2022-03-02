/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__IncludeTranslater__
#define	__IncludeTranslater__

/*
 * $Header: /fuji/lewis/RCS/IncludeTranslater.hh,v 1.4 1992/10/21 04:58:50 lewis Exp $
 *
 * Description:
 *
 * An IncludeTranslater is a Translater which maps the file names in #includes to shorter names.
 * This is intended for use with UNIX systems restricted to short names, like HP's and DROS
 * where the limit is 8/3. This class will not only map according to the given dictionary, but
 * will also add entries to the dictionary given by a defualt algorithm (override _AutoGenName() to change)
 * to a good name.
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: IncludeTranslater.hh,v $
 *		Revision 1.4  1992/10/21  04:58:50  lewis
 *		Update for new GenClass support. Got to build under AUX.
 *
 *		Revision 1.3  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *		
 *
 *
 */

#include	"StringDictionary.hh"
#include	"Translater.hh"




class	IncludeTranslater : public virtual Translater {
	public:
		IncludeTranslater (Mapping(String,String)& dictionary, size_t autoGenIfLongerThan);

	protected:
		override	void	HandleToken (const Token& t);
		nonvirtual	Boolean	_HandleToken (const Token& t);


		/*
		 * Generic algotithm to shorten names. Override to do what you want.
		 */
		virtual		String	_AutoGenName (const String& oldName, size_t maxLength);

		nonvirtual	Boolean	GenNameUsed (const String& name);

		nonvirtual	Boolean	TranslatePreProcDirective (const String& text);
		nonvirtual	void	TranslateIncludeDirective (const String& includePrefix, const String& includeFileName, const String& includeSuffix);

		/*
		 * Break the include directive into 3 parts - before name, name, and after to EOL.
		 * Could be much easier with regular expresion pattern matcher.
		 */
		static	Boolean	ParseIncludeDirective (const String& inputStr, String& includePrefix, String& includeFileName, String& includeSuffix);

	private:
		Mapping(String,String)&	fIncludeDictionary;
		size_t	 		fAutoGenIfLongerThan;
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__IncludeTranslater__*/

