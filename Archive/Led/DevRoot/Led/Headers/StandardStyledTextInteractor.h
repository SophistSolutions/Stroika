/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StandardStyledTextInteractor_h__
#define	__StandardStyledTextInteractor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StandardStyledTextInteractor.h,v 2.87 2003/11/26 19:55:01 lewis Exp $
 */

/*
@MODULE:	StandardStyledTextInteractor
@DESCRIPTION:
		<p>Combine the functionality of @'TextInteractor', and @'StandardStyledTextImager', and add a bunch of
	related functionality (which only makes sense when you have both of these features).</p>

 */


/*
 * Changes:
 *	$Log: StandardStyledTextInteractor.h,v $
 *	Revision 2.87  2003/11/26 19:55:01  lewis
 *	fix name of args from withWhatBytes->withWhatCharCount. OLDCODENOTE - change InteractiveReplaceEarlyPostReplaceHook () to return bool result (SPR#1564)
 *	
 *	Revision 2.86  2003/09/22 22:06:10  lewis
 *	rename kLedRTFClipType to ClipFormat (& likewise for LedPrivate/HTML). Add FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (and subclass override to check RTF/HTML etc file fomrats) when were given no good file suffix etc and must peek at file data to guess type. Allow NULL SinkStream to CTOR for a Reader - since we could be just calling QuickLookAppearsToBeRightFormat () which doesnt need an output stream. All for SPR#1552 - so we use same code in OpenDoc code as in when you drop a file on an open Led window (and so we handle read-only files etc better)
 *	
 *	Revision 2.85  2003/09/22 20:20:56  lewis
 *	for SPR#1552: add StyledTextFlavorPackageInternalizer::InternalizeFlavor_HTML.
 *	Lose StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes and
 *	replace with StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (and base 
 *	class code to call InternalizeBestFlavor with new suggestedClipType). Moved kRTFClipType from 
 *	FlavorPackage module to StandardStyledTextInteractor module - since not needed in FlavorPackage code.
 *	
 *	Revision 2.84  2003/05/20 22:56:26  lewis
 *	SPR#1493: InteractiveUndoHelperMakeTextRep and EmptySelStyleTextRep now take selStart/end
 *	args (and regionStart/end args for InteractiveUndoHelperMakeTextRep)
 *	
 *	Revision 2.83  2003/04/12 20:41:09  lewis
 *	Lose unneeded extra CTOR for StandardStyledTextIOSinkStream and some other old ifdefed out code
 *	
 *	Revision 2.82  2003/04/10 19:10:56  lewis
 *	made InteractiveSetFont virtual (needed for SPR#1329).
 *	
 *	Revision 2.81  2003/04/10 15:56:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.80  2003/03/25 15:49:05  lewis
 *	SPR#1375- added (back) StandardStyledTextInteractor::SetDefaultFont override - for very
 *	LIMITED purpose. Only set EmptySel (basically) if the current selection is empty. Helpful
 *	occasionally - like when user creates a NEW edit control and says 'set default font'. Makes
 *	for sensible default behavior.
 *	
 *	Revision 2.79  2003/03/21 13:59:37  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to
 *	TextInteractor::ProcessSimpleClick (adding extra arg). Also lots of other small
 *	code cleanups
 *	
 *	Revision 2.78  2003/03/14 14:38:50  lewis
 *	SPR#1343 - Get rid of funny code for setting anchor, and override StandardStyledTextInteractor::WhileSimpleMouseTracking
 *	to properly handle tracking over embeddings and fix the WordProcessor::WhileSimpleMouseTracking to
 *	do likewise (it was already correct except for cosmetics). Net effect is that clicking on embedding
 *	works correctly again on Win32 (was OK on ohter platforms) - and is now clearer on ALL
 *	
 *	Revision 2.77  2003/03/05 17:25:12  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.76  2003/01/29 17:59:48  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont
 *	REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the
 *	'default font'
 *	
 *	Revision 2.75  2003/01/11 19:28:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.74  2002/11/20 20:05:00  lewis
 *	workaround qUsingMemberNameToOverloadInWithUsingBug
 *	
 *	Revision 2.73  2002/11/19 17:45:53  lewis
 *	Added GetTextStore() method of StyledTextIOSinkStream to help with SPR#1175
 *	
 *	Revision 2.72  2002/11/19 12:36:56  lewis
 *	Added SetInsertionStart () accessor to IOSinkStream to support fix for SPR#1174
 *	
 *	Revision 2.71  2002/11/14 17:29:58  lewis
 *	use using declaration for SetSelection instead of explicit declare(may need old crap for
 *	back compat on some old compilers)
 *	
 *	Revision 2.70  2002/11/14 17:01:10  lewis
 *	SPR#1171- added SetEmptySelectionStyle () overload (with no args). Call from SetSelection_ and do what we used
 *	to do there. Then call in a number of other places in application code to get the seleciton grabbed
 *	
 *	Revision 2.69  2002/10/22 00:38:49  lewis
 *	Add URL context/menu command - SPR#1136
 *	
 *	Revision 2.68  2002/10/21 12:55:39  lewis
 *	SPR#1135 - Add SummarizeFontAndColorTable method to TextIO::SrcStream and use that to generate RTF
 *	color and font tables
 *	
 *	Revision 2.67  2002/10/07 23:22:39  lewis
 *	added GetStyleDatabase () wrapper
 *	
 *	Revision 2.66  2002/09/28 18:11:58  lewis
 *	SPR#1116 - progress on rtf writing for embedded tables
 *	
 *	Revision 2.65  2002/09/19 14:14:10  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.64  2002/09/17 16:18:14  lewis
 *	SPR#1095 (table support) - added Push/Pop context support in SinkStream to support redirecting contents
 *	to embedded table
 *	
 *	Revision 2.63  2002/05/06 21:33:34  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.62  2001/11/27 00:29:44  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.61  2001/10/20 13:38:56  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.60  2001/10/17 20:42:52  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.59  2001/09/12 17:23:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2001/09/05 23:42:52  lewis
 *	make StandardStyledTextIOSinkStream::GetCachedText () method available as helper to fix SPR#1014
 *	
 *	Revision 2.57  2001/09/04 21:45:58  lewis
 *	related (though not directly) to SPR#1011. Moved EmptySelStyleTextRep to be a nested class,
 *	rather than a private detail in .cpp file (so could be subclassed to fix SPR#1011).
 *	
 *	Revision 2.56  2001/08/29 23:00:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.55  2001/08/28 18:43:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.54  2001/07/20 20:49:25  lewis
 *	improve workaround for qRuntimeCrashMaybeCodeGenBugWithAppStartupBug- based
 *	on suggestion by Gordon Tilman in email dated 2001-07-20
 *	
 *	Revision 2.53  2001/07/19 23:43:40  lewis
 *	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug support
 *	
 *	Revision 2.52  2000/08/29 15:41:15  lewis
 *	SPR#0810- added qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor define
 *	
 *	Revision 2.51  2000/08/28 20:16:04  lewis
 *	SPR#0827- replace indivudal command-name setters with WordProcessor::SetCommandNames () taking struct
 *	
 *	Revision 2.50  2000/06/23 21:45:13  lewis
 *	SPR#0806- Hook InternalizeFlavor_FILEDataRawBytes () to handle reading in RTF files (d&d etc)
 *	
 *	Revision 2.49  2000/05/30 23:34:58  lewis
 *	moved more default (empty) implemtantions into StyledTextIOReader::SinkStream
 *	and StyledTextIOWriter::SrcStream so various subclasses are simpler - and so as
 *	we add methods to these bases - we only need todo overrides where we actually know
 *	about the extra data (simple code cleanup)
 *	
 *	Revision 2.48  2000/04/25 23:00:48  lewis
 *	more work on SPR#0731 and SPR#0732- must apply Hidden stuff at the end -
 *	not in the flush - and even THAT may not be late enough. And must re-call
 *	AssureColorTableBuilt_FillIn/FONTTABLEONE TOO for sub-call to reader - cuz that
 *	changes avialable colors/fonts (if diff ones used in hidden text than in the main body text)
 *	
 *	Revision 2.47  2000/04/25 16:16:29  lewis
 *	partial work on SPR#0722- added HidableTextMarkerOwner::GetHidableRegionsWithData ()
 *	and started using that.
 *	
 *	Revision 2.46  2000/04/24 16:47:02  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor -
 *	but simply owned using Led_RefCntPtr<>. Also - did all the proper subclassing in WP
 *	class etc to make this work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.45  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.44  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.43  2000/04/14 19:17:35  lewis
 *	SPR#0739- react to change where we move much TextIntearctor flavor package code
 *	into its own module, and reorg the updateCursorPos etc stuff into separate code
 *	from code specific to each flavor type
 *	
 *	Revision 2.42  2000/03/31 23:45:36  lewis
 *	SPR#0717- got first cut (imperfect) at saving hidableText in RTF (problem is assumes
 *	its currently being shown). Other cleanups - esp to hidable text code (invariants)
 *	
 *	Revision 2.41  2000/03/31 00:50:03  lewis
 *	more work on SPR#0717: support RTF reading of \v tags - hidden text. Integrated HiddenText
 *	support into WordProcessor class (in a maner patterend after how we treat WordProcessor::ParagraphDatabasePtr
 *	
 *	Revision 2.40  1999/12/27 16:03:59  lewis
 *	SPR#0667- SoftLineBreak support
 *	
 *	Revision 2.39  1999/12/09 03:22:37  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.38  1999/12/07 20:07:24  lewis
 *	make StandardStyledTextInteractor::GetContinuousStyleInfo const
 *	
 *	Revision 2.37  1999/11/13 22:33:01  lewis
 *	lose qSupportLed22CompatAPI support
 *	
 *	Revision 2.36  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.35  1999/07/02 14:46:09  lewis
 *	spr#0604- Lose StandardStyledTextInteractor::InteractiveReplace_ and replace with
 *	new StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook () which
 *	overrides new TextInteractor::InteractiveReplaceEarlyPostReplaceHook(). Also -
 *	had to add revised hack fEmptySelectionStyleSuppressMode to StandardStyledTextInteractor to make this fix work
 *	
 *	Revision 2.34  1999/05/03 22:04:59  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.33  1999/04/28 14:58:07  lewis
 *	Added new methods StandardStyledTextInteractor::Get/SetEmptySelectionStyle- and used
 *	within StandardStyledTextInteractor::SetDefaultFont to simplify (and was needed in
 *	LedItView::OnInitialUpdate() to fix bug where we didnt reset things properly on NEWDOCUMENT call
 *	
 *	Revision 2.32  1999/03/25 02:46:52  lewis
 *	add new method InteractiveSetFont - as a replacement for what people SHOULD be calling
 *	to get the undoable behavior on SetDefaultFont. Later - once people have had time to
 *	convert their calls - we will change the sematnics of SetDefaultFont
 *	
 *	Revision 2.31  1999/03/10 17:17:39  lewis
 *	as part of cleanups - it turned out fSavedInfoStartAt was unneeded. Just use fInsertionStart
 *	
 *	Revision 2.30  1999/03/09 22:45:31  lewis
 *	Keep fCached text as direct-object vector instread of a pointer as a simplification / speed tweek.
 *	
 *	Revision 2.29  1999/03/09 22:35:26  lewis
 *	lose fSavedStyleTCharsCount - always can use GetCachedTextSize() instead - must be same.
 *	And re-arange someme code in Flush() code as speed tweek (tested carefully so SHOULD be safe)
 *	
 *	Revision 2.28  1999/03/08 19:21:15  lewis
 *	convert to supporting Led_tChar/UNICODE (untested, but compiles)
 *	
 *	Revision 2.27  1999/02/21 20:04:16  lewis
 *	override Writer::SRC::GetFirstIndent/GetMargins() calls so we can write out RTF /
 *	paragraph margin information
 *	
 *	Revision 2.26  1999/02/21 13:56:21  lewis
 *	Added SetFirstIndent/SetLeftMargin/SetRightMargin/RTF\li\ri\fi, and amended\pard
 *	support for these RTF tags (READING ONLY). Works (in preliminary testing), but
 *	with a few small sprs noted in dbase (validation and fixing defaults)
 *	
 *	Revision 2.25  1998/10/30 14:19:54  lewis
 *	vector<> instea of Led_Array, and msvc60 pragma warnigns changes
 *	
 *	Revision 2.24  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.23  1998/04/25  01:23:36  lewis
 *	Add Get/Set StandardTabStopList()
 *
 *	Revision 2.22  1998/04/08  01:37:15  lewis
 *	Moved (and renamed) StyledTextIOSinkStream_StandardStyledTextImager/
 *	StyledTextIOWriterSrcStream_StandardStyledTextImager
 *
 *	code here (from StyledTextIO) for better modularity (avoid having to include
 *	WordProcessor code in StyledTextIO module, which was
 *	then included by other lower level modules.
 *	Also - WOrdProcessor-specific stuff (justifications etc) now handled in that
 *	module, and not even seen by this module.
 *
 *
 *		<<RENAMED StandardStyledTextInteractor.h FROM StyledTextInteractor.h>>
 *
 *	Revision 2.21  1998/03/04  20:16:46  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1997/09/29  14:47:15  lewis
 *	StandardStyledTextInteractor CTOR no longer takes style database as arg.
 *	Re-organized virtual/nonvirtual overrides of SetSelection and SetDefaultFont
 *	to make simpler to follow.
 *
 *	Revision 2.18  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.17  1997/07/23  23:05:34  lewis
 *	docs changes
 *
 *	Revision 2.16  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.15  1997/07/12  20:06:07  lewis
 *	AutoDoc support
 *	Use string class instead of char* for command names.
 *
 *	Revision 2.14  1997/06/28  17:13:22  lewis
 *	Add InteractiveUndoHelperMakeTextRep () method.
 *
 *	Revision 2.13  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1997/01/20  05:19:57  lewis
 *	Use new eDefaultUpdate updatemode.
 *
 *	Revision 2.11  1997/01/10  03:00:22  lewis
 *	throw specifier.
 *
 *	Revision 2.10  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.9  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/10/15  18:27:56  lewis
 *	Add InternalizeFlavor_RTF/ExternalizeFlavor_RTF.
 *
 *	Revision 2.7  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.6  1996/06/01  02:05:05  lewis
 *	Added FontChangeCommandName stuff.
 *
 *	Revision 2.5  1996/05/23  19:26:39  lewis
 *	New Led_IncrementalFontSpecification support and
 *	revised/renamed DoContinuousStyle ()->GetContinuousStyleInfo()
 *
 *	Revision 2.4  1996/04/18  15:13:33  lewis
 *	ExternalizeFlavor_STYL etc now take from/to range as optional parameters.
 *	Similarly (but not optional) for AddEmbedding_()
 *	And similarly for InternalizeFlavor_STYLAndTEXT etc...
 *
 *	Revision 2.3  1996/03/05  18:25:26  lewis
 *	Revamped Copy/Paste support around concept of internalizing
 *	and externalizing 'flavor packages'. Then used this also to support
 *	drag / drop.
 *
 *	Revision 2.2  1996/03/04  07:41:37  lewis
 *	Major cleanup of Copy/Paste code so much better sharing across OSs, and
 *	support for STYL, nice support for selected embeddings, and
 *	runs of embeddings, and native clip format, etc.
 *	Also, support for Click/DoubleClick in embeddings causing OPEN command,etc.
 *
 *	Revision 2.1  1996/02/26  18:43:02  lewis
 *	TextIteracter renamed-> TextInteractor.
 *	Override HookLosingTextStore to delete embeddings.
 *	Override new OnCopyCommand_CopyFlavors () part of cleanup of
 *	clipboard sharing code.
 *	And new OnCopyCommand_CopySTYL and OnCopyCommand_CopyNative ().
 *
 *	Revision 2.0  1996/02/05  04:15:22  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1996/02/05  04:06:23  lewis
 *	Initial revision
 *
 *
 *
 *
 */

