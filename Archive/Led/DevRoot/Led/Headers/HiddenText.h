/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__HiddenText_h__
#define	__HiddenText_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/HiddenText.h,v 2.42 2003/04/01 18:17:57 lewis Exp $
 */

/*
@MODULE:	HiddenText
@DESCRIPTION:
		<p>This module provides generic support for keeping arbitrary ranges of text hidden within
	a Led buffer, but making it available for later display.</p>.
 */


/*
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: HiddenText.h,v $
 *	Revision 2.42  2003/04/01 18:17:57  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.41  2003/01/29 19:15:03  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.40  2003/01/11 19:28:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.39  2002/12/03 20:02:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.38  2002/12/03 02:20:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.37  2002/11/27 15:58:18  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.36  2002/10/02 00:20:44  lewis
 *	SPR#1117- lose commandhandler argument to UniformHidableTextMarkerOwner, ColoredUniformHidableTExtMarkerOwner etc
 *	
 *	Revision 2.35  2002/05/06 21:33:24  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.34  2001/11/27 00:29:38  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.33  2001/10/20 13:38:54  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.32  2001/10/17 20:42:48  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.31  2001/09/26 15:41:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.30  2001/09/01 22:26:49  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.29  2001/08/28 18:43:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.28  2001/01/03 15:34:21  lewis
 *	FIX LAST CHECKIN
 *	
 *	Revision 2.27  2001/01/03 15:29:34  lewis
 *	qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
 *	
 *	Revision 2.26  2000/10/05 02:37:35  lewis
 *	added ColoredUniformHidableTextMarkerOwner
 *	
 *	Revision 2.25  2000/08/31 15:38:25  lewis
 *	rename tmphack kludge name xxxx -> SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper
 *	(not a much better name), and some cleanups of the class (and docs)
 *	
 *	Revision 2.24  2000/08/28 20:13:19  lewis
 *	More cleanups to hidden text code relating to 0784- fixing this code to work well
 *	
 *	Revision 2.23  2000/08/13 05:21:44  lewis
 *	SPR#0824- LED_DECLARE_USE_BLOCK_ALLOCATION instead of DECLARE_USE_BLOCK_ALLOCATION
 *	
 *	Revision 2.22  2000/08/13 04:37:44  lewis
 *	remove SuppressedDidUpdateText code since not needed anymore
 *	
 *	Revision 2.21  2000/08/13 04:35:19  lewis
 *	remove SuppressedDidUpdateText code since not needed anymore, and ifdef out a bug
 *	 workaround hack I think is no longer needed
 *	
 *	Revision 2.20  2000/08/13 04:15:04  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and patching computeNExtCursorPosition.
 *	Not 100% done - but mostly. VERY big change -  and many bug fixes related to this
 *	hidden text stuff folded in as well - but not complete
 *	
 *	Revision 2.19  2000/04/29 23:49:12  lewis
 *	convert tmphack globals for supressing this and that into instance variables and used classes to safely
 *	construct/destruct/set/unset the values (in presence of exceptions)
 *	
 *	Revision 2.18  2000/04/25 16:16:29  lewis
 *	partial work on SPR#0722- added HidableTextMarkerOwner::GetHidableRegionsWithData () and
 *	started using that.
 *	
 *	Revision 2.17  2000/04/25 00:57:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.16  2000/04/24 21:23:06  lewis
 *	quick cut at using new Internalizer/Externalizer code to save/restore contents of a hidabletext
 *	marker when hidden/restored
 *	
 *	Revision 2.15  2000/04/15 14:32:33  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.14  2000/04/14 22:40:19  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.13  2000/04/03 15:38:59  lewis
 *	added UniformHidableTextMarkerOwner::IsHidden () implementation, and make a couple methods
 *	protected instead of private
 *	
 *	Revision 2.12  2000/03/31 21:17:40  lewis
 *	Added quick-and-dirty implemention of HidableTextMarkerOwner::GetHidableRegionsContiguous (). And added
 *	HidableTextMarkerOwner::GetHidableRegions () - related to spr#0717.
 *	
 *	Revision 2.11  2000/03/31 17:00:51  lewis
 *	SPR#0721- use DECLARE_USE_BLOCK_ALLOCATION macro, and cleanups
 *	
 *	Revision 2.10  2000/03/31 00:50:03  lewis
 *	more work on SPR#0717: support RTF reading of \v tags - hidden text. Integrated HiddenText support into
 *	WordProcessor class (in a maner patterend after how we treat WordProcessor::ParagraphDatabasePtr
 *	
 *	Revision 2.9  2000/03/30 16:58:54  lewis
 *	now delete zero-sized hidden markers when they are in the 'shown' state. Other bugfixes
 *	
 *	Revision 2.8  2000/03/30 16:28:15  lewis
 *	renamed HiddenTextMarker* to HidableTextMarker*. Added Invariant () code. Implemented
 *	MakeRegionHidable/MakeRegionUNHidable.
 *	Got rid of fMarkers vector<> (after an unsucccessful experiment with using a map<>).
 *	
 *	Revision 2.7  2000/03/29 23:35:32  lewis
 *	SPR#0720 - more cleanups of SimpleStyleMarkerByFontSpec code
 *	
 *	Revision 2.6  2000/03/29 05:16:40  lewis
 *	LAST CHECKIN BOGUS COMMENT (no real change)
 *	
 *	Revision 2.5  2000/03/29 05:15:15  lewis
 *	experimental new qUseNewIMECode IME fixes. Must test at LEC. Basicly - use newer (documented) versions of the
 *	APIs since the old ones seem to sometimes mysteriously fail on Win2k
 *	
 *	Revision 2.4  2000/03/28 03:41:03  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class takes extra RunElement argument.
 *	Now this new code is called by default StyledTextImager::DrawText() etc overrides. And now HiddenText code properly
 *	merges at least its own drawing and StandardStyledTextImager::StyleMarker. All this respecting
 *	qOldStyleMarkerBackCompatHelperCode - so old code (not new hidden text code) should still compile
 *	and work roughly as before with this turned on.
 *	
 *	Revision 2.3  2000/03/18 03:46:10  lewis
 *	COMMENTS
 *	
 *	Revision 2.2  2000/03/17 22:32:25  lewis
 *	a bit more work - now partially functional. Can mark discontiguous regions as hidable -
 *	and then show/hide commands work
 *	
 *	Revision 2.1  2000/03/16 00:32:31  lewis
 *	Very preliminary first cut at this new module - part of SPR#0717
 *	
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
#include	"StandardStyledTextImager.h"		//only for template used in light-underline-xxxx - maybe move to WP file... so dont need this include here!




#if		qLedUsesNamespaces
namespace	Led {
#endif






#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4786)
#endif



/*
@CLASS:			HidableTextMarkerOwner
@DESCRIPTION:	<p>HidableTextMarkerOwner is a @'MarkerOwner' class, collecting together a set of
	@'HidableTextMarkerOwner::HidableTextMarker' elements. Each of these hidden-text elements can contain
	either currently hidden text, or text which could potentially be hidden.</p>
		<p>Note that not all @'HidableTextMarkerOwner::HidableTextMarker' elements need to share the same
	'hidden' / 'shown' state. Some can be hidden while others shown. And markers of different subclasses of
	@'HidableTextMarkerOwner::HidableTextMarker' can be collected together in a single, or in multiple
	@'HidableTextMarkerOwner's.</p>
		<p>See @'HidableTextMarkerOwner::HidableTextMarker' for more details, and subclasses.</p>
		<p><em>Implemenation Note:</em><br>
		I considered implementing this using a map<> for its red-black tree implementation. That might turn out to
	be faster if you have lots of hidden text. But the APIs STL provides you when you try todo this are not very friendly.
	I battled them for a while (you must make a map with a second bogus field, and then everywhere try to say 'first' after
	you access the interator - and thats not so bad in MY code - but when I use the builtin STL helpers - its more of a pain, and
	then the lookup - find routine was a real pain - cuz I don't want to lookup by Marker* - but by POSITION).</p>
*/
class	HidableTextMarkerOwner : public MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;

	public:
		HidableTextMarkerOwner (TextStore& textStore);
		virtual ~HidableTextMarkerOwner ();


	public:
		virtual	void	HideAll ();
		virtual	void	HideAll (size_t from, size_t to);
		virtual	void	ShowAll ();
		virtual	void	ShowAll (size_t from, size_t to);

	public:
		virtual	void	MakeRegionHidable (size_t from, size_t to);
		virtual	void	MakeRegionUnHidable (size_t from, size_t to);

	public:
		nonvirtual	DiscontiguousRun<bool>	GetHidableRegions () const;
		nonvirtual	DiscontiguousRun<bool>	GetHidableRegions (size_t from, size_t to) const;

	public:
		nonvirtual	bool	GetHidableRegionsContiguous (size_t from, size_t to, bool hidden) const;


	public:
		nonvirtual	Led_RefCntPtr<FlavorPackageInternalizer>	GetInternalizer () const;
		nonvirtual	void										SetInternalizer (const Led_RefCntPtr<FlavorPackageInternalizer>& i);
	private:
		Led_RefCntPtr<FlavorPackageInternalizer>	fInternalizer;

	public:
		nonvirtual	Led_RefCntPtr<FlavorPackageExternalizer>	GetExternalizer () const;
		nonvirtual	void										SetExternalizer (const Led_RefCntPtr<FlavorPackageExternalizer>& e);
	private:
		Led_RefCntPtr<FlavorPackageExternalizer>	fExternalizer;

	protected:
		class	HidableTextMarker;
		class	FontSpecHidableTextMarker;
		class	LightUnderlineHidableTextMarker;

	// called by 'HidableTextMarker's to compute their 'saved data'
	protected:
		virtual	void	CollapseMarker (HidableTextMarker* m);
		virtual	void	ReifyMarker (HidableTextMarker* m);

	protected:
		virtual	HidableTextMarker*	MakeHidableTextMarker ();

	public:
		override	TextStore*	PeekAtTextStore () const;
	public:
		override	void	AboutToUpdateText (const UpdateInfo& updateInfo);
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	private:
		TextStore&									fTextStore;
		mutable	MarkerMortuary<HidableTextMarker>	fMarkersToBeDeleted;
	protected:
		typedef	vector<HidableTextMarker*>	MarkerList;
	protected:
		nonvirtual	MarkerList	CollectAllInRange (size_t from, size_t to) const;
		nonvirtual	MarkerList	CollectAllInRange_OrSurroundings (size_t from, size_t to) const;


	public:
		nonvirtual	void	Invariant () const;

	#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
	#endif

	public:
		friend	class	HidableTextMarker;
};

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif












