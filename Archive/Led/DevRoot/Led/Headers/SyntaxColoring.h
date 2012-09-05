/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SyntaxColoring_h__
#define	__SyntaxColoring_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SyntaxColoring.h,v 2.16 2002/09/09 03:15:29 lewis Exp $
 */

/*
@MODULE:	SyntaxColoring
@DESCRIPTION:
		<p>The basic idea is that we will be - to some extent - mimicking the implementation of
	@'StandardStyledTextImager'.
	Recall that the class @'StyledTextImager' is what provides the basic infrastructure
	for displaying styled text.
	It introduces the special marker class @'StyledTextImager::StyleMarker' - which we can subclass todo
	our own kind of special display of text.
	And @'StandardStyledTextImager' simply leverages off this generic implementation, and maintains a database
	of non-overlapping StyleMarkers according to the usual editing, and style application conventions most
	standard text editing packages support (eg. apply style to region of text etc).
	</p>

		<p>Where we will diverge, is that we won't generate our style markers from external function calls or UI commands.
	Instead, we will programmaticly generate the style markers ourselves based on a simple
	lexical analysis of the text (@'SyntaxAnalyzer').
	</p>
 */

 /*
 * Changes:
 *	$Log: SyntaxColoring.h,v $
 *	Revision 2.16  2002/09/09 03:15:29  lewis
 *	change cmpFunction to return int so you can pass in Led_tStrCmp etc functions as value
 *	
 *	Revision 2.15  2002/05/06 21:33:37  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.14  2001/11/27 00:29:46  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.13  2001/10/17 21:46:42  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.12  2001/10/16 15:19:50  lewis
 *	DOCs
 *	
 *	Revision 2.11  2001/09/26 15:41:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.10  2001/08/28 18:43:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2001/07/19 21:05:21  lewis
 *	fix some small sytnax quirks only complained about by MWERKS compiler
 *	
 *	Revision 2.8  2000/08/13 05:21:45  lewis
 *	SPR#0824- LED_DECLARE_USE_BLOCK_ALLOCATION instead of DECLARE_USE_BLOCK_ALLOCATION
 *	
 *	Revision 2.7  2000/04/15 14:32:36  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.6  2000/04/14 22:40:23  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.5  2000/03/31 16:59:13  lewis
 *	SPR#0721- use DECLARE_USE_BLOCK_ALLOCATION macro
 *	
 *	Revision 2.4  2000/03/29 23:35:33  lewis
 *	SPR#0720 - more cleanups of SimpleStyleMarkerByFontSpec code
 *	
 *	Revision 2.3  2000/03/29 22:27:26  lewis
 *	Added SimpleStyleMarkerByFontSpec/TrivialFontSpecStyleMarker utility classes to StyledTextImager
 *	module, and used that to re-implement the SyntaxColoring style marker implementations
 *	(soon more to simplify). SPR#0720
 *	
 *	Revision 2.2  2000/03/28 04:11:34  lewis
 *	fix SynaxColoring code to reacto to changes for SPR#0718
 *	
 *	Revision 2.1  1999/12/24 23:08:11  lewis
 *	first cut - Moved here (and MAJORLY REVISED) from Tutorial code.
 *	
 *
 *
 *
 *		<<< CLONED FROM TUTORIAL CODE >>>
 *
 *
 *	Revision 2.3  1999/12/11 18:16:51  lewis
 *	add  operator new (size_t n)/delete/Led_BlockAllocated stuff as speed tweek for SyntaxHilightStyleMarker allocation
 *	
 *	Revision 2.2  1999/07/19 19:23:20  lewis
 *	use vector instead of Led_Array, default qSupportOnlyMarkersWhichOverlapVisibleRegion and DOCS
 *	
 *	Revision 2.1  1997/07/23 23:40:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.0  1997/03/04  20:42:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 *
 *
 */

#include	"LedSupport.h"
#include	"TextInteractor.h"
#include	"StyledTextImager.h"





#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			SyntaxAnalyzer
@DESCRIPTION:	<p>This abstract class is the basic for defining the rules for hooking in arbitrary syntactical analyses into the
	syntax coloring code.</p>
		<p>See the @'TrivialRGBSyntaxAnalyzer' class as a trivial example, and the @'TableDrivenKeywordSyntaxAnalyzer' as a more
	useful starting point for various syntax coloring strategies.</p>
