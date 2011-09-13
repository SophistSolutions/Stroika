/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Linguistics_Words_h_
#define	_Stroika_Foundation_Linguistics_Words_h_	1

#include	"../StroikaPreComp.h"

#include	<string>

#include	"../Configuration/Common.h"


/*
 * TODO:
 *		(o)		Very VERY primitive linguistic support, but this could easily evolve over time. Led has a bunch of
 *				lingusitic code I could move here, and I'm sure I could dig up more...
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	Linguistics {

			using	std::string;
			using	std::wstring;


			/*
			 *	Implement current-ui-language-specific noun-pluralization logic for the given noun string (assuming the count of that noun
			 *	is given (english rules - if count != 1 - append s, but we dont wnat that logic to proliferate through the app, so
			 *	its easier to localize.
			 */
			string	PluralizeNoun (const string& s, int count = 1000);
			wstring	PluralizeNoun (const wstring& s, int count = 1000);

			wstring	MungeStringSoSingular (const wstring& s);
			string	MungeStringSoSingular (const string& s);

			string	CapitalizeEachWord (const string& s);	// e.g. 'joe smith' becomes 'Joe Smith'
			wstring	CapitalizeEachWord (const wstring& s);
			string	CapitalizeEachSentence (const string& s);	// e.g. 'joe smith' becomes 'Joe smith'
			wstring	CapitalizeEachSentence (const wstring& s);
			string	UnCapitalizeFirstWord (const string& s);	// e.g. 'Joe Smith' becomes 'joe Smith', but 'IBM eats' stays 'IBM eats'
			wstring	UnCapitalizeFirstWord (const wstring& s);
			bool	IsAllCaps (const string& s);
			bool	IsAllCaps (const wstring& s);

		}
	}
}
#endif	/*_Stroika_Foundation_Linguistics_Words_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Words.inl"