/*
@CLASS:			UniformHidableTextMarkerOwner
@DESCRIPTION:	<p>UniformHidableTextMarkerOwner is a @'HidableTextMarkerOwner' class, which has a notion of
	whether or not <em>all</em> the hidden text markers are hidden or not. You can manually change the hidden
	state of particular markers - if you wish (via @'HidableTextMarkerOwner::HideAll ()' commands with a from/to range).</p>
		<p>But the default behavior is that all the markers share this hidden or not hidden state, and NEWLY created
	'hiding' markers inherit this state.</p>
*/
class	UniformHidableTextMarkerOwner : public HidableTextMarkerOwner {
	private:
		typedef	HidableTextMarkerOwner	inherited;

	public:
		UniformHidableTextMarkerOwner (TextStore& textStore);

	public:
		override	void	HideAll ();
		override	void	ShowAll ();

	public:
		nonvirtual	bool	IsHidden () const;

	public:
		override	void	MakeRegionHidable (size_t from, size_t to);

	private:
		bool	fHidden;
};






/*
@CLASS:			HidableTextMarkerOwner::HidableTextMarker
@BASES:			@'StyledTextImager::StyleMarker'
@DESCRIPTION:	
*/
class	HidableTextMarkerOwner::HidableTextMarker : public StyledTextImager::StyleMarker {
	private:
		typedef	StyledTextImager::StyleMarker	inherited;
	public:
		HidableTextMarker ();