*/
class	SyntaxAnalyzer {
	public:
		virtual	void	AdjustLookBackRange (TextStore* ts, size_t* lookBackStart, size_t* lookBackTo) const = 0;
		virtual	void	AddMarkers (TextStore* ts, TextInteractor* interactor, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const = 0;
};


/*
@CLASS:			TrivialRGBSyntaxAnalyzer
@BASES:			@'SyntaxAnalyzer'
@DESCRIPTION:	<p>A simple example @'SyntaxAnalyzer', which demonstrates the little you need todo to hook in your own
			syntax analysis rules.</p>
*/
class	TrivialRGBSyntaxAnalyzer : public SyntaxAnalyzer {
	public:
		override	void	AdjustLookBackRange (TextStore* ts, size_t* lookBackStart, size_t* lookBackTo) const;
		override	void	AddMarkers (TextStore* ts, TextInteractor* interactor, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const;
};


/*
@CLASS:			TableDrivenKeywordSyntaxAnalyzer
@BASES:			@'SyntaxAnalyzer'
@DESCRIPTION:	<p>A simple table-driven @'SyntaxAnalyzer', which looks up keywords from (constructor argument) tables.
			The elements of the argument table can be in any order - but no initial substring of a later string can come before
			an earlier one. You can specify an arbitrary compare function for matching keywords - but the two most obtious are
			@'Led_tStrnCmp' and @'Led_tStrniCmp'.</p>
				<p>This class also has two pre-built static tables for two common syntax coloring cases you may want to use,
			or start from: kCPlusPlusKeywords and kVisualBasicKeywords.</p>
*/
class	TableDrivenKeywordSyntaxAnalyzer : public SyntaxAnalyzer {
	private:
		typedef	SyntaxAnalyzer	inherited;
	public:
		class	KeywordTable {
			public:
				KeywordTable (const	Led_tChar* keyWords[], size_t nKeywords, int (*cmpFunction) (const Led_tChar*, const Led_tChar*, size_t) = Led_tStrnCmp);

			public:
				nonvirtual	unsigned	MaxKeywordLength () const;
				nonvirtual	size_t		KeywordLength (const Led_tChar* t, size_t nTChars) const;

			private:
				const	Led_tChar** fKeywords;
				size_t				fNKeywords;
				size_t				fMaxKeywordLength;
				int					(*fCmpFunction) (const Led_tChar*, const Led_tChar*, size_t);
		};
	public:
		static	KeywordTable	kCPlusPlusKeywords;
		static	KeywordTable	kVisualBasicKeywords;

	public:
		TableDrivenKeywordSyntaxAnalyzer (const KeywordTable& keyTable);

	public:
		override	void	AdjustLookBackRange (TextStore* ts, size_t* lookBackStart, size_t* lookBackTo) const;
		override	void	AddMarkers (TextStore* ts, TextInteractor* interactor, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const;

	private:
		KeywordTable	fKeywordTable;
};







/*
@CLASS:			SyntaxColoringMarkerOwner
@DESCRIPTION:	<p>An abstract class for controling the basic syntax coloring functionality. Try one of the subclasses,
		like @'SimpleSyntaxColoringMarkerOwner or @'WindowedSyntaxColoringMarkerOwner'.</p>
			<p>This class is usually used by instantiating a concrete subclass in your @'TextImager::HookGainedNewTextStore' override,
		and then destroyed in your  @'TextImager::HookLosingTextStore' override.
		You must also override @'TextImager::TabletChangedMetrics' to @'SyntaxColoringMarkerOwner::RecheckAll'.</p>
*/
class	SyntaxColoringMarkerOwner : public MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;

	public:
		SyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer);
		virtual ~SyntaxColoringMarkerOwner ();

	public:
		nonvirtual	void	RecheckAll ();

	protected:
		virtual		void	RecheckRange (size_t updateFrom, size_t updateTo)	=	0;

	public:
		class	ColoredStyleMarker;
	public:
		/*
		@CLASS:			SyntaxColoringMarkerOwner::FontChangeStyleMarker
		@BASES:			@'TrivialFontSpecStyleMarker'
		@DESCRIPTION:	<p>This is used internally by the syntax coloring code, and is exposed only in case you want to write your own
			Syntax Analyzer code. This simply takes a @'Led_FontSpecification' object and applies that to the given text.</p>
		*/
		typedef	TrivialFontSpecStyleMarker	FontChangeStyleMarker;

	public:
		override	TextStore*	PeekAtTextStore () const;
	protected:
		TextInteractor&			fInteractor;
		TextStore&				fTextStore;
		const SyntaxAnalyzer&	fSyntaxAnalyzer;
};




/*
@CLASS:			SimpleSyntaxColoringMarkerOwner
@BASES:			@'SyntaxColoringMarkerOwner'
@DESCRIPTION:	<p>A simple but effective brute-force coloring algorithm. This simply colors the entire document.
			And when any part of the document changes - this simply recolors the document. This runs very quickly. But
			for large documents, where you may never scroll to view large subsets of the document (or where you need to be able
			to open quickly) - this may not be your best choice.</p>
				<p>See also @'WindowedSyntaxColoringMarkerOwner'.</p>
*/
class	SimpleSyntaxColoringMarkerOwner : public SyntaxColoringMarkerOwner {
	private:
		typedef	SyntaxColoringMarkerOwner	inherited;