#include	"StandardStyledTextImager.h"
#include	"TextInteractor.h"

#include	"StyledTextIO.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif






/*
@CONFIGVAR:		qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
@DESCRIPTION:	<p>This format isn't terribly useful right now. It may become much more useful in future versions, if I decide
			to rewrite it, to make it much faster than RTF. Then I may use it internally more.</p>
				<p>It should cause little harm being turned on, but some people (SPR#0810) have requested the ability to
			have this code stripped out.</p>
				<p>Turn ON by default.</p>
 */
#ifndef	qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
	#define	qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor	1
#endif







#if		qXWindows
extern			Led_ClipFormat	kLedPrivateClipFormat;
extern			Led_ClipFormat	kRTFClipFormat;
extern			Led_ClipFormat	kHTMLClipFormat;
#else
extern	const	Led_ClipFormat	kLedPrivateClipFormat;
extern	const	Led_ClipFormat	kRTFClipFormat;
extern	const	Led_ClipFormat	kHTMLClipFormat;
#endif









#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)		//qQuiteAnnoyingDominanceWarnings
#endif


class	SimpleEmbeddedObjectStyleMarker;
/*
@CLASS:			StandardStyledTextInteractor
@BASES:			virtual @'TextInteractor', @'StandardStyledTextImager'
@DESCRIPTION:
		<p>Simple mixin of StandardStyledTextImager and TextInteractor.</p>
		<p>Since this class is designed to make easier implementing a standard font/stylerun
	type editor, we will override SetDefaultFont() and re-interpret it to mean
	setting the font for the current selection of text, or for the NULL selection,
	just as with Apples TE.</p>
		<p>NB:	We considered using @'InteractorImagerMixinHelper<IMAGER>' template as base-class helper.
	But it didn't help. We do too many overrides it doesn't, so it doesn't help much. And then
	later mixing together two subclasses of InteractorImagerMixinHelper (like for word processor)
	requires disambiguating stuff in InteractorImagerMixinHelper itself (like Draw).
	Just not worth much. -- LGP 970707.</p>
*/
class	StandardStyledTextInteractor : public virtual TextInteractor, public StandardStyledTextImager {
	private:
		typedef	void*	inherited;	// prevent accidental references to this name in subclasses to base class name
	protected:
		StandardStyledTextInteractor ();
	public:
		virtual ~StandardStyledTextInteractor ();

