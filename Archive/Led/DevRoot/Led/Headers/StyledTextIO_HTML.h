/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextIO_HTML_h__
#define	__StyledTextIO_HTML_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextIO_HTML.h,v 2.22 2003/05/27 23:10:07 lewis Exp $
 */


/*
@MODULE:	StyledTextIO_HTML
@DESCRIPTION:
		<p>Subclasses of @'StyledTextIOReader' and @'StyledTextIOWriter' are where the knowledge of particular file formats resides.
	For example, the knowledge of how to read HTML is in @'StyledTextIOReader_HTML' and the knowledge of how to write HTML is in
	@'StyledTextIOWriter_HTML'.</p>
 */

/*
 * Changes:
 *	$Log: StyledTextIO_HTML.h,v $
 *	Revision 2.22  2003/05/27 23:10:07  lewis
 *	SPR#1506: improved fLastCharSpace handling. SPR#1505: fixed ParseHTMLTagArgOut code (now much better). SPR#1504: big change to use <span style=> to replace <font> tag usage (both reader and writer, but left in old <font> tag reader code for back compat). SPR#0574: added beginnings of CSS support.
 *	
 *	Revision 2.21  2003/05/23 16:41:51  lewis
 *	SPR#1141: got preliminary table reading/writing working for HTML. Doesn't yet support any attributes to tags - but gets the right number of cells and thier contents decently written
 *	
 *	Revision 2.20  2002/05/06 21:33:35  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.19  2001/11/27 00:29:45  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.18  2001/10/17 20:42:53  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.17  2001/08/29 23:00:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.16  2001/08/28 18:43:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  2001/07/16 20:07:55  lewis
 *	SPR#0954- support UL/LI lists in reading/writing HTML (using new partial list support in Led)
 *	
 *	Revision 2.14  2001/05/29 20:21:41  lewis
 *	SPR#0941- fixup some more corner cases on new HTML font writing code. Bad optimization
 *	on stripping out open font tags before a close paragraph. Now fixed
 *	
 *	Revision 2.13  2001/05/09 19:33:41  lewis
 *	SPR#0910- MANY big changes to HTML writer - maintaining a stack of tags. Point its to get
 *	right nesting semantics required for XML. Also - bugs with the current HTML reader require
 *	those nesting semantics anyhow. Also - fixed color names to be lowercase. And - fixed to write
 *	out some colors by name (same as ones we recognize on reading).
 *	
 *	Revision 2.12  2001/02/05 22:28:51  lewis
 *	as part of SPR#0853- ALSO fix <br> / softline break support
 *	
 *	Revision 2.11  2001/02/05 17:18:42  lewis
 *	SPR#0853. Original request was to support align=center/etc in <p> tags. I DID fix this. But
 *	also had to substantially rewrite the emitting of NLs and the reading of <p> codes. Now these
 *	work much better - I believe (more true to the HTML spec). AND they support justification.
 *	
 *	Revision 2.10  2001/01/20 01:14:29  lewis
 *	cleanups. SPR#0852- fixup so works a bit better on Non-UNICODE mac
 *	
 *	Revision 2.9  2001/01/19 22:44:21  lewis
 *	SPR#0852- improve UNICODE HTML support. Handle entity refs properly - both named and numeric.
 *	Still must test/fixup a bit on mac maybe?
 *	
 *	Revision 2.8  2001/01/03 14:58:08  Lewis
 *	make sure forward delcalre of typedef is public - even if methods taht use it arent' - cuz Borland
 *	compiler seems to check typedefs name accessibility. Not a real serious problem - nor clearly a borland bug.
 *	
 *	Revision 2.7  2000/08/29 15:10:42  lewis
 *	SPR#0819- support HTML writer writing out AHREF= tags for StandardURLStyleMarker classes
 *	
 *	Revision 2.6  2000/08/29 13:35:38  lewis
 *	SPR#0819- handle reading '<a href=foo>bar</a>' properly - creating a URL object with that
 *	'bar' text as the enclosed text.
 *	
 *	Revision 2.5  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.4  2000/04/14 22:40:22  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.3  2000/02/02 17:14:03  lewis
 *	SPR#0707- add (primitive) HTML <DIV align=> support
 *	
 *	Revision 2.2  1999/12/18 15:38:48  lewis
 *	added support for <SUB><SUP>(SPR#0663), and wrap fontname in quotes
 *	
 *	Revision 2.1  1999/12/18 03:56:46  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *
 *
 *	<========== CODE MOVED HERE FROM StyledTextIO.cpp ==========>
 *
 *
 *
 *
 *
 */

