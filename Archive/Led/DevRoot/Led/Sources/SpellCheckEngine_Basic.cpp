/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/SpellCheckEngine_Basic.cpp,v 2.34 2004/02/12 18:40:49 lewis Exp $
 *
 * Changes:
 *	$Log: SpellCheckEngine_Basic.cpp,v $
 *	Revision 2.34  2004/02/12 18:40:49  lewis
 *	SPR#1658, SPR#1595: generate better suggestions when the misspelled word is capitalized or all-caps when builtin dictionary entry isn't (doesn't)
 *	
 *	Revision 2.33  2004/02/12 17:13:41  lewis
 *	fix typo of last fix
 *	
 *	Revision 2.32  2004/02/12 17:07:40  lewis
 *	SPR#1657: update SpellCheckEngine_Basic::LookupWord_ () to treat a RightSingleQuoteMark as an apopstrophe in lookups
 *	
 *	Revision 2.31  2004/02/12 04:52:49  lewis
 *	SPR#1642: Added code to heuristic fucntion to give a bonus for transposition (still overall
 *	negative - just less than normally for mismatched words).
 *	
 *	Revision 2.30  2004/02/12 04:12:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.29  2004/02/12 04:08:16  lewis
 *	SPR#1644: Fixed SpellCheckEngine_Basic::Heuristic () to take into account when the suggestion
 *	word is longer than the missing word, and to penalize the extra characters off the end.
 *	
 *	Revision 2.28  2004/02/12 03:44:20  lewis
 *	SPR#1645: SpellCheckEngine_Basic::GenerateSuggestions () adjusted to look for such splits.
 *	
 *	Revision 2.27  2003/12/17 20:57:53  lewis
 *	changed SpellCheckEngine::GetTextBreaksUsed() (returning Led_RefCntPtr<>) to PeekAtTextBreaksUsed ()
 *	returning TextBreaks*. Reason is we don't want to have to return a long-lived object. Interferes with
 *	COM stuff (be returning object part of COM object - and that could go away. Could add ref from that -
 *	but... not worth it here)
 *	
 *	Revision 2.26  2003/12/12 19:13:08  lewis
 *	shorten included filename for MacOS (filename len limitaiton)
 *	
 *	Revision 2.25  2003/12/12 02:44:45  lewis
 *	use IsASCIIUpper () helper and avoid mistaken degenerate comparison of char value with 128
 *	
 *	Revision 2.24  2003/12/11 21:12:59  lewis
 *	SPR#1594: much improved handling of upper case words lookups
 *	
 *	Revision 2.23  2003/12/11 19:18:27  lewis
 *	SPR#1593: add SpellCheckEngine::GetTextBreaksUsed () method so I can utilize the right TextBreaks
 *	routine from the spellcheck loop. Then use that in MyCallback::GetNextMisspelling () so that we
 *	backup to the start of each word when beginning a chunk spellchecking, and more importantly -
 *	fix the code to actaully check successive chunks of no misspellings found in the first one
 *	
 *	Revision 2.22  2003/12/11 16:33:51  lewis
 *	SPR#1592: fix a couple bugs with spellchecking - mostly with hyphenated word lookup - but also
 *	a serious mem overrun (reading) in the Hueristic function
 *	
 *	Revision 2.21  2003/12/10 22:59:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.20  2003/12/10 22:46:27  lewis
 *	SPR#1558: SpellCheckEngine_Basic::LookupWord_() updated to handle hyphenized words - and various
 *	related docs updated
 *	
 *	Revision 2.19  2003/12/09 20:49:28  lewis
 *	SPR#1585: added UD support (add word to UD, new SpellCheckEngine_Basic_Simple, major revision
 *	to SpellCheckEngine_Basic::EditableDictionary, etc)
 *	
 *	Revision 2.18  2003/06/30 14:28:32  lewis
 *	qBasicString_Missing_CompareOverload_T for GCC 2.96
 *	
 *	Revision 2.17  2003/06/25 16:44:55  lewis
 *	SPR#1543: Added OtherStringToIgnore_Number () so we can ignore numbers as undefined words. SPR#1540:
 *	did binary search word lookup (LookupWordHelper_()). Fixed up regression tests to reflect cleaner
 *	lookup semantics
 *	
 *	Revision 2.16  2003/06/23 22:09:30  lewis
 *	SPR#1538: dictionary source compiler work
 *	
 *	Revision 2.15  2003/06/20 16:13:21  lewis
 *	SPR#1538: separated out dictionary data into separate object and supported compiled dictionary
 *	object from MungeDictionaries utility and #include that directly. All basically works - though
 *	needs lots of tuning!
 *	
 *	Revision 2.14  2003/06/04 03:24:54  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.13  2003/06/04 02:45:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2003/06/03 23:31:03  lewis
 *	use new AsymetricCaseInsensativeCompare () helper function instead of
 *	Led_tStrniCmp - in part cuz of trouble compiling old code on GCC (and I
 *	wanted to change it anyhow)
 *	
 *	Revision 2.11  2003/06/03 21:48:12  lewis
 *	slight improvement in spellcheck guessing algorithm
 *	
 *	Revision 2.10  2003/06/03 20:27:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2003/06/02 23:15:53  lewis
 *	a few other small fixes to scanforundefinedwords, and improved cut-off support for
 *	suggestions list. A few more words to dict.
 *	
 *	Revision 2.8  2003/06/02 19:03:39  lewis
 *	fix a few bugs in ScanForUndefinedWords - and adjusted heuristic a bit, and
 *	embellished dictionary slightly
 *	
 *	Revision 2.7  2003/06/02 18:23:03  lewis
 *	minor fixes to scanforundefinedwords
 *	
 *	Revision 2.6  2003/06/02 16:14:04  lewis
 *	SPR#1513: first draft of (windows only so far) simple spellchecking dialog.
 *	Basically working (75%). A few fixes to generic dialog support code
 *	
 *	Revision 2.5  2003/05/30 15:57:51  lewis
 *	catch excetion in regression test
 *	
 *	Revision 2.4  2003/05/29 16:31:19  lewis
 *	SPR#1512: get better SpellCheckEngine_Basic regression test working and cleanup docs etc.
 *	
 *	Revision 2.3  2003/03/21 14:51:40  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 2.2  2002/09/11 14:18:54  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  2002/09/09 03:08:09  lewis
 *	SPR#1093- first draft of SpellCheckEngine/SpellCheckEngine_Basic
 *	
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<cmath>
#include	<cctype>

