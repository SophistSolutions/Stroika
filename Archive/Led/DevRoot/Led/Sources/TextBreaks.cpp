/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/TextBreaks.cpp,v 2.28 2003/12/11 20:51:22 lewis Exp $
 *
 * Changes:
 *	$Log: TextBreaks.cpp,v $
 *	Revision 2.28  2003/12/11 20:51:22  lewis
 *	I noted an assert error in a regression test when buidling for single-byte - and the issue turnedou to be that I had accidentally been testing with FindLineBreaks instead of FindWordBreaks(FIXED)
 *	
 *	Revision 2.27  2003/12/09 20:43:01  lewis
 *	TextBreaks_Basic::CharToCharacterClass () now checks IsPunct for non-UNICODE as well
 *	
 *	Revision 2.26  2003/11/26 17:56:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.25  2003/11/25 22:59:26  lewis
 *	SPR#1563: Renamed TextBreaks_OLD to TextBreaks_Basic, and added TextBreaks_Basic_TextEditor and
 *	TextBreaks_Basic_WP. These different subclasses have slightly different word-break rules, appropraite
 *	for their use. Default to WP case (mostly for backward compat). Further cleanup word-break code and add
 *	more special cases appropriate to both WP and texteditor subclasses
 *	
 *	Revision 2.24  2003/11/25 21:56:42  lewis
 *	SPR#1563: first make CharToCharacterClass stuff  into methods of TextBreaks_OLD, and merge the two
 *	methods (related) into one common virtual method (so can be more easily overridden). Then fixed the
 *	existing (default) code so it does a better job handling apostrophes (single quotes) by default.
 *	Added primitive regression test mechanism for this code.
 *	
 *	Revision 2.23  2003/06/02 23:19:09  lewis
 *	SPR#1520: Fixed some bugs with TextBreaks_OLD::FindWordBreaks() dealing with hoiw it scans
 *	back for runs starting before the cursor.
 *	
 *	Revision 2.22  2003/06/02 17:49:34  lewis
 *	fixed small bug in TextBreaks_OLD::FindWordBreaks () with finding start of the word where word
 *	is 'otherclass' and starts at beginning of BUF (found working on SPR#1513 - spellcheck SPR)
 *	
 *	Revision 2.21  2003/02/28 19:09:22  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.20  2003/02/27 15:49:34  lewis
 *	had to rename CharacterProperties::isspace etc to CharacterProperties::IsSpace() to avoid
 *	name conflicts with GCC MACRO implemeantion of ISXXXX functions
 *	
 *	Revision 2.19  2003/02/24 21:57:00  lewis
 *	SPR#1306- use CodePageConverter in a few places instead of direct WideCharToMultibyte etc
 *	calls so MacOS can compile for UNICODE
 *	
 *	Revision 2.18  2003/02/24 21:31:57  lewis
 *	SPR#1309- treat _ as word char
 *	
 *	Revision 2.17  2003/02/24 19:33:34  lewis
 *	SPR#1307- moved memoizing code from TextBreaks.cpp to CharacterProperties class and use that
 *	class directly instead of my private copy of myispunct
 *	
 *	Revision 2.16  2003/02/14 20:40:20  lewis
 *	SPR#1303 - qLed_CharacterSet is now obsolete. Instead - use qSingleByteCharacters,
 *	qMultiByteCharacters, or qWideCharacters
 *	
 *	Revision 2.15  2003/02/14 19:21:33  lewis
 *	just like for MacOS - with UNICODE - treat yen character and nonbreaking space and a few
 *	others as word characters
 *	
 *	Revision 2.14  2003/01/10 21:12:05  lewis
 *	SPR#1229 - added qTestMyISWXXXFunctions assertion checking code.
 *	I noted that iswpunct returns different values with VC6 vs. VC7 for a large
 *	range of characters. Now I can build it on VC7 (under LedLineIt). Lost
 *	some obsolete functions and minor cleanups to isWXX gen code.
 *	
 *	Revision 2.13  2003/01/05 16:49:03  lewis
 *	for SPR#1220 - added myiswpunct and use that instead of myiswalpha cuz routine
 *	is a bit smaller and more like old code
 *	
 *	Revision 2.12  2003/01/05 03:43:25  lewis
 *	SPR#1220- Added qBuildMemoizedISXXXBuilderProc and myiswalpha so produces right
 *	answers on Win9x
 *	
 *	Revision 2.11  2002/05/06 21:33:57  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.10  2001/11/27 00:30:02  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.9  2001/08/29 23:36:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.8  2001/08/28 18:43:43  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.7  2000/11/10 16:10:57  lewis
 *	SPR#0846 - textbreaks in UNICODE were wrong. 99% of the bug was that after copying prevChar -
 *	I forgot the line to copy prevWordClass. But at the same time - cleaned up lots of other things -
 *	including revising (a bit better) how we deal with apostrophe chars. REALLY - must do total
 *	rewrite of this stuff someday to follow the full UNICODE spec. But this works as well -
 *	in fact probably better - than ever before (I believe)
 *	
 *	Revision 2.6  2000/07/23 16:16:17  lewis
 *	include CodePage.h and use codepage define from there
 *	
 *	Revision 2.5  2000/07/09 11:38:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2000/06/17 05:08:28  lewis
 *	SPR#0793- Lose qUseSystemWordBreakRoutine define - and replace it with
 *	new TextBreaks_System class.
 *	
 *	Revision 2.3  2000/06/16 21:13:56  lewis
 *	SPR#0778- kludge which may give us 99% of what we want on fixing word-break etc algoritms.
 *	Treat unknown characters as word characters - and things work pretty well. Sending to LEC
 *	to test if I'm right...
 *	
 *	Revision 2.2  2000/06/15 22:28:45  lewis
 *	SPR#0778- Made TextStore own a smart pointer to a TextBreak object - and by default
 *	create one with the old algorithms.
 *	
 *	Revision 2.1  2000/06/15 20:01:50  lewis
 *	SPR#0778- Broke out old word/line break code from TextStore - in preparation for making it
 *	virtual/generic - and doing different algorithms (UNICODE)Headers/TextBreaks.h
 *	
 *
 *
 *	<========== CODE MOVED HERE FROM TextStore.h ==========>
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<cctype>

#include	"CodePage.h"
#include	"LedConfig.h"

#if		qMacOS
	#include	"TextUtils.h"
#endif


#include	"TextBreaks.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif











#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif





	// These SHOULD work for UNICODE, MBYTE and SingleByte case...
	inline	bool	IsASCIISpace (Led_tChar c)
		{
			return isascii (c) and isspace (c);
		}
	inline	bool	IsASCIIAlnum (Led_tChar c)
		{
			return isascii (c) and isalnum (c);
		}
	inline	bool	IsASCIIAlpha (Led_tChar c)
		{
			return isascii (c) and isalpha (c);
		}
	inline	bool	IsASCIIDigit (Led_tChar c)
		{
			return isascii (c) and isdigit (c);
		}


#if		qWideCharacters
static	bool	SJIS_IsLeadByte (unsigned char c)
	{
		// Based on code from LEC - mtcdef.h
		return ((c >= 0x81 and c <= 0x9f) or (c >= 0xe0 and c <= 0xfc));
	}
static	bool	SJIS_IsBOLChar (const char* mbChar)
	{
		Led_AssertNotNil (mbChar);
		unsigned char byte0 = (unsigned char)mbChar[0];
		unsigned char byte1 = (unsigned char)mbChar[1];
		// Based on code from LEC - jwrap.c
		static unsigned char yBits[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
		static unsigned char yBOLTable[4][32] = {
		 {0x0,  0x0,  0x0,  0x0,  0x2,  0x52, 0x0,  0xcc, 
		  0x0,  0x0,  0x0,  0x20, 0x0,  0x0,  0x0,  0x20, 
		  0x0,  0x0,  0x0,  0x0,  0x98, 0xff, 0x0,  0x0,  
		  0x0,  0x0,  0x0,  0xc0, 0x0,  0x0,  0x0,  0x0},
		 {0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0xdf, 0x3,  0x3c, 0x1,  0x40, 0x55, 0x55, 0x5,  
		  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
		 {0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x80, 0xa2, 0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x0,  0x2a, 0x10, 0x0,  0x0},
		 {0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0x55, 0x1,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0xa8, 0x40, 0x60, 0x0,  0x8,  0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0}
		};
		unsigned char	hi;
		unsigned char	lo;
		if (SJIS_IsLeadByte (byte0)) {
			if (byte0 < (unsigned char)0x81 || byte0 > (unsigned char)0x83) return FALSE;
			else { hi = byte0; hi -= 0x80; lo = byte1; }
		}
		else {
			hi = 0;
			lo = byte0;
		}
		bool	isBOLChar	= (yBOLTable[hi][lo/8] & yBits[lo&7]);
		return (isBOLChar);
	}
static	bool	SJIS_IsEOLChar (const char* mbChar)
	{
		Led_AssertNotNil (mbChar);
	 	unsigned char byte0 = (unsigned char)mbChar[0];
	 	unsigned char byte1 = (unsigned char)mbChar[1];
		// Based on code from LEC - jwrap.c
		static unsigned char yBits[8] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };
		static unsigned char yEOLTable[2][32] = {
		 {0x0,  0x0,  0x0,  0x0,  0x0,  0x1,  0x0,  0x10, 
		  0x0,  0x0,  0x0,  0x8,  0x0,  0x0,  0x0,  0x8,  
		  0x0,  0x0,  0x0,  0x0,  0x4,  0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0},
		 {0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x0,  0xa0, 0xaa, 0xaa, 0x2,  
		  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  
		  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0}
		};
	  unsigned char hi;
	  unsigned char lo;
		if (SJIS_IsLeadByte (byte0)) {
			if (byte0 != 0x81) {
				return false;
			}
			else {
				hi = 1;
				lo = byte1;
			}
	    }
		else {
			hi = 0;
			lo = byte0;
		}
		bool	isEOLChar	=	bool (yEOLTable[hi][lo/8] & yBits[lo&7]);
		return (isEOLChar);
	}

// CONSIDER USING CodePageConverter class here!!!
	inline	unsigned	SJIS_To_Kuten_Row (const char mbChar[2])
		{
			// See alec@lec.com's 950111 email message/SPEC
			unsigned	char	c1	=	mbChar[0];
			unsigned	char	c2	=	mbChar[1];
			int	adjust		=	(c2 < 159? 1: 0);
			int	rowOffset	=	(c1 < 160? 112: 176);
			int	result		=	((c1 - rowOffset) << 1) - adjust - 32;
			Led_Assert (result >= 0);
			return (result);
		}
	inline	bool	IsJapaneseBOLChar (wchar_t c)
		{
			char	mbyteChars[2];
			size_t	nBytesInThisChar	=	2;
			CodePageConverter (kCodePage_SJIS).MapFromUNICODE (&c, 1, mbyteChars, &nBytesInThisChar);
			Led_Assert (nBytesInThisChar >= 0 and nBytesInThisChar <= 2);
			if (nBytesInThisChar == 0) {
				return 0;	// if No SJIS code page, not much we can do!
			}
			return SJIS_IsBOLChar (mbyteChars);
		}
	inline	bool	IsJapaneseEOLChar (wchar_t c)
		{
			char	mbyteChars[2];
			size_t	nBytesInThisChar	=	2;
			CodePageConverter (kCodePage_SJIS).MapFromUNICODE (&c, 1, mbyteChars, &nBytesInThisChar);
			Led_Assert (nBytesInThisChar >= 0 and nBytesInThisChar <= 2);
			if (nBytesInThisChar == 0) {
				return 0;	// if No SJIS code page, not much we can do!
			}
			return SJIS_IsEOLChar (mbyteChars);
		}
	inline	unsigned	GetJapaneseKutenRow (wchar_t c)
		{
			char	mbyteChars[2];
			size_t	nBytesInThisChar	=	2;
			CodePageConverter (kCodePage_SJIS).MapFromUNICODE (&c, 1, mbyteChars, &nBytesInThisChar);
			Led_Assert (nBytesInThisChar >= 0 and nBytesInThisChar <= 2);
			if (nBytesInThisChar == 0) {
				return 0;	// if No SJIS code page, not much we can do!
			}
			if (SJIS_IsLeadByte (mbyteChars[0])) {
				return SJIS_To_Kuten_Row (mbyteChars);
			}
			else {
				return 0;
			}
		}
#endif














/*
 ********************************************************************************
 ******************************** TextBreaks_Basic ******************************
 ********************************************************************************
 */

TextBreaks_Basic::TextBreaks_Basic ():
	inherited ()
{
	#if		qDebug
		// NB: since this is called in this CTOR - it doesn't capture (or pay attention to) subclass overrides of CharToCharacterClass
		// That fact is important - since subclasses might change its result in a way to voilate this regression test. Thats fine - if its
		// desired by the subclass. This is just a test to make sure logical changes we make to this code have intended
		// consequences... LGP 2003-11-24
		RegressionTest ();
	#endif
}

void	TextBreaks_Basic::FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordStartResult, size_t* wordEndResult, bool* wordReal
									) const
{
	Led_AssertNotNil (startOfText);
	Led_AssertNotNil (wordStartResult);
	Led_AssertNotNil (wordEndResult);
	Led_AssertNotNil (wordReal);
	Led_Assert (textOffsetToStartLookingForWord <= lengthOfText);
	#if		qMultiByteCharacters
		Led_Assert (Led_IsValidMultiByteString (startOfText, lengthOfText));
		Led_Assert (Led_IsValidMultiByteString (startOfText, textOffsetToStartLookingForWord));
		Led_Assert (Led_IsValidMultiByteString (&startOfText[textOffsetToStartLookingForWord], lengthOfText-textOffsetToStartLookingForWord));
	#endif

	if (textOffsetToStartLookingForWord == lengthOfText) {
		*wordStartResult = textOffsetToStartLookingForWord;
		*wordEndResult = textOffsetToStartLookingForWord;
		*wordReal = false;
		return;
	}
	/*
	 *	First figure out the class of characters we are looking at. Then - scan backwards
	 *	looking for the same class of characters. Then scan forwards (from our starting point)
	 *	looking for the same class of characters. Return a signal if the class of characters is
	 *	space or not (treat all other word-classes as the same for this purpose).
	 *	Except that there is a special sentinal class which always breaks anything.
	 */
	Led_Assert (textOffsetToStartLookingForWord < lengthOfText);	// cuz we checked at start - and returned if equal

	CharacterClasses	charClass = CharToCharacterClass (startOfText, lengthOfText, &startOfText[textOffsetToStartLookingForWord]);

	if (charClass == eSentinalClass) {
		*wordStartResult = textOffsetToStartLookingForWord;
		*wordEndResult = textOffsetToStartLookingForWord+1;
		*wordReal = true;
		return;
	}

	// Scan back - while character of the same class
	{
		const Led_tChar* cur = &startOfText[textOffsetToStartLookingForWord];
		if (charClass != eOtherCharacterClass and textOffsetToStartLookingForWord != 0) {
			for (const Led_tChar* maybeCur = Led_PreviousChar (startOfText, cur); ; maybeCur = Led_PreviousChar (startOfText, maybeCur)) {
				if (*maybeCur == '\n') {	//SPR#0354 - don't cross lines
					break;
				}
				CharacterClasses	curCharClass = CharToCharacterClass (startOfText, lengthOfText, maybeCur);
				if (charClass == curCharClass) {
					cur = maybeCur;
				}
				else {
					break;
				}

				if (maybeCur == startOfText) {
					break;
				}
			}
		}
		*wordStartResult = cur-startOfText;
	}
	// Scan forward - while character of the same class
	{
		const Led_tChar* cur = &startOfText[textOffsetToStartLookingForWord];
		for (; cur < &startOfText[lengthOfText]; cur = Led_NextChar (cur)) {
			if (*cur == '\n') {			//SPR#0354
				break;
			}
			CharacterClasses	curCharClass = CharToCharacterClass (startOfText, lengthOfText, cur);
			/*
			 *	On a change of char-class break. Except for the case of the special class OTHER - in which case we break
			 *	return a single char as result.
			 */
			if (charClass != curCharClass or (cur != &startOfText[textOffsetToStartLookingForWord] and charClass == eOtherCharacterClass)) {
			 	break;
			}
		}
		*wordEndResult = cur-startOfText;
	}
	*wordReal = not (charClass == eSpaceClass) and (*wordStartResult != *wordEndResult);
	#if		qMultiByteCharacters
		Led_Assert (Led_IsValidMultiByteString (startOfText, *wordStartResult));
		Led_Assert (Led_IsValidMultiByteString (startOfText, *wordEndResult));
	#endif
}

void	TextBreaks_Basic::FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordEndResult, bool* wordReal
									) const
{
	Led_AssertNotNil (startOfText);
	Led_AssertNotNil (wordEndResult);
	Led_AssertNotNil (wordReal);
	Led_Assert (textOffsetToStartLookingForWord <= lengthOfText);		// Cannot look at characters

	if (textOffsetToStartLookingForWord == lengthOfText) {
		*wordEndResult = textOffsetToStartLookingForWord;
		*wordReal = false;
		return;
	}

	#if		qMultiByteCharacters
		Led_Assert (textOffsetToStartLookingForWord <= lengthOfText);
		Led_Assert (Led_IsValidMultiByteString (startOfText, textOffsetToStartLookingForWord));														// initial segment valid
		Led_Assert (Led_IsValidMultiByteString (&startOfText[textOffsetToStartLookingForWord], lengthOfText-textOffsetToStartLookingForWord));		// second segment valid
	#endif


	#if		qSingleByteCharacters || qWideCharacters
		Led_tChar	thisChar = startOfText[textOffsetToStartLookingForWord];
	#elif	qMultiByteCharacters
		Led_tChar	thisChar[2]	=	{	startOfText[textOffsetToStartLookingForWord], Led_IsLeadByte (startOfText[textOffsetToStartLookingForWord])? startOfText[textOffsetToStartLookingForWord+1]: '\0' };
	#endif


	bool				isSpaceChar		=	IsASCIISpace (thisChar);
	CharacterClasses	startCharClass = CharToCharacterClass (startOfText, lengthOfText, &startOfText[textOffsetToStartLookingForWord]);


	// Scan forward - while character of the same class
	if (startCharClass == eSentinalClass) {
		*wordEndResult = textOffsetToStartLookingForWord+1;
		Led_Assert (not isspace (thisChar));	// else we need to cleanup the wordReal logic below...
	}
	else {
		#if		qWideCharacters
			Led_tChar	prevChar	=	thisChar;		// for Kinsoku rule - need to keep track of previous character...
														// But since we skip first char at start of loop, initialize with
														// first char!

			CharacterClasses	prevCharWordClass	=	startCharClass;
		#endif

		const Led_tChar*	end	=	&startOfText[lengthOfText];
		const Led_tChar*	cur =	Led_NextChar (&startOfText[textOffsetToStartLookingForWord]);
		for (; cur < end; cur = Led_NextChar (cur)) {
			#if		qSingleByteCharacters || qWideCharacters
				Led_tChar	thisChar = *cur;
			#elif	qMultiByteCharacters
				Led_tChar	thisChar[2]	=	{	*cur, Led_IsLeadByte (*cur)? *(cur+1): '\0' };
			#endif

			CharacterClasses	charClass = CharToCharacterClass (startOfText, lengthOfText, cur);
			if (charClass == eSentinalClass) {
				break;
			}

			/*
			 *	On a change of char-class break (space-ness) - we return a possible row break.
			 */
			bool	curCharSpaceChar	=	IsASCIISpace (thisChar);
			if (isSpaceChar != curCharSpaceChar) {
			 	break;
			}
			#if		qWideCharacters
				// 	FROM CHARLESVIEW EDITOR - (Basically) ALL I COPIED WAS THE COMMENT!
				//
				// Here is the Kinsoku rule:
				// The following character combinations cannot be broken:
				//	 an EOL character followed by any character.
				//	 any character followed by an BOL character.
				//	 any non-white space english characters.
				if (not isSpaceChar) {
					if ((charClass != eWordClass or prevCharWordClass != eWordClass or IsASCIISpace (thisChar)) 
							and not IsJapaneseEOLChar (prevChar) and not IsJapaneseBOLChar (thisChar)
							) {
						break;
					}
				}
				prevChar = thisChar;
				prevCharWordClass = charClass;
			#endif
		}
		*wordEndResult = cur-startOfText;
	}
	*wordReal = (not (IsASCIISpace (thisChar))) and (textOffsetToStartLookingForWord != *wordEndResult);
	Led_Assert (*wordEndResult <= lengthOfText);		// LGP added 950208 - in response to Alecs email message of same date - not
														// sure this assert is right, but might help debugging later...
	#if		qMultiByteCharacters
		Led_Assert (Led_IsValidMultiByteString (startOfText, *wordEndResult));	// be sure 
	#endif
}

