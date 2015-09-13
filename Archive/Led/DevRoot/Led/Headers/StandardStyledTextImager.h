/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StandardStyledTextImager_h__
#define	__StandardStyledTextImager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StandardStyledTextImager.h,v 1.36 2003/04/01 18:17:57 lewis Exp $
 */

/*
@MODULE:	StandardStyledTextImager
@DESCRIPTION:	<p>@'StandardStyledTextImager' is not intended to be a general and flexible
	style run interface. Rather, it is intended to mimic the usual style-run
	support found in other text editors, like MS-Word, or Apples TextEdit.</p>
		<p>It keeps style runs in markers which are a subclass of StyledTextImager::StyleMarker,
	called @'StandardStyledTextImager::StandardStyleMarker'.</p>

 */


/*
 * Changes:
 *	$Log: StandardStyledTextImager.h,v $
 *	Revision 1.36  2003/04/01 18:17:57  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 1.35  2003/03/27 15:47:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.34  2003/02/01 15:49:17  lewis
 *	last SPR# was really 1279(not 1271)
 *	
 *	Revision 1.33  2003/02/01 15:48:17  lewis
 *	SPR#1271- Added TextImager::GetDefaultSelectionFont () and overrode in StandardStyledTextImager, and used
 *	that in WM_GETFONT Led_Win32 callback
 *	
 *	Revision 1.32  2003/01/11 19:28:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.31  2002/11/27 15:58:26  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 1.30  2002/10/23 18:05:02  lewis
 *	small tweeks to get compiling on GCC
 *	
 *	Revision 1.29  2002/10/07 23:18:55  lewis
 *	moved some class declarations lexically, and created AbstractStyleDatabaseRep and
 *	Led_RefCntPtr<AbstractStyleDatabaseRep> StyleDatabasePtr (instead of styledatabserep). Part of SPR#1118.
 *	
 *	Revision 1.28  2002/09/20 15:07:23  lewis
 *	get rid of qTypedefsOfPrivateAccessNamesNotAllowedInSourceFile - wasn't really a bug
 *	
 *	Revision 1.27  2002/09/19 14:14:09  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 1.26  2002/05/06 21:33:33  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.25  2001/11/27 00:29:44  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.24  2001/10/19 20:47:13  lewis
 *	DocComments
 *	
 *	Revision 1.23  2001/10/17 20:42:52  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 1.22  2001/08/29 23:00:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.21  2001/08/28 18:43:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.20  2001/07/31 15:18:13  lewis
 *	qTypedefInTemplateToSpecScopeOfNestedTypeCompilerBug
 *	
 *	Revision 1.19  2000/09/26 13:55:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.18  2000/08/31 15:38:25  lewis
 *	rename tmphack kludge name xxxx -> SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper
 *	(not a much better name), and some cleanups of the class (and docs)
 *	
 *	Revision 1.17  2000/08/28 20:14:38  lewis
 *	added (temphack) xxxx template to copy out font style info from StandardStyledTextImager::StandardStyleMarker
 *	objects into call to MakeFontSpec()
 *	
 *	Revision 1.16  2000/04/24 16:47:02  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor - but simply owned using
 *	Led_RefCntPtr<>. Also - did all the proper subclassing in WP class etc to make this work (almost -
 *	still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 1.15  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 1.14  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 1.13  2000/03/28 03:41:04  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class takes extra
 *	RunElement argument. Now this new code is called by default StyledTextImager::DrawText() etc
 *	overrides. And now HiddenText code properly merges at least its own drawing and
 *	StandardStyledTextImager::StyleMarker. All this respecting qOldStyleMarkerBackCompatHelperCode -
 *	so old code (not new hidden text code) should still compile and work roughly as before with this turned on.
 *	
 *	Revision 1.12  1999/12/19 15:38:23  lewis
 *	SPR#0666 - kill obsolete private function and wrap in qSupportLed23CompatAPI old code which
 *	should be needed cuz its a trivial wrapper - and I can see no need for this functinality anyhow
 *	
 *	Revision 1.11  1999/12/07 20:06:15  lewis
 *	make GetContinuousStyleInfo/ GetContinuousStyleInfo_ const methods
 *	
 *	Revision 1.10  1999/11/13 22:33:01  lewis
 *	lose qSupportLed22CompatAPI support
 *	
 *	Revision 1.9  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 1.8  1999/07/13 22:39:12  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds
 *	that were only needed for these old compilers
 *	
 *	Revision 1.7  1999/05/03 22:04:58  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 1.6  1999/05/03 21:41:24  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 1.5  1999/02/05 01:50:01  lewis
 *	add override of SummarizeStyleMarkers so we can restrict paying attention to only markers
 *	of our current styledatabase (at least by default - that default can be cahnged in a subclass
 *	
 *	Revision 1.4  1998/10/30 14:19:19  lewis
 *	Use vector<> instead lf Led_Array, and cleanup some code due to compiler bug fixes - nested classes
 *	now working better (with template expansion).
 *	
 * Revision 1.3  1998/05/05  00:29:36  lewis
 * support MWERKSCWPro3 - and lose old bug workarounds.
 *
 * Revision 1.2  1998/04/09  01:28:28  lewis
 * Moved StandardStyleMarker here from StyledTextImager module (was accidentally in wrong place)
 *
 * Revision 1.1  1998/04/08  01:37:02  lewis
 * Initial revision
 *
 *
 *
 *	<<CODE MOVED HERE FROM StyledTextImager.cpp>>
 *
 *
 */