	public:
		nonvirtual	void	Hide ();
		nonvirtual	void	Show ();
		nonvirtual	bool	IsShown () const;

	protected:
		nonvirtual	HidableTextMarkerOwner&	GetOwner () const;

	private:
		bool						fShown;

	public:
		friend	class	HidableTextMarkerOwner;
};




	
/*
@CLASS:			HidableTextMarkerHelper<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>This class can be used with any 'BASECLASS' of subtype @'StyledTextImager::StyleMarker' and which also has
			an IsShown () method. Really it should most probably just be used with subclasses of @'HidableTextMarkerOwner::HidableTextMarker'.
			Note that the 'BASECLASS' should also be a CONCRETE base class - or at least have already defined its DrawSegment, etc methods. Basically
			all this helper does is choose whether or not to invoke those inherited DrawSegment etc methods or to return fake zero-size versions.
			</p>
*/
template	<typename	BASECLASS>
	class	HidableTextMarkerHelper : public BASECLASS {
		private:
			typedef	BASECLASS	inherited;

		public:
			typedef	StyledTextImager::RunElement	RunElement;

		public:
			HidableTextMarkerHelper ();

		public:
			override	int		GetPriority () const;

		protected:
			override	void			DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
													size_t from, size_t to, const TextLayoutBlock& text,
													const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, 
													Led_Distance* pixelsDrawn
												);
			override	void			MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement,
													size_t from, size_t to,
													const Led_tChar* text,
													Led_Distance* distanceResults
												) const;
			override	Led_Distance	MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, 
													size_t from, size_t to
												) const;

			override	Led_Distance	MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement,
													size_t from, size_t to
												) const;
	};