#if		qSingleByteCharacters || qWideCharacters
TextBreaks_Basic::CharacterClasses	TextBreaks_Basic::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const
{
	#if		qSingleByteCharacters || qWideCharacters
		Led_tChar	c = *charToExamine;
	#elif	qMultiByteCharacters
		Led_tChar	c[2]	=	{	*charToExamine, Led_IsLeadByte (*charToExamine)? *(charToExamine+1): '\0' };
	#endif

	if (c == 0) {
		return eSentinalClass;
	}
	/*
		*	Return appropriate class for all characters we are SURE of. Some deepend on
		*	context (like decimal point). For those - we return eOtherCharacterClass, and
		*	let the calling software check those special cases.
		*/
	if (IsASCIISpace (c)) {
		return (eSpaceClass);
	}
	if (IsASCIIAlnum (c)) {
		return (eWordClass);
	}
	#if		qWideCharacters
		{
			unsigned	kutenRow	=	GetJapaneseKutenRow (c);
			switch (kutenRow) {
				case	3:		return (eRomanjiOrDigitClass);
				case	4:		return (eHiraganaClass);
				case	5:		return (eKatakanaClass);
				default: {
					if (kutenRow >= 16 and kutenRow <= 84) {
						return (eKanjiClass);
					}
				}
			}
		}
	#endif

	switch (c) {
		case	'.': {		// PERIOD before digits
			if (charToExamine > startOfText and charToExamine < &startOfText[lengthOfText]) {
				const	Led_tChar*	nextChar	=	charToExamine + 1;	// cuz we KNOW we are single-byte...
				
				if (IsASCIIDigit (*nextChar)) {
					return (eWordClass);
				}
			}
		}
		break;
	}

	if (not CharacterProperties::IsPunct (c)) {
		return eWordClass;
	}

	return eOtherCharacterClass;
}
#elif	qMultiByteCharacters
TextBreaks_Basic::CharacterClasses	TextBreaks_Basic::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine)
{
	#error	"Not yet supported - now looking like it never will be - use UNICODE instead"
}
#endif