#include	"StyledTextImager.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif







/*
@CLASS:			StandardStyledTextImager
@BASES:			virtual @'StyledTextImager'
@DESCRIPTION:	<p>StandardStyledTextImager is not intended to be a general and flexible
			style run interface (like @'StyledTextImager'). Rather, it is intended to mimic
			traditional API for handling styled text found in other text editors, like MS-Word,
			or Apples TextEdit.
			</p>

				<p>You can specify ranges of text, and apply font styles to them.
			And there are routines (like @'StandardStyledTextImager::GetContinuousStyleInfo' etc)
			to help find/idenitfiy font style runs, etc.</p>

				<p>This class is built trivially ontop of the @'StyledTextImager'.
			The rendering of styles and layout issues are all taken care of. 
			This class is merely responsable for preserving a disjoint cover (partition) of styles
			(@'StandardStyledTextImager::StandardStyleMarker'), and coalescing adjacent
			ones that have the same font info.</p>
*/
class	StandardStyledTextImager : public virtual StyledTextImager {
	private:
		typedef	StyledTextImager	inherited;
	protected:
		StandardStyledTextImager ();
		virtual ~StandardStyledTextImager ();

	protected:
		override	void	HookLosingTextStore ();
		nonvirtual	void	HookLosingTextStore_ ();
		override	void	HookGainedNewTextStore ();
		nonvirtual	void	HookGainedNewTextStore_ ();

	/*
	 *	Interface to getting and setting STANDARD style info. These routines ignore
	 *	custom style markers.
	 */
	public:
		/*
		@CLASS:			StandardStyledTextImager::InfoSummaryRecord
		@BASES:			@'Led_FontSpecification'
		@DESCRIPTION:	<p>Add a length attribute to @'Led_FontSpecification'. Used in specifying style runs by
			@'StandardStyledTextImager'.</p>
		*/
		struct	InfoSummaryRecord : public Led_FontSpecification {
			public:
				InfoSummaryRecord (const Led_FontSpecification& fontSpec, size_t length);

			public:
				size_t	fLength;
			STLDefCTORDeclare_BWA(InfoSummaryRecord)
		};

		nonvirtual	Led_FontSpecification	GetStyleInfo (size_t charAfterPos) const;
		nonvirtual	vector<InfoSummaryRecord>
										GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const;
		nonvirtual	void				SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo);
		nonvirtual	void				SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos);
		nonvirtual	void				SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos);

	public:
		override	Led_FontMetrics	GetFontMetricsAt (size_t charAfterPos) const;


	public:
		override	Led_FontSpecification	GetDefaultSelectionFont () const;

	// Do macstyle lookalike routines (DoSetStyle, DoContinuous etc)
	public:
		virtual		Led_IncrementalFontSpecification	GetContinuousStyleInfo (size_t from, size_t nTChars) const;	// was DoContinuousStyle()
	protected:
		nonvirtual	Led_IncrementalFontSpecification	GetContinuousStyleInfo_ (const vector<InfoSummaryRecord>& summaryInfo) const;

	#if		qMacOS
	public:
			nonvirtual	bool	DoContinuousStyle_Mac (size_t from, size_t nTChars, short* mode, TextStyle* theStyle);
	#endif


	#if		qMacOS
	// macstyle routines to get/set 'styl' resources for range of text...
	public:
		static		vector<InfoSummaryRecord>	Convert (const ScrpSTElement* teScrapFmt, size_t nElts);
		static		void						Convert (const vector<InfoSummaryRecord>& fromLedStyleRuns, ScrpSTElement* teScrapFmt);	// Assumed pre-alloced and same legnth as fromLedStyleRuns
	#endif

	public:
		class	StandardStyleMarker;


	public:
		class	AbstractStyleDatabaseRep;
		class	StyleDatabaseRep;

	public:
		/*
		@CLASS:			StandardStyledTextImager::StyleDatabasePtr
		@BASES:			@'Led_RefCntPtr<T>', (T=@'StandardStyledTextImager::StyleDatabaseRep')
		@DESCRIPTION:	
		*/
		typedef	Led_RefCntPtr<AbstractStyleDatabaseRep>	StyleDatabasePtr;
	public:
		nonvirtual	StyleDatabasePtr			GetStyleDatabase () const;
		nonvirtual	void						SetStyleDatabase (const StyleDatabasePtr& styleDatabase);
	protected:
		virtual		void						HookStyleDatabaseChanged ();


	private:
		StyleDatabasePtr	fStyleDatabase;
		bool				fICreatedDatabase;

	// Debug support
