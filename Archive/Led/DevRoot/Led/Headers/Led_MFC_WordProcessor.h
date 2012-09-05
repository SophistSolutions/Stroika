/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_MFC_WordProcessor_h__
#define	__Led_MFC_WordProcessor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_MFC_WordProcessor.h,v 2.36 2003/12/31 16:52:30 lewis Exp $
 */


/*
@MODULE:	Led_MFC_WordProcessor
@DESCRIPTION:
		<p>Some stuff which applies only to use of @'Led_MFC' along with @'WordProcessor'
	classes (e.g. OLE embeddings)</p>.
 */



/*
 * Changes:
 *	$Log: Led_MFC_WordProcessor.h,v $
 *	Revision 2.36  2003/12/31 16:52:30  lewis
 *	change DoWriteToOLE1Stream arg to Byte** from void** so we can avoid a cast before doing delete (and avoid warning of illegal delete of void*)
 *	
 *	Revision 2.35  2003/05/08 00:07:18  lewis
 *	SPR#1467: added kSelectTableIntraCellAll_CmdID
 *	
 *	Revision 2.34  2003/05/07 21:10:59  lewis
 *	SPR#1467: implemented various select commands (word/row/paragraph/table/cell/row/col)
 *	
 *	Revision 2.33  2003/04/15 22:17:04  lewis
 *	SPR#1425: added kRemoveTableRows_CmdID/kRemoveTableColumns_CmdID support
 *	
 *	Revision 2.32  2003/04/04 19:53:14  lewis
 *	SPR#1407: cleanup new command handling code
 *	
 *	Revision 2.31  2003/04/04 14:41:41  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.30  2003/04/03 16:41:22  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated
 *	command classes, just builtin to TextInteractor/WordProcessor (and instead of template params
 *	use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.29  2003/04/01 18:17:57  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment ()
 *	and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.28  2003/01/29 19:15:07  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.27  2003/01/21 13:26:54  lewis
 *	SPR#1186 - first cut at Indents dialog support (MacOS dialog still not done)
 *	
 *	Revision 2.26  2003/01/11 19:28:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.25  2002/12/16 20:31:29  lewis
 *	SPR#1209- fix arg to ControlItemContextInternalizer::CTOR
 *	
 *	Revision 2.24  2002/11/27 15:58:21  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.23  2002/11/19 19:51:28  lewis
 *	SPR#1176 - Added insert row/col before/after commands and a few minor table fixups which were
 *	releated (not 100% - see spr#1177/78 for details)
 *	
 *	Revision 2.22  2002/10/25 17:16:09  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item and fix Led_MFC D&D code so it worked with CharacterMap.exe
 *	(for some wierd reason - I need to hack it to pass along the full FORMATC???).
 *	
 *	Revision 2.21  2002/10/22 00:38:45  lewis
 *	Add URL context/menu command - SPR#1136
 *	
 *	Revision 2.20  2002/09/11 04:20:15  lewis
 *	SPR#1094- added VERY preliminary Table support - table class and new InsertTable command
 *	
 *	Revision 2.19  2002/05/06 21:33:29  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.18  2001/11/27 00:29:41  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.17  2001/10/20 13:38:55  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.16  2001/10/17 20:42:50  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.15  2001/09/12 17:23:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.14  2001/08/31 21:04:35  lewis
 *	SPR#0995- Skip WordProcessorCommonCommandHelper_MFC<BASECLASS,CMD_INFO,CMD_ENABLER>::OnPaint ()
 *	handling when reading in OLE objects (but re-inval the area). Speed tweek.
 *	
 *	Revision 2.13  2001/08/29 23:00:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2001/08/28 18:43:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.11  2001/08/16 18:53:41  lewis
 *	SPR#0959- more work on 'GetInfo' support for embeddings.
 *	
 *	Revision 2.10  2001/07/17 19:09:36  lewis
 *	SPR#0959- added preliminary RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 2.9  2001/07/11 21:51:01  lewis
 *	SPR#0906- Partly implemented bullet/list support. Just did basic support (bullet lists,
 *	no other style lists). Did support indent levels (including new Increase/Decrease indent
 *	level commands). Still no RTF/HTML support, and a small display bug on following lines
 *	when I add bullet attribute.
 *	
 *	Revision 2.8  2000/10/03 13:36:35  lewis
 *	SPR#0839- reorganize the WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> code.
 *	Now MFC wrapper support MUCH simpler, and all this stuff much easier to use from other platforms.
 *	OnPerformCommand/OnUpdateCommand simple virtual overrides, and _MSG simple single msg hooks for MFC.
 *	
 *	Revision 2.7  2000/09/30 19:35:01  lewis
 *	Added TextInteractorCommandHelper_DefaultCmdInfo/TextInteractorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>.
 *	Added TextInteractor::OnSelectAllCommand. Moved Led_MFC_TmpCmdUpdater to Led_MFC.h
 *	
 *	Revision 2.6  2000/08/28 20:12:07  lewis
 *	Add OnUpdateHideSelectionCommands/OnHideSelection/OnUnHideSelection support
 *	
 *	Revision 2.5  2000/06/13 22:13:55  lewis
 *	SPR#0785- map CMD_INFO::kParagraphSpacingCommand_CmdID to OnParagraphSpacingChangeCommand
 *	
 *	Revision 2.4  2000/06/13 16:26:14  lewis
 *	SPR#0767 - last tidbit - unknown embeddings - and RTFOLE embeddings - must use Led_TWIPS_Point
 *	
 *	Revision 2.3  2000/04/26 13:32:52  lewis
 *	fix DocContextDefiner to allow nested calls (as happens with sub internalize call with hidden text code
 *	
 *	Revision 2.2  2000/04/24 19:44:35  lewis
 *	SPR#0744- overide MakeDefaultInternalizer () to create one that will build a Led_MFC_ControlItem::DocContextDefiner
 *	on the stack so internalizing OLE emebddings works right now
 *	
 *	Revision 2.1  2000/04/24 17:29:21  lewis
 *	SPR#0743- added new Led_MFC_WordProcessor module - and broke some stuff from Led_MFC out and into it
 *	
 *
 *
 *
 *	<========== CODE based on Led_MFC.h - moved some code here ==========>
 *
 */