#include	"CodePage.h"

#include	"SpellCheckEngine_Basic.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif


namespace	{

#if		qIncludeBakedInDictionaries
	#if		qMacOS
		// Short filenames on MacOS
		#include	"Dictionary-Compiled-US-English."
	#else
		#include	"Dictionary-Compiled-US-English.inc"
	#endif
#endif


	inline	bool	IsASCIIUpper (Led_tChar c)
		{
			return isascii (c) and isupper (c);
		}

	inline	bool	AsymetricCaseInsensativeCompare (Led_tChar wordChar, Led_tChar dictChar)
		{
			if (wordChar == dictChar) {
				return true;
			}
			if (isascii (wordChar) and isupper (wordChar)) {
				return tolower (wordChar) == dictChar;
			}
			return false;
		}
	inline	bool	AsymetricCaseInsensativeCompare (const Led_tChar* word, const Led_tChar* dictWord)
		{
			const Led_tChar*	wi	=	word;
			const Led_tChar*	di	=	dictWord;
			for (; ; ++wi, ++di) {
				if (not AsymetricCaseInsensativeCompare (*wi, *di)) {
					return false;
				}
				if (*wi == '\0' and *di == '\0') {
					return true;
				}
				if (*wi == '\0' or *di == '\0') {
					return false;
				}
			}
		}

	struct	DictLookup_Compare : public binary_function <SpellCheckEngine_Basic::InfoBlock, Led_tString, bool> {
		DictLookup_Compare (const Led_tChar* base):
			fBase (base)
			{
			}
		bool operator()(const SpellCheckEngine_Basic::InfoBlock& _Left, const Led_tString& _Right) const
			{
				// Speed tweeked compare... don't construct string object to safe time and possible heap fragmentation
				#if		qBasicString_Missing_CompareOverload_T
					bool	answer	=	_Right.compare (fBase + _Left.fIndex, 0, _Left.fWordLen) > 0;
				#else
					bool	answer	=	_Right.compare (0, _Right.length (), fBase + _Left.fIndex, _Left.fWordLen) > 0;
				#endif
				#if		qDebug
					Led_tString	left	=	Led_tString (fBase + _Left.fIndex, fBase + _Left.fIndex + _Left.fWordLen);
					Led_Assert (answer == (left < _Right));
				#endif
				return (answer);
			}
		const Led_tChar*	fBase;
	};
}



namespace {
	inline	bool	IsASCIISpace (Led_tChar c)
		{
			return isascii (c) and isspace (c);
		}
	inline	bool	IsASCIIAlnum (Led_tChar c)
		{
			return isascii (c) and isalnum (c);
		}
}





/*
 ********************************************************************************
 *************************** SpellCheckEngine_Basic *****************************
 ********************************************************************************
 */
#if		qIncludeBakedInDictionaries
const	SpellCheckEngine_Basic::CompiledDictionary		SpellCheckEngine_Basic::kDictionary_US_English (Dictionary_US_English);
#endif

SpellCheckEngine_Basic::SpellCheckEngine_Basic (const Dictionary* mainDictionary):
	inherited (),
	fDictionaries ()
{
	if (mainDictionary != NULL) {
		fDictionaries.push_back (mainDictionary);
	}
}

SpellCheckEngine_Basic::~SpellCheckEngine_Basic ()
{
}

/*
@METHOD:		SpellCheckEngine_Basic::ScanForUndefinedWord
@DESCRIPTION:	<p>Overrides @'SpellCheckEngine::ScanForUndefinedWord'.</p>
*/
bool	SpellCheckEngine_Basic::ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
												const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
												)
{
	Led_RequireNotNil (startBuf);
	Led_RequireNotNil (endBuf);
	Led_RequireNotNil (cursor);
	Led_RequireNotNil (wordStartResult);
	Led_RequireNotNil (wordEndResult);
	Led_Require (*cursor == NULL or (*cursor >= startBuf and *cursor <= endBuf));

	if (*cursor == NULL) {
		*cursor = startBuf;
	}

	// preliminary implementation - shouldn't set output vars (wordStartResult/wordEndResult) result UNLESS WE are returning true...
	while (ScanForWord (startBuf, endBuf, cursor, wordStartResult, wordEndResult)) {
		if (not LookupWord (Led_tString (*wordStartResult, *wordEndResult)) and
			not OtherStringToIgnore (Led_tString (*wordStartResult, *wordEndResult))
			) {
			return true;
		}
	}
	return false;
}