#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
#endif
};




/*
@CLASS:			StandardStyledTextImager::StandardStyleMarker
@BASES:			@'StyledTextImager::StyleMarker'
@DESCRIPTION:
		<p>Private, implementation detail class. Part of @'StandardStyledTextImager' implementation.</p>
		<p>Should NOT be subclassed. These participate in routines to grab
	runs of style info, etc, and aren't really designed to be subclassed. Also, all elements
	of this type in the text buffer are summarily deleted upon deletion of the owning StyleDatabase.</p>
*/
class	StandardStyledTextImager::StandardStyleMarker : public	StyledTextImager::StyleMarker {
	private:
		typedef	StyledTextImager::StyleMarker	inherited;
	public:
		StandardStyleMarker (const Led_FontSpecification& styleInfo = TextImager::GetStaticDefaultFont ());

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

	public:
		nonvirtual	Led_FontSpecification	GetInfo () const;
		nonvirtual	void					SetInfo (const Led_FontSpecification& fsp);
	public:
		Led_FontSpecification	fFontSpecification;
};






/*
@CLASS:			StandardStyledTextImager::AbstractStyleDatabaseRep
@BASES:			@'MarkerOwner'
@DESCRIPTION:	<p>xxx.</p>
*/
class	StandardStyledTextImager::AbstractStyleDatabaseRep : public virtual MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;

	public:
		virtual		vector<InfoSummaryRecord>	GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const														=	0;
		virtual		void						SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo)			=	0;
		nonvirtual	void						SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos);
		virtual		void						SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos)	=	0;

	// Debug support
	public:
		nonvirtual	void	Invariant () const;
#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
#endif
};






/*
@CLASS:			StandardStyledTextImager::StyleDatabaseRep
@BASES:			@'MarkerCover'<@'StandardStyleMarker',@'Led_FontSpecification',@'Led_IncrementalFontSpecification'>
@DESCRIPTION:	<p>Reference counted object which stores all the style runs objects for a
	@'StandardStyledTextImager'. An explicit storage object like this is used, instead of using
	the @'StandardStyledTextImager' itself, so as too allow you to have either multiple views onto the
	same text which use the SAME database of style runs, or to allow using different style info databases,
	all live on the same text.</p>
*/
class	StandardStyledTextImager::StyleDatabaseRep : public StandardStyledTextImager::AbstractStyleDatabaseRep, private MarkerCover<StandardStyledTextImager::StandardStyleMarker,Led_FontSpecification,Led_IncrementalFontSpecification> {
	private:
		typedef	MarkerCover<StandardStyledTextImager::StandardStyleMarker,Led_FontSpecification,Led_IncrementalFontSpecification>	inheritedMC;

	public:
		StyleDatabaseRep (TextStore& textStore);

	public:
		override	vector<InfoSummaryRecord>	GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const;
		override	void						SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo);
		override	void						SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos);

#if		qDebug
	protected:
		override	void	Invariant_ () const;
#endif
};





