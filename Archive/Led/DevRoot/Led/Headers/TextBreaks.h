/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__TextBreaks_h__
#define	__TextBreaks_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/TextBreaks.h,v 2.13 2003/11/26 17:56:32 lewis Exp $
 */

/*
@MODULE:	TextBreaks
@DESCRIPTION:
		<p>This code handles finding word boundaries (for double click processing), and finding
	points in a range of text where it is safe to word-wrap. This code knows nothing about the UI modules,
	and only operates on strings of text (@'Led_tChar's).</p>
		<p>Though this module contains a few implementations, you probably just want to use the
	@'TextBreaks_DefaultImpl' implementation.</p>
 */

/*
 *
 * Changes:
 *	$Log: TextBreaks.h,v $
 *	Revision 2.13  2003/11/26 17:56:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2003/11/25 22:59:25  lewis
 *	SPR#1563: Renamed TextBreaks_OLD to TextBreaks_Basic, and added TextBreaks_Basic_TextEditor
 *	and TextBreaks_Basic_WP. These different subclasses have slightly different word-break rules,
 *	appropraite for their use. Default to WP case (mostly for backward compat). Further cleanup
 *	word-break code and add more special cases appropriate to both WP and texteditor subclasses
 *	
 *	Revision 2.11  2003/11/25 21:56:42  lewis
 *	SPR#1563: first make CharToCharacterClass stuff  into methods of TextBreaks_OLD, and merge the two methods
 *	(related) into one common virtual method (so can be more easily overridden). Then fixed the existing (default)
 *	code so it does a better job handling apostrophes (single quotes) by default. Added primitive regression
 *	test mechanism for this code.
 *	
 *	Revision 2.10  2002/05/06 21:33:38  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.9  2001/11/27 00:29:46  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.8  2001/10/20 13:38:57  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.7  2001/10/17 20:42:54  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.6  2001/08/29 23:01:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2001/08/28 18:43:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2000/11/10 16:07:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2000/06/17 05:08:27  lewis
 *	SPR#0793- Lose qUseSystemWordBreakRoutine define - and replace it with
 *	new TextBreaks_System class.
 *	
 *	Revision 2.2  2000/06/15 22:28:44  lewis
 *	SPR#0778- Made TextStore own a smart pointer to a TextBreak object - and by
 *	default create one with the old algorithms.
 *	
 *	Revision 2.1  2000/06/15 20:02:03  lewis
 *	SPR#0778- Broke out old word/line break code from TextStore - in preparation for
 *	making it virtual/generic - and doing different algorithms (UNICODE)Headers/TextBreaks.h
 *	
 *
 *
 *	<========== CODE MOVED HERE FROM TextStore.h ==========>
 *
 *
 */

#include	"LedSupport.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif














/*
@CLASS:			TextBreaks
@DESCRIPTION:
		<p>DOCS SOMEWHAT OBSOLETE - BASED ON ORIGINAL LED CODE.</p>

		<p>Word/Row Break/Boundary calculation routines.</p>

		<p>Note that because some script systems (notably Japanese) have different notions
	of "words" for selection (e.g. double-click) and for row (aka line) breaking,
	we have two correspondingly different notions supported here.</p>

		<p>Also - we provide both static routines operating on strings, and routines similarly
	named, operating on this textstore buffer. The reason for the string-based routines
	is so that the EXACT same algorithm can be used on text not conveniently (or efficiently)
	already stored in a TextStore buffer. We provide the TextStore based versions for
	convenience - it is our main target of these sorts of operations.</p>

		<p>One place where the behavior of these routines is not obvious from its name (and so we
	must nail down by fiat) is what happens when asked to find the word starting from
	a space character. Do we find the word BEFORE or AFTER the space(s). Or do we condider
	the string of spaces to be a word? In order to avoid the arbitary choice of selecting
	the forward or backward word (likely to subtly impact on GUI choices) we instead here
	return a flag indicating if the word is REAL, or simply a run of spaces. If the caller
	was looking for a real word and got a run of spaces - he can simply reset the position
	we were looking from to the END of the run of spaces, or just before it.</p>
*/
class	TextBreaks {
	protected:
		TextBreaks () {}
	
	public:
		virtual ~TextBreaks () {};