#if		qDebug
void	TextBreaks_Basic::RegressionTest ()
{
	{
		const	Led_tChar*	kTest	=	LED_TCHAR_OF ("This is a good test");
		size_t				wordStartResult	=	0;
		size_t				wordEndResult	=	0;
		bool				wordReal		=	0;

		FindWordBreaks (kTest, Led_tStrlen (kTest), 1, &wordStartResult, &wordEndResult, &wordReal);
		Led_Assert (wordEndResult == 4);
		Led_Assert (wordReal == true);

		FindWordBreaks (kTest, Led_tStrlen (kTest), 4, &wordStartResult, &wordEndResult, &wordReal);
		Led_Assert (wordEndResult == 5);
		Led_Assert (wordReal == false);
	}
}
#endif







/*
 ********************************************************************************
 ***************************** TextBreaks_Basic_WP ******************************
 ********************************************************************************
 */

TextBreaks_Basic_WP::TextBreaks_Basic_WP ():
	inherited ()
{
	#if		qDebug
		// NB: since this is called in this CTOR - it doesn't capture (or pay attention to) subclass overrides of CharToCharacterClass
		// That fact is important - since subclasses might change its result in a way to voilate this regression test. Thats fine - if its
		// desired by the subclass. This is just a test to make sure logical changes we make to this code have intended
		// consequences... LGP 2003-11-24
		RegressionTest ();
	#endif
}

