/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SpellCheckEngine_h__
#define	__SpellCheckEngine_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SpellCheckEngine.h,v 2.9 2003/12/17 20:57:46 lewis Exp $
 */

/*
@MODULE:	SpellCheckEngine
@DESCRIPTION:	<p></p>

 */


/*
 * Changes:
 *	$Log: SpellCheckEngine.h,v $
 *	Revision 2.9  2003/12/17 20:57:46  lewis
 *	changed SpellCheckEngine::GetTextBreaksUsed() (returning Led_RefCntPtr<>) to PeekAtTextBreaksUsed () returning TextBreaks*. Reason is we don't want to have to return a long-lived object. Interferes with COM stuff (be returning object part of COM object - and that could go away. Could add ref from that - but... not worth it here)
 *	
 *	Revision 2.8  2003/12/17 01:16:38  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.7  2003/12/11 19:18:22  lewis
 *	SPR#1593: add SpellCheckEngine::GetTextBreaksUsed () method so I can utilize the right TextBreaks
 *	routine from the spellcheck loop. Then use that in MyCallback::GetNextMisspelling () so that we
 *	backup to the start of each word when beginning a chunk spellchecking, and more importantly -
 *	fix the code to actaully check successive chunks of no misspellings found in the first one
 *	
 *	Revision 2.6  2003/12/10 22:45:30  lewis
 *	SPR#1558: SpellCheckEngine_Basic::LookupWord_() updated to handle hyphenized words - and various
 *	related docs updated
 *	
 *	Revision 2.5  2003/12/09 20:49:18  lewis
 *	SPR#1585: added UD support (add word to UD, new SpellCheckEngine_Basic_Simple, major revision
 *	to SpellCheckEngine_Basic::EditableDictionary, etc)
 *	
 *	Revision 2.4  2003/06/25 16:36:58  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2003/06/12 15:17:47  lewis
 *	doccomment
 *	
 *	Revision 2.2  2003/05/29 16:31:17  lewis
 *	SPR#1512: get better SpellCheckEngine_Basic regression test working and cleanup docs etc.
 *	
 *	Revision 2.1  2002/09/09 03:08:03  lewis
 *	SPR#1093- first draft of SpellCheckEngine/SpellCheckEngine_Basic
 *	
 *
 *
 */

#include	"LedSupport.h"
#include	"TextBreaks.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif





/*
@CLASS:			SpellCheckEngine
@DESCRIPTION:	<p>Abstract spellchecker low-level API. This just defines the basic functionality used for looking for misspelled
				words, and for finding guesses.
			</p>
*/
class	SpellCheckEngine {
	protected:
		SpellCheckEngine ();
	public:
		virtual ~SpellCheckEngine ();

	public:
		/*
		@METHOD:		SpellCheckEngine::ScanForUndefinedWord
		@DESCRIPTION:	<p>Look in the given buffer input buffer, starting at 'cursor' for the next undefined
					word Set wordStartResult/wordEndResult according to what is found. Note that if
					'*cursor' is NULL, then it is treated as being 'startBuf' instead. Return true if something is
					found (setting only in this case wordStartResult/wordEndResult) and return false if no undefined
					words are found. In either case - set 'cursor' on output to reflect how far we scanned ahead.
					It is intended that this function be used iteratively and that you repeatedly pass IN
					the same cursor that was passed out from the last call. On the first call - set the cursor value
					to NULL. Please don't confuse the cursor value with the POINTER TO the cursor value which is
					what is passed in.
					</p>
						<p>
		<code>
			const int nChars	=	100;
			Led_tChar	textToSearch [nChars];
			// fill in textToSearch from someplace
			const Led_tChar*	cursor			=	NULL;
			const Led_tChar*	wordStartResult	=	NULL;
			const Led_tChar*	wordEndResult	=	NULL;
			while (ScanForUndefinedWord (textToSearch, textToSearch + nChars, &cursor,
										 &wordStartResult, &wordEndResult
										 )
					) {
				// we found a possible undefined word.
				Led_tString	word	=	Led_tString (wordStartResult, wordEndResult);
			}
			// no more undefined words.
		</code>
					</p>
						<p>Note that in the case where you haven't examined an entire buffer, but just bits at a time
					(chunking) - you may find apparant undefined words at the edges of the buffer. It is up to you to
					overlap your calls to check for undefined words in such a way that you verify any found undefined
					words truely are undefined, by examining a larger surrounding region.
					</p>
		*/
		virtual	bool	ScanForUndefinedWord (const Led_tChar* startBuf, const Led_tChar* endBuf, const Led_tChar** cursor,
												const Led_tChar** wordStartResult, const Led_tChar** wordEndResult
												) = 0;