/*
@CLASS:			HidableTextMarkerOwner::FontSpecHidableTextMarker
@BASES:			@'SimpleStyleMarkerByFontSpec<BASECLASS>, where BASECLASS = @'HidableTextMarkerHelper<BASECLASS>' where BASECLASS = @'HidableTextMarkerOwner::HidableTextMarker'
@DESCRIPTION:	<p>Adds the argument style specification to the selected text. Smart enough to handle style overlaps with some kinds of
	style markers (most importantly - @'StandardStyledTextImager::StandardStyleMarker').</p>
		<p>May have to also add embedding support to this. And may want to move this functionality out of here, and into
	some other helper file, and just USE the functionality here.</p>
		<p>See also @'HidableTextMarkerOwner::LightUnderlineHidableTextMarker'.</p>
*/
#if		qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
	typedef	SimpleStyleMarkerByFontSpec<HidableTextMarkerOwner::HidableTextMarker>	SimpleStyleMarkerByFontSpec_HidableTextMarkerOwner_HidableTextMarker;
#endif
class	HidableTextMarkerOwner::FontSpecHidableTextMarker :
#if		qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
	public HidableTextMarkerHelper <SimpleStyleMarkerByFontSpec_HidableTextMarkerOwner_HidableTextMarker >
#else
	public HidableTextMarkerHelper <SimpleStyleMarkerByFontSpec <HidableTextMarkerOwner::HidableTextMarker> >
#endif
{
	private:
		typedef	HidableTextMarkerHelper < SimpleStyleMarkerByFontSpec <HidableTextMarkerOwner::HidableTextMarker>	> inherited;
	public:
		FontSpecHidableTextMarker (const Led_IncrementalFontSpecification& styleInfo);

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION(FontSpecHidableTextMarker);

	protected:
		override	Led_FontSpecification		MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const;

	public:
		Led_IncrementalFontSpecification	fFontSpecification;
};






/*
@CLASS:			HidableTextMarkerOwner::LightUnderlineHidableTextMarker
@BASES:			@'HidableTextMarkerHelper<BASECLASS>', where BASECLASS = @'SimpleStyleMarkerWithLightUnderline<BASECLASS>, where BASECLASS = @'HidableTextMarkerOwner::HidableTextMarker'
@DESCRIPTION:	<p>Adds a light dashed underline to the given region. One advantage of
			this over @'HidableTextMarkerOwner::FontSpecHidableTextMarker' is that
			it works well with other embeddings and display markers, cuz it lets them
			do their drawing, and simply adds the underline.</p>
*/
#if		qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
	typedef	SimpleStyleMarkerWithLightUnderline <SimpleStyleMarkerByIncrementalFontSpec <SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper <SimpleStyleMarkerWithExtraDraw <HidableTextMarkerOwner::HidableTextMarker> > > > 	SimpleStyleMarkerWithLightUnderline_SimpleStyleMarkerByIncrementalFontSpec_SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper_SimpleStyleMarkerWithExtraDraw_HidableTextMarkerOwner_HidableTextMarker;