TextBreaks_Basic_WP::CharacterClasses	TextBreaks_Basic_WP::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const
{
	switch (*charToExamine) {
		case	',': {		// COMMA between digits
			if (charToExamine > startOfText and charToExamine < &startOfText[lengthOfText]) {
				const	Led_tChar*	prevChar	=	Led_PreviousChar (startOfText, charToExamine);
				const	Led_tChar*	nextChar	=	charToExamine + 1;	// cuz we KNOW we are single-byte...
				
				if (IsASCIIDigit (*prevChar) and IsASCIIDigit (*nextChar)) {
					return (eWordClass);
				}
			}
		}
		break;

		#if		qWideCharacters
			case	0x2019:	// curly apostrophe
		#endif
		case	'\'': {
			// APOSTROPHE between digits or letters
			if (charToExamine > startOfText and charToExamine < &startOfText[lengthOfText]) {
				const	Led_tChar*	prevChar	=	Led_PreviousChar (startOfText, charToExamine);
				const	Led_tChar*	nextChar	=	charToExamine + 1;	// cuz we KNOW we are single-byte...

				// E.g.: Fred's or Lewis', but not Jim'
				if ((IsASCIIAlnum (*prevChar) and *nextChar=='s') or (*prevChar == 's' and IsASCIISpace (*nextChar))) {
					return (eWordClass);
				}
			}
		}
		break;
	}

	#if		qSingleByteCharacters && qMacOS
		const	char	kNonBreakingSpace	=	'\0xCA';	// Inside-Mac : Text (Appendix A-6 thru A-15)
		if (*charToExamine == kNonBreakingSpace) {
			return (eWordClass);
		}
		const	char	kPoundSign	=	'\0xA3';			// Inside-Mac : Text (Appendix A-6 thru A-15)
		if (*charToExamine == kPoundSign) {
			return (eWordClass);
		}
		const	char	kYenSign	=	'\0xB4';			// Inside-Mac : Text (Appendix A-6 thru A-15)
		if (*charToExamine == kYenSign) {
			return (eWordClass);
		}
		const	char	kCentSign	=	'\0xA2';			// Inside-Mac : Text (Appendix A-6 thru A-15)
		if (*charToExamine == kCentSign) {
			return (eWordClass);
		}
	#endif

	#if		qWideCharacters
		// Mimic what we did for MacOS (Inside-Mac : Text (Appendix A-6 thru A-15))
		if (*charToExamine == kNonBreakingSpace or *charToExamine == kPoundSign or *charToExamine == kYenSign or *charToExamine == kCentSign) {
			return eWordClass;
		}
	#endif
	if (*charToExamine == '$' or *charToExamine == '%' or *charToExamine == '-') {
		return (eWordClass);
	}

	return inherited::CharToCharacterClass (startOfText, lengthOfText, charToExamine);
}

