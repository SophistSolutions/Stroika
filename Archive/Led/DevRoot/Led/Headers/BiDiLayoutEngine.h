/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__BiDiLayout_h__
#define	__BiDiLayout_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/BiDiLayoutEngine.h,v 2.21 2003/12/12 19:13:59 lewis Exp $
 */

/*
@MODULE:	BiDiLayout
@DESCRIPTION:	<p>Code to break up a string to be displayed - which might contain right-to-left text mixed
			with left-to-right text into logical and display text runs according to the UNICODE Bidirectional
			editing algorithm.
			</p>

 */


/*
 * Changes:
 *	$Log: BiDiLayoutEngine.h,v $
 *	Revision 2.21  2003/12/12 19:13:59  lewis
 *	SPR#1596: overlaod TextLayoutBlock_Copy::BlockRep::operator delete () cuz we allocate using new[].
 *	
 *	Revision 2.20  2003/11/27 20:10:28  lewis
 *	added op!= for ScriptRunElts
 *	
 *	Revision 2.19  2003/11/03 20:02:57  lewis
 *	SPR#1551: Lose unused qMapR2VPosForTL stuff. Add new efficeint storage/copying TextLayoutBlock_Copy class (performance). Add 'initialDirection' CTOR argument so you can specify (optionally) the base direction to use for a given line layout (when its externally specified, like on successive rows of a paragraph)
 *	
 *	Revision 2.18  2003/01/12 23:15:56  lewis
 *	change so qUseFriBidi defaults to OFF and if on it takes precedence over qTryToUseUNISCRIBEForTextRuns
 *	
 *	Revision 2.17  2003/01/10 17:48:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.16  2003/01/10 15:22:25  lewis
 *	as part of SPR#1226 - added prelim MapRealOffsetToVirtual/MapVirtualOffsetToReal methods, and
 *	qDebugHack_UpperCaseCharsTratedAsRTL support
 *	
 *	Revision 2.15  2003/01/08 04:38:24  lewis
 *	SPR#1218- qTryToUseUNISCRIBEForTextRuns defaults to OFF and qUseFriBidi defaults OFF if
 *	qTryToUseUNISCRIBEForTextRuns is TRUE
 *	
 *	Revision 2.14  2003/01/02 22:35:18  lewis
 *	disable qTryToUseUNISCRIBEForTextRuns - since it works SO badly
 *	
 *	Revision 2.13  2003/01/02 15:11:25  lewis
 *	SPR#1218- restructure code so can sue qTryToUseUNISCRIBEForTextRuns or other means to breakup text
 *	
 *	Revision 2.12  2002/12/13 18:39:51  lewis
 *	SPR#1204 - added TextLayoutBlock::GetCharacterDirection () API
 *	
 *	Revision 2.11  2002/12/12 16:49:53  lewis
 *	BIDI work- Added CopyOutRealText/CopyOutVirtualText (maybe replace the peek methods with this?). Added
 *	qDebugHack_ReverseDirections define so I can test with ENGLISH text (like abc) and see it reversed.
 *	Debugging hack only
 *	
 *	Revision 2.10  2002/12/04 19:38:31  lewis
 *	SPR#1194- fix buffer sizes for fribidi calls - to avoid crash. Also - added qMapR2VPosForTL to only conidiotnally
 *	make that available. Get rid of fVirtaulLength/fRealLength distinction - make API assume these are always the same
 *	
 *	Revision 2.9  2002/12/03 15:48:54  lewis
 *	SPR#1191- use new TextDirection enum defined in LedGDI.h. Get rid of ScriptRunElement::fLength.
 *	Add direction arg to Led_Tablet_::TabbedTextOut () and use that to call (WIN32)Led_Tablet_::SetTextAlign().
 *	Hopefully this fixes display on Win9x of Arabic text
 *	
 *	Revision 2.8  2002/12/02 00:50:04  lewis
 *	SPR#1183 BIDI code - added TextLayoutBlock::LessThanVirtualStart and cleanup a few warnings
 *	
 *	Revision 2.7  2002/11/28 20:54:45  lewis
 *	work on BIDI code (SPR#1183) - Add GetRealText/GetVirtualText no-arg routines. Plus, lots of fixups to the scriptRun code
 *	
 *	Revision 2.6  2002/11/28 03:46:09  lewis
 *	added fRealText to the TextLayoutBlock_VirtualSubset class so it could copy the source text by value
 *	and have a buffer of the right size and contiguous
 *	
 *	Revision 2.5  2002/11/28 02:30:59  lewis
 *	A couple simple name changes (GetVirtualText->PeekAtVirtualText and TextLayoutBlock_Subset to TextLayoutBlock_VirtualSubset
 *	
 *	Revision 2.4  2002/11/28 02:16:13  lewis
 *	tons of changes/improvements/cleanups. Temporarily get rid of MapReal/VirtualOffsetToEachOther routines since unused right now,
 *	and I'm not sure I'll need them. Wait and see. Added/fixed GetScriptRuns code to keep track of MUCH more more information.
 *	THIS is the basic information I will really use/need.
 *	
 *	Revision 2.3  2002/11/27 15:10:56  lewis
 *	add GetVirtualText{Length} method and fix code for PeekAt in SubTextLayoutBlock code (end correctly
 *	
 *	Revision 2.2  2002/11/26 22:45:42  lewis
 *	looked into using IBM's ICU package for bidi layout - but it was to big and intrusive to be worth the effort right now.
 *	Also - added GetScriptRuns () support to API and a few other small improvements
 *	
 *	Revision 2.1  2002/11/25 23:38:25  lewis
 *	SPR#1183- checked in very prelim version of BidiLayoutEngine
 *	
 *	
 *
 *
 */