/*
@METHOD:		SpellCheckEngine_Basic::LookupWord_
@ACCESS:		protected
@DESCRIPTION:	<p>Override (implement) @'SpellCheckEngine::LookupWord_'.</p>
*/
bool	SpellCheckEngine_Basic::LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult)
{
	Invariant ();

	/*
	 *	See if we find the word as-is, and after that, try again after a few 'rewriting' tricks
	 */
	if (LookupWordHelper_ (checkWord, matchedWordResult)) {
		return true;
	}

	if (checkWord.empty ()) {
		return false;
	}

	{
		/*
		 *	Don't compare completely case-insensatively. However- if we lookup a word which
		 *	is capitalized, allow it to match a word in the dictionary whcih is not (since the given
		 *	word could be starting a sentence).
		 */
		if (checkWord[0] < 127 and isupper (checkWord[0])) {
			Led_tString	w2	=	checkWord;
			w2[0] = tolower (w2[0]);
			if (LookupWordHelper_ (w2, matchedWordResult)) {
				return true;
			}

			/*
			 * If the word is ALL UPPERCASE - then just treat it as a Capitalized word lookup. That is - "UPPER" should match
			 * the dictionary word "Upper" or "upper", but not "uPper".
			 */
			{
				bool		allUpper		=	true;
				Led_tString	caseFixedWord	=	checkWord;
				for (Led_tString::iterator i = caseFixedWord.begin (); i != caseFixedWord.end (); ++i) {
					if (IsASCIIUpper (*i)) {
						*i = tolower (*i);
					}
					else {
						allUpper = false;
						break;
					}
				}
				if (allUpper and LookupWordHelper_ (caseFixedWord, matchedWordResult)) {
					return true;
				}
				caseFixedWord[0] = checkWord[0];
				if (allUpper and LookupWordHelper_ (caseFixedWord, matchedWordResult)) {
					return true;
				}
			}
		}
	}

	// Look for hyphenated words
	{
		/*
		 *	In our dictionary, we allow for hypenated words to be present. But thats not common. Its only for cases
		 *	where the hyphenated word isn't constructable by other means (for example - a hypenated foreign expression
		 *	where the elemental words are not part of the base language).
		 *
		 *	Next, you can put hyphens between any normal english words (even with the usual rewriting rules - like
		 *	appending 's' to the base noun word).
		 *
		 *	Then - for some special cases - some words  maybe entered in the dictionary with a suffixing hypen - to
		 *	indicate that they ONLY match when used as a hypenated prefix. For example - 'anti-' might be so-coded,
		 *	if you didn't want to allow for the word 'anti' by itself, but just as a prefix as in 'anti-war'.
		 */
		Led_tString	genMWR;
		size_t	lastFoundAt			=	0;
		bool	mustCheckLastSeg	=	false;
		for (size_t i = checkWord.find ('-'); i != Led_tString::npos or mustCheckLastSeg; i = checkWord.find ('-', i + 1)) {
			if (i == Led_tString::npos) {
				Led_Assert (mustCheckLastSeg);
				mustCheckLastSeg = false;
				i = checkWord.length ();
			}
			else {
				mustCheckLastSeg = true;	// if we find a dash - we must check afer it as well
			}
			Led_tString	segWord	=	Led_tString (checkWord.substr (lastFoundAt, i-lastFoundAt));
			Led_tString	tmpMWR;
			if (segWord.empty ()) {
				// if any segment is empty - thats not legit - so treat that as misspelled
				return false;
			}
			else if (LookupWord_ (segWord, &tmpMWR)) {
				if (lastFoundAt != 0) {
					genMWR += LED_TCHAR_OF ("-");
				}
				genMWR += tmpMWR;
			}
			else if (i != checkWord.length () and LookupWordHelper_ (segWord + LED_TCHAR_OF ("-"), &tmpMWR)) {
				if (lastFoundAt != 0) {
					genMWR += LED_TCHAR_OF ("-");
				}
				Led_Assert (tmpMWR.length () >= 2);	// must include the dash - whcih we strip...
				genMWR += tmpMWR.substr (0, tmpMWR.length () - 1);
			}
			else {
				// if any segment cannot be found - then the hyphenated word as a whole cannot
				return false;
			}
			lastFoundAt = i + 1;
		}

		// If all segments found - then return the generated (combined genMWR) matchedWordResult.
		// We know there were hyphens found if genMWR not empty
		if (not genMWR.empty ()) {
			if (matchedWordResult != NULL) {
				*matchedWordResult = genMWR;
			}
			return true;
		}
	}

	#if		qWideCharacters
		{
			const	wchar_t	kRightSingleQuotationMark	=	L'\x2019';
			size_t	apos	=	checkWord.find (kRightSingleQuotationMark);
			if (apos != Led_tString::npos) {
				Led_tString	tmp	=	checkWord;
				tmp[apos] = '\'';
				return LookupWord_ (tmp, matchedWordResult);
			}
		}
	#endif

	// PROBABALY LOTS MORE LOGIC/RULES TO ADD HERE!!!
	{
	}

	return false;
}

/*
@METHOD:		SpellCheckEngine_Basic::LookupWordHelper_
@ACCESS:		private
@DESCRIPTION:	<p>Lookup the given word (case-sensative match - using @'DictLookup_Compare'). Looks through
			all loaded dictionaries. Performs no fancy (linguistic) mapping (like tricks with punctutation
			stripping etc). Thats handled at a higher level.</p>
*/
bool	SpellCheckEngine_Basic::LookupWordHelper_ (const Led_tString& checkWord, Led_tString* matchedWordResult) const
{
	Invariant ();

	for (vector<const Dictionary*>::const_iterator i = fDictionaries.begin (); i != fDictionaries.end (); ++i) {
		const Dictionary*	dict		=	*i;
		Led_AssertNotNil (dict);

		const InfoBlock*	ibsStart	=	NULL;
		const InfoBlock*	ibsEnd		=	NULL;
		dict->GetInfoBlocks (&ibsStart, &ibsEnd);
		const Led_tChar*	dictBufStart	=	dict->GetTextBase ();

		const InfoBlock*	r		=	lower_bound (ibsStart, ibsEnd, checkWord, DictLookup_Compare (dictBufStart));
		if (r != ibsEnd) {
			Led_tString		x	=	Led_tString (dictBufStart + (*r).fIndex, dictBufStart + (*r).fIndex + (*r).fWordLen);
			if (x == checkWord) {
				if (matchedWordResult != NULL) {
					*matchedWordResult = x;
				}
				return true;
			}
		}
	}
	return false;
}

/*
@METHOD:		SpellCheckEngine_Basic::OtherStringToIgnore
@ACCESS:		protected
@DESCRIPTION:	<p>Check if the given string should be ignored as an undefined word. Look for special patterns that won't be
			found in our dictionary (e.g. numbers, strings of punctuation, etc).</p>
				<p>This is typically called from @'SpellCheckEngine_Basic::ScanForUndefinedWord'</p>
*/
bool	SpellCheckEngine_Basic::OtherStringToIgnore (const Led_tString& checkWord)
{
	return	OtherStringToIgnore_AllPunctuation (checkWord) or
			OtherStringToIgnore_Sentinals (checkWord) or
			OtherStringToIgnore_Number (checkWord)
			;
}

bool	SpellCheckEngine_Basic::OtherStringToIgnore_AllPunctuation (const Led_tString& checkWord)
{
	for (size_t i = 0; i < checkWord.length (); ++i) {
		Led_tChar	c	=	checkWord[i];
		if (not CharacterProperties::IsPunct (c)) {
			return false;
		}
	}
	return true;
}