	public:
		SimpleSyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer);
		virtual ~SimpleSyntaxColoringMarkerOwner ();

	protected:
		override	void	RecheckRange (size_t updateFrom, size_t updateTo);

	public:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	private:
		vector<Marker*>	fMarkers;
};





/*
@CLASS:			WindowedSyntaxColoringMarkerOwner
@BASES:			@'SyntaxColoringMarkerOwner'
@DESCRIPTION:	<p>This @'SyntaxColoringMarkerOwner' tries to be clever about what areas to syntax analyze. It only analyzes
			the current window. This makes for very fast opening of large documents (independent of actual file size). But it can make
			typing and scrolling somewhat slower. This really doesn't matter as long as its faster than some particular user-measurable
			threshold. I think on a 400Mz or faster Pentium machine - this will always be fast enuf to be a better choice than
			@'SimpleSyntaxColoringMarkerOwner'. But you can easily try both, and see for yourself.</p>
				<p>In addition to settup according to the docs in @'SyntaxColoringMarkerOwner' - you must also override @'TextInteractor::UpdateScrollBars' to
			call @'WindowedSyntaxColoringMarkerOwner::RecheckScrolling'.</p>
				<p>See also @'SimpleSyntaxColoringMarkerOwner'.</p>
*/
class	WindowedSyntaxColoringMarkerOwner : public SyntaxColoringMarkerOwner {
	private:
		typedef	SyntaxColoringMarkerOwner	inherited;

	public:
		WindowedSyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer);
		virtual ~WindowedSyntaxColoringMarkerOwner ();

	public:
		nonvirtual	void	RecheckScrolling ();

	protected:
		override	void	RecheckRange (size_t updateFrom, size_t updateTo);

	public:
		override	void	AboutToUpdateText (const UpdateInfo& updateInfo);
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	private:
		vector<Marker*>	fMarkers;
		bool								fDeletedLines;
		// scrolling speed tweek - so we don't inval too much!
		size_t								fCachedWindowStart;
		size_t								fCachedWindowEnd;
};



















/*
@CLASS:			SyntaxColoringMarkerOwner::ColoredStyleMarker
@BASES:			@'SimpleStyleMarkerByFontSpec'
@DESCRIPTION:	<p>This is used internally by the syntax coloring code, and is exposed only in case you want to write your own
			Syntax Analyzer code. This simply takes a @'Led_Color' object and uses that to color the given text.</p>
*/
class	SyntaxColoringMarkerOwner::ColoredStyleMarker : public SimpleStyleMarkerByFontSpec<> {
	private:
		typedef	SimpleStyleMarkerByFontSpec<>	inherited;
	public:
		ColoredStyleMarker (const Led_Color& color);

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION (ColoredStyleMarker);

	protected:
		override	Led_FontSpecification		MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const;

	public:
		Led_Color				fColor;
};










/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	SyntaxColoringMarkerOwner::ColoredStyleMarker
	inline	SyntaxColoringMarkerOwner::ColoredStyleMarker::ColoredStyleMarker (const Led_Color& color):
		inherited (),
		fColor (color)
		{
		}



//	class	TableDrivenKeywordSyntaxAnalyzer::KeywordTable
	inline	TableDrivenKeywordSyntaxAnalyzer::KeywordTable::KeywordTable (const	Led_tChar* keyWords[], size_t nKeywords, int (*cmpFunction) (const Led_tChar*, const Led_tChar*, size_t)):
		fKeywords (keyWords),
		fNKeywords (nKeywords),
		fMaxKeywordLength (),
		fCmpFunction (cmpFunction)
		{
			Led_RequireNotNil (cmpFunction);
			unsigned	u	=	0;
			for (size_t i = 0; i < nKeywords; ++i) {
				u = max (u, unsigned (Led_tStrlen (keyWords[i])));
			}
			fMaxKeywordLength = u;
		}
	inline	unsigned	TableDrivenKeywordSyntaxAnalyzer::KeywordTable::MaxKeywordLength () const
		{
			return fMaxKeywordLength;
		}
	inline	size_t		TableDrivenKeywordSyntaxAnalyzer::KeywordTable::KeywordLength (const Led_tChar* t, size_t nTChars) const
		{
			for (size_t i = 0; i < fNKeywords; ++i) {
				const	size_t	kKeywordLen	=	Led_tStrlen (fKeywords[i]);
				if (kKeywordLen <= nTChars and fCmpFunction (fKeywords[i], t, kKeywordLen) == 0) {
					return kKeywordLen;
				}
			}
			return 0;
		}


#if		qLedUsesNamespaces
}
#endif



#endif	/*__SyntaxColoring_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
