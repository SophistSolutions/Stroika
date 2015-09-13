/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SpellCheckEngine_Basic_h__
#define	__SpellCheckEngine_Basic_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SpellCheckEngine_Basic.h,v 2.17 2004/02/12 04:12:24 lewis Exp $
 */

/*
@MODULE:	SpellCheckEngine_Basic
@DESCRIPTION:	<p></p>

 */


/*
 * Changes:
 *	$Log: SpellCheckEngine_Basic.h,v $
 *	Revision 2.17  2004/02/12 04:12:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.16  2004/02/11 01:08:15  lewis
 *	SPR#1635: qFailToCompileLargeDataInitializedArraysBug bug workaround support (disable
 *	builtin dicts for MSVC60)
 *	
 *	Revision 2.15  2003/12/17 20:57:46  lewis
 *	changed SpellCheckEngine::GetTextBreaksUsed() (returning Led_RefCntPtr<>) to PeekAtTextBreaksUsed ()
 *	returning TextBreaks*. Reason is we don't want to have to return a long-lived object. Interferes with
 *	COM stuff (be returning object part of COM object - and that could go away. Could add ref from that -
 *	but... not worth it here)
 *	
 *	Revision 2.14  2003/12/12 01:41:52  lewis
 *	qMixinDisambiguatingNameInBothBug BWA for MWERS/MACOS
 *	
 *	Revision 2.13  2003/12/11 19:18:22  lewis
 *	SPR#1593: add SpellCheckEngine::GetTextBreaksUsed () method so I can utilize the right TextBreaks
 *	routine from the spellcheck loop. Then use that in MyCallback::GetNextMisspelling () so that we
 *	backup to the start of each word when beginning a chunk spellchecking, and more importantly - fix
 *	the code to actaully check successive chunks of no misspellings found in the first one
 *	
 *	Revision 2.12  2003/12/11 02:57:53  lewis
 *	SPR#1590: respoect AddToDictioanryEnabled callback value to disable the addtodict dialog button.
 *	Also - added similar OptionsDIalogEnabled callback and respected that so button disabled (til we've
 *	had time to implement an options dialog)
 *	
 *	Revision 2.11  2003/12/10 22:45:39  lewis
 *	SPR#1558: SpellCheckEngine_Basic::LookupWord_() updated to handle hyphenized words - and various
 *	related docs updated
 *	
 *	Revision 2.10  2003/12/09 20:49:18  lewis
 *	SPR#1585: added UD support (add word to UD, new SpellCheckEngine_Basic_Simple, major revision to
 *	SpellCheckEngine_Basic::EditableDictionary, etc)
 *	
 *	Revision 2.9  2003/06/30 04:35:46  lewis
 *	#include <memory> because of auto_ptr reference
 *	
 *	Revision 2.8  2003/06/27 13:30:04  lewis
 *	change size of fIndex to 22 bits so existing large dictionary fits. Maybe can make smaller again once
 *	I rewrite code to write strings into a smartly shared table
 *	
 *	Revision 2.7  2003/06/25 16:44:46  lewis
 *	SPR#1543: Added OtherStringToIgnore_Number () so we can ignore numbers as undefined words. SPR#1540: did
 *	binary search word lookup (LookupWordHelper_()). Fixed up regression tests to reflect cleaner lookup
 *	semantics
 *	
 *	Revision 2.6  2003/06/23 22:09:28  lewis
 *	SPR#1538: dictionary source compiler work
 *	
 *	Revision 2.5  2003/06/20 16:13:19  lewis
 *	SPR#1538: separated out dictionary data into separate object and supported compiled dictionary
 *	object from MungeDictionaries utility and #include that directly. All basically works - though
 *	needs lots of tuning!
 *	
 *	Revision 2.4  2003/06/02 23:15:53  lewis
 *	a few other small fixes to scanforundefinedwords, and improved cut-off support for suggestions
 *	list. A few more words to dict.
 *	
 *	Revision 2.3  2003/06/02 16:13:51  lewis
 *	SPR#1513: first draft of (windows only so far) simple spellchecking dialog. Basically working (75%).
 *	A few fixes to generic dialog support code
 *	
 *	Revision 2.2  2003/05/29 16:31:18  lewis
 *	SPR#1512: get better SpellCheckEngine_Basic regression test working and cleanup docs etc.
 *	
 *	Revision 2.1  2002/09/09 03:08:05  lewis
 *	SPR#1093- first draft of SpellCheckEngine/SpellCheckEngine_Basic
 *	
 *
 *
 */

#include	<memory>
#include	<set>

#include	"LedSupport.h"
#include	"SpellCheckEngine.h"
#include	"TextBreaks.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif


#if		qFailToCompileLargeDataInitializedArraysBug
	#define	qIncludeBakedInDictionaries	0
#endif



