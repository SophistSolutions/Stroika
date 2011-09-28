/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_Format_h_
#define	_Stroika_Foundation_Characters_Format_h_	1

#include	"../StroikaPreComp.h"

#include	<string>

#include	"../Configuration/Common.h"
#include	"CodePage.h"





/*
@CONFIGVAR:		qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
@DESCRIPTION:	Very surprising difference between Microsoft of Linux interpretations of sprintf(). It appears the Linux/gcc approach was
				blessed by ISO - and so is now 'correct'. But no matter, because its so fundementally different, they will have a difficult
				time changing.

				The issue is - for sprintf - and in our case vswprintf - what is the interpretation of '%s'. Does this match a char* or
				wchar_t* string in the argument list?

				According to Microsoft, it would appear to match wchar_t*.
					From http://msdn.microsoft.com/en-us/library/ybk95axf(v=vs.71).aspx:
					int main( void )
					{
					   wchar_t buf[100];
					   int len = swprintf( buf, L"%s", L"Hello world" );
					   printf( "wrote %d characters\n", len );
					   len = swprintf( buf, L"%s", L"Hello\xffff world" );
					   // swprintf fails because string contains WEOF (\xffff)
					   printf( "wrote %d characters\n", len );
					}

				But according to http://www.tin.org/bin/man.cgi?section=3&topic=vsnprintf:
					  s      If  no  l  modifier  is  present:  The  const char * argument is
							  expected to be a pointer to an array of character type  (pointer
							  to  a string).  Characters from the array are written up to (but
							  not including) a terminating null byte ('\0'); if a precision is
							  specified,  no more than the number specified are written.  If a
							  precision is given, no null byte need be present; if the  preci-
							  sion is not specified, or is greater than the size of the array,
							  the array must contain a terminating null byte.

							  If an l modifier is present: The const  wchar_t  *  argument  is
							  expected  to  be a pointer to an array of wide characters.  Wide
							  characters from the array are converted to multibyte  characters
							  (each  by  a  call  to the wcrtomb() function, with a conversion
							  state starting in the initial state before the first wide  char-
							  acter),  up  to and including a terminating null wide character.
							  The resulting multibyte characters are written up  to  (but  not
							  including)  the  terminating null byte. If a precision is speci-
							  fied, no more bytes than the number specified are  written,  but
							  no  partial multibyte characters are written. Note that the pre-
							  cision determines the number of bytes written, not the number of
							  wide  characters  or screen positions.  The array must contain a
							  terminating null wide character, unless a precision is given and
							  it  is  so  small  that  the  number of bytes written exceeds it
							  before the end of the array is reached.
	*/
#ifndef	qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
	#error "qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat should normally be defined indirectly by StroikaConfig.h"
#endif




namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			using	std::string;
			using	std::wstring;

			using	Characters::CodePage;

			/*
			 * Format is the Stroika wrapper on sprintf().
			 * The main differences between sprintf () and Format are:
			 *		(1)		No need for buffer management. ANY sized buffer will be automatically allocated internally and returned as a
			 *				string (eventually probably using Stroika::String - but for now std::string).
			 *
			 *		(2)		This version of format has a SUBTLE - but important difference from std::c++ / sprintf() in the interpretation of
			 *				%s format strings in Format(const wchar_t*).  The interpretation of Format (const char*,...) is the same
			 *				but for Format (const wchar_t* format, ...) - %s is assumed to match a const wchar_t* string in the variable
			 *				argument list.
			 *	
			 *				This deviation from the c++ standard (technically not a deviation because Stroika::Foundation::Characters::Format() isn't
			 *				covered by the stdc++ ;-)) - is because of two reasons:
			 *					a)	Microsoft has this interpretation.
			 *					b)	Its a BETTER interpretation (allowing for stuff like (Format (TSTR("time%s"), count==1? TSTR (""): TSTR ("s"));
			 *
			 *		See also qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
			 */
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