/*
@CLASS:			SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper
@BASES:			BASECLASS
@DESCRIPTION:	<p>Simple helper class so that @'SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>' will use the font specification of any
			embedded @'StandardStyledTextImager::StandardStyleMarker' will have its associated @'Led_FontSpecification' copied out and used
			in the FontSpec.</p>
				<p>As a kludge - this class depends on the user ALSO using the template @'SimpleStyleMarkerWithExtraDraw<BASECLASS>'. We override
			its @'SimpleStyleMarkerWithExtraDraw<BASECLASS>::MungeRunElement' () method. This isn't strictly necesary, but it saves us alot of code.
			That way - we can just change the RunElt to return NULL for the marker - and then the caller will use its default algorithm, and call
			the MakeFontSpec () method.</p>
				<p>The nature of class class could change in the future - so its not recomended that anyone directly use it.</p>
*/
template	<class	BASECLASS>
	class	SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper : public BASECLASS {
		private:
			typedef	BASECLASS	inherited;

		public:
			typedef	StyledTextImager::RunElement	RunElement;

		protected:
			override	RunElement	MungeRunElement (const RunElement& inRunElt) const;
		protected:
			override	Led_FontSpecification		MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const;

		private:
			mutable	Led_FontSpecification	fFSP;
	};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

// class StandardStyledTextImager::StandardStyleMarker
	inline	StandardStyledTextImager::StandardStyleMarker::StandardStyleMarker (const Led_FontSpecification& styleInfo):
		StyleMarker (),
		fFontSpecification (styleInfo)
		{
		}
	inline	Led_FontSpecification	StandardStyledTextImager::StandardStyleMarker::GetInfo () const
		{
			return fFontSpecification;
		}
	inline	void					StandardStyledTextImager::StandardStyleMarker::SetInfo (const Led_FontSpecification& fsp)
		{
			fFontSpecification = fsp;
		}




// class StandardStyledTextImager::InfoSummaryRecord
	inline	StandardStyledTextImager::InfoSummaryRecord::InfoSummaryRecord (const Led_FontSpecification& fontSpec, size_t length):
		Led_FontSpecification (fontSpec),
		fLength (length)
		{
		}




// class StandardStyledTextImager
	inline	StandardStyledTextImager::StyleDatabasePtr	StandardStyledTextImager::GetStyleDatabase () const
		{
			return fStyleDatabase;
		}
	inline	Led_FontSpecification	StandardStyledTextImager::GetStyleInfo (size_t charAfterPos) const
		{
			vector<StandardStyledTextImager::InfoSummaryRecord>	result	=	fStyleDatabase->GetStyleInfo (charAfterPos, 1);
			Led_Assert (result.size () == 1);
			return result[0];
		}
	inline	vector<StandardStyledTextImager::InfoSummaryRecord>	StandardStyledTextImager::GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const
		{
			return (fStyleDatabase->GetStyleInfo (charAfterPos, nTCharsFollowing));
		}
	inline	void	StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo)
		{
			fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfo);
		}
	inline	void	StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos)
		{
			fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos);
		}
	inline	void	StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos)
		{
			fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, nStyleInfos, styleInfos);
		}




// class StandardStyledTextImager::AbstractStyleDatabaseRep
	inline	void	StandardStyledTextImager::AbstractStyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos)
		{
			SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos.size (), &styleInfos.front ());
		}
	inline	void	StandardStyledTextImager::AbstractStyleDatabaseRep::Invariant () const
		{
			#if		qDebug
			Invariant_ ();
			#endif
		}






// class SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>
template	<class	BASECLASS>
	typename	SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::RunElement	SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::MungeRunElement (const RunElement& inRunElt) const
		{
			#if		!qTypedefInTemplateToSpecScopeOfNestedTypeCompilerBug
				typedef	StyledTextImager::StyleMarker	StyleMarker;
			#endif
			typedef	StandardStyledTextImager::StandardStyleMarker	SSM;

			fFSP = Led_FontSpecification ();
			RunElement	result	=	inherited::MungeRunElement (inRunElt);
			for (vector<StyleMarker*>::iterator i = result.fSupercededMarkers.begin (); i != result.fSupercededMarkers.end (); ++i) {
				if (SSM* ssm = dynamic_cast<SSM*> (*i)) {
					fFSP = ssm->fFontSpecification;
				}
			}
			if (SSM* ssm = dynamic_cast<SSM*> (result.fMarker)) {
				fFSP = ssm->fFontSpecification;
				result.fMarker = NULL;
			}
			return result;
		}
template	<class	BASECLASS>
	Led_FontSpecification	SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::MakeFontSpec (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/) const
		{
			return fFSP;
		}



#if		qLedUsesNamespaces
}
#endif



#endif	/*__StandardStyledTextImager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