#if		qDebug
void	TextBreaks_Basic_WP::RegressionTest ()
{
	{
		const	Led_tChar*	kTest	=	LED_TCHAR_OF ("This is a good test");
		size_t				wordStartResult	=	0;
		size_t				wordEndResult	=	0;
		bool				wordReal		=	0;

		FindWordBreaks (kTest, Led_tStrlen (kTest), 1, &wordStartResult, &wordEndResult, &wordReal);
		Led_Assert (wordEndResult == 4);
		Led_Assert (wordReal == true);

		FindWordBreaks (kTest, Led_tStrlen (kTest), 4, &wordStartResult, &wordEndResult, &wordReal);
		Led_Assert (wordEndResult == 5);
		Led_Assert (wordReal == false);
	}

	{
		const	Led_tChar*	kTest	=	LED_TCHAR_OF ("This is a good test of Simone's bug with the 'word'.");
		size_t				wordStartResult	=	0;
		size_t				wordEndResult	=	0;
		bool				wordReal		=	0;

		FindWordBreaks (kTest, Led_tStrlen (kTest), 25, &wordStartResult, &wordEndResult, &wordReal);
		Led_Assert (wordEndResult == 31);
		Led_Assert (wordReal == true);
	}
}
#endif






/*
 ********************************************************************************
 ************************* TextBreaks_Basic_TextEditor **************************
 ********************************************************************************
 */

