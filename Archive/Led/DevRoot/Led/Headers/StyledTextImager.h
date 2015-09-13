/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextImager_h__
#define	__StyledTextImager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextImager.h,v 2.78 2003/05/07 18:28:53 lewis Exp $
 */

/*
@MODULE:	StyledTextImager
@DESCRIPTION:
		<p>Basic foundations of display of 'styled', or marked up, text. The central
	class of this module is @'StyledTextImager'</p>

 */


/*
 * Changes:
 *	$Log: StyledTextImager.h,v $
 *	Revision 2.78  2003/05/07 18:28:53  lewis
 *	SPR#1408: add overload of StyleMarkerSummarySinkForSingleOwner::CTOR that takes TextLayoutBlock
 *	
 *	Revision 2.77  2003/04/01 18:17:58  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.76  2003/01/29 19:15:09  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.75  2003/01/29 17:56:39  lewis
 *	Add qAssertWarningForEqualPriorityMarkers
 *	
 *	Revision 2.74  2002/12/16 17:29:10  lewis
 *	For SPR#1206 - display of style info wrong for RTL text -
 *	StyledTextImager::StyleMarkerSummarySink::ProduceOutputSummary
 *	now pays attention to the TextLayoutBlock object and returns runs
 *	in virtual order.
 *	
 *	Revision 2.73  2002/12/01 17:52:37  lewis
 *	SPR#1183- summarizestylemark fixes for BIDI
 *	
 *	Revision 2.72  2002/11/27 15:58:30  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.71  2002/09/19 14:14:17  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like
 *	qOldStyleMarkerBackCompatHelperCode, qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.70  2002/05/06 21:33:37  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.69  2001/11/27 00:29:46  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.68  2001/10/19 20:47:13  lewis
 *	DocComments
 *	
 *	Revision 2.67  2001/10/17 20:42:54  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.66  2001/09/05 14:51:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.65  2001/08/29 23:00:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.64  2001/08/28 18:43:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.63  2001/07/31 15:17:35  lewis
 *	!qTypedefInTemplateToSpecScopeOfNestedTypeCompilerBug
 *	
 *	Revision 2.62  2000/09/25 21:56:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.61  2000/09/01 00:53:01  lewis
 *	small tweeks to get compiling under XWindows
 *	
 *	Revision 2.60  2000/09/01 00:26:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.59  2000/08/31 19:53:53  lewis
 *	use new Mac Led_Pen/Led_GDI_Obj_Selector classes on mac draw dashed underline
 *	
 *	Revision 2.58  2000/08/31 15:18:28  lewis
 *	fix typos uncovered compiling on MWERKS
 *	
 *	Revision 2.57  2000/08/28 20:18:26  lewis
 *	Make StyledTextImager::StyleMarker::GetPriority () non-pure-virtual, and return
 *	default value. Lots of new helper templates - like SimpleStyleMarkerByIncrementalFontSpec<>,
 *	SimpleStyleMarkerWithExtraDraw<>, ETC. Related to SPR#0784 etc - hidden text marker stuff. Lots more.
 *	
 *	Revision 2.56  2000/08/13 05:21:45  lewis
 *	SPR#0824- LED_DECLARE_USE_BLOCK_ALLOCATION instead of DECLARE_USE_BLOCK_ALLOCATION
 *	
 *	Revision 2.55  2000/08/13 04:15:05  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and
 *	patching computeNExtCursorPosition. Not 100% done - but mostly. VERY big change -
 *	and many bug fixes related to this hidden text stuff folded in as well - but not complete
 *	
 *	Revision 2.54  2000/04/15 14:32:36  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.53  2000/04/14 22:40:22  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.52  2000/03/31 16:59:13  lewis
 *	SPR#0721- use DECLARE_USE_BLOCK_ALLOCATION macro
 *	
 *	Revision 2.51  2000/03/29 23:35:33  lewis
 *	SPR#0720 - more cleanups of SimpleStyleMarkerByFontSpec code
 *	
 *	Revision 2.50  2000/03/29 22:27:25  lewis
 *	Added SimpleStyleMarkerByFontSpec/TrivialFontSpecStyleMarker utility classes to
 *	StyledTextImager module, and used that to re-implement the SyntaxColoring style
 *	marker implementations (soon more to simplify). SPR#0720
 *	
 *	Revision 2.49  2000/03/28 03:41:04  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class
 *	takes extra RunElement argument. Now this new code is called by default
 *	StyledTextImager::DrawText() etc overrides. And now HiddenText code properly merges
 *	at least its own drawing and StandardStyledTextImager::StyleMarker. All this respecting
 *	qOldStyleMarkerBackCompatHelperCode - so old code (not new hidden text code) should still
 *	compile and work roughly as before with this turned on.
 *	
 *	Revision 2.48  2000/03/18 18:59:46  lewis
 *	More relating to SPR#0718. Lose qNewMarkerCombineCode. Added qOldStyleMarkerBackCompatHelperCode
 *	for backward compat code. Cleanups, and improved docs, and fixed
 *	StyleMarkerSummarySinkForSingleOwner::CombineElements () for new API
 *	
 *	Revision 2.47  2000/03/18 03:48:49  lewis
 *	SPR#0718 - tmporary new qNewMarkerCombineCode code. Basicly trying to redesign how we
 *	summarize style markers so we keep track of lower priority markers. Idea is that eventually
 *	the high-pri ones will be able to USE the lowPri ones to produce a COMPOSITE draw routine.
 *	Just got first step working - new RunElement xtra data and new Summarize code working.
 *	Still must cleanup, document, deal with backward compat, etc. Then pass along the new
 *	info so it can be used. DID perf-testing on this part so far - and for the RTF 1.5Doc -
 *	no discrernable speed change
 *	
 *	Revision 2.46  1999/12/28 13:30:51  lewis
 *	fix trivial error so compiles on mac
 *	
 *	Revision 2.45  1999/12/28 10:11:56  lewis
 *	move nested classes lexically outside StyledTextImager so class definition
 *	not so busy and nested
 *	
 *	Revision 2.44  1999/12/14 18:05:29  lewis
 *	qHeavyDebugging for Invariant() calls
 *	
 *	Revision 2.43  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.42  1999/07/13 22:39:15  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various
 *	BugWorkArounds that were only needed for these old compilers
 *	
 *	Revision 2.41  1999/05/03 22:05:01  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.40  1999/05/03 21:41:25  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.39  1999/02/05 13:37:21  lewis
 *	DOCS
 *	
 *	Revision 2.38  1999/02/05 01:46:34  lewis
 *	add DOCCOMMENTS and method StyledTextImager::StyleMarkerSummarySink::CombineElements,
 *	and override in subclass StyleMarkerSummarySinkForSingleOwner, instead of old way
 *	
 *	Revision 2.37  1998/10/30 14:18:29  lewis
 *	vector<> instead of Led_Array, and some comments/docs.
 *	
 *	Revision 2.36  1998/07/24  01:02:19  lewis
 *	new StyleMarkerSummarySinkForSingleOwner
 *
 *	Revision 2.35  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.34  1998/04/09  01:28:53  lewis
 *	Moved StandardStyleMarker out of here to StandardStyledTextImager (was accidnetally in wrong place)
 *
 *	Revision 2.33  1998/04/08  01:41:04  lewis
 *	Moved StandardStyledTextIamger stuff to its own separate module.
 *
 *	Revision 2.32  1998/03/04  20:16:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.31  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.30  1997/10/01  01:59:13  lewis
 *	Added overloaded SetStyleInfo () which takes count and InfoSummaryRecord* args (instead of Led_Array).
 *
 *	Revision 2.29  1997/09/29  14:45:43  lewis
 *	Redo StyledTextImagers StyleDatabase using new Led_RefCntPtr and MarkerCovers.
 *	Lose qSupportLed22CompatAPI.
 *
 *	Revision 2.28  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.27  1997/07/23  23:05:21  lewis
 *	docs changes
 *
 *	Revision 2.26  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.25  1997/07/12  20:05:31  lewis
 *	Renamed TextImager & TextStore etc to lose _ in name.
 *	AutoDoc support.
 *	more...
 *
 *	Revision 2.24  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1997/06/18  02:46:40  lewis
 *	Use new StyleMarkerSummarySink instead of SummarizeStyleMarkers () - now that we
 *	have new TextStore_::Collect API.
 *
 *	Revision 2.22  1997/03/22  13:47:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.21  1997/03/04  20:06:49  lewis
 *	Fix GetStyleInfo/GetFontMetricsAt
 *
 *	Revision 2.20  1997/01/10  03:00:31  lewis
 *	throw specifiers.
 *
 *	Revision 2.19  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.18  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.16  1996/07/19  16:33:05  lewis
 *	Cleanups
 *	Renamed GetStandardStyledTextImager () -> GetSharedStyleDatabase ().
 *	Added CollectAllStyleMarkersInRange () method to style dbase.
 *
 *	Revision 2.15  1996/06/01  02:04:56  lewis
 *	qNoRTTISupportBug.
 *
 *	Revision 2.14  1996/05/23  19:25:10  lewis
 *	Lots of changes relating to new Led_FontSpecification. Especailly to
 *	SetFontInfo/GetFontInfo/InfoSummaryRecord.
 *	DoContinuoues has been replaces with GetContinuousStyleInfo()
 *
 *	Revision 2.13  1996/05/14  20:20:45  lewis
 *	make StyledTextImager arg to DrawSegment() const and added Led_Tablet
 *	arg as well - so we can pass along offscreen tablet, and avoid the
 *	Acquire.
 *	Did some typedef inherrited declarations.
 *
 *	Revision 2.12  1996/04/18  15:12:01  lewis
 *	Lose (now unneeed) CalcSegmentSize () override, and a few other cleanups.
 *
 *	Revision 2.11  1996/03/16  18:33:27  lewis
 *	Make Invariant's const.
 *	New tabstop support - Measure/Calc methods (mostly) no longer take hOrigin etc
 *	args.
 *
 *	Revision 2.10  1996/02/26  18:42:20  lewis
 *	Renamed CollectAllStyleMarkersInRange --> CollectAllStandardStyleMarkersInRange ()
 *	to improve clarity about what it does.
 *
 *	Revision 2.9  1996/02/05  04:11:29  lewis
 *	HookLosingTextStore/HookGainedNewTextStore instead of SpecifyTextStore.
 *	Cleanup/make portable DoContinuousStyle stuff.
 *	Support NULL-selection style.
 *
 *	Revision 2.8  1996/01/03  23:52:39  lewis
 *	AccumulateMarkerForDeletion/Finalize etc stuff now done in styledatabase
 *	instead of StyledTextImager, so can be done when there ARE no imagers
 *	(like if we have multiple views onto imager, and we destroy last
 *	imager, but still want to be able to change the text...
 *	)
 *
 *	Revision 2.7  1995/12/15  02:14:57  lewis
 *	DrawSegment() methods now take an invalidRect arg for logical clipping
 *
 *	Revision 2.6  1995/12/09  05:34:03  lewis
 *	StandardStyledTextImager::SharedStyleDatabase::PeekAtTextStore () const no longer inline
 *
 *	Revision 2.5  1995/12/06  01:27:11  lewis
 *	Add LessThan() method (to cleanup implementation code).
 *	override new MarkerOwner_::PeekAtTextStore () method.
 *
 *	Revision 2.4  1995/11/25  00:20:53  lewis
 *	StandardStyledTextImager is now a virtual StyledTextImager.
 *	(don't remember why?)
 *
 *	Revision 2.3  1995/11/04  23:08:01  lewis
 *	Add new MeasureSegmentHeight() methods (overrides)
 *	DrawSegment now takes baseLine as an argument.
 *
 *	Revision 2.2  1995/11/02  21:52:01  lewis
 *	Added support for a style database being shared among styled imagers
 *	so in cases like opendoc with multiple views onto a single doc,
 *	we share same style info.
 *	Simplified stle run info to only include length, and not start, and
 *	added conversion rou;nes and utilities to convert to/from Apple style
 *	run formats (and DoContinuous).
 *
 *	Revision 2.1  1995/10/19  23:46:24  lewis
 *	Fixed screwed up const arg to MeasureText
 *
 *	Revision 2.0  1995/10/19  23:45:10  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/10/19  22:11:39  lewis
 *	LOTSA changes. Mostly broke up into two layers, - StyledTextImager
 *	which handles mapping the Led TextImager APIs into Marker ones, and
 *	StnadardStyledTextImager which implements traditional style-runs
 *	ontop of StyledTextImager.
 *	Lots of changes to get this actually working, but much more todo.
 *
 *	Revision 1.1  1995/10/09  22:21:03  lewis
 *	Initial revision
 *
 *
 *
 *
 */