bool	SpellCheckEngine_Basic::OtherStringToIgnore_Number (const Led_tString& checkWord)
{
	for (size_t i = 0; i < checkWord.length (); ++i) {
		Led_tChar	c	=	checkWord[i];
		// at least for English - we could tighten up the IsPunct call to just c==',' or c=='.' Not sure enough about
		// other languages, so don't bother about this. Anyhow - stuff that is all punctuation and/or digits
		// should probably be ignored anyhow... LGP 2003-06-25
		if (not (CharacterProperties::IsPunct (c) or CharacterProperties::IsDigit (c))) {
			return false;
		}
	}
	return true;
}

namespace	{
	void	AddToListsHelper (const size_t kMaxSug, Led_tString	topSugs[], float topSugScores[], float* scoreCutOff, float s, const Led_tString& w)
		{
			Led_RequireNotNil (scoreCutOff);
			if (s > *scoreCutOff) {
				// find where it fits in the array, and then adjust array
				for (size_t ii = 0; ii < kMaxSug; ++ii) { 
					if (s > topSugScores[ii]) {
						copy_backward (topSugs + ii, topSugs + kMaxSug - 1, topSugs + kMaxSug);
						copy_backward (topSugScores + ii, topSugScores + kMaxSug - 1, topSugScores + kMaxSug);
						topSugs[ii] = w;
						topSugScores[ii] = s;
						break;
					}
				}
				*scoreCutOff = topSugScores[kMaxSug-1];
			}
		}
}

vector<Led_tString>	SpellCheckEngine_Basic::GenerateSuggestions (const Led_tString& misspelledWord)
{
	const size_t	kMaxSug	=	8;
	Led_tString	topSugs[kMaxSug];
	float		topSugScores[kMaxSug];
	float		scoreCutOff	=	-100000.0f;

	fill (topSugScores, topSugScores + kMaxSug, scoreCutOff);

	//See if the word is a hyphenated combination of two words
	if (misspelledWord.find ('-') == Led_tString::npos) {
		size_t	maxInitSegSize	=	misspelledWord.size () - 1;
		for (size_t initialWordSize = 1; initialWordSize < maxInitSegSize; ++initialWordSize) {
			if (LookupWord (misspelledWord.substr (0, initialWordSize)) and
				LookupWord (misspelledWord.substr (initialWordSize))
				) {
				AddToListsHelper (kMaxSug, topSugs, topSugScores, &scoreCutOff, -1.2f, misspelledWord.substr (0, initialWordSize) + LED_TCHAR_OF ("-") + misspelledWord.substr (initialWordSize));
			}
		}
	}

	// Iterate over each dictionary
	for (vector<const Dictionary*>::const_iterator i = fDictionaries.begin (); i != fDictionaries.end (); ++i) {
		const Dictionary*	dict		=	*i;
		Led_AssertNotNil (dict);
		const InfoBlock*	ibsStart	=	NULL;
		const InfoBlock*	ibsEnd		=	NULL;
		dict->GetInfoBlocks (&ibsStart, &ibsEnd);
		const Led_tChar*	dictBufStart	=	dict->GetTextBase ();
		// Look at each word in the dictionary and assign each a score (see if they would be a good suggestion)
		for (const InfoBlock* i = ibsStart; i != ibsEnd; ++i) {
			Led_tString	w	=	Led_tString (dictBufStart + (*i).fIndex, dictBufStart + (*i).fIndex + (*i).fWordLen);
			float		s	=	Heuristic (misspelledWord, w, scoreCutOff);
			AddToListsHelper (kMaxSug, topSugs, topSugScores, &scoreCutOff, s, w);
		}
	}

	// Look at the CASE of the original word - compate with the top suggestion. Perhaps
	// clone the original suggestion and right a slightly better one which is the same as the first
	// but with case properties improved.
	if (not misspelledWord.empty ()) {
		Led_tString	topSug	=	topSugs[0];
		if (not topSug.empty ()) {
			bool	capitalize	=	false;
			bool	allCaps		=	false;
			{
				if (CharacterProperties::IsAlpha (misspelledWord[0]) and CharacterProperties::IsAlpha (topSug[0])) {
					if (isupper (misspelledWord[0]) and not isupper (topSug[0])) {
						capitalize = true;
					}
				}
				for (size_t i = 0; i < misspelledWord.length (); ++i) {
					if (CharacterProperties::IsAlpha (misspelledWord[i])) {
						if (isupper (misspelledWord[i])) {
							allCaps = true;
						}
						else {
							allCaps = false;
							break;	// if we find ANY letters alpha - but non-capitalized - then breakout and say NOT whole thing capitalized
						}

					}
				}
			}
			if (allCaps) {
				Led_tString	newWord		=	topSug;
				{
					for (size_t i = 0; i < newWord.length (); ++i) {
						if (CharacterProperties::IsAlpha (newWord[i])) {
							newWord[i] = toupper (newWord[i]);
						}
					}
				}
				float		newScore	=	topSugScores[0];
				newScore += 0.1f;
				AddToListsHelper (kMaxSug, topSugs, topSugScores, &scoreCutOff, newScore, newWord);
			}
			else if (capitalize) {
				Led_tString	newWord		=	topSug;
				{
					if (CharacterProperties::IsAlpha (newWord[0])) {
						newWord[0] = toupper (newWord[0]);
					}
				}
				float		newScore	=	topSugScores[0];
				newScore += 0.1f;
				AddToListsHelper (kMaxSug, topSugs, topSugScores, &scoreCutOff, newScore, newWord);
			}
		}
	}

	// Look for a big gap in scores, and perhaps cut-off there
	size_t	endScoreList	=	kMaxSug;
	{
		const float	kTrigger	=	1.5f;		// MUST BE RETUNED EACH TIME I CHANGE HUERISTIC FUNCTION!
		float	lastScore	=	0.0f;
		for (size_t i = 0; i < kMaxSug; ++i) {
			if (i != 0) {
				if (fabs (lastScore - topSugScores[i]) > kTrigger) {
					endScoreList = i;
					break;
				}
			}
			lastScore = topSugScores[i];
		}
	}

	vector<Led_tString>	result;
	for (size_t j = 0; j < endScoreList; ++j) {
		if (topSugs[j].empty ()) {
			break;
		}
		else {
			result.push_back (topSugs[j]);
		}
	}
	return result;
}