TextBreaks_Basic_TextEditor::TextBreaks_Basic_TextEditor ():
	inherited ()
{
	#if		qDebug
		// NB: since this is called in this CTOR - it doesn't capture (or pay attention to) subclass overrides of CharToCharacterClass
		// That fact is important - since subclasses might change its result in a way to voilate this regression test. Thats fine - if its
		// desired by the subclass. This is just a test to make sure logical changes we make to this code have intended
		// consequences... LGP 2003-11-24
		RegressionTest ();
	#endif
}

TextBreaks_Basic_TextEditor::CharacterClasses	TextBreaks_Basic_TextEditor::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const
{
	if (*charToExamine == '$' or *charToExamine == '%') {
		return (eWordClass);
	}
	if (*charToExamine == '_') {
		return (eWordClass);	// SPR#1309 - I think this works a little better in text word selection
	}
	return inherited::CharToCharacterClass (startOfText, lengthOfText, charToExamine);
}

#if		qDebug
void	TextBreaks_Basic_TextEditor::RegressionTest ()
{
	{
		const	Led_tChar*	kTest	=	LED_TCHAR_OF ("This is a good test of Simone's bug with the 'word'.");
		size_t				wordStartResult	=	0;
		size_t				wordEndResult	=	0;
		bool				wordReal		=	0;

		FindWordBreaks (kTest, Led_tStrlen (kTest), 25, &wordStartResult, &wordEndResult, &wordReal);
		Led_Assert (wordEndResult == 29);
		Led_Assert (wordReal == true);
	}
}
#endif