#include	"LedGDI.h"
#include	"LedSupport.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif





/*
@CONFIGVAR:		qUseFriBidi
@DESCRIPTION:	<p>Defines whether or not we use the FriBidi package to generate text layouts for bidirectional text
			such as Hebrew or Arabic. This defaults to off, because it requires linking with an extra library.</p>
				<p>Note that if BOTH @'qUseFriBidi' and @'qTryToUseUNISCRIBEForTextRuns' are defined, then
			@'qUseFriBidi' will take precedence.</p>
 */
#ifndef	qUseFriBidi
	#define	qUseFriBidi		0
#endif




/*
@CONFIGVAR:		qTryToUseUNISCRIBEForTextRuns
@DESCRIPTION:	<p>Defines whether or not we try to use the UNISCRIBE Windows SDK to generate text layouts for bidirectional text
			such as Hebrew or Arabic. This defaults to true iff @'qWideCharacters' and @'qWindows' is true.
			Note that UNISCRIBE isn't necessarily available on a particular system. This code just tries to use
			UNISCRIBE if its available. So - you MAY want to use this option TOGETHER with @'qUseFriBidi'.</p>
				<p>Note that if BOTH @'qUseFriBidi' and @'qTryToUseUNISCRIBEForTextRuns' are defined, then
			@'qUseFriBidi' will take precedence.</p>
 */
#ifndef	qTryToUseUNISCRIBEForTextRuns
	#if		qWideCharacters && qWindows && qUniscribeAvailableWithSDK
		#define	qTryToUseUNISCRIBEForTextRuns		1
	#else
		#define	qTryToUseUNISCRIBEForTextRuns		0
	#endif
#endif




/*
@CONFIGVAR:		qUseICUBidi
@DESCRIPTION:	<p>Defines whether or not we use the IBM ICU package to generate text layouts for bidirectional text 
			such as Hebrew or Arabic. THIS IS NOT YET IMPLEMENTED (as of 3.1a3) and may never be - as the ICU package is huge,
			and only setup easily to work with a DLL copy of itself.</p>
 */
#ifndef	qUseICUBidi
	#define	qUseICUBidi		0
#endif



/*
@CLASS:			TextLayoutBlock
@DESCRIPTION:	<p>xxx -- XPLAIN - Add ref to URL docs for this API. Note always pass in PARAGRAPH as unit for layout (cuz in spec)
			</p>
*/
class	TextLayoutBlock {
	protected:
		TextLayoutBlock ();
		virtual ~TextLayoutBlock ();

	public:
		nonvirtual	const Led_tChar*	PeekAtRealText () const;
		nonvirtual	const Led_tChar*	PeekAtVirtualText () const;
		nonvirtual	size_t				GetTextLength () const;


	public:
		virtual	void	PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const		=	0;
		virtual	void	PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const	=	0;


	public:
		nonvirtual	TextDirection	GetCharacterDirection (size_t realCharOffset) const;