	public:
		struct	CommandNames;

	// This class builds commands with command names. The UI may wish to change these
	// names (eg. to customize for particular languages, etc)
	// Just patch these strings here, and commands will be created with these names.
	// (These names appear in text of undo menu item)
	public:
		static	const CommandNames&	GetCommandNames ();
		static	void				SetCommandNames (const CommandNames& cmdNames);
		static	CommandNames		MakeDefaultCommandNames ();
	private:
	#if		qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
		static	CommandNames&	sCommandNames ();
	#else
		static	CommandNames	sCommandNames;
	#endif

	protected:
		override	void	HookLosingTextStore ();
		nonvirtual	void	HookLosingTextStore_ ();
		override	void	HookGainedNewTextStore ();
		nonvirtual	void	HookGainedNewTextStore_ ();

	public:
		override	void	SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont);

	public:
		virtual	void	InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont);

	public:
		override	Led_IncrementalFontSpecification	GetContinuousStyleInfo (size_t from, size_t nTChars) const;

	public:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	public:
		override	bool	ShouldEnablePasteCommand () const;
		override	bool	CanAcceptFlavor (Led_ClipFormat clipFormat) const;

	public:
		class	StandardStyledTextIOSinkStream;
		class	StandardStyledTextIOSrcStream;

	public:
		class	StyledTextFlavorPackageInternalizer;
		class	StyledTextFlavorPackageExternalizer;


	protected:
		override	void	HookStyleDatabaseChanged ();

	protected:
		override	Led_RefCntPtr<FlavorPackageInternalizer>	MakeDefaultInternalizer ();
		override	Led_RefCntPtr<FlavorPackageExternalizer>	MakeDefaultExternalizer ();

	protected:
		override	bool	ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor);
		override	void	WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor);

	public:
		override	void	InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode = eDefaultUpdate);
		override	void	SetSelection (size_t start, size_t end);
		#if		qUsingMemberNameToOverloadInWithUsingBug
			void	SetSelection (size_t start, size_t end, TextInteractor::UpdateMode updateMode)
				{
					TextInteractor::SetSelection (start, end, updateMode);
				}
		#else
			using	TextInteractor::SetSelection;
		#endif
	protected:
		override	bool	InteractiveReplaceEarlyPostReplaceHook (size_t withWhatCharCount);
		nonvirtual	void	SetSelection_ (size_t start, size_t end);	// simply refills fEmptySelectionStyle
	private:
		bool					fEmptySelectionStyleSuppressMode;
		Led_FontSpecification	fEmptySelectionStyle;
	public:
		nonvirtual	Led_FontSpecification	GetEmptySelectionStyle () const;
		nonvirtual	void					SetEmptySelectionStyle ();
		nonvirtual	void					SetEmptySelectionStyle (Led_FontSpecification newEmptyFontSpec);

	public:
		nonvirtual	vector<SimpleEmbeddedObjectStyleMarker*>	CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const;

	protected:
		override	InteractiveReplaceCommand::SavedTextRep*		InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd);

	public:
		class	EmptySelStyleTextRep;
		friend	class	EmptySelStyleTextRep;
};