#include	"LedSupport.h"
#include	"MarkerCover.h"
#include	"TextImager.h"




#if		qMacOS
	struct	TextStyle;
	struct	ScrpSTElement;
#endif


#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CONFIGVAR:		qAssertWarningForEqualPriorityMarkers
@DESCRIPTION:	<p>Debugging hack for @'StyledTextImager::StyleMarkerSummarySink::CombineElements'.
			It CAN cause problems having multiple markers of the same priority overlapping, so this
			code will assert-out / warn when that happens. It COULD be OK - so thats why this is optional
			checking.</p>
 */
#ifndef	qAssertWarningForEqualPriorityMarkers
#define	qAssertWarningForEqualPriorityMarkers	qDebug
#endif




/*
@CLASS:			StyledTextImager
@BASES:			virtual @'TextImager'
@DESCRIPTION:	<p>The class StyledTextImager is a @'TextImager' which knows about special markers,
			either owned by itself, or the TextStore, which it uses to render and
			measure the text. It is intended that these special markers it uses to
			render the text (@'StyledTextImager::StyleMarker''s) be general enough to support both standard
			style runs, as well as other fancier text adornments, like pictures, opendoc
			embeddings, etc.</p>
				<p>You can add arbitrary, and overlapping StyleMarkers to this class, and it will
			simply render them. Since it must pick ONE StyleMarker to ask todo the drawing,
			it asks the one with the highest priority (@'StyledTextImager::StyleMarker::GetPriority' ()). If you
			have some style marker which is smart enuf to pay attention to the
			overlapping of markers (RARE - IF EVER) it is up to your marker to find which
			other markers it overlaps with, and handle this combination itself.</p>
				<p>This class is intended to make easy things like wrapping keywords with little
			markers which affect how they are displayed. It is ideal for something like
			a programming text editor which colors keywords, or a typical web browser
			that has to keep associated links with parts of the text anyhow (keep it in a
			marker that subclasses from SytleMarker, and then change the color, or font of
			your display).</p>
				<p>For the more conventional Style-Run type API, see the class @'StandardStyledTextImager'.</p>
*/
class	StyledTextImager : public virtual TextImager {
	private:
		typedef	TextImager	inherited;