#if		_MSC_VER == 1200
	//A bit of a hack for MSVC60, cuz this needs to be done before including <set> - otherwise we get
	// lots of needless warnigns - regardless of what is done later -- LGP 980925
	#pragma	warning (4 : 4786)
#endif

#include	<afxwin.h>
#include	<afxole.h>
#include	<oleidl.h>

#include	<set>

#include	"Led_MFC.h"
#include	"StyledTextIO_RTF.h"
#include	"WordProcessor.h"



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)
#endif



#if		qLedUsesNamespaces
namespace	Led {
#endif








/*
 **************** MFC/WordProcessor specific Configuration variables **************
 */


/*
@CONFIGVAR:		qSupportOLEControlEmbedding
@DESCRIPTION:	<p>You may want to shut this off for building an OLE control itself. Or for building applications that
	don't want to include the OLE libaries.</p>
		<p>Turn ON by default
 */
#ifndef	qSupportOLEControlEmbedding
#define	qSupportOLEControlEmbedding								1
#endif










#if		qSupportOLEControlEmbedding
class	Led_MFC_ControlItem;
#endif






/*
@CLASS:			WordProcessorCommonCommandHelper_MFC<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>BASECLASS is generally @'WordProcessor'.
			</p>
				<p>To use this class, you must also
			define DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap() to declare the
			actual message map for the template.</p>
*/
template	<typename	BASECLASS>	class	WordProcessorCommonCommandHelper_MFC :
	public BASECLASS
{
	private:
		typedef	BASECLASS	inherited;

	public:
		WordProcessorCommonCommandHelper_MFC ();


#if		qSupportOLEControlEmbedding
	protected:
		afx_msg	void	OnPaint ();

	protected:
		/*
		@CLASS:			WordProcessorCommonCommandHelper_MFC<BASECLASS>::ControlItemContextInternalizer
		@BASES:			@'WordProcessor::WordProcessorFlavorPackageInternalizer'
		@DESCRIPTION:	<p>Add call to @'Led_MFC_ControlItem::DocContextDefiner' so that we can instantiate OLE embeddings using the
			MFC mechanism.</p>
				<p>Note - as of Led 3.0d6, and MSVC60SP3, we had to declare this here cuz I couldn't get it compiling when nested locally
			within the MakeDefaultInternalizer () function - which would probably have been better.</p>
		*/
		class	ControlItemContextInternalizer : public WordProcessor::WordProcessorFlavorPackageInternalizer {
			private:
				typedef	WordProcessor::WordProcessorFlavorPackageInternalizer	inherited;
			public:
				ControlItemContextInternalizer (COleDocument* doc, 
						TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
						const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
						const WordProcessor::HidableTextDatabasePtr& hiddenTextDatabase
				):
				FlavorPackageInternalizer (ts),
				inherited (ts, styleDatabase, paragraphDatabase, hiddenTextDatabase),
				fDocument (doc)
					{
					}
			public:
				override	bool	InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
					{
						Led_MFC_ControlItem::DocContextDefiner tmp (fDocument);
						return inherited::InternalizeBestFlavor (flavorPackage, from, to);
					}
			protected:
				COleDocument*	fDocument;
		};
	protected:
		override	Led_RefCntPtr<FlavorPackageInternalizer>	MakeDefaultInternalizer ();
#endif
	public:
		override		Led_SDK_String		GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m);