/*
@CLASS:			StandardStyledTextInteractor::CommandNames
@DESCRIPTION:	<p>Command names for each of the user-visible commands produced by the @'TextInteractor' module.
			This name is used used in the constructed Undo command name, as
			in, "Undo Change Font". You can replace this name with whatever you like.
			You change this value with @'StandardStyledTextInteractor::SetCommandNames'.</p>
				<p> The point of this is to allow for different UI-language localizations,
			without having to change Led itself.</p>
				<p>See also @'TextInteractor::GetCommandNames'.</p>
				<p>See also @'TextInteractor::CommandNames'.</p>
*/
struct	StandardStyledTextInteractor::CommandNames {
	Led_SDK_String	fFontChangeCommandName;
};





/*
@CLASS:			StandardStyledTextInteractor::StandardStyledTextIOSinkStream
@BASES:			@'StyledTextIOReader::SinkStream'
@DESCRIPTION:	<p>This is a writer sink stream which talks to a StandardStyledTextImager and/or WordProcessor
	class. It knows about StyleDatabases, and ParagraphDatabases, and writes content to them from the
	input reader class.</p>
*/
class	StandardStyledTextInteractor::StandardStyledTextIOSinkStream : public virtual StyledTextIOReader::SinkStream {
	private:
		typedef	StyledTextIOReader::SinkStream	inherited;
	public:
		StandardStyledTextIOSinkStream (TextStore* textStore,
				const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
				size_t insertionStart = 0
			);
		~StandardStyledTextIOSinkStream ();