	public:
		nonvirtual	size_t	MapRealOffsetToVirtual (size_t i) const;
		nonvirtual	size_t	MapVirtualOffsetToReal (size_t i) const;

	public:
		struct	ScriptRunElt {
			// NB:		Since length of a run is always the same for virtual and real - we COULD make this struct smaller!
			TextDirection	fDirection;
			size_t			fRealStart;
			size_t			fRealEnd;
			size_t			fVirtualStart;
			size_t			fVirtualEnd;

			nonvirtual	bool operator== (const ScriptRunElt& rhs) const;
			nonvirtual	bool operator!= (const ScriptRunElt& rhs) const;
		};
		struct	LessThanVirtualStart;

	public:
		virtual	vector<ScriptRunElt>	GetScriptRuns () const												=	0;


	public:
		nonvirtual	void	CopyOutRealText (const ScriptRunElt& scriptRunElt, Led_tChar* buf) const;
		nonvirtual	void	CopyOutVirtualText (const ScriptRunElt& scriptRunElt, Led_tChar* buf) const;

	public:
		nonvirtual	void		PeekAtRealText (const ScriptRunElt& scriptRunElt, const Led_tChar** startText, const Led_tChar** endText) const;
		nonvirtual	Led_tString	GetRealText () const;
		nonvirtual	Led_tString	GetRealText (const ScriptRunElt& scriptRunElt) const;
		nonvirtual	void		PeekAtVirtualText (const ScriptRunElt& scriptRunElt, const Led_tChar** startText, const Led_tChar** endText) const;
		nonvirtual	Led_tString	GetVirtualText () const;
		nonvirtual	Led_tString	GetVirtualText (const ScriptRunElt& scriptRunElt) const;

	// Debug support
	public:
		nonvirtual	void	Invariant () const;
#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
#endif
};


bool	operator== (const TextLayoutBlock& lhs, const TextLayoutBlock& rhs);


/*
@CLASS:			TextLayoutBlock::LessThanVirtualStart
@BASES:			
@DESCRIPTION:	
	<p>Use this class when you have a vector of ScriptRunElt and want to sort it by virtual start.</p>
	<p>As in:
	</p>
	<code><pre>
	vector&lt;TextLayoutBlock::ScriptRunElt&gt; runs = get_em ();
	sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
	</pre></code>
*/
struct	TextLayoutBlock::LessThanVirtualStart : public binary_function <TextLayoutBlock::ScriptRunElt, TextLayoutBlock::ScriptRunElt,bool> {
	bool	operator () (const ScriptRunElt& lhs, const ScriptRunElt& rhs)
		{
			int	diff	=	int (lhs.fVirtualStart) - int (rhs.fVirtualStart);
			return (diff < 0);
		}
};



/*
@CLASS:			TextLayoutBlock_Basic
@DESCRIPTION:	<p>NB: the CTOR copies the argument string by value, and doesn't retain the pointer itself.
			</p>
*/
class	TextLayoutBlock_Basic : public TextLayoutBlock {
	private:
		typedef	TextLayoutBlock	inherited;

	public:
		TextLayoutBlock_Basic (const Led_tChar* realText, const Led_tChar* realTextEnd);
		TextLayoutBlock_Basic (const Led_tChar* realText, const Led_tChar* realTextEnd, TextDirection initialDirection);

	private:
		nonvirtual	void	Construct (const Led_tChar* realText, const Led_tChar* realTextEnd, const TextDirection* initialDirection);

	private:
		#if		qTryToUseUNISCRIBEForTextRuns
			nonvirtual	bool	Construct_UNISCRIBE (const TextDirection* initialDirection);
		#endif
		#if		qUseFriBidi
			nonvirtual	void	Construct_FriBidi (const TextDirection* initialDirection);
		#endif
		#if		qUseICUBidi
			nonvirtual	void	Construct_ICU (const TextDirection* initialDirection);
		#endif
		nonvirtual	void	Construct_Default ();

	public:
		override	void	PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const;
		override	void	PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const;

	public:
		override	vector<ScriptRunElt>	GetScriptRuns () const;

	private:
		size_t							fTextLength;
		Led_SmallStackBuffer<Led_tChar>	fRealText;
		Led_SmallStackBuffer<Led_tChar>	fVirtualText;
		vector<ScriptRunElt>			fScriptRuns;
};