/*
@CONFIGVAR:		qIncludeBakedInDictionaries
@DESCRIPTION:	<p>Turning this on (its on by default) includes into the binary - pre-built dictionaries (currently just US-English).
			Including this allows direct access to the US-English dictionary. However - its large - adding about 2-3MB (depending
			on if you use UNICODE or single-byte - among other things) to the size of your binary.</p>
 */
#ifndef	qIncludeBakedInDictionaries
#define	qIncludeBakedInDictionaries		1
#endif




/*
@CLASS:			SpellCheckEngine_Basic
@DESCRIPTION:	<p>
			</p>
*/
class	SpellCheckEngine_Basic : public SpellCheckEngine {
	private:
		typedef	SpellCheckEngine	inherited;

	public:
		class	Dictionary;
		class	EditableDictionary;
		class	CompiledDictionary;

	public:
		SpellCheckEngine_Basic (const Dictionary* mainDictionary = NULL);
		~SpellCheckEngine_Basic ();

	public:
		override	bool	ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
												const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
												);

	protected:
		override	bool	LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult);

	private:
		nonvirtual	bool	LookupWordHelper_ (const Led_tString& checkWord, Led_tString* matchedWordResult) const;

	protected:
		virtual		bool	OtherStringToIgnore (const Led_tString& checkWord);
		nonvirtual	bool	OtherStringToIgnore_AllPunctuation (const Led_tString& checkWord);
		nonvirtual	bool	OtherStringToIgnore_Sentinals (const Led_tString& checkWord);
		nonvirtual	bool	OtherStringToIgnore_Number (const Led_tString& checkWord);

	public:
		override	vector<Led_tString>	GenerateSuggestions (const Led_tString& misspelledWord);

	public:
		override	UDInterface*	GetUDInterface ();

	public:
		override	TextBreaks*		PeekAtTextBreaksUsed ();


	private:
		nonvirtual	float	Heuristic (const Led_tString& misspelledWord, const Led_tString& candidateWord, float atLeast);

	private:
		nonvirtual	bool	ScanForWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
												const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
												);

	public:
		nonvirtual	Led_RefCntPtr<TextBreaks>	GetTextBreaker () const;
		nonvirtual	void						SetTextBreaker (const Led_RefCntPtr<TextBreaks>& textBreaker);
	private:
		mutable	Led_RefCntPtr<TextBreaks>	fTextBreaker;

	public:
		struct	InfoBlock {
			unsigned	int	fIndex:22;
			unsigned	int	fWordLen:8;
			unsigned	int	fXXX:2;	// some flags - I forget the design - but I'll need this...
		};


#if		qIncludeBakedInDictionaries
	public:
		static	const	CompiledDictionary		kDictionary_US_English;
#endif

	public:
		nonvirtual	vector<const Dictionary*>	GetDictionaries () const;
		nonvirtual	void						SetDictionaries (const vector<const Dictionary*>& dictionaries);
	private:
		vector<const Dictionary*>	fDictionaries;


#if		qDebug
	protected:
		override	void	Invariant_ () const;
#endif

#if		qDebug
	public:
		/*
		@METHOD:		SpellCheckEngine_Basic::RegressionTest
		@DESCRIPTION:	<p>This function only exists if @'qDebug' is on. When run, it performs a basic regression test.</p>
		*/
		static	void	RegressionTest ();
	private:
		static	void	RegressionTest_1 ();
#endif
};


class	SpellCheckEngine_Basic::Dictionary {
	public:
		typedef	SpellCheckEngine_Basic::InfoBlock	InfoBlock;

	protected:
		Dictionary ();
	public:
		virtual ~Dictionary ();

	public:
		virtual	const Led_tChar*	GetTextBase () const	=	0;
		virtual	const Led_tChar*	GetTextEnd () const	=	0;
		virtual	void				GetInfoBlocks (const InfoBlock** start, const InfoBlock**end) const	=	0;
};

class	SpellCheckEngine_Basic::EditableDictionary : public SpellCheckEngine_Basic::Dictionary {
	private:
		typedef	SpellCheckEngine_Basic::Dictionary	inherited;

	public:
		EditableDictionary ();
	public:
		virtual ~EditableDictionary ();

	public:
			nonvirtual	void	AddWordToUserDictionary (const Led_tString& word);

	public:
		override	const Led_tChar*	GetTextBase () const;
		override	const Led_tChar*	GetTextEnd () const;
		override	void				GetInfoBlocks (const InfoBlock** start, const InfoBlock**end) const;

	public:
		nonvirtual	void				ReadFromBuffer (const Led_tChar* readOnlyRAMDictStart, const Led_tChar* readOnlyRAMDictEnd);
		nonvirtual	vector<Led_tChar>	SaveToBuffer () const;

	private:
		nonvirtual	void	ConstructInfoBlocksEtcFromWordList ();

	private:
		set<Led_tString>	fSortedWordList;
		Led_tChar*			fDictBufStart;
		Led_tChar*			fDictBufEnd;
		vector<InfoBlock>	fInfoBlocks;
};