#if		qMacOS
/*
 ********************************************************************************
 ***************************** TextBreaks_System ********************************
 ********************************************************************************
 */
void	TextBreaks_System::FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordStartResult, size_t* wordEndResult, bool* wordReal
									) const
{
	Led_AssertNotNil (startOfText);
	Led_AssertNotNil (wordStartResult);
	Led_AssertNotNil (wordEndResult);
	Led_AssertNotNil (wordReal);
	Led_Assert (textOffsetToStartLookingForWord <= lengthOfText);
#if		qMultiByteCharacters
	Led_Assert (Led_IsValidMultiByteString (startOfText, lengthOfText));
	Led_Assert (Led_IsValidMultiByteString (startOfText, textOffsetToStartLookingForWord));
	Led_Assert (Led_IsValidMultiByteString (&startOfText[textOffsetToStartLookingForWord], lengthOfText-textOffsetToStartLookingForWord));
#endif

	if (textOffsetToStartLookingForWord == lengthOfText) {
		*wordStartResult = textOffsetToStartLookingForWord;
		*wordEndResult = textOffsetToStartLookingForWord;
		*wordReal = false;
		return;
	}
	/*
	 *	Unclear which script we should pass along here. Using the system script (smSystemScript) because
	 *	for Japanese people with Japanese systems - this will work fine. In other words - just use
	 *	what the user TENDs to be using. Otherwise - I'm afraid we'd have to add a (non-portable) script
	 *	parameter to this function. Perhaps it might be better to decide on the script based on the
	 *	characterset we are compiled to use?
	 */
	OffsetTable	results;
	memset (&results, 0, sizeof (results));
	::FindWordBreaks ((char*)startOfText, lengthOfText, textOffsetToStartLookingForWord, true, 0, results, smSystemScript);
	*wordStartResult = results[0].offFirst;
	*wordEndResult = results[0].offSecond;

	/*
	 *	Apple's ::FindWordBreaks() routine is rather quirky. It seems to consider a run of spaces
	 *	to be a word. But - unfortunately - it gives no indication if the return value is to be
	 *	considered as a word - or not.
	 *
	 *	We currently use CharacterType() to check if the character is a space. I'm not 100% this
	 *	is always the right thing todo - LGP 950129.
	 */
	short	theType	=	::CharacterType ((Ptr)&startOfText[*wordStartResult], 0, smSystemScript) & (smcTypeMask | smcClassMask);
	*wordReal = (*wordStartResult != *wordEndResult) and not (theType == (smCharPunct | smPunctBlank));
	#if		qMultiByteCharacters
		Led_Assert (Led_IsValidMultiByteString (startOfText, *wordStartResult));
		Led_Assert (Led_IsValidMultiByteString (startOfText, *wordEndResult));
	#endif
}