	protected:
		DECLARE_MESSAGE_MAP ()

	protected:
		#if		qSupportOLEControlEmbedding
			friend	class	Led_MFC_ControlItem;
		#endif
};







#if		qSupportOLEControlEmbedding
/*
@CLASS:			Led_MFC_ControlItem
@BASES:			COleClientItem, @'SimpleEmbeddedObjectStyleMarker'
@DESCRIPTION:
		<p>A Led embedding object for embedded OLE controls. Useful for applications which
	want to allow for embedded OLE objects. (these docs could really use embellishing!).</p>
		<p>NB: This also supports the embedding format @'RTFIO::RTFOLEEmbedding::kEmbeddingTag'.</p>
*/
	class	Led_MFC_ControlItem : public COleClientItem, public SimpleEmbeddedObjectStyleMarker, public RTFIO::RTFOLEEmbedding	{
		public:
			static	const	Led_ClipFormat			kClipFormat;
			static	const	Led_PrivateEmbeddingTag	kEmbeddingTag;

			// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
			//  IMPLEMENT_SERIALIZE requires the class have a constructor with
			//  zero arguments.  Normally, OLE items are constructed with a
			//  non-NULL document pointer.
			Led_MFC_ControlItem (COleDocument* pContainer = NULL);
			~Led_MFC_ControlItem ();

		public:
			struct	DocContextDefiner;
			static	SimpleEmbeddedObjectStyleMarker*	mkLed_MFC_ControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len);
			static	SimpleEmbeddedObjectStyleMarker*	mkLed_MFC_ControlItemStyleMarker (ReaderFlavorPackage& flavorPackage);
		protected:
			static	SimpleEmbeddedObjectStyleMarker*	mkLed_MFC_ControlItemStyleMarker_ (const char* embeddingTag, const void* data, size_t len, Led_MFC_ControlItem* builtItem);
			static	SimpleEmbeddedObjectStyleMarker*	mkLed_MFC_ControlItemStyleMarker_ (ReaderFlavorPackage& flavorPackage, Led_MFC_ControlItem* builtItem);

		public:
			nonvirtual	COleDocument&	GetDocument () const;
			nonvirtual	Led_MFC&		GetActiveView () const;
		
		public:
			override	void	OnChange (OLE_NOTIFICATION wNotification, DWORD dwParam);
			override	void	OnActivate ();
			override	BOOL	DoVerb (LONG nVerb, CView* pView, LPMSG lpMsg = NULL);

		protected:
			override	void	OnGetItemPosition (CRect& rPosition);
			override	void	OnDeactivateUI (BOOL bUndoable);
			override	BOOL	OnChangeItemPosition (const CRect& rectPos);
			
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
			override	void			DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo);
			override	bool			HandleOpen ();

		public:
			override	vector<PrivateCmdNumber>	GetCmdNumbers () const;
			override	bool						IsCmdEnabled (PrivateCmdNumber cmd) const;

		private:
			Led_Size	fSize;

		// Support RTFIO::RTFOLEEmbedding API
		public:
			override	void			PostCreateSpecifyExtraInfo (Led_TWIPS_Point size);
			override	Led_SDK_String	GetObjClassName ()	const;
			override	void			DoWriteToOLE1Stream (size_t* nBytes, Byte** resultData);
			override	Led_Size		GetSize ();

		public:
			override	const char*		GetTag () const;
			override	void			Write (SinkStream& sink);
			override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

			override	void	Serialize (CArchive& ar);

		private:
			DECLARE_SERIAL(Led_MFC_ControlItem)
	};



	/*
	 *	Because this mkLed_MFC_ControlItemStyleMarker () gets called from so many levels
	 *	removed from anything that knows about MFC/COleDocuments, and since building
	 *	control items really requires we know the document (before we can call
	 *	serialize), we need some sort of hack to communicate this info from that code
	 *	that knows, to this produdure.
	 *
	 *	The hack chosen, is the venerable global variable. But we do this hack with
	 *	a little but of control/circumspection. We use a tmp stack-based class which defines
	 *	a context, and methods which might indirectly call this must create one of these
	 *	little objects on the stack which will have the side-effect of setting/restoring
	 *	the global variable. We don't support nested calls (cuz no need, would be easy).
	 */
	struct	Led_MFC_ControlItem::DocContextDefiner {
		public:
			DocContextDefiner (COleDocument* doc);
			~DocContextDefiner ();
		public:
			static	COleDocument*	GetDoc ();
		private:
			static	COleDocument*	sDoc;
		private:
			COleDocument*	fOldDoc;
		public:
			static	set<HWND>	sWindowsWhichHadDisplaySupressed;
	};