	public:
		// if fontSpec is NULL, use default. Probably later we will return and update the fontspec with
		// ApplyStyle
		override	size_t					current_offset () const;
		override	void					AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec);
		override	void					ApplyStyle (size_t from, size_t to, const vector<StandardStyledTextImager::InfoSummaryRecord>& styleRuns);
		override	Led_FontSpecification	GetDefaultFontSpec () const;
		override	void					InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, size_t at);
		override	void					AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding);
		override	void					AppendSoftLineBreak ();
		override	void					InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner);
		override	void					Flush ();

	public:
		nonvirtual	size_t						GetInsertionStart () const;
		nonvirtual	void						SetInsertionStart (size_t insertionStart);
		nonvirtual	size_t						GetOriginalStart () const;
		nonvirtual	size_t						GetCachedTextSize () const;

	protected:
		nonvirtual	const vector<Led_tChar>&	GetCachedText () const;


	protected:
		nonvirtual	TextStore&									GetTextStore () const;
		nonvirtual	StandardStyledTextImager::StyleDatabasePtr	GetStyleDatabase () const;

	protected:
		nonvirtual	void	PushContext (TextStore* ts,
											const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
											size_t insertionStart
											);
		nonvirtual	void	PopContext ();
	private:
		struct	Context {
			TextStore*										fTextStore;
			StandardStyledTextImager::StyleDatabasePtr		fStyleRunDatabase;
			size_t											fOriginalStart;
			size_t											fInsertionStart;
		};
		vector<Context>		fSavedContexts;

	private:
		TextStore*											fTextStore;
		StandardStyledTextImager::StyleDatabasePtr			fStyleRunDatabase;
		size_t												fOriginalStart;
		size_t												fInsertionStart;
		vector<StandardStyledTextImager::InfoSummaryRecord>	fSavedStyleInfo;
		vector<Led_tChar>									fCachedText;
};