SpellCheckEngine_Basic::UDInterface*	SpellCheckEngine_Basic::GetUDInterface ()
{
	return NULL;
}

TextBreaks*		SpellCheckEngine_Basic::PeekAtTextBreaksUsed ()
{
	return GetTextBreaker ();
}

float	SpellCheckEngine_Basic::Heuristic (const Led_tString& misspelledWord, const Led_tString& candidateWord, float atLeast)
{
	// Totally quick and dirty hack impl...
	float	h	=	0.0f;
	size_t	mwl	=	misspelledWord.length ();
	size_t	cwl	=	candidateWord.length ();
	float	thisCharImportance	=	2.0f;
	for (size_t i = 0; i < mwl; ++i) {
		if (i >= cwl) {
			h -= thisCharImportance * 5.0f;
		}
		else if (misspelledWord[i] != candidateWord[i]) {
			float	prevH	=	h;		// saved H so we can see how much we adjusted for transposition bonus

			// do case mapping - REDO using CodePage.h. code(IsAlpha) etc... add a ToLower(C)
			if (AsymetricCaseInsensativeCompare (misspelledWord[i], candidateWord[i])) {
				h -= thisCharImportance * 0.1f;
			}
			else {
				// see if the right character is just before or after (a transpose? or missing character?)
				if (i > 0 and AsymetricCaseInsensativeCompare (misspelledWord[i], candidateWord[i-1])) {
					h -= thisCharImportance * 0.5f;
				}
				else if (i+1 < cwl and AsymetricCaseInsensativeCompare (misspelledWord[i], candidateWord[i+1])) {
					h -= thisCharImportance * 0.5f;
				}
				else {
					h -= thisCharImportance * 3.0f;
				}
			}

			// See if the mismatch is a transposition (with the following character- no need to check PREV because
			// already captured in previous iteration)
			if (i < mwl and (misspelledWord[i] == candidateWord[i+1]) and (misspelledWord[i+1] == candidateWord[i])) {
				float	thisLetterCost	=	h - prevH;
				h += (- thisLetterCost) * 1.2f;	// unsure what right cost - must take care of THIS char plus next neg...
			}

		}

		// speed tweek - cut-off heuristic calc if we're already below the cut-off
		if (h < atLeast) {
			return -10000.0f;
		}

		// letters at the beginning are more important than at the end
		if (i == 0) {
			thisCharImportance *= 0.5f;
		}
		else if (i < 3) {
			thisCharImportance *= 0.8f;
		}
		else {
			thisCharImportance *= 0.9f;
		}
	}
	{
		const float	kPenaltyForLettersOffEnd				=	0.5f;
		const float	kPenaltyForLettersOffEndGrowthFactor	=	1.2f;
		float	curPenalty	=	kPenaltyForLettersOffEnd;
		for (size_t i = mwl; i < cwl; ++i) {
			// extra chars off the end have growing significance...
			h -= curPenalty;
			curPenalty *= kPenaltyForLettersOffEndGrowthFactor;
		}
	}
	return h;
}

/*
@METHOD:		SpellCheckEngine_Basic::ScanForWord
@ACCESS:		private
@DESCRIPTION:	<p>Look in the given buffer - starting at 'cursor' - for the next word Set wordStartResult/
			wordEndResult according to what is found and return true if we find a word, and return false otherwise.
			In either case - set 'cursor' on output to reflect how far we scanned ahead. It is indented that
			this function be used iteratively and that you repeatedly pass IN the same cursor that was passed out.</p>
*/
bool	SpellCheckEngine_Basic::ScanForWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
										const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
									)
{
	Led_RequireNotNil (startBuf);
	Led_RequireNotNil (endBuf);
	Led_RequireNotNil (cursor);
	Led_RequireNotNil (wordStartResult);
	Led_RequireNotNil (wordEndResult);
	Led_Require ((*cursor >= startBuf and *cursor <= endBuf));

	if (*cursor >= endBuf) {
		return false;
	}

	size_t	bufLen		=	endBuf-startBuf;

	size_t	initialCrs	=	*cursor-startBuf;
	size_t	p			=	initialCrs;
	size_t	wordStart	=	0;
	size_t	wordEnd		=	0;
	bool	wordReal	=	false;
	// Find a real word...
	while (not wordReal or wordStart < initialCrs) {
		GetTextBreaker ()->FindWordBreaks (startBuf, bufLen, p, &wordStart, &wordEnd, &wordReal);
		if (not wordReal or wordStart < initialCrs) {
			if (p < bufLen) {
				p = Led_NextChar (&startBuf[p]) - startBuf;
				wordStart = p;
				wordEnd = p;
			}
			else {
				wordStart = p;
				wordEnd = p;
				break;
			}
		}
	}

	if (not wordReal) {
		// no real word to be found...
		//maybe the first 'word' was a series of spaces and there may not have been a word after (end of buf).
		*cursor = startBuf + wordEnd;
		return false;
	}

	Led_Assert (wordReal);
	Led_Assert (wordStart < wordEnd);
	p = wordEnd;

	Led_Assert (*cursor <= startBuf + p);
	Led_Assert ((*cursor < startBuf + p) or (*cursor == endBuf));
	*cursor = startBuf + p;

	*wordStartResult = startBuf + wordStart;
	*wordEndResult = startBuf + wordEnd;

	return true;
}

vector<const SpellCheckEngine_Basic::Dictionary*>	SpellCheckEngine_Basic::GetDictionaries () const
{
	return fDictionaries;
}

void	SpellCheckEngine_Basic::SetDictionaries (const vector<const Dictionary*>& dictionaries)
{
	fDictionaries = dictionaries;
}