#endif
class	HidableTextMarkerOwner::LightUnderlineHidableTextMarker :
#if		qNestedClassesWithoutTypeDefArgToTemplateCompilerBug
	public HidableTextMarkerHelper <SimpleStyleMarkerWithLightUnderline_SimpleStyleMarkerByIncrementalFontSpec_SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper_SimpleStyleMarkerWithExtraDraw_HidableTextMarkerOwner_HidableTextMarker>
#else
	public HidableTextMarkerHelper <SimpleStyleMarkerWithLightUnderline <SimpleStyleMarkerByIncrementalFontSpec <SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper <SimpleStyleMarkerWithExtraDraw <HidableTextMarkerOwner::HidableTextMarker> > > > >
#endif
{
	private:
		typedef	HidableTextMarkerHelper <SimpleStyleMarkerWithLightUnderline <SimpleStyleMarkerByIncrementalFontSpec <SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper <SimpleStyleMarkerWithExtraDraw <HidableTextMarkerOwner::HidableTextMarker> > > > >	inherited;
	public:
		LightUnderlineHidableTextMarker (const Led_IncrementalFontSpecification& fsp = Led_IncrementalFontSpecification ());

		override	Led_Color	GetUnderlineBaseColor () const;

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION(LightUnderlineHidableTextMarker);
};






/*
@CLASS:			ColoredUniformHidableTextMarkerOwner
@BASES:			@'UniformHidableTextMarkerOwner'
@DESCRIPTION:	<p>A @'UniformHidableTextMarkerOwner' where you can specify (simply) a color for the
			hidable text markers (when they are shown).</p>
*/
class	ColoredUniformHidableTextMarkerOwner : public UniformHidableTextMarkerOwner {
	private:
		typedef	UniformHidableTextMarkerOwner	inherited;

	public:
		ColoredUniformHidableTextMarkerOwner (TextStore& textStore);

	public:
		nonvirtual	Led_Color	GetColor () const;
		nonvirtual	void		SetColor (const Led_Color& color);
		nonvirtual	bool		GetColored () const;
		nonvirtual	void		SetColored (bool colored);

	protected:
		nonvirtual	void	FixupSubMarkers ();
	protected:
		override	HidableTextMarker*	MakeHidableTextMarker ();