	protected:
		StyledTextImager ();

	public:
		class	StyleMarker;
		struct	RunElement;
		class	StyleMarkerSummarySink;
		class	StyleMarkerSummarySinkForSingleOwner;

	protected:
		virtual	vector<RunElement>	SummarizeStyleMarkers (size_t from, size_t to) const;
		virtual	vector<RunElement>	SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const;

	protected:
// Must override Draw/Measure text routines so style runs get hooked in and have some effect
// when this class is mixed in.
		override		void		DrawSegment (Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
											Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										);
		override		void		MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
														Led_Distance* distanceResults
													) const;
		override	Led_Distance	MeasureSegmentHeight (size_t from, size_t to) const;
		override	Led_Distance	MeasureSegmentBaseLine (size_t from, size_t to) const;


	// Debug support
	public:
		nonvirtual	void	Invariant () const;
#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
#endif
};




/*
@CLASS:			StyledTextImager::StyleMarker
@BASES:			@'Marker'
@DESCRIPTION:	<p>Abstract class - subclass this to provide custom style drawing. Managed by
			class @'StyledTextImager'.</p>
*/
class	StyledTextImager::StyleMarker : public Marker {
	protected:
		StyleMarker ();

	public:
		enum	{ eBaselinePriority = 0 };
	public:
		virtual	int	GetPriority () const;