#if		qDebug
/*
@METHOD:		SpellCheckEngine_Basic::Invariant_
@DESCRIPTION:	<p>Check validity of SpellCheck engine. Called by @'SpellCheckEngine::Invariant'.</p>
*/
void	SpellCheckEngine_Basic::Invariant_ () const
{
	Led_Assert (sizeof (InfoBlock) == sizeof (int));	// Not a REAL requirement - but we want to make sure - for the most part - the
														// compiler lays this out for us to be SMALL.
	for (vector<const Dictionary*>::const_iterator i = fDictionaries.begin (); i != fDictionaries.end (); ++i) {
		const Dictionary*	dict		=	*i;
		Led_AssertNotNil (dict);
		const InfoBlock*	ibsStart	=	NULL;
		const InfoBlock*	ibsEnd		=	NULL;
		dict->GetInfoBlocks (&ibsStart, &ibsEnd);
		const Led_tChar*	dictBufStart=	dict->GetTextBase ();
		const Led_tChar*	dictBufEnd	=	dict->GetTextEnd ();
		if (dictBufStart != dictBufEnd) {
			Led_AssertNotNil (dictBufStart);
			Led_AssertNotNil (dictBufEnd);
			Led_Assert (dictBufStart <= dictBufEnd);	// empty is a boring case - but I suppose not illegal
			size_t	bufSize	=	dictBufEnd - dictBufStart;
			Led_tString	prevWord;
			for (const InfoBlock* i = ibsStart; i != ibsEnd; ++i) {
				Led_AssertNotNil (dictBufStart);
				Led_Assert ((*i).fIndex < bufSize);
				Led_Assert ((*i).fIndex + (*i).fWordLen <= bufSize);
				Led_Assert ((*i).fWordLen > 0);
				Led_tString	w	=	Led_tString (dictBufStart + (*i).fIndex, dictBufStart + (*i).fIndex + (*i).fWordLen);
				Led_Assert (not w.empty ());
				if (i != ibsStart) {
					// Assure words in alphabetical order
					Led_Assert (Led_tStrCmp (prevWord.c_str (), w.c_str ()) < 0);
				}
				prevWord = w;
				int	breakHere	=	1;// break here to look at each word
			}
		}
	}
}
#endif

#if		qDebug
void	SpellCheckEngine_Basic::RegressionTest ()
{
	try {
		RegressionTest_1 ();
	}
	catch (...) {
		Led_Assert (false);	// catch and just report here as assert errors and exceptions...
							// (cuz currently on MacOS - for UNICODE builds - we generate
							// exceptions cuz not enough UNICODE support there yet -- LGP 2003-05-30)
	}
}

void	SpellCheckEngine_Basic::RegressionTest_1 ()
{
	const Led_tChar* xxx = LED_TCHAR_OF ("IBM\na\napple\ndog\nfrog\ngood\nthis\nzipper\n");
	SpellCheckEngine_Basic::EditableDictionary	testerDict;
	testerDict.ReadFromBuffer (xxx, xxx + Led_tStrlen (xxx));
	SpellCheckEngine_Basic tester (&testerDict);
	const Led_tChar* testText	=	LED_TCHAR_OF ("This is a very good test. ");

	tester.Invariant ();

	{
		bool	r1		=	tester.LookupWord (LED_TCHAR_OF ("Frog"));
		Led_Assert (r1);
	}

	{
		Led_tString	r2S;
		bool	r2n		=	tester.LookupWord (LED_TCHAR_OF ("ziPPer"), &r2S);
		bool	r2y		=	tester.LookupWord (LED_TCHAR_OF ("Zipper"), &r2S);
		Led_Assert (not r2n and r2y and r2S == LED_TCHAR_OF ("zipper"));
	}

	{
		Led_tString	r3S;
		Led_Assert (not tester.LookupWord (LED_TCHAR_OF ("ibm")))
		Led_Assert (not tester.LookupWord (LED_TCHAR_OF ("Ibm")))
		bool	r3		=	tester.LookupWord (LED_TCHAR_OF ("IBM"), &r3S);
		Led_Assert (r3 and r3S == LED_TCHAR_OF ("IBM"));
	}

	{
		const Led_tChar* wordStart = NULL;
		const Led_tChar* wordEnd = NULL;
		const Led_tChar*	p	=	NULL;
		bool	result = tester.ScanForUndefinedWord (testText, testText + Led_tStrlen (testText), &p, &wordStart, &wordEnd);
		Led_Assert (result and Led_tString (wordStart, wordEnd) == LED_TCHAR_OF ("is"))
	}

	{
		const Led_tChar*	cursor			=	NULL;
		const Led_tChar*	wordStartResult	=	NULL;
		const Led_tChar*	wordEndResult	=	NULL;
		int	nWordsFound	=	0;
		while (tester.ScanForUndefinedWord (testText, testText + Led_tStrlen (testText), &cursor,
										&wordStartResult, &wordEndResult
										)
				) {
			// we found a possible undefined word.
			Led_tString	word	=	Led_tString (wordStartResult, wordEndResult);
			nWordsFound++;
			if (nWordsFound == 1) {
				Led_Assert (Led_tString (wordStartResult, wordEndResult) == LED_TCHAR_OF ("is"))
			}
			if (nWordsFound == 2) {
				Led_Assert (Led_tString (wordStartResult, wordEndResult) == LED_TCHAR_OF ("very"))
			}
			if (nWordsFound == 3) {
				Led_Assert (Led_tString (wordStartResult, wordEndResult) == LED_TCHAR_OF ("test"))
			}
		}
		Led_Assert (nWordsFound == 3);
	}
}
#endif







/*
 ********************************************************************************
 ****************** SpellCheckEngine_Basic::EditableDictionary ******************
 ********************************************************************************
 */
SpellCheckEngine_Basic::EditableDictionary::EditableDictionary ():
	inherited (),
	fSortedWordList (),
	fDictBufStart (NULL),
	fDictBufEnd (NULL),
	fInfoBlocks ()
{
}

SpellCheckEngine_Basic::EditableDictionary::~EditableDictionary ()
{
	delete[] fDictBufStart;
}

void	SpellCheckEngine_Basic::EditableDictionary::AddWordToUserDictionary (const Led_tString& word)
{
	fSortedWordList.insert (word);
	ConstructInfoBlocksEtcFromWordList ();
}