#endif









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	WordProcessorCommonCommandHelper_MFC<BASECLASS>
	#define	DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap(BASECLASS)\
		typedef	WordProcessorCommonCommandHelper_MFC<BASECLASS>	WordProcessorCommonCommandHelper_MFC ## BASECLASS ## _HackTypeDef;\
		BEGIN_MESSAGE_MAP(WordProcessorCommonCommandHelper_MFC ## BASECLASS ## _HackTypeDef, WordProcessorCommonCommandHelper_MFC ## BASECLASS ## _HackTypeDef::inherited)\
			ON_WM_PAINT()\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kSelectTableIntraCellAll_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kSelectTableCell_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kSelectTableRow_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kSelectTableColumn_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kSelectTable_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kFontStylePlain_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kFontStyleBold_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kFontStyleItalic_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kFontStyleUnderline_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kFontStyleStrikeout_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kSubScriptCommand_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kSuperScriptCommand_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kChooseFontCommand_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kBaseFontSize_CmdID,				WordProcessor::kLastFontSize_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kFontMenuFirst_CmdID,				WordProcessor::kFontMenuLast_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kBaseFontColor_CmdID,				WordProcessor::kLastFontColor_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kHideSelection_CmdID,				WordProcessor::kUnHideSelection_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kFirstJustification_CmdID,			WordProcessor::kLastJustification_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kParagraphSpacingCommand_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kParagraphIndentsCommand_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kFirstListStyle_CmdID,				WordProcessor::kLastListStyle_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kIncreaseIndent_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kDecreaseIndent_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kFirstShowHideGlyph_CmdID,			WordProcessor::kLastShowHideGlyph_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kInsertTable_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kInsertTableRowAbove_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kInsertTableRowBelow_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kInsertTableColBefore_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kInsertTableColAfter_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kRemoveTableRows_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kRemoveTableColumns_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kInsertURL_CmdID)\
			LED_MFC_HANDLE_COMMAND_M		(WordProcessor::kInsertSymbol_CmdID)\
			LED_MFC_HANDLE_COMMAND_RANGE_M	(WordProcessor::kFirstSelectedEmbedding_CmdID,		WordProcessor::kLastSelectedEmbedding_CmdID)\
		END_MESSAGE_MAP()

	#if		qSupportLed30CompatAPI
		// Obsolete because of SPR#1407 - just call DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap
		// directly...
		#define	DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMapX(BASECLASS,CMD_INFO)\
			DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap (BASECLASS)
	#endif

	template	<typename	BASECLASS>
		inline	WordProcessorCommonCommandHelper_MFC<BASECLASS>::WordProcessorCommonCommandHelper_MFC ():
				inherited ()
			{
			}