	private:
		Led_Color	fColor;
		bool		fColored;
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	HidableTextMarkerOwner
	inline	Led_RefCntPtr<FlavorPackageInternalizer>	HidableTextMarkerOwner::GetInternalizer () const
		{
			return fInternalizer;
		}
	inline	Led_RefCntPtr<FlavorPackageExternalizer>	HidableTextMarkerOwner::GetExternalizer () const
		{
			return fExternalizer;
		}
	inline	void	HidableTextMarkerOwner::Invariant () const
		{
			#if		qDebug
				Invariant_ ();
			#endif
		}



//	class	HidableTextMarkerOwner::HidableTextMarker
	inline	HidableTextMarkerOwner::HidableTextMarker::HidableTextMarker ():
		inherited (),
		fShown (true)
		{
		}
	inline	HidableTextMarkerOwner&	HidableTextMarkerOwner::HidableTextMarker::GetOwner () const
		{
			MarkerOwner*	mo	=	inherited::GetOwner ();
			HidableTextMarkerOwner*	hmo	=	dynamic_cast<HidableTextMarkerOwner*> (mo);
			Led_EnsureNotNil (hmo);
			return *hmo;
		}
	inline	void	HidableTextMarkerOwner::HidableTextMarker::Hide ()
		{
			if (fShown) {
				GetOwner ().CollapseMarker (this);
			}
			Led_Ensure (not fShown);
		}
	inline	void	HidableTextMarkerOwner::HidableTextMarker::Show ()
		{
			if (not fShown) {
				GetOwner ().ReifyMarker (this);
			}
			Led_Ensure (fShown);
		}
	inline	bool	HidableTextMarkerOwner::HidableTextMarker::IsShown () const
		{
			return fShown;
		}










//	class	HidableTextMarkerHelper<BASECLASS>
	template	<typename	BASECLASS>
		inline	HidableTextMarkerHelper<BASECLASS>::HidableTextMarkerHelper ():
				inherited ()
			{
			}
	template	<typename	BASECLASS>
			int	HidableTextMarkerHelper<BASECLASS>::GetPriority () const
			{
				// simpleembedings are priorty 100 - maybe should use ENUM CONST? return 100;		// large enough to override most other markers (esp StyledTextImager::StyleMarker which we know how to aggregate).
				return 200;		// large enough to override most other markers (esp StyledTextImager::StyleMarker which we know how to aggregate).
			}
	template	<typename	BASECLASS>
		void	HidableTextMarkerHelper<BASECLASS>::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
													size_t from, size_t to, const TextLayoutBlock& text,
													const Led_Rect& drawInto, const Led_Rect& invalidRect, Led_Coordinate useBaseLine, 
													Led_Distance* pixelsDrawn
												)
			{
				if (IsShown ()) {
					inherited::DrawSegment (imager, runElement, tablet, from, to, text, drawInto, invalidRect, useBaseLine, pixelsDrawn);
				}
				else {
					*pixelsDrawn = 0;
				}
			}
	template	<typename	BASECLASS>
		void	HidableTextMarkerHelper<BASECLASS>::MeasureSegmentWidth (const StyledTextImager* imager, const StyledTextImager::RunElement& runElement, size_t from, size_t to, const Led_tChar* text,
																Led_Distance* distanceResults
															) const
			{
				if (IsShown ()) {
					inherited::MeasureSegmentWidth (imager, runElement, from, to, text, distanceResults);
				}
				else {
					size_t	len	=	to-from;
					for (size_t i = 0; i < len; ++i) {
						distanceResults[i] = 0;
					}
				}
			}
	template	<typename	BASECLASS>
		Led_Distance	HidableTextMarkerHelper<BASECLASS>::MeasureSegmentHeight (const StyledTextImager* imager, const StyledTextImager::RunElement& runElement, size_t from, size_t to) const
			{
				if (IsShown ()) {
					return inherited::MeasureSegmentHeight (imager, runElement, from, to);
				}
				else {
					return 0;
				}
			}
	template	<typename	BASECLASS>
		Led_Distance	HidableTextMarkerHelper<BASECLASS>::MeasureSegmentBaseLine (const StyledTextImager* imager, const StyledTextImager::RunElement& runElement, size_t from, size_t to) const
			{
				if (IsShown ()) {
					return inherited::MeasureSegmentBaseLine (imager, runElement, from, to);
				}
				else {
					return 0;
				}
			}


//	class	FontSpecHidableTextMarkerOwner::FontSpecHidableTextMarker
	inline	HidableTextMarkerOwner::FontSpecHidableTextMarker::FontSpecHidableTextMarker (const Led_IncrementalFontSpecification& styleInfo):
		inherited (),
		fFontSpecification (styleInfo)
		{
		}



//	class	UniformHidableTextMarkerOwner
	inline	bool	UniformHidableTextMarkerOwner::IsHidden () const
		{
			return fHidden;
		}









//	class	ColoredUniformHidableTextMarkerOwner
	inline	ColoredUniformHidableTextMarkerOwner::ColoredUniformHidableTextMarkerOwner (TextStore& textStore):
		inherited (textStore),
		fColor (Led_Color::kRed),
		fColored (false)
		{
		}
	inline	Led_Color	ColoredUniformHidableTextMarkerOwner::GetColor () const
		{
			return fColor;
		}
	inline	void	ColoredUniformHidableTextMarkerOwner::SetColor (const Led_Color& color)
		{
			fColor = color;
			FixupSubMarkers ();
		}
	inline	bool	ColoredUniformHidableTextMarkerOwner::GetColored () const
		{
			return fColored;
		}
	inline	void	ColoredUniformHidableTextMarkerOwner::SetColored (bool colored)
		{
			fColored = colored;
			FixupSubMarkers ();
		}



#if		qLedUsesNamespaces
}
#endif



#endif	/*__HiddenText_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