	public:
		typedef	StyledTextImager::RunElement	RunElement;

	public:
		/*
		@METHOD:		StyledTextImager::StyleMarker::DrawSegment
		@DESCRIPTION:	<p>This pure-virtual hook function is called when the given range of text needs to be
					drawn. (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
						<p>NB: an extra 'invalidRect' argument was added to this method in Led 3.1a6. Note that
					this is incompatible change.</p>
		*/
		virtual	void			DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
												size_t from, size_t to, const TextLayoutBlock& text,
												const Led_Rect& drawInto, const Led_Rect& invalidRect, Led_Coordinate useBaseLine, 
												Led_Distance* pixelsDrawn
											)																			=	0;
		
		/*
		@METHOD:		StyledTextImager::StyleMarker::MeasureSegmentWidth
		@DESCRIPTION:	<p>This pure-virtual hook function is called when the given range of text needs to be
					measured (character widths). (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
		*/
		virtual	void			MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement,
												size_t from, size_t to,
												const Led_tChar* text,
												Led_Distance* distanceResults
											) const																		=	0;
		/*
		@METHOD:		StyledTextImager::StyleMarker::MeasureSegmentWidth
		@DESCRIPTION:	<p>This pure-virtual hook function is called when the given range of text needs to be
					measured (hieght of text segment). (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
		*/
		virtual	Led_Distance	MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, 
												size_t from, size_t to
											) const																		=	0;

		/*
		@METHOD:		StyledTextImager::StyleMarker::MeasureSegmentBaseLine
		@DESCRIPTION:	<p>This pure-virtual hook function is called when the given range of text needs to be
					measured (baseline of text segment). (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
		*/
		virtual	Led_Distance	MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement,
												size_t from, size_t to
											) const																		=	0;
};


		
/*
@CLASS:			StyledTextImager::RunElement
@DESCRIPTION:	<p>A simple summary structure typically used from
			@'StandardStyledTextImager::SummarizeStyleMarkers', or subclasses, to represent the net effect of overlapping
			style runs.</p>
				<p><code><pre>
	struct	StyledTextImager::RunElement {
		...
		StyleMarker*          fMarker;
		size_t                fLength;
		vector&lt;StyleMarker*&gt;  fSupercededMarkers;
		...
	};
</pre></code></p>
*/
struct	StyledTextImager::RunElement {
	RunElement (StyleMarker* marker, size_t length);