/*
@CLASS:			StandardStyledTextInteractor::StandardStyledTextIOSrcStream
@BASES:			@'StyledTextIOWriter::SrcStream'
@DESCRIPTION:	<p>This is a writer source stream which talks to a StandardStyledTextImager and/or WordProcessor
	class. It knows about StyleDatabases, and ParagraphDatabases, and gets content from them for the
	output writer class.</p>
*/
class	StandardStyledTextInteractor::StandardStyledTextIOSrcStream : public virtual StyledTextIOWriter::SrcStream {
	private:
		typedef	StyledTextIOWriter::SrcStream	inherited;
	public:
		StandardStyledTextIOSrcStream (TextStore* textStore,
				const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
				size_t selectionStart = 0, size_t selectionEnd = kBadIndex
			);
		StandardStyledTextIOSrcStream (StandardStyledTextImager* textImager, size_t selectionStart = 0, size_t selectionEnd = kBadIndex);

	public:
		override	size_t										readNTChars (Led_tChar* intoBuf, size_t maxTChars);
		override	size_t										current_offset () const;
		override	void										seek_to (size_t to);
		override	size_t										GetTotalTextLength () const;
		override	vector<InfoSummaryRecord>					GetStyleInfo (size_t from, size_t len) const;
		override	vector<SimpleEmbeddedObjectStyleMarker*>	CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const;
		override	Table*										GetTableAt (size_t at) const;
		override	void										SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const;
		override	size_t										GetEmbeddingMarkerPosOffset () const;