#include	"StyledTextIO.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif


/*
@CONFIGVAR:		qThrowAwayMostUnknownHTMLTags
@DESCRIPTION:	<p>By default, on reading, we throw away or interpret losely most tags. This makes the reader
			very lossy, but produces the most human-readable result. For now control which behavior
			you want via this compiletime flag. -- LGP 961015</p>
 */
#ifndef	qThrowAwayMostUnknownHTMLTags
#define	qThrowAwayMostUnknownHTMLTags	1
#endif



/*
@CONFIGVAR:		qWriteOutMostHTMLEntitiesByName
@DESCRIPTION:	<p>By default, off - cuz that works more compatably with many older web browser (such as Netscape 4.x).
			And writing them by number is slightly faster.</p>
 */
#ifndef	qWriteOutMostHTMLEntitiesByName
#define	qWriteOutMostHTMLEntitiesByName	0
#endif




/*
@CLASS:			HTMLInfo
@DESCRIPTION:
*/
class	HTMLInfo {
	public:
		HTMLInfo ();

	public:
		struct	EntityRefMapEntry {
			EntityRefMapEntry (const string& entityRefName, wchar_t charValue);

			string	fEntityRefName;
			wchar_t	fCharValue;

			STLDefCTORDeclare_BWA (EntityRefMapEntry)
		};

		static	EntityRefMapEntry	sDefaultEntityRefMapTable[];
		static	const size_t		kDefaultEntityRefMapTable_Count;


	// HTMLFontSizes are numbers from 1..7, and the default/normal is 3.
	// Eventually, these could become virtual methods, and be hooked into stylesheets.
	public:
		static	int	HTMLFontSizeToRealFontSize (int size);
		static	int	RealFontSizeToHTMLFontSize (int size);

	public:
		string			fDocTypeTag;
		string			fHTMLTag;
		string			fHeadTag;
		string			fStartBodyTag;
		vector<string>	fUnknownHeaderTags;
		Led_tString		fTitle;
};


/*
@CLASS:			StyledTextIOReader_HTML
@BASES:			@'StyledTextIOReader'
@DESCRIPTION:
*/
class	StyledTextIOReader_HTML : public StyledTextIOReader {
	public:
		StyledTextIOReader_HTML (SrcStream* srcStream, SinkStream* sinkStream, HTMLInfo* saveHTMLInfoInto = NULL);

	public:
		override	void	Read ();
		override	bool	QuickLookAppearsToBeRightFormat ();

	public:
		typedef	HTMLInfo::EntityRefMapEntry	EntityRefMapEntry;
	protected:
		virtual	const vector<EntityRefMapEntry>&	GetEntityRefMapTable () const;

	protected:
		enum ThingyType { eEntityRef, eTag, eEOF, eBangComment };
		nonvirtual	ThingyType	ScanTilNextHTMLThingy ();
		nonvirtual	void		ScanTilAfterHTMLThingy (ThingyType thingy);
	
	protected:
		nonvirtual	bool	LookingAt (const char* text) const;

	protected:
		nonvirtual	Led_tString	MapInputTextToTString (const string& text);

	protected:
		nonvirtual	void	EmitText (const Led_tString& text, bool skipNLCheck = false);
		virtual		void	EmitText (const Led_tChar* text, size_t nBytes, bool skipNLCheck = false);
		nonvirtual	void	HandleHTMLThingy (ThingyType thingy, const string& text);
		virtual		void	HandleHTMLThingy (ThingyType thingy, const char* text, size_t nBytes);