	StyleMarker*			fMarker;
	size_t					fLength;
	vector<StyleMarker*>	fSupercededMarkers;

	STLDefCTORDeclare_BWA(RunElement)
};




/*
@CLASS:			StyledTextImager::StyleMarkerSummarySink
@BASES:			@'TextStore::MarkerSink'
@DESCRIPTION:	<p>A marker sink used in calls to @'TextStore::CollectAllMarkersInRangeInto' to extract from the textstore
			the style marker information which applies to a given region of text. This is typically used from
			@'StandardStyledTextImager::SummarizeStyleMarkers', or subclasses. And you would override it (and that method)
			to provide an alternate mechanism for combining/interpretting style markers within a region (say when the overlap).</p>
*/
class	StyledTextImager::StyleMarkerSummarySink : public TextStore::MarkerSink {
	private:
		typedef	TextStore::MarkerSink	inherited;

	public:
		StyleMarkerSummarySink (size_t from, size_t to);
		StyleMarkerSummarySink (size_t from, size_t to, const TextLayoutBlock& text);
		
	public:
		nonvirtual	vector<RunElement>	ProduceOutputSummary () const;

	public:
		override	void	Append (Marker* m);

	protected:
		virtual		void	CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker);

	private:
		nonvirtual	void	SplitIfNeededAt (size_t markerPos);

	private:
		vector<RunElement>	fBuckets;
		const TextLayoutBlock*	fText;	// make this a reference once I've gotten rid of CTOR that takes no TextLayoutBlock - LGP 2002-12-16
		size_t				fFrom;
		size_t				fTo;
};





/*
@CLASS:			StyledTextImager::StyleMarkerSummarySinkForSingleOwner
@BASES:			@'StyledTextImager::StyleMarkerSummarySink'
@DESCRIPTION:	<p>Ignore style markers from an owner other than the one given as argument in the constructor.</p>
*/
class	StyledTextImager::StyleMarkerSummarySinkForSingleOwner : public StyledTextImager::StyleMarkerSummarySink {
	private:
		typedef	StyleMarkerSummarySink	inherited;
	public:
		StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to);
		StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to, const TextLayoutBlock& text);

	protected:
		override	void	CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker);
	private:
		const MarkerOwner&	fOwner;
};













/*
@CLASS:			SimpleStyleMarkerByFontSpec<BASECLASS>
@BASES:			BASECLASS = @'StyledTextImager::StyleMarker'
@DESCRIPTION:	<p>Very frequently, you will want to implement a style-marker which just uses some @'Led_FontSpecification'</p>
			and applies that to the given text, in a maner vaguely similar to what @'StandardStyledTextImager::StandardStyleMarker' does.
				<p>This class is an abstract class, where you must specify the particular @'Led_FontSpecification' by overriding
			the pure-virtual @'SimpleStyleMarkerByFontSpec::MakeFontSpec'. That font-spec is then used in the various
			DrawSegemnt () etc overrides.</p>
				<p>This class is not intended to be an abstract interface one programs to, but rather a helper class for subclasses
			of the abstract @'StyledTextImager::StyleMarker' class. So you are not encouraged to declare variables of the type
			<code>SimpleStyleMarkerByFontSpec<>*</code>. Just use the class as a helper.</p>
				<p>See @'TrivialFontSpecStyleMarker' for an even simpler class to use.</p>
*/
template	<class	BASECLASS = StyledTextImager::StyleMarker>
	class	SimpleStyleMarkerByFontSpec : public BASECLASS {
		private:
			typedef	BASECLASS	inherited;

		public:
			typedef	StyledTextImager::RunElement	RunElement;

		protected:
			SimpleStyleMarkerByFontSpec ();

		protected:
			/*
			@METHOD:		SimpleStyleMarkerByFontSpec<BASECLASS>::MakeFontSpec
			@DESCRIPTION:	<p>Virtual method which subclasses override to specify how <em>they</em> want to have the given text displayed.
						Hopefully enough context is passed into this function to make this helper class widely applicable. All it must do is return
						a simple @'Led_FontSpecification' result, and that will be used for all measurements and
						display of this marker.</p>
							<p>By default, it just returns the default font associated with the imager.</p>
			*/
			virtual	Led_FontSpecification		MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const;


		public:
			override	void			DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
												size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
												Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
											);
			override	void			MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
													const Led_tChar* text,
													Led_Distance* distanceResults
												) const;
			override	Led_Distance	MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const;
			override	Led_Distance	MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const;
	};