const Led_tChar*	SpellCheckEngine_Basic::EditableDictionary::GetTextBase () const
{
	return fDictBufStart;
}

const Led_tChar*	SpellCheckEngine_Basic::EditableDictionary::GetTextEnd () const
{
	return fDictBufEnd;
}

void		SpellCheckEngine_Basic::EditableDictionary::GetInfoBlocks (const InfoBlock** start, const InfoBlock**end) const
{
	Led_RequireNotNil (start);
	Led_RequireNotNil (end);
	*start = &*fInfoBlocks.begin ();
	*end = &*fInfoBlocks.end ();
}

void	SpellCheckEngine_Basic::EditableDictionary::ReadFromBuffer (const Led_tChar* readOnlyRAMDictStart, const Led_tChar* readOnlyRAMDictEnd)
{
	fSortedWordList.clear ();

	// ASSUMES VALS COME IN CRLF or LF separated. This code does not assume that the
	// input is already in dictionary order (though we write it that way).
	for (const Led_tChar* p = readOnlyRAMDictStart; p < readOnlyRAMDictEnd; ) {
		// scan for \r or \n or \r\n to mark end of word
		const Led_tChar*	i1	=	Led_tStrChr (p, '\r');
		const Led_tChar*	i2	=	Led_tStrChr (p, '\n');
		if (i1 == NULL) {
			i1 = p + Led_tStrlen (p);
		}
		if (i2 == NULL) {
			i2 = p + Led_tStrlen (p);
		}
		const Led_tChar*	wordStart	=	p;
		const Led_tChar*	wordEnd		=	min (i1, i2);
		if (wordStart != wordEnd) {
			fSortedWordList.insert (Led_tString (wordStart, wordEnd));
		}

		p = wordEnd;
		if (p < readOnlyRAMDictEnd) {
			p++;	// skip CR or LF
		}
		// If CRLF then set p to point after CRLF (not just CR or LF)
		if (p + 1 == i2) {
			p = p + 1;
		}
	}
	ConstructInfoBlocksEtcFromWordList ();
}

vector<Led_tChar>	SpellCheckEngine_Basic::EditableDictionary::SaveToBuffer () const
{
	Led_SmallStackBuffer<Led_tChar>	buf (1);

	#if		qWindows
		const Led_tChar	kLineTerm[]	=	LED_TCHAR_OF ("\r\n");
	#elif	qMacOS
		const Led_tChar	kLineTerm[]	=	LED_TCHAR_OF ("\r");
	#else
		const Led_tChar	kLineTerm[]	=	LED_TCHAR_OF ("\n");
	#endif
	const size_t	kLineTerm_Length=	Led_NEltsOf (kLineTerm) - 1;
	size_t	totalBufSizeSoFar	=	0;
	for (set<Led_tString>::const_iterator i = fSortedWordList.begin (); i != fSortedWordList.end (); ++i) {
		{
			size_t	prevCopyTo	=	totalBufSizeSoFar;
			totalBufSizeSoFar += (*i).length ();
			buf.GrowToSize (totalBufSizeSoFar);
			copy ((*i).begin (), (*i).end (), static_cast<Led_tChar*> (buf) + prevCopyTo);
		}
		{
			size_t	prevCopyTo	=	totalBufSizeSoFar;
			totalBufSizeSoFar += kLineTerm_Length;
			buf.GrowToSize (totalBufSizeSoFar);
			copy (kLineTerm, kLineTerm + kLineTerm_Length, static_cast<Led_tChar*> (buf) + prevCopyTo);
		}
	}
	return vector<Led_tChar> (static_cast<Led_tChar*> (buf), static_cast<Led_tChar*> (buf) + totalBufSizeSoFar);
}

void	SpellCheckEngine_Basic::EditableDictionary::ConstructInfoBlocksEtcFromWordList ()
{
	// Clear old buffer values
	delete [] fDictBufStart;
	fDictBufStart = NULL;
	fDictBufEnd = NULL;
	fInfoBlocks.clear ();

	size_t	totalBlockSize	=	0;
	{
		for (set<Led_tString>::const_iterator i = fSortedWordList.begin (); i != fSortedWordList.end (); ++i) {
			totalBlockSize += (*i).length ();
		}
	}

	fDictBufStart = new Led_tChar [totalBlockSize];
	fDictBufEnd = fDictBufStart + totalBlockSize;

	/*
	 *	Now walk the list (already in the right order for the InfoBlock list), and fill in the fDictBuffer area,
	 *	and the InfoBlock list at the same time.
	 */
	Led_tChar*	intoBufPtr	=	fDictBufStart;
	for (set<Led_tString>::const_iterator i = fSortedWordList.begin (); i != fSortedWordList.end (); ++i) {
		copy ((*i).begin (), (*i).end (), intoBufPtr);

		InfoBlock	iB;
		(void)::memset (&iB, 0, sizeof (iB));
		iB.fIndex = intoBufPtr - fDictBufStart;
		iB.fWordLen = (*i).length ();
		iB.fXXX = 0;//tmphack
		Led_Assert (iB.fWordLen > 0);
		fInfoBlocks.push_back (iB);

		intoBufPtr += (*i).length ();
	}

}







/*
 ********************************************************************************
 ***************** SpellCheckEngine_Basic::CompiledDictionary *******************
 ********************************************************************************
 */
SpellCheckEngine_Basic::CompiledDictionary::CompiledDictionary (const CompiledDictionaryData& data):
	inherited (),
	fData (data)
{
}

const Led_tChar*	SpellCheckEngine_Basic::CompiledDictionary::GetTextBase () const
{
	return fData.fTextDataStart;
}

const Led_tChar*	SpellCheckEngine_Basic::CompiledDictionary::GetTextEnd () const
{
	return fData.fTextDataEnd;
}

void	SpellCheckEngine_Basic::CompiledDictionary::GetInfoBlocks (const InfoBlock** start, const InfoBlock**end) const
{
	Led_RequireNotNil (start);
	Led_RequireNotNil (end);
	*start = fData.fInfoBlocksStart;
	*end = fData.fInfoBlocksEnd;
}








/*
 ********************************************************************************
 ***************************** TextBreaks_SpellChecker **************************
 ********************************************************************************
 */