	public:
		nonvirtual	size_t	GetCurOffset () const;
		nonvirtual	size_t	GetSelStart () const;
		nonvirtual	size_t	GetSelEnd () const;

	private:
		TextStore*									fTextStore;
		StandardStyledTextImager::StyleDatabasePtr	fStyleRunDatabase;
		size_t										fCurOffset;
		size_t										fSelStart;
		size_t										fSelEnd;
};















/*
@CLASS:			StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer
@BASES:			virtual @'FlavorPackageInternalizer'
@DESCRIPTION:	<p>Add RTF, Led_Native, and SingleSelectedEmbedding support.</p>
*/
class	StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer : public virtual FlavorPackageInternalizer {
	private:
		typedef	FlavorPackageInternalizer	inherited;

	public:
		StyledTextFlavorPackageInternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase);

	public:
		override	bool	InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
													size_t from, size_t to
												);
	public:
		override	void	InternalizeFlavor_FILEGuessFormatsFromName (
									#if		qMacOS
										const FSSpec* fileName,
									#elif	qWindows || qXWindows
										const Led_SDK_Char* fileName,
									#endif
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage
								);
		override	void	InternalizeFlavor_FILEGuessFormatsFromStartOfData (
									Led_ClipFormat* suggestedClipFormat,
									CodePage* suggestedCodePage,
									const Byte* fileStart, const Byte* fileEnd
								);

	public:
		#if		qMacOS
		virtual		bool	InternalizeFlavor_STYLAndTEXT (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
		#endif
		#if		qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
		virtual		bool	InternalizeFlavor_Native (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
		#endif
		virtual		bool	InternalizeFlavor_RTF (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
		virtual		bool	InternalizeFlavor_HTML (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);
		virtual		bool	InternalizeFlavor_OtherRegisteredEmbedding (ReaderFlavorPackage& flavorPackage, size_t from, size_t to);

	public:
		virtual	StandardStyledTextIOSinkStream*	mkStandardStyledTextIOSinkStream (size_t insertionStart);

	protected:
		StandardStyledTextImager::StyleDatabasePtr	fStyleDatabase;
};




/*
@CLASS:			StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer
@BASES:			virtual @'FlavorPackageExternalizer'
@DESCRIPTION:	<p>Add RTF, Led_Native, and SingleSelectedEmbedding support.</p>
*/
class	StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer : public virtual FlavorPackageExternalizer {
	private:
		typedef	FlavorPackageExternalizer	inherited;

	public:
		StyledTextFlavorPackageExternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase);

	public:
		override	void	ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
		override	void	ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to);


	public:
		#if		qMacOS
		nonvirtual	void	ExternalizeFlavor_STYL (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
		#endif
		#if		qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
		nonvirtual	void	ExternalizeFlavor_Native (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
		#endif
		nonvirtual	void	ExternalizeFlavor_RTF (WriterFlavorPackage& flavorPackage, size_t from, size_t to);
		nonvirtual	void	ExternalizeFlavor_SingleSelectedEmbedding (WriterFlavorPackage& flavorPackage, SimpleEmbeddedObjectStyleMarker* embedding);

	protected:
		virtual	StandardStyledTextIOSrcStream*	mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd);


	protected:
		StandardStyledTextImager::StyleDatabasePtr	fStyleDatabase;
};






/*
@CLASS:			StandardStyledTextInteractor::EmptySelStyleTextRep
@BASES:			@'InteractiveReplaceCommand::SavedTextRep'
@DESCRIPTION:
*/
class	StandardStyledTextInteractor::EmptySelStyleTextRep : public InteractiveReplaceCommand::SavedTextRep {
	private:
		typedef	InteractiveReplaceCommand::SavedTextRep	inherited;
	public:
		EmptySelStyleTextRep (StandardStyledTextInteractor* interactor, size_t selStart, size_t selEnd);

	public:
		override	size_t	GetLength () const;
		override	void	InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite);

	private:
		Led_FontSpecification	fSavedStyle;
};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	StandardStyledTextInteractor
	/*
	@METHOD:		StandardStyledTextInteractor::GetCommandNames
	@DESCRIPTION:	<p>Returns command name for each of the user-visible commands produced by this module.
		This name is used used in the constructed Undo command name, as
		in, "Undo Change Font". You can replace this name with whatever you like.You change this value with
		WordProcessor::SetCommandNames.</p>
			<p> The point of this is to allow for different UI-language localizations,
				without having to change Led itself.</p>
			<p>See also @'StandardStyledTextInteractor::CommandNames'.</p>
	*/
	inline	const StandardStyledTextInteractor::CommandNames&	StandardStyledTextInteractor::GetCommandNames ()
		{
			#if		qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
				return sCommandNames ();
			#else
				return sCommandNames;
			#endif
		}
	/*
	@METHOD:		StandardStyledTextInteractor::SetCommandNames
	@DESCRIPTION:	<p>See @'StandardStyledTextInteractor::GetCommandNames'.</p>
	*/
	inline	void	StandardStyledTextInteractor::SetCommandNames (const StandardStyledTextInteractor::CommandNames& cmdNames)
		{
			#if		qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
				sCommandNames () = cmdNames;
			#else
				sCommandNames = cmdNames;
			#endif
		}