	protected:
		virtual		void	HandleHTMLThingy_EntityReference (const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingy_Tag (const char* text, size_t nBytes);
	protected:
		nonvirtual	void	ExtractHTMLTagIntoTagNameBuf (const char* text, size_t nBytes, char* tagBuf, size_t tagBufSize, bool* isStartTag);

	protected:
		nonvirtual	Led_IncrementalFontSpecification	ExtractFontSpecFromCSSStyleAttribute (const char* text, size_t nBytes);
		nonvirtual	void								ApplyCSSStyleAttributeToCurrentFontStack (const char* text, size_t nBytes);
		nonvirtual	void								GrabAndApplyCSSStyleFromTagText (const char* text, size_t nBytes);

	protected:
		virtual		void	HandleHTMLThingyTag_BANG_doctype (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_a (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_b (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_basefont (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_big (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_blockquote (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_br (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_body (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_code (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_comment (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_dir (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_div (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_em (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_font (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_head (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_html (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_hr (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_hN (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_i (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_img (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_li (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_listing (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_ol (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_p (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_plaintext (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_pre (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_s (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_samp (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_small (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_span (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_strike (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_strong (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_sub (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_sup (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_table (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_td (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_th (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_title (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_tr (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_tt (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_u (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_ul (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_var (bool start, const char* text, size_t nBytes);
		virtual		void	HandleHTMLThingyTag_xmp (bool start, const char* text, size_t nBytes);

		virtual		void	HandleHTMLThingyTagUnknown (bool start, const char* text, size_t nBytes);

	protected:
		nonvirtual	void	BasicFontStackOperation (bool start);
		nonvirtual	void	EmitForcedLineBreak ();
	protected:
		nonvirtual	bool	ParseHTMLTagArgOut (const string& tagText, const string& attrName, string* attrValue);
		nonvirtual	bool	ParseCSSTagArgOut (const string& text, const string& attrName, string* attrValue);

	protected:
		nonvirtual	void	StartPara ();
		nonvirtual	void	EndParaIfOpen ();
	private:
		bool	fInAPara;

	// Implement the quirky font/size rules (1..7) HTML prescribes
	protected:
		virtual	void	SetHTMLFontSize (int to);
	protected:
		int		fHTMLBaseFontSize;
		int		fHTMLFontSize;

	protected:
		HTMLInfo*						fSaveHTMLInfoInto;
		bool							fReadingBody;
		vector<Led_FontSpecification>	fFontStack;
		bool							fComingTextIsTitle;
		bool							fNormalizeInputWhitespace;
		bool							fLastCharSpace;
		bool							fHiddenTextMode;
		Led_tString						fHiddenTextAccumulation;
		size_t							fCurAHRefStart;
		string							fCurAHRefText;
		unsigned int					fULNestingCount;
		bool							fLIOpen;
		unsigned int					fTableOpenCount;
		bool							fTableRowOpen;
		bool							fTableCellOpen;
};


/*
@CLASS:			StyledTextIOWriter_HTML
@BASES:			@'StyledTextIOWriter'
@DESCRIPTION:
*/
class	StyledTextIOWriter_HTML : public StyledTextIOWriter {
	public:
		StyledTextIOWriter_HTML (SrcStream* srcStream, SinkStream* sinkStream, const HTMLInfo* getHTMLInfoFrom = NULL);
		~StyledTextIOWriter_HTML ();

	public:
		override	void	Write ();

	public:
		typedef	HTMLInfo::EntityRefMapEntry	EntityRefMapEntry;
	protected:
		virtual	const vector<EntityRefMapEntry>&	GetEntityRefMapTable () const;

	public:
		typedef	StyledTextIOWriter::SrcStream::Table	Table;

	protected:
		class	WriterContext;

	protected:
		nonvirtual	void	WriteHeader (WriterContext& /*writerContext*/);
		nonvirtual	void	WriteBody (WriterContext& writerContext);
		nonvirtual	void	WriteInnerBody (WriterContext& writerContext);
		nonvirtual	void	WriteBodyCharacter (WriterContext& writerContext, Led_tChar c);
		nonvirtual	void	WriteTable (WriterContext& writerContext, Table* table);
		nonvirtual	void	WriteOpenTag (WriterContext& writerContext, const string& tagName, const string& tagExtras = string ());
		nonvirtual	void	WriteOpenTagSpecial (WriterContext& writerContext, const string& tagName, const string& tagFullText);
		nonvirtual	void	WriteCloseTag (WriterContext& writerContext, const string& tagName);
		nonvirtual	void	WriteOpenCloseTag (WriterContext& writerContext, const string& tagName, const string& tagExtras = string ());
		nonvirtual	bool	IsTagOnStack (WriterContext& writerContext, const string& tagName);
		nonvirtual	void	EmitBodyFontInfoChange (WriterContext& writerContext, const Led_FontSpecification& newOne, bool skipDoingOpenTags);
		nonvirtual	void	AssureStyleRunSummaryBuilt (WriterContext& writerContext);
		nonvirtual	string	MapOutputTextFromWString (const wstring& text);
		nonvirtual	string	MapOutputTextFromTString (const Led_tString& text);

	protected:
		const HTMLInfo*											fGetHTMLInfoFrom;
		vector<StandardStyledTextImager::InfoSummaryRecord>		fStyleRunSummary;
		Led_tChar												fSoftLineBreakChar;
};



class	StyledTextIOWriter_HTML::WriterContext {
	public:
		WriterContext (StyledTextIOWriter_HTML& writer);
		WriterContext (WriterContext& parentContext, SrcStream& srcStream);

	public:
		nonvirtual	StyledTextIOWriter_HTML&		GetWriter () const;
	private:
		StyledTextIOWriter_HTML&		fWriter;

	public:
		nonvirtual	SrcStream&	GetSrcStream () const;
		nonvirtual	SinkStream&	GetSinkStream () const;

	private:
		StyledTextIOWriter::SrcStream&				fSrcStream;
	public:
		StandardStyledTextImager::InfoSummaryRecord	fLastEmittedISR;
		size_t										fLastStyleChangeAt;
		size_t										fIthStyleRun;
		size_t										fLastForcedNLAt;
		bool										fEmittedStartOfPara;
		bool										fEmittingList;
		bool										fEmittingListItem;
		vector<string>								fTagStack;
		bool										fInTableCell;

	public:
		nonvirtual	size_t								GetCurSrcOffset () const;
		nonvirtual	SimpleEmbeddedObjectStyleMarker*	GetCurSimpleEmbeddedObjectStyleMarker () const;
};










/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	HTMLInfo::EntityRefMapEntry
	inline	HTMLInfo::EntityRefMapEntry::EntityRefMapEntry (const string& entityRefName, wchar_t charValue):
			fEntityRefName (entityRefName),
			fCharValue (charValue)
		{
		}



//	class	StyledTextIOReader_HTML
	inline	void	StyledTextIOReader_HTML::EmitText (const Led_tString& text, bool skipNLCheck)
		{
			EmitText (text.c_str (), text.length (), skipNLCheck);
		}
	inline	void	StyledTextIOReader_HTML::HandleHTMLThingy (StyledTextIOReader_HTML::ThingyType thingy, const string& text)
		{
			HandleHTMLThingy (thingy, text.c_str (), text.length ());
		}





//	class	StyledTextIOWriter_HTML::WriterContext
	inline	StyledTextIOWriter_HTML::WriterContext::WriterContext (StyledTextIOWriter_HTML& writer):
		fWriter (writer),
		fSrcStream (fWriter.GetSrcStream ()),
		fLastEmittedISR (Led_IncrementalFontSpecification (), 0),
		fLastStyleChangeAt (0),
		fIthStyleRun (0),
		fLastForcedNLAt (0),
		fEmittedStartOfPara (false),
		fEmittingList (false),
		fEmittingListItem (false),
		fTagStack (),
		fInTableCell (false)
		{
		}
	inline	StyledTextIOWriter_HTML::WriterContext::WriterContext (WriterContext& parentContext, SrcStream& srcStream):
		fWriter (parentContext.fWriter),
		fSrcStream (srcStream),
		fLastEmittedISR (Led_IncrementalFontSpecification (), 0),
		fLastStyleChangeAt (0),
		fIthStyleRun (0),
		fLastForcedNLAt (0),
		fEmittedStartOfPara (false),
		fEmittingList (false),
		fEmittingListItem (false),
		fTagStack (),
		fInTableCell (true)
		{
		}
	inline	StyledTextIOWriter_HTML&	StyledTextIOWriter_HTML::WriterContext::GetWriter () const
		{
			return fWriter;
		}
	inline	StyledTextIOWriter::SrcStream&	StyledTextIOWriter_HTML::WriterContext::GetSrcStream () const
		{
			return fSrcStream;
		}
	inline	StyledTextIOWriter::SinkStream&	StyledTextIOWriter_HTML::WriterContext::GetSinkStream () const
		{
			return fWriter.GetSinkStream ();
		}


#if		qLedUsesNamespaces
}
#endif



#endif	/*__StyledTextIO_HTML_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
