/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Format_h_
#define	_Stroika_Foundation_Characters_Format_h_	1

#include	"../StroikaPreComp.h"

#include	<cctype>
#include	<locale>
#include	<string>
#include	<vector>


#include	"../Configuration/Common.h"
#include	"CodePage.h"
#include	"TChar.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			using	std::string;
			using	std::wstring;
			using	std::basic_string;

			using	Characters::CodePage;

			string	Format (const char* format, ...);
			wstring	Format (const wchar_t* format, ...);


			int		HexString2Int (const string& s);
			int		HexString2Int (const wstring& s);
			int		String2Int (const string& s);
			int		String2Int (const wstring& s);
			float	String2Float (const wstring& s);	// returns R4LLib::nan () if invalid string
			float	String2Float (const wstring& s, float returnValIfInvalidString);
			wstring	Float2String (float f, unsigned int precision = 6);		// map nan to empty string, and use limited precision, and strip trialing .0...

		}
	}
}
#endif	/*_Stroika_Foundation_Characters_Format_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Format.inl"