TextBreaks_SpellChecker::TextBreaks_SpellChecker ():
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

TextBreaks_SpellChecker::CharacterClasses	TextBreaks_SpellChecker::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const
{
	switch (*charToExamine) {
		case	'$':
		case	'%':
		case	'-': {
			return (eWordClass);
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
	return inherited::CharToCharacterClass (startOfText, lengthOfText, charToExamine);
}

#if		qDebug
void	TextBreaks_SpellChecker::RegressionTest ()
{
	{
		const	Led_tChar*	kTest	=	LED_TCHAR_OF ("This is a good test of Simone's bug with the 'word'.");
		size_t				wordEndResult	=	0;
		bool				wordReal		=	0;

		FindLineBreaks (kTest, Led_tStrlen (kTest), 25, &wordEndResult, &wordReal);
		Led_Assert (wordEndResult == 31);
		Led_Assert (wordReal == true);
	}
}
#endif









/*
 ********************************************************************************
 *********************** SpellCheckEngine_Basic_Simple **************************
 ********************************************************************************
 */
SpellCheckEngine_Basic_Simple::SpellCheckEngine_Basic_Simple ():
	inherited  (),
	fMainDictionary (NULL),
	fUDName (),
	fUD (NULL)
{
	#if		qIncludeBakedInDictionaries
		SetMainDictionary (&kDictionary_US_English);
	#endif
}

SpellCheckEngine_Basic_Simple::~SpellCheckEngine_Basic_Simple ()
{
	delete fUD;
}

SpellCheckEngine_Basic_Simple::UDInterface*	SpellCheckEngine_Basic_Simple::GetUDInterface ()
{
	return this;
}

bool	SpellCheckEngine_Basic_Simple::AddWordToUserDictionarySupported () const
{
	return fUD != NULL;
}

void	SpellCheckEngine_Basic_Simple::AddWordToUserDictionary (const Led_tString& word)
{
	Led_ThrowIfNull (fUD);
	fUD->AddWordToUserDictionary (word);
	WriteToUD ();
}

const SpellCheckEngine_Basic_Simple::Dictionary*	SpellCheckEngine_Basic_Simple::GetMainDictionary () const
{
	return fMainDictionary;
}

void	SpellCheckEngine_Basic_Simple::SetMainDictionary (const Dictionary* mainDictionary)
{
	fMainDictionary = mainDictionary;
	vector<const Dictionary*>	dicts;
	if (fMainDictionary != NULL) {
		dicts.push_back (fMainDictionary);
	}
	if (fUD != NULL) {
		dicts.push_back (fUD);
	}
	SetDictionaries (dicts);
}

SpellCheckEngine_Basic_Simple::UDDictionaryName	SpellCheckEngine_Basic_Simple::GetUserDictionary () const
{
	return fUDName;
}

void	SpellCheckEngine_Basic_Simple::SetUserDictionary (const UDDictionaryName& userDictionary)
{
	fUDName = userDictionary;
	#if		qMacOS
		static	FSSpec	zero;
		memset (&zero, 0, sizeof (zero));
		bool	noUD		=	(::memcmp (&userDictionary, &zero, sizeof (zero)) == 0);
	#else
		bool	noUD		=	userDictionary.length () == 0;
	#endif
	delete fUD;
	fUD = NULL;

	if (not noUD) {
		fUD = new EditableDictionary ();
		ReadFromUD ();
	}
	SetMainDictionary (fMainDictionary);		// hack to force call to SetDictionaries ()
}

void	SpellCheckEngine_Basic_Simple::ReadFromUD ()
{
	/*
	 *	Ignore any errors reading from the UD (at least file-not-found errors).
	 */
	try {
		#if		qMacOS
			FileReader	reader (&fUDName);
		#else
			FileReader	reader (fUDName.c_str ());
		#endif
		#if		qWideCharacters
			size_t				fileLen		=	reader.GetFileEnd () - reader.GetFileStart ();
			CodePage			useCodePage	=	CodePagesGuesser ().Guess (reader.GetFileStart (), fileLen);
			CodePageConverter	cpc			=	CodePageConverter (useCodePage, CodePageConverter::eHandleBOM);
			size_t				outCharCnt	=	cpc.MapToUNICODE_QuickComputeOutBufSize (reinterpret_cast<const char*> (reader.GetFileStart ()), fileLen);
			Led_SmallStackBuffer<Led_tChar>	fileData2 (outCharCnt);
			cpc.MapToUNICODE (reinterpret_cast<const char*> (reader.GetFileStart ()), fileLen, static_cast<wchar_t*> (fileData2), &outCharCnt);
			fUD->ReadFromBuffer (static_cast<Led_tChar*> (fileData2), static_cast<Led_tChar*> (fileData2) + outCharCnt);
		#else
			fUD->ReadFromBuffer (reinterpret_cast<const Led_tChar*> (reader.GetFileStart ()), reinterpret_cast<const Led_tChar*> (reader.GetFileEnd ()));
		#endif
	}
	catch (...) {
	}
}

void	SpellCheckEngine_Basic_Simple::WriteToUD ()
{
	Led_ThrowIfNull (fUD);
	vector<Led_tChar>	data	=	fUD->SaveToBuffer ();

	#if		qMacOS
		FileWriter	writer (&fUDName);
	#else
		FileWriter	writer (fUDName.c_str ());
	#endif

	#if		qWideCharacters
		CodePageConverter			cpc			=	CodePageConverter (kCodePage_UTF8, CodePageConverter::eHandleBOM);
		size_t						outCharCnt	=	cpc.MapFromUNICODE_QuickComputeOutBufSize (&*data.begin (), data.size ());
		Led_SmallStackBuffer<char>	fileData2 (outCharCnt);
		cpc.MapFromUNICODE (&*data.begin (), data.size (), fileData2, &outCharCnt);
		writer.Append (reinterpret_cast<const Byte*> (static_cast<char*> (fileData2)), outCharCnt);
	#else
		writer.Append (reinterpret_cast<const Byte*> (&*data.begin ()), data.size ());
	#endif
}

#if		qLedUsesNamespaces
}
#endif






// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