/*
@CLASS:			SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>
@BASES:			BASECLASS (which should be a subclass of @'SimpleStyleMarkerByFontSpec<BASECLASS>')
@DESCRIPTION:	<p>This helper can be used to avoid manually subclassing @'SimpleStyleMarkerByFontSpec<BASECLASS>'
			and instead, just takes an @'Led_IncrementalFontSpecification' and does the right thing - mapping
			that into what is displayed.</p>
*/
template	<class	BASECLASS>
	class	SimpleStyleMarkerByIncrementalFontSpec : public BASECLASS {
		private:
			typedef	BASECLASS	inherited;

		public:
			typedef	StyledTextImager::RunElement	RunElement;

		public:
			SimpleStyleMarkerByIncrementalFontSpec (const Led_IncrementalFontSpecification& styleInfo = Led_IncrementalFontSpecification ());

		protected:
			override	Led_FontSpecification		MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const;

		public:
			Led_IncrementalFontSpecification	fFontSpecification;
	};




	
	
/*
@CLASS:			TrivialFontSpecStyleMarker
@BASES:			@'SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>' with BASECLASS = @'SimpleStyleMarkerByFontSpec<BASECLASS>'.
@DESCRIPTION:	<p>This class just adds to @'SimpleStyleMarkerByFontSpec' a field which is the @'Led_IncrementalFontSpecification'.
			This is <em>not</em> intended to be subclassed. If you do subclass - beware the overload of operator new () and
			block-allocation usage. Or better yet, subclass @'SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>' instead.</p>
*/
class	TrivialFontSpecStyleMarker : public SimpleStyleMarkerByIncrementalFontSpec<SimpleStyleMarkerByFontSpec<> > {
	private:
		typedef	SimpleStyleMarkerByIncrementalFontSpec<SimpleStyleMarkerByFontSpec<> >	inherited;
	public:
		TrivialFontSpecStyleMarker (const Led_IncrementalFontSpecification& styleInfo);

	public:
		override	int		GetPriority () const;

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION (TrivialFontSpecStyleMarker);
};







/*
@CLASS:			SimpleStyleMarkerWithExtraDraw<BASECLASS>
@BASES:			@'SimpleStyleMarkerByFontSpec<BASECLASS>' with BASECLASS defaulting to @'StyledTextImager::StyleMarker'
@DESCRIPTION:	<p>Very frequently, you will want to implement a style-marker which just draws some
			extra stuff drawn at the end.</p>
				<p>This class is an abstract class, where you must specify the particular extra drawing in the
			@'SimpleStyleMarkerWithExtraDraw<BASECLASS>::DrawExtra' method.</p>
				<p>See also @'SimpleStyleMarkerWithLightUnderline<BASECLASS>'.</p>
*/
template	<typename	BASECLASS = StyledTextImager::StyleMarker>
	class	SimpleStyleMarkerWithExtraDraw : public SimpleStyleMarkerByFontSpec <BASECLASS> {
		private:
			typedef	SimpleStyleMarkerByFontSpec<BASECLASS>	inherited;

		public:
			typedef	StyledTextImager::RunElement	RunElement;

		protected:
			SimpleStyleMarkerWithExtraDraw ();

		protected:
			/*
			@METHOD:		SimpleStyleMarkerWithExtraDraw<BASECLASS>::DrawExtra
			@DESCRIPTION:	<p>Pure virtual method which subclasses override to specify how <em>they</em> want to
				draw (some additional markup - e.g. an underline).</p>
			*/
			virtual		void			DrawExtra (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
												size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
												Led_Coordinate useBaseLine, Led_Distance pixelsDrawn
											)		=	0;
	
		protected:
			virtual	RunElement	MungeRunElement (const RunElement& inRunElt) const;

		public:
			override	void			DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
												size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
												Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
											);
			override	void			MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
													const Led_tChar* text,
													Led_Distance* distanceResults
												) const;
			override	Led_Distance	MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const;
			override	Led_Distance	MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const;
	};