void	TextBreaks_System::FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordEndResult, bool* wordReal
									) const
{
	Led_AssertNotNil (startOfText);
	Led_AssertNotNil (wordEndResult);
	Led_AssertNotNil (wordReal);
	Led_Assert (textOffsetToStartLookingForWord <= lengthOfText);		// Cannot look at characters

	if (textOffsetToStartLookingForWord == lengthOfText) {
		*wordEndResult = textOffsetToStartLookingForWord;
		*wordReal = false;
		return;
	}

#if		qMultiByteCharacters
	Led_Assert (textOffsetToStartLookingForWord <= lengthOfText);
	Led_Assert (Led_IsValidMultiByteString (startOfText, textOffsetToStartLookingForWord));														// initial segment valid
	Led_Assert (Led_IsValidMultiByteString (&startOfText[textOffsetToStartLookingForWord], lengthOfText-textOffsetToStartLookingForWord));		// second segment valid
#endif

	/*
	 *	Unclear which script we should pass along here. Using the system script (smSystemScript) because
	 *	for Japanese people with Japanese systems - this will work fine. In other words - just use
	 *	what the user TENDs to be using. Otherwise - I'm afraid we'd have to add a (non-portable) script
	 *	parameter to this function. Perhaps it might be better to decide on the script based on the
	 *	characterset we are compiled to use?
	 */
	OffsetTable	results;
	memset (&results, 0, sizeof (results));
	::FindWordBreaks ((char*)startOfText, lengthOfText, textOffsetToStartLookingForWord, true, BreakTablePtr (-1), results, smSystemScript);
	*wordEndResult = results[0].offSecond;

	/*
	 *	Apple's ::FindWordBreaks() routine is rather quirky. It seems to consider a run of spaces
	 *	to be a word. But - unfortunately - it gives no indication if the return value is to be
	 *	considered as a word - or not.
	 *
	 *	We currently use CharacterType() to check if the character is a space. I'm not 100% this
	 *	is always the right thing todo - LGP 950129.
	 */
	short	theType	=	::CharacterType ((Ptr)&startOfText[results[0].offFirst], 0, smSystemScript) & (smcTypeMask | smcClassMask);
	*wordReal = (results[0].offFirst != *wordEndResult) and not (theType == (smCharPunct | smPunctBlank));
	Led_Assert (*wordEndResult <= lengthOfText);		// LGP added 950208 - in response to Alecs email message of same date - not
													// sure this assert is right, but might help debugging later...
	#if		qMultiByteCharacters
		Led_Assert (Led_IsValidMultiByteString (startOfText, *wordEndResult));	// be sure 
	#endif
}
#endif





#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