#if		qSupportOLEControlEmbedding
	template	<typename	BASECLASS>
		void	WordProcessorCommonCommandHelper_MFC<BASECLASS>::OnPaint ()
			{
				if (Led_MFC_ControlItem::DocContextDefiner::GetDoc () != NULL) {
					// supress display but create CPaintDC object to eat update event (and accumulate window to re-inval just below)
					CPaintDC	dc (this);
					Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySupressed.insert (GetHWND ());
					return;
				}
				inherited::OnPaint ();
			}
	template	<typename	BASECLASS>
		Led_RefCntPtr<FlavorPackageInternalizer>	WordProcessorCommonCommandHelper_MFC<BASECLASS>::MakeDefaultInternalizer ()
			{
				return new ControlItemContextInternalizer (dynamic_cast<COleDocument*> (GetDocument ()), GetTextStore (), GetStyleDatabase (), GetParagraphDatabase (), GetHidableTextDatabase ());
			}
#endif
	template	<typename	BASECLASS>
		Led_SDK_String		WordProcessorCommonCommandHelper_MFC<BASECLASS>::GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m)
			{
				#if		qSupportOLEControlEmbedding
				if (dynamic_cast<Led_MFC_ControlItem*> (m) != NULL) {
					return Led_SDK_TCHAROF ("OLE embedding");
				}
				#endif
				return inherited::GetPrettyTypeName (m);
			}






	#if		qSupportOLEControlEmbedding
//class	Led_MFC_ControlItem
		#if		!qDebug
		inline	COleDocument&	Led_MFC_ControlItem::GetDocument () const
			{
				// See debug version for ensures...
				return *(COleDocument*)COleClientItem::GetDocument ();
			}
		inline	Led_MFC&	Led_MFC_ControlItem::GetActiveView () const
			{
				// See debug version for ensures...
				return *(Led_MFC*)COleClientItem::GetActiveView ();
			}
		#endif


//class	Led_MFC_ControlItem::DocContextDefiner
		inline	Led_MFC_ControlItem::DocContextDefiner::DocContextDefiner (COleDocument* doc):
			fOldDoc (sDoc)
			{
				Led_RequireNotNil (doc);
				sDoc = doc;
			}
		inline	Led_MFC_ControlItem::DocContextDefiner::~DocContextDefiner ()
			{
				Led_AssertNotNil (sDoc);
				sDoc = fOldDoc;
			}
		inline	COleDocument*	Led_MFC_ControlItem::DocContextDefiner::GetDoc ()
			{
				return sDoc;
			}
	#endif


#if		qLedUsesNamespaces
}
#endif

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif


#endif	/*__Led_MFC_WordProcessor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