/*
@CLASS:			SimpleStyleMarkerWithLightUnderline<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>Very frequently, you will want to implement a style-marker which just draws some
			extra stuff drawn at the end.</p>
				<p>This template is typically used with the default BASECLASS of @'SimpleStyleMarkerWithExtraDraw<BASECLASS>'.</p>
*/
template	<class	BASECLASS = SimpleStyleMarkerWithExtraDraw <StyledTextImager::StyleMarker> >
	class	SimpleStyleMarkerWithLightUnderline : public BASECLASS {
		private:
			typedef	BASECLASS	inherited;

		public:
			typedef	StyledTextImager::RunElement	RunElement;

		public:
			SimpleStyleMarkerWithLightUnderline ();

		protected:
			override	void			DrawExtra (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
												size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
												Led_Coordinate useBaseLine, Led_Distance pixelsDrawn
											);

		public:
			virtual	Led_Color	GetUnderlineBaseColor () const;
	};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
// class StyledTextImager::StyleMarker
	inline	StyledTextImager::StyleMarker::StyleMarker ():
		Marker ()
		{
		}
	inline	void	StyledTextImager::Invariant () const
		{
			#if		qDebug && qHeavyDebugging
				Invariant_ ();
			#endif
		}


// class StyledTextImager::RunElement
	inline	StyledTextImager::RunElement::RunElement (StyledTextImager::StyleMarker* marker, size_t length):
		fMarker (marker),
		fLength (length)
		{
		}


// class SimpleStyleMarkerByFontSpec<BASECLASS>
	template	<class	BASECLASS>
		inline	SimpleStyleMarkerByFontSpec<BASECLASS>::SimpleStyleMarkerByFontSpec ():
			inherited ()
			{
			}
	template	<class	BASECLASS>
		Led_FontSpecification		SimpleStyleMarkerByFontSpec<BASECLASS>::MakeFontSpec (const StyledTextImager* imager, const RunElement& /*runElement*/) const
			{
				Led_RequireNotNil (imager);
				return imager->GetDefaultFont ();
			}
	template	<class	BASECLASS>
		void			SimpleStyleMarkerByFontSpec<BASECLASS>::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
													size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
													Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
												)
			{
				Led_RequireNotNil (imager);
				imager->DrawSegment_ (tablet, MakeFontSpec (imager, runElement), from, to, text, drawInto, useBaseLine, pixelsDrawn);
			}
	template	<class	BASECLASS>
		void			SimpleStyleMarkerByFontSpec<BASECLASS>::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
			{
				Led_RequireNotNil (imager);
				imager->MeasureSegmentWidth_ (MakeFontSpec (imager, runElement), from, to, text, distanceResults);
			}
	template	<class	BASECLASS>
		Led_Distance	SimpleStyleMarkerByFontSpec<BASECLASS>::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
			{
				Led_RequireNotNil (imager);
				return (imager->MeasureSegmentHeight_ (MakeFontSpec (imager, runElement), from, to));
			}
	template	<class	BASECLASS>
		Led_Distance	SimpleStyleMarkerByFontSpec<BASECLASS>::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
			{
				Led_RequireNotNil (imager);
				return (imager->MeasureSegmentBaseLine_ (MakeFontSpec (imager, runElement), from, to));
			}







// class SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>
	template	<class	BASECLASS>
		inline	SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>::SimpleStyleMarkerByIncrementalFontSpec (const Led_IncrementalFontSpecification& styleInfo):
			fFontSpecification (styleInfo)
			{
			}
	template	<class	BASECLASS>
		Led_FontSpecification		SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>::MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const
			{
				Led_RequireNotNil (imager);
				Led_FontSpecification	fsp	=	inherited::MakeFontSpec (imager, runElement);
				fsp.MergeIn (fFontSpecification);
				return fsp;
			}





// class TrivialFontSpecStyleMarker
	inline	TrivialFontSpecStyleMarker::TrivialFontSpecStyleMarker (const Led_IncrementalFontSpecification& styleInfo):
		inherited (styleInfo)
		{
		}