	public:
		nonvirtual	bool	LookupWord (const Led_tString& checkWord, Led_tString* matchedWordResult = NULL);
	protected:
		/*
		@METHOD:		SpellCheckEngine::LookupWord_
		@ACCESS:		protected
		@DESCRIPTION:	<p>pure virtual method of @'SpellCheckEngine' - called internally by @'SpellCheckEngine::LookupWord'.
					This method is overriden in subclasses to to the actual word lookup - returning true (and setting matchedWordResult) if the
					given word is found (considered legitimate).</p>
						<p>This function may do much more than just lookikng the word up in a dictionary. It may perform
					all sorts of lingustic manipulations (stripping trailing 's' from nouns etc) to look for a match.</p>
		*/
		virtual		bool	LookupWord_ (const Led_tString& checkWord, Led_tString* matchedWordResult) = 0;

	public:
		/*
		@METHOD:		SpellCheckEngine::GenerateSuggestions
		@DESCRIPTION:	<p>This generates a list of words which are close to the given misspelled word. (ORDER
				ALPHA OR MOST LIKELY FIRST????)</p>
		*/
		virtual	vector<Led_tString>	GenerateSuggestions (const Led_tString& misspelledWord) = 0;


	public:
		/*
		@METHOD:		SpellCheckEngine::PeekAtTextBreaksUsed
		@DESCRIPTION:	<p>This method can return NULL. If it returns non-NULL, you can use this @'TextBreaks' object
					to break the source text at word boundaries in much the same manner as the spellcheck engine does.</p>
						<p>This can be useful if you provide a UI which breaks the input into chunks - but wants
					to make sure the chunks correspond at the edges to word boundaries.</p>
						<p>Note - the lifetime of the PeekAt call is short. It is at least til the next
					call to the spellcheck engine (should I gaurantee longer?).
		*/
		virtual	TextBreaks*		PeekAtTextBreaksUsed ()	=	0;

	public:
		class	UDInterface;
		/*
		@METHOD:		SpellCheckEngine::GetUDInterface
		@DESCRIPTION:	<p>This method can return NULL if there is no UDInterface supported.</p>
		*/
		virtual	UDInterface*	GetUDInterface ()	=	0;

	public:
		/*
		@METHOD:		SpellCheckEngine::Invariant
		@DESCRIPTION:	<p>if @'qDebug' is on, this called the virtual @'SpellCheckEngine::Invariant' () method
			(in subclasses) to check the state. It can be called freely of @'qDebug' is off - it will have no effect. If
			it is on, however, the time used by this funcion could be significant.</p>
		*/
		nonvirtual	void	Invariant () const;

#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
#endif
};




/*
@CLASS:			SpellCheckEngine::UDInterface
@DESCRIPTION:	<p>
			</p>
*/
class	SpellCheckEngine::UDInterface {
	protected:
		UDInterface ();
	public:
		virtual ~UDInterface ();

	public:
		/*
		@METHOD:		SpellCheckEngine::UDInterface::AddWordToUserDictionarySupported
		@DESCRIPTION:	<p>This method allows for the UDInterface interface to be supported by a spellcheck engine, but still to dynamically
					turn on/off UD support (say if the UD is loaded or not).</p>
		*/
		virtual	bool	AddWordToUserDictionarySupported () const			=	0;

	public:
		/*
		@METHOD:		SpellCheckEngine::UDInterface::AddWordToUserDictionary
		@DESCRIPTION:	<p>Add the given word the current open (or primary) user dictionary. This is typically called from
					the 'add word to dictionary' button in the spellcheck dialog.</p>
		*/
		virtual	void	AddWordToUserDictionary (const Led_tString& word)	=	0;
};






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

// class SpellCheckEngine
	inline	SpellCheckEngine::SpellCheckEngine ()
		{
		}
	/*
	@METHOD:		SpellCheckEngine::LookupWord
	@DESCRIPTION:	<p>Lookup the given word and see if its found in the system (dictionaries). Return true
				if found and set (optional - can be NULL) OUT argument 'matchedWordResult'
				to indicate the original word in the dictionary matched
				(due to case mapping, this may not be the same as the 'checkWord').</p>
					<p>Also note that just because a word doesn't appear in the dictionary, (i.e. isn't returned
				from LookupWord) - doesn't mean it will be found as a missing (misspelled) word. Some words
				are ignored like possibly all upper case words, punctuation, numbers, and others algorithmicly checked
				words.</p>
	*/
	inline	bool	SpellCheckEngine::LookupWord (const Led_tString& checkWord, Led_tString* matchedWordResult)
		{
			return LookupWord_ (checkWord, matchedWordResult);
		}
	inline	void	SpellCheckEngine::Invariant () const
		{
			#if		qDebug
				Invariant_ ();
			#endif
		}


// class SpellCheckEngine::UDInterface
	inline	SpellCheckEngine::UDInterface::UDInterface ()
		{
		}
	inline	SpellCheckEngine::UDInterface::~UDInterface ()
		{
		}



#if		qLedUsesNamespaces
}
#endif



#endif	/*__SpellCheckEngine_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