/*
@CLASS:			TextLayoutBlock_Copy
@DESCRIPTION:	<p>This class is designed to allow for very cheap and quick copying of TextLayoutBlocks. This class
			and be stack/assign copied (not just by pointer). It is an indexpensive way to cache a TextLayoutBlock, or
			to return one from a function.
			</p>
*/
class	TextLayoutBlock_Copy : public TextLayoutBlock {
	private:
		typedef	TextLayoutBlock	inherited;

	public:
		TextLayoutBlock_Copy (const TextLayoutBlock& from);
		TextLayoutBlock_Copy (const TextLayoutBlock_Copy& from);

	public:
		override	void	PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const;
		override	void	PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const;

	public:
		override	vector<ScriptRunElt>	GetScriptRuns () const;

	private:
		struct	BlockRep {
			nonvirtual	void	operator delete (void* p);

			size_t				fTextLength;
			const Led_tChar*	fRealText;
			const Led_tChar*	fVirtualText;
			const ScriptRunElt*	fScriptRuns;
			const ScriptRunElt*	fScriptRunsEnd;
			// and we allocate extra space off the end of this object for the acutal data...
		};
		Led_RefCntPtr<BlockRep>	fRep;
};






/*
@CLASS:			TextLayoutBlock_VirtualSubset
@DESCRIPTION:	<p>NB: Saves a reference to its CTOR argument- 'subsetOf' so this must be destroyed AFTER
			the original.</p>
				<p>It is not obvious if this code should may a COPY of the 'real text' or refer back to the same original text with its original
			offsets. However - the problem with doing so is that since we've picked a contigous range of the virtual space, that does
			not coorespond to a contiguous space in the real space necessarily - except for the entire original buffer. That wouldn't be
			the same size - however - as the virtual buffer. Our API assumes these two spaces are the same size. So - the best compromise
			appears to be to copy the source text into a new buffer with new offsets that range from 0 to max-length and have nothing todo with
			the original buffer offsets.
			</p>
*/
class	TextLayoutBlock_VirtualSubset : public TextLayoutBlock {
	private:
		typedef	TextLayoutBlock	inherited;

	public:
		TextLayoutBlock_VirtualSubset (const TextLayoutBlock& subsetOf, size_t vStart, size_t vEnd);

	public:
		override	void	PeekAtRealText_ (const Led_tChar** startText, const Led_tChar** endText) const;
		override	void	PeekAtVirtualText_ (const Led_tChar** startText, const Led_tChar** endText) const;

	public:
		override	vector<ScriptRunElt>	GetScriptRuns () const;

	private:
		const TextLayoutBlock&			fSubsetOf;
		size_t							fStart;
		size_t							fEnd;
		vector<ScriptRunElt>			fScriptRuns;
		Led_SmallStackBuffer<Led_tChar>	fRealText;
};








/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	TextLayoutBlock;
	inline	const Led_tChar*	TextLayoutBlock::PeekAtRealText () const
		{
			const	Led_tChar*	s	=	NULL;
			const	Led_tChar*	e	=	NULL;
			PeekAtRealText_ (&s, &e);
			return s;
		}
	inline	const Led_tChar*	TextLayoutBlock::PeekAtVirtualText () const
		{
			const	Led_tChar*	s	=	NULL;
			const	Led_tChar*	e	=	NULL;
			PeekAtVirtualText_ (&s, &e);
			return s;
		}
	inline	size_t				TextLayoutBlock::GetTextLength () const
		{
			const	Led_tChar*	s	=	NULL;
			const	Led_tChar*	e	=	NULL;
			PeekAtVirtualText_ (&s, &e);
			return e-s;
		}
	inline	void	TextLayoutBlock::Invariant () const
		{
			#if		qDebug
					Invariant_ ();
			#endif
		}




// class	TextLayoutBlock::ScriptRunElt
	inline	bool TextLayoutBlock::ScriptRunElt::operator== (const ScriptRunElt& rhs) const
		{
			return (fDirection == rhs.fDirection and
					fRealStart == rhs.fRealStart and
					fRealEnd == rhs.fRealEnd and
					fVirtualStart == rhs.fVirtualStart and
					fVirtualEnd == rhs.fVirtualEnd
				);
		}
	inline	bool TextLayoutBlock::ScriptRunElt::operator!= (const ScriptRunElt& rhs) const
		{
			return not (*this == rhs);
		}



#if		qLedUsesNamespaces
}
#endif



#endif	/*__BiDiLayout_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