//	class	StandardStyledTextInteractor::StandardStyledTextIOSinkStream
	inline	TextStore&	StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetTextStore () const
		{
			Led_EnsureNotNil (fTextStore);
			return *fTextStore;
		}
	inline	StandardStyledTextImager::StyleDatabasePtr	StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetStyleDatabase () const
		{
			return fStyleRunDatabase;
		}
	/*
	@METHOD:		StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart
	@DESCRIPTION:	<p>Returns where (in TextStore marker coordinates - not relative to the sinkstream) where the next character
				will be inserted. See also @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart'</p>
	*/
	inline	size_t	StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart () const
		{
			return fInsertionStart;
		}
	/*
	@METHOD:		StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart
	@DESCRIPTION:	<p>See also @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetInsertionStart'</p>
	*/
	inline	void	StandardStyledTextInteractor::StandardStyledTextIOSinkStream::SetInsertionStart (size_t insertionStart)
		{
			fInsertionStart = insertionStart;
		}
	inline	size_t	StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetOriginalStart () const
		{
			return fOriginalStart;
		}
	inline	size_t	StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetCachedTextSize () const
		{
			return fCachedText.size ();
		}
	inline	const vector<Led_tChar>&	StandardStyledTextInteractor::StandardStyledTextIOSinkStream::GetCachedText () const
		{
			return fCachedText;
		}



//	class	StandardStyledTextInteractor::StandardStyledTextIOSrcStream
	inline	size_t	StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetCurOffset () const
		{
			return fCurOffset;
		}
	inline	size_t	StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetSelStart () const
		{
			return fSelStart;
		}
	inline	size_t	StandardStyledTextInteractor::StandardStyledTextIOSrcStream::GetSelEnd () const
		{
			return fSelEnd;
		}



#if		qLedUsesNamespaces
}
#endif



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif

#endif	/*__StandardStyledTextInteractor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