	public:
		/*
		@METHOD:		TextBreaks::FindWordBreaks
		@DESCRIPTION:	<p>For the given text, and offset information, return the start and end of the next word. Here word
			is defined rather losely. In particular, sequences of whitespace are considered a 'word'. In that case, the wordReal
			value is returned false.</p>
				<p>Must think a bit about generalizing this for other languages (though it was adequate for SJIS/Japanese).</p>
				<p>NB: textOffsetToStartLookingForWord is zero-based.</p>
				<p>No word was found - iff *wordStartResult and *wordEndResult are equal.</p>
				<p> *wordStartResult and *wordEndResult are zero-based.</p>
		*/
		virtual	void	FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordStartResult, size_t* wordEndResult, bool* wordReal
										) const = 0;

	public:
		/*
		@METHOD:		TextBreaks::FindLineBreaks
		@DESCRIPTION:	<p>For the given text, and offset information, return the next reasonable point in the text to break the row,
			for word-wrapping (for example, a space). <code>wordReal</code> somehow flags if we were forced to break at an unnatural
			place, but I cannot recall the details.</p>
				<p>Must think a bit about generalizing this for other languages (though it was adequate for SJIS/Japanese, using the
			Kinsoku rule, but that code no longer exists as part of Led). The basic API will probably persist, but the implemntation
			will probably need to change significantly.</p>
		*/
		virtual	void	FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordEndResult, bool* wordReal
										) const = 0;
};





/*
@CLASS:			TextBreaks_Basic
@BASES:			@'TextBreaks'
@DESCRIPTION:	<p>The original text break algorithm I had implemented long ago. Its been touched up a bit in Led 3.0
			to work better with UNICODE. But its still not up to the 3.0 UNICODE spec - or even very close.</p>
*/
class	TextBreaks_Basic : public TextBreaks {
	private:
		typedef	TextBreaks	inherited;
	public:
		TextBreaks_Basic ();

	public:
		override	void	FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordStartResult, size_t* wordEndResult, bool* wordReal
										) const;
		override	void	FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordEndResult, bool* wordReal
										) const;

	protected:
		enum	CharacterClasses	{
			eSpaceClass,
			eSentinalClass,				// Special character which always breaks on either side, and stands by itself
			eWordClass,
		#if		qSingleByteCharacters
		#elif	qWideCharacters
			eKanjiClass,
			eKatakanaClass,
			eHiraganaClass,
			eRomanjiOrDigitClass,
		#else
			#error	"That character set not yet fully supported"
		#endif
			eOtherCharacterClass		//	e.g. some punctuation... (;)
		};
	protected:
		virtual	CharacterClasses	CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const;

#if		qDebug
	private:
		nonvirtual	void	RegressionTest ();
#endif
};




/*
@CLASS:			TextBreaks_Basic_WP
@BASES:			@'TextBreaks_Basic'
@DESCRIPTION:	<p></p>
*/
class	TextBreaks_Basic_WP : public TextBreaks_Basic {
	private:
		typedef	TextBreaks_Basic	inherited;
	public:
		TextBreaks_Basic_WP ();

	protected:
		override	CharacterClasses	CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const;

#if		qDebug
	private:
		nonvirtual	void	RegressionTest ();
#endif
};





/*
@CLASS:			TextBreaks_Basic_TextEditor
@BASES:			@'TextBreaks_Basic'
@DESCRIPTION:	<p></p>
*/
class	TextBreaks_Basic_TextEditor : public TextBreaks_Basic {
	private:
		typedef	TextBreaks_Basic	inherited;
	public:
		TextBreaks_Basic_TextEditor ();

	protected:
		override	CharacterClasses	CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText, const Led_tChar* charToExamine) const;

#if		qDebug
	private:
		nonvirtual	void	RegressionTest ();
#endif
};





#if		qMacOS
/*
@CLASS:			TextBreaks_System
@BASES:			@'TextBreaks'
@DESCRIPTION:	<p>Similar to the behavior you got in Led 2.3 with the old 'qUseSystemWordBreakRoutine' define.</p>
		<p>Right now - only implemented for MacOS.</p>
*/
class	TextBreaks_System : public TextBreaks {
	public:
	/*
	 */
	public:
		// NB: textOffsetToStartLookingForWord is zero-based.
		// No word was found - iff *wordStartResult and *wordEndResult are equal.
		// *wordStartResult and *wordEndResult are zero-based.
		override	void	FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordStartResult, size_t* wordEndResult, bool* wordReal
										) const;
		override	void	FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
											size_t* wordEndResult, bool* wordReal
										) const;
};
#endif



/*
@CLASS:			TextBreaks_DefaultImpl
@BASES:			@'TextBreaks'
@DESCRIPTION:	<p>This typedef maps to one of a number of different possible implementations of
			the @'TextBreaks' abstract class'. When you must construct a textbreaks class - this is
			probably what you should use.</p>
				<p>Note that it defaults to @'TextBreaks_Basic_WP'.</p>
*/
typedef	TextBreaks_Basic_WP	TextBreaks_DefaultImpl;



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



#if		qLedUsesNamespaces
}
#endif


#endif	/*__TextBreaks_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