class	SpellCheckEngine_Basic::CompiledDictionary : public SpellCheckEngine_Basic::Dictionary {
	private:
		typedef	SpellCheckEngine_Basic::Dictionary	inherited;

	public:
		struct	CompiledDictionaryData {
			const Led_tChar*	fTextDataStart;
			const Led_tChar*	fTextDataEnd;
			const InfoBlock*	fInfoBlocksStart;
			const InfoBlock*	fInfoBlocksEnd;
		};

	public:
		CompiledDictionary (const CompiledDictionaryData& data);

	public:
		override	const Led_tChar*	GetTextBase () const;
		override	const Led_tChar*	GetTextEnd () const;
		override	void				GetInfoBlocks (const InfoBlock** start, const InfoBlock**end) const;

	private:
		CompiledDictionaryData	fData;
};




/*
@CLASS:			TextBreaks_SpellChecker
@BASES:			@'TextBreaks_Basic'
@DESCRIPTION:	<p>Special purpose text-break implementation uses inside spell-checker. Not recomended for use elsewhere, but
			it could be.</p>
*/
class	TextBreaks_SpellChecker : public TextBreaks_Basic {
	private:
		typedef	TextBreaks_Basic	inherited;
	public:
		TextBreaks_SpellChecker ();

	protected:
		override	CharacterClasses	CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const;

#if		qDebug
	private:
		nonvirtual	void	RegressionTest ();
#endif
};




/*
@CLASS:			SpellCheckEngine_Basic_Simple
@DESCRIPTION:	<p>This is a simple wrapper providing most pre-packaged spellcheck functionality a spellchekcing application would typically want.
			You easily specify one pre-built 'system' dictionary, and one optional user-dictionary (you just specify the file name
			and the rest is taken care of automatically).
			</p>
*/
class	SpellCheckEngine_Basic_Simple : public SpellCheckEngine_Basic, private SpellCheckEngine::UDInterface {
	private:
		typedef	SpellCheckEngine_Basic	inherited;

	public:
		SpellCheckEngine_Basic_Simple ();
		~SpellCheckEngine_Basic_Simple ();

	public:
		#if		qMixinDisambiguatingNameInBothBug
			typedef	inherited::UDInterface	UDInterface;
		#endif
		override	UDInterface*	GetUDInterface ();

	// From SpellCheckEngine::UDInterface
	public:
		override	bool	AddWordToUserDictionarySupported () const;
		override	void	AddWordToUserDictionary (const Led_tString& word);

	public:
		nonvirtual	const Dictionary*	GetMainDictionary () const;
		nonvirtual	void				SetMainDictionary (const Dictionary* mainDictionary);
	private:
		const Dictionary*	fMainDictionary;

	public:
		#if		qMacOS
			typedef	FSSpec	UDDictionaryName;
		#else
			typedef	Led_SDK_String	UDDictionaryName;
		#endif
		nonvirtual	UDDictionaryName	GetUserDictionary () const;
		nonvirtual	void				SetUserDictionary (const UDDictionaryName& userDictionary);
	private:
		UDDictionaryName	fUDName;
		EditableDictionary*	fUD;

	private:
		nonvirtual	void	ReadFromUD ();
		nonvirtual	void	WriteToUD ();
};




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

// class SpellCheckEngine_Basic
	inline	bool	SpellCheckEngine_Basic::OtherStringToIgnore_Sentinals (const Led_tString& checkWord)
		{
			return checkWord.length () == 1 and checkWord[0] == '\0';
		}
	/*
	@METHOD:		SpellCheckEngine_Basic::GetTextBreaker
	@DESCRIPTION:	<p>Returns a @'Led_RefCntPtr<T>' wrapper on the @'TextBreaks' subclass associated
				with this SpellCheckEngine. 
				</p>
					<p>If none is associated with the TextStore right now - and default one is built and returned.</p>
					<p>See also See @'SpellCheckEngine_Basic::SetTextBreaker'.</p>
	*/
	inline	Led_RefCntPtr<TextBreaks>	SpellCheckEngine_Basic::GetTextBreaker () const
		{
			if (fTextBreaker.IsNull ()) {
				fTextBreaker = Led_RefCntPtr<TextBreaks> (new TextBreaks_SpellChecker ());
			}
			return fTextBreaker;
		}
	/*
	@METHOD:		SpellCheckEngine_Basic::SetTextBreaker
	@DESCRIPTION:	<p>See @'SpellCheckEngine_Basic::GetTextBreaker'.</p>
	*/
	inline	void	SpellCheckEngine_Basic::SetTextBreaker (const Led_RefCntPtr<TextBreaks>& textBreaker)
		{
			fTextBreaker = textBreaker;
		}



// class SpellCheckEngine_Basic
	inline	SpellCheckEngine_Basic::Dictionary::Dictionary ()
		{
		}
	inline	SpellCheckEngine_Basic::Dictionary::~Dictionary ()
		{
		}




#if		qLedUsesNamespaces
}
#endif



#endif	/*__SpellCheckEngine_Basic_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