// class SimpleStyleMarkerWithExtraDraw<BASECLASS>
	template	<class	BASECLASS>
		inline	SimpleStyleMarkerWithExtraDraw<BASECLASS>::SimpleStyleMarkerWithExtraDraw ():
			inherited ()
			{
			}
	template	<class	BASECLASS>
		typename	SimpleStyleMarkerWithExtraDraw<BASECLASS>::RunElement	SimpleStyleMarkerWithExtraDraw<BASECLASS>::MungeRunElement (const RunElement& inRunElt) const
			{
				#if		!qTypedefInTemplateToSpecScopeOfNestedTypeCompilerBug
					typedef	StyledTextImager::StyleMarker	StyleMarker;
				#endif
				RunElement	newRunElement	=	inRunElt;
				Led_Require (inRunElt.fMarker == (StyleMarker*)this);
				newRunElement.fMarker = NULL;
				for (vector<StyleMarker*>::iterator i = newRunElement.fSupercededMarkers.begin (); i != newRunElement.fSupercededMarkers.end (); ) {
					if (newRunElement.fMarker == NULL or newRunElement.fMarker->GetPriority () < (*i)->GetPriority ()) {
						newRunElement.fMarker = (*i);
						i = newRunElement.fSupercededMarkers.erase (i);
					}
					else {
						++i;
					}
				}
				return newRunElement;
			}
	template	<class	BASECLASS>
		void			SimpleStyleMarkerWithExtraDraw<BASECLASS>::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
													size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
													Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
												)
			{
				Led_RequireNotNil (imager);
				Led_AssertNotNil (pixelsDrawn);	// if allowed to pass NULL - I must pass my  own value so I can pass along to DrawExtra - see if anyone does this - LGP 2000-07-08

				RunElement	re	=	MungeRunElement (runElement);
				if (re.fMarker == NULL) {
					imager->DrawSegment_ (tablet, MakeFontSpec (imager, re), from, to, text, drawInto, useBaseLine, pixelsDrawn);
				}
				else {
					re.fMarker->DrawSegment (imager, re, tablet, from, to, text, drawInto, invalidRect, useBaseLine, pixelsDrawn);
				}
				DrawExtra (imager, re, tablet, from, to, text, drawInto, useBaseLine, *pixelsDrawn);
			}
	template	<class	BASECLASS>
		void			SimpleStyleMarkerWithExtraDraw<BASECLASS>::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
			{
				Led_RequireNotNil (imager);
				RunElement	re	=	MungeRunElement (runElement);
				if (re.fMarker == NULL) {
					imager->MeasureSegmentWidth_ (MakeFontSpec (imager, re), from, to, text, distanceResults);
				}
				else {
					re.fMarker->MeasureSegmentWidth (imager, re, from, to, text, distanceResults);
				}
			}
	template	<class	BASECLASS>
		Led_Distance	SimpleStyleMarkerWithExtraDraw<BASECLASS>::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
			{
				Led_RequireNotNil (imager);
				RunElement	re	=	MungeRunElement (runElement);
				if (re.fMarker == NULL) {
					return (imager->MeasureSegmentHeight_ (MakeFontSpec (imager, re), from, to));
				}
				else {
					return (re.fMarker->MeasureSegmentHeight (imager, re, from, to));
				}
			}
	template	<class	BASECLASS>
		Led_Distance	SimpleStyleMarkerWithExtraDraw<BASECLASS>::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
			{
				Led_RequireNotNil (imager);
				RunElement	re	=	MungeRunElement (runElement);
				if (re.fMarker == NULL) {
					return (imager->MeasureSegmentBaseLine_ (MakeFontSpec (imager, re), from, to));
				}
				else {
					return (re.fMarker->MeasureSegmentBaseLine (imager, re, from, to));
				}
			}





// class SimpleStyleMarkerWithLightUnderline<BASECLASS>
	template	<typename	BASECLASS>
		inline	SimpleStyleMarkerWithLightUnderline<BASECLASS>::SimpleStyleMarkerWithLightUnderline ():
			inherited ()
			{
			}
	template	<typename	BASECLASS>
		void	SimpleStyleMarkerWithLightUnderline<BASECLASS>::DrawExtra (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, Led_Tablet tablet,
												size_t /*from*/, size_t /*to*/, const TextLayoutBlock& /*text*/, const Led_Rect& drawInto,
												Led_Coordinate useBaseLine, Led_Distance pixelsDrawn
											)
			{
				Led_Color	lightColor	=	Led_Color::kWhite/2 + GetUnderlineBaseColor () / 2;	// (white - baseColor)/2 + baseColor, but careful to avoid int overflow...
				#if		qWindows
					Led_Pen	pen (PS_DOT, 1, lightColor.GetOSRep ());
				#elif	qMacOS
					Led_Pen	pen (patCopy, &Led_Pen::kGrayPattern, lightColor);
				#elif	qXWindows
					Led_Pen	pen;
				#endif
				Led_GDI_Obj_Selector	penWrapper (tablet, pen);
				Led_Coordinate	underlineAt	=	useBaseLine;
				if (underlineAt + 1 < drawInto.bottom) {
					underlineAt += 1;
				}
				tablet->MoveTo (Led_Point (underlineAt, drawInto.left));
				tablet->LineTo (Led_Point (underlineAt, drawInto.left + pixelsDrawn));
			}
	template	<typename	BASECLASS>
		Led_Color	SimpleStyleMarkerWithLightUnderline<BASECLASS>::GetUnderlineBaseColor () const
			{
				return Led_Color::kBlack;
			}


#if		qLedUsesNamespaces
}
#endif



#endif	/*__StyledTextImager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
