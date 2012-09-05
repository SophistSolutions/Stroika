/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextEmbeddedObjects_h__
#define	__StyledTextEmbeddedObjects_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextEmbeddedObjects.h,v 2.61 2003/04/01 18:17:57 lewis Exp $
 */

/*
@MODULE:	StyledTextEmbeddedObjects
@DESCRIPTION:
		<p>This module introduces a whole bunch of classes which leverage the Led @'Marker', and particularly the
	Led 'StyledTextImager::StyleMarker' mechanism - to implement visual object embedding within the text.</p>
		<p>The objects we embed are of type @'SimpleEmbeddedObjectStyleMarker'. But this module also contains other classes
	to support the creation of these embedding markers, to get them properly inserted into the text. And this module
	contains many @'SimpleEmbeddedObjectStyleMarker' subclasses, such as @'StandardDIBStyleMarker', which utilize this
	mechanism to display a particular data type as an emebdding.</p>
 */


/*
 * Changes:
 *	$Log: StyledTextEmbeddedObjects.h,v $
 *	Revision 2.61  2003/04/01 18:17:57  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.60  2003/01/29 17:59:15  lewis
 *	instead of having JUST hardwired 100 value - make eEmbeddedObjectPriority relative to eBaselinePriority
 *	
 *	Revision 2.59  2003/01/11 19:28:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2002/11/27 15:58:28  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.57  2002/09/19 14:14:12  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.56  2002/05/06 21:33:34  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.55  2001/11/27 00:29:45  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.54  2001/10/17 20:42:52  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.53  2001/10/09 21:02:28  lewis
 *	SPR#1058- qURLStyleMarkerNewDisplayMode depends on qSupportLed23CompatAPI
 *	
 *	Revision 2.52  2001/09/26 15:41:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.51  2001/09/18 14:03:17  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 2.50  2001/08/29 23:00:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.49  2001/08/28 18:43:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2001/08/20 22:22:28  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly enlarged text).
 *	Can use qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 2.47  2001/08/16 18:53:41  lewis
 *	SPR#0959- more work on 'GetInfo' support for embeddings.
 *	
 *	Revision 2.46  2001/07/17 19:09:37  lewis
 *	SPR#0959- added preliminary RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 2.45  2001/01/03 14:51:52  Lewis
 *	typedef inheirted and lose pointless assert(unsigned >=0)
 *	
 *	Revision 2.44  2000/09/02 21:52:02  lewis
 *	use name Led_DIB instead of BITMAPINFO
 *	
 *	Revision 2.43  2000/09/01 00:53:00  lewis
 *	small tweeks to get compiling under XWindows
 *	
 *	Revision 2.42  2000/08/13 04:07:27  lewis
 *	InsertEmbeddingForExistingSentinal/AddEmbedding require an owner now - no default value
 *	
 *	Revision 2.41  2000/07/09 04:31:30  lewis
 *	Added optional DIB associated with StandardUnknownTypeStyleMarker object (for SPR#0498)
 *	
 *	Revision 2.40  2000/06/13 16:26:14  lewis
 *	SPR#0767 - last tidbit - unknown embeddings - and RTFOLE embeddings - must use Led_TWIPS_Point
 *	
 *	Revision 2.39  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.38  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.37  2000/04/14 19:17:35  lewis
 *	SPR#0739- react to change where we move much TextIntearctor flavor package code into its own module,
 *	and reorg the updateCursorPos etc stuff into separate code from code specific to each flavor type
 *	
 *	Revision 2.36  2000/03/28 03:41:04  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class takes
 *	extra RunElement argument.
 *	Now this new code is called by default StyledTextImager::DrawText() etc overrides.
 *	And now HiddenText code
 *	properly merges at least its own drawing and StandardStyledTextImager::StyleMarker.
 *	All this respecting
 *	qOldStyleMarkerBackCompatHelperCode - so old code (not new hidden text code)
 *	should still compile and work roughly as before with this turned on.
 *	
 *	Revision 2.35  1999/12/14 18:04:07  lewis
 *	Preliminary qXWindows support
 *	
 *	Revision 2.34  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.33  1999/07/13 22:39:13  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds 
 *	that were only needed for these old compilers
 *	
 *	Revision 2.32  1999/06/16 15:22:48  lewis
 *	add EmbeddedObjectCreatorRegistry::Lookup helper method as part of fix to spr# 0590
 *	
 *	Revision 2.31  1999/05/03 22:04:59  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.30  1998/10/30 14:20:16  lewis
 *	Led_Array obsolete - use vector<>
 *	
 *	Revision 2.29  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.28  1998/04/25  01:24:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.27  1998/03/04  20:17:16  lewis
 *	*** empty log message ***
 *
 *	Revision 2.26  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.25  1997/12/24  03:26:31  lewis
 *	Added "ShownSize" APIs to StandardUnknownTypeStyleMarker.
 *	Docs.
 *
 *	Revision 2.24  1997/09/29  14:43:45  lewis
 *	Lose qLedFirstIndex support.
 *
 *	Revision 2.23  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.22  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.21  1997/07/12  20:03:27  lewis
 *	AutoDoc support.
 *	And lose _ in a few names, like TextInteractor, SinkStream, TextStore, etc.
 *
 *	Revision 2.20  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1997/06/18  02:46:09  lewis
 *	qOverrideDefaultSentinalValue - so some users can change sentinal value (not
 *	sure its a good idea, but it was requested).
 *
 *	Revision 2.18  1997/01/10  03:02:32  lewis
 *	throw specifier.
 *
 *	Revision 2.17  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.16  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1996/10/15  18:28:17  lewis
 *	For URLD object, lose kClipFormat and replace with kURLDClipFormat and kWin32URLClipFormat.
 *
 *	Revision 2.14  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.13  1996/07/03  00:53:38  lewis
 *	Moved URL definition stuff to LedSupport.hh, and now called
 *	Led_URLD, instead of untyped data.
 *	Move OpenURL, and other misc URL utilities to Led_URLManager class in LedSupport.
 *
 *	Revision 2.12  1996/06/01  02:04:26  lewis
 *	Lose qNoRTTISupportBug
 *
 *	Revision 2.11  1996/05/23  19:24:02  lewis
 *	DrawSegmemnt no longer takes areaLeft arg - instead just a pixelsDrawn
 *	arg. And its now OPTIONAL (and typically NULL) for performance sake.
 *
 *	Revision 2.10  1996/05/14  20:19:52  lewis
 *	make StyledTextImager arg to DrawSegment() const and added Led_Tablet
 *	arg as well - so we can pass along offscreen tablet, and avoid the
 *	Acquire.
 *
 *	Revision 2.9  1996/05/04  19:19:30  lewis
 *	VERY substantial changes to picture oriented concrete classes.
 *	Use QT on windows to display picts. Use DIBS on PC. Now all fully
 *	supported. And opening URLDs works on PC, etc.
 *	Lots of work done here.
 *
 *	Revision 2.8  1996/04/18  15:09:10  lewis
 *	lots of changes.
 *	added support for multi-flavored embeddings, and created one
 *	on mac (pict and urld) - do pc version later?
 *	EmbeddingTag renamed to Led_PrivateEmbeddingTag.
 *	A bunch more changes..
 *
 *	Revision 2.7  1996/03/16  18:39:29  lewis
 *	Revised tab-stop support.
 *	FindBrowser () static member for URL class.
 *
 *	Revision 2.6  1996/03/05  18:25:56  lewis
 *	Renamed Led_ClipboardObjectAcquire::ClipFormat --> Led_ClipFormat.
 *	more?
 *
 *	Revision 2.5  1996/03/04  07:39:44  lewis
 *	Lots of changes.
 *	Added EmbeddingTag to Assoc registry.
 *	Added StandardUnknownTypeStyleMarker.
 *	Added StandardURLStyleMarker.
 *	Made embedding tags for classes be scoped static members isntead of globals.
 *
 *	Revision 2.4  1996/02/26  18:39:52  lewis
 *	Define kEmbeddingSentinalChar.
 *	Get rid of Style DBase param for AddEmbedding. Instead provide optional
 *	ownerForEMbedding parameter (if NULL, use textstore).
 *
 *	Revision 2.3  1996/01/03  23:51:42  lewis
 *	Declare EmbeddingTag here, and helper function AddEmbedding().
 *
 *	Revision 2.2  1995/12/15  02:14:24  lewis
 *	DrawSegment() now takes an invalidRect arg for logical clipping.
 *
 *	Revision 2.1  1995/12/06  01:25:35  lewis
 *	Make GetPictureData () method to access pict data.
 *
 *	Revision 2.0  1995/11/25  00:19:42  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/11/25  00:19:30  lewis
 *	Initial revision
 *
 *
 *
 *
 */
#include	<memory>

#if		qMacOS
	#include	<Processes.h>		// for URL support
#endif

#include	"TextInteractor.h"
#include	"StyledTextImager.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif


typedef	char	Led_PrivateEmbeddingTag[10];


/*
 *	qOverrideDefaultSentinalValue allows you to use a differnt value for the sentinal than
 *	the default of zero. NB, that zero is a good default choice because it isn't a valid
 *	character code, and nearly all other (if not all other) choices are valid characters
 *	which you might be confusing with a sentinal.
 */
#if		qOverrideDefaultSentinalValue
	const	Led_tChar	kEmbeddingSentinalChar	=	qOverrideDefaultSentinalValue;
#else
	const	Led_tChar	kEmbeddingSentinalChar	=	'\0';
#endif






// Its up to you in your application for each one of these you want to support to add a line such
// as:
//
//	EmbeddedObjectCreatorRegistry::Get ().AddAssoc (CL::PICT, StandardMacPictureStyleMarker::Creator);
//
// 		For each of the marker types you want automatically taken care of. All the default ones,
//	are added automatlicly by default. If you don't like these, you can use SetAssocList to replace
//	them.
//
//
class	SimpleEmbeddedObjectStyleMarker;



/*
@CLASS:			EmbeddedObjectCreatorRegistry
@DESCRIPTION:
		<p>Class which manages the registry of mappings from clipboard and Led private file format magic tag strings,
	to function pointers capable of reading these objects from RAM or a flavor package.</p>
*/
class	EmbeddedObjectCreatorRegistry {
	public:
		EmbeddedObjectCreatorRegistry ();

	public:
		static	EmbeddedObjectCreatorRegistry&	Get ();	// build one if doesn't exist yet, and return ref to it
	private:
		static	EmbeddedObjectCreatorRegistry*	sThe;

	public:
		struct	Assoc {
			private:
				union	{
					Led_ClipFormat			fFormatTag;		// use if fFormatTagCount==1
					const Led_ClipFormat*	fFormatTags;	// use if fFormatTagCount>1
				};
			public:
				size_t								fFormatTagCount;
				Led_PrivateEmbeddingTag				fEmbeddingTag;
				SimpleEmbeddedObjectStyleMarker*	(*fReadFromMemory) (const char* embeddingTag, const void* data, size_t len);
				SimpleEmbeddedObjectStyleMarker*	(*fReadFromFlavorPackage) (ReaderFlavorPackage& flavorPackage);
			
			public:
				nonvirtual	Led_ClipFormat	GetIthFormat (size_t i) const;

			private:
				friend	class	EmbeddedObjectCreatorRegistry;
		};

	public:
		nonvirtual	void					AddAssoc (Assoc assoc);
		nonvirtual	void					AddAssoc (const char* embeddingTag,
														SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len)
													);
		nonvirtual	void					AddAssoc (Led_ClipFormat clipFormat, const char* embeddingTag,
														SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
														SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage)
													);
		nonvirtual	void					AddAssoc (const Led_ClipFormat* clipFormats, size_t clipFormatCount, const char* embeddingTag,
														SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
														SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage)
													);
		nonvirtual	const vector<Assoc>&	GetAssocList () const;
		nonvirtual	void					SetAssocList (const vector<Assoc>& assocList);
		nonvirtual	void					AddStandardTypes ();
		nonvirtual	bool					Lookup (const char* embeddingTag, Assoc* result) const;
	private:
		vector<Assoc>	fAssocList;

		friend	struct	FooBarBlatzRegistryCleanupHack;
};





const	Led_Size		kDefaultEmbeddingMargin	=	Led_Size (4, 4);	// space left around embedding so you can see selected.
																		// Not needed for all embeddings, but often helpful
/*
@CLASS:			SimpleEmbeddedObjectStyleMarker
@BASES:			@'StyledTextImager::StyleMarker'
@DESCRIPTION:	<p>An abstract class which contains most of the functionality for supporting Led embeddings in a Styled Text
	document.</p>
		<p>All the various kinds of embeddings Led supports are subclasses of this class.</p>
		<p>See @'InsertEmbeddingForExistingSentinal' and @'AddEmbedding' for more details on how to add these markers to the
	text. These utilities are not <em>needed</em> - but can simplify the process of adding embeddings.</p>
		<p>Also, to understand more about how these work, see @'StyledTextImager::StyleMarker' for more details.</p>
		<p>(NEED LOTS MORE DETAILS - THIS IS IMPORTANT)</p>
*/
class	SimpleEmbeddedObjectStyleMarker : public StyledTextImager::StyleMarker {
	private:
		typedef	StyledTextImager::StyleMarker	inherited;

	protected:
		SimpleEmbeddedObjectStyleMarker ();

	public:
		enum { eEmbeddedObjectPriority = eBaselinePriority + 100 };
		override	int		GetPriority () const;

	// Subclasses simply must override DrawSegment and MeasureSegmentWidth/MeasureSegmentHeight
	// the rest we can default properly here
	public:
		override	Led_Distance	MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const;

	public:
		override	void	DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ();


	// Native clip format writing support
	public:
		class	SinkStream {
			public:
				virtual	void	write (const void* buffer, size_t bytes)		=	0;
		};
		virtual	const char*		GetTag () const															=	0;
		virtual	void			Write (SinkStream& sink)												=	0;
		virtual	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage)					=	0;


	// Click in embedding processing
	// HandleClick () by default, does nothing on single click, and on double click calls Open method - which in turn by
	// default does nothing. HandleClick returns false if it EATS the click. If it returns true if the click
	// should be handled as it normally would
	public:
		virtual	bool	HandleClick (Led_Point clickedAt, unsigned clickCount);
		virtual	bool	HandleOpen ();

	public:
		enum	PrivateCmdNumber	{ eMinPrivateCmdNum = 1, eOpenCmdNum = eMinPrivateCmdNum, eMaxPrivateCmdNum = 100 };
		virtual	vector<PrivateCmdNumber>	GetCmdNumbers () const;
		virtual	bool						IsCmdEnabled (PrivateCmdNumber cmd) const;
		virtual	Led_SDK_String				GetCmdText (PrivateCmdNumber cmd);
		virtual	void						DoCommand (PrivateCmdNumber cmd);

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
		static	CommandNames	sCommandNames;
};



/*
@CLASS:			SimpleEmbeddedObjectStyleMarker::CommandNames
@DESCRIPTION:	<p>Command names for each of the user-visible commands produced by the
			@'SimpleEmbeddedObjectStyleMarker' module.</p>
*/
struct	SimpleEmbeddedObjectStyleMarker::CommandNames {
	Led_SDK_String	fOpenCommandName;
};






#if		qMacOS || qWindows
/*
@CLASS:			StandardMacPictureStyleMarker
@BASES:			@'SimpleEmbeddedObjectStyleMarker'
@DESCRIPTION:	<p>A Macintosh format picture embedding.</p>
*/
class	StandardMacPictureStyleMarker : public SimpleEmbeddedObjectStyleMarker {
	private:
		typedef	SimpleEmbeddedObjectStyleMarker	inherited;

	public:
		#if		qMacOS
			typedef	Led_Picture**	PictureHandle;
		#elif	qWindows
			typedef	HANDLE			PictureHandle;
		#endif

	public:
		static	const	Led_ClipFormat			kClipFormat;
		static	const	Led_PrivateEmbeddingTag	kEmbeddingTag;

	public:
		StandardMacPictureStyleMarker (const Led_Picture* pictData, size_t picSize);
		~StandardMacPictureStyleMarker ();

	public:
		static	SimpleEmbeddedObjectStyleMarker*	mk (const char* embeddingTag, const void* data, size_t len);
		static	SimpleEmbeddedObjectStyleMarker*	mk (ReaderFlavorPackage& flavorPackage);

		#if		qWindows
			static	const Led_DIB*	sUnsupportedFormatPict;	// Must be set externally by user of this class before we ever build one of these
															// objects, or an assert error.
															// Reason for this design is we need access to some pict resource, but we don't want
															// Led to depend on any such things (would make build/distr/name conflicts etc
															// more complex). So in main, if you ever plan to use these, then load resource and assign
															// to this member.
		#endif
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

	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream& sink);
		override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		nonvirtual	PictureHandle		GetPictureHandle () const;
		nonvirtual	size_t				GetPictureByteSize () const;

	private:
		PictureHandle	fPictureHandle;
		#if		qWindows
			size_t		fPictureSize;
		#endif
};
#endif





/*
@CLASS:			StandardDIBStyleMarker
@BASES:			@'SimpleEmbeddedObjectStyleMarker'
@DESCRIPTION:	<p>A Windows DIB (Device Independent Bitmap) picture embedding. Note that this can still be
			portably displayed on both Mac and Windows.</p>
*/
class	StandardDIBStyleMarker : public SimpleEmbeddedObjectStyleMarker {
	private:
		typedef	SimpleEmbeddedObjectStyleMarker	inherited;

	public:
		static	const	Led_ClipFormat			kClipFormat;
		static	const	Led_PrivateEmbeddingTag	kEmbeddingTag;

	public:
		StandardDIBStyleMarker (const Led_DIB* dibData);
		~StandardDIBStyleMarker ();

	public:
		static	SimpleEmbeddedObjectStyleMarker*	mk (const char* embeddingTag, const void* data, size_t len);
		static	SimpleEmbeddedObjectStyleMarker*	mk (ReaderFlavorPackage& flavorPackage);

	public:
		#if		qMacOS
			static	Led_Picture**	sUnsupportedFormatPict;	// Must be set externally by user of this class before we ever build one of these
															// objects, or an assert error.
															// Reason for this design is we need access to some pict resource, but we don't want
															// Led to depend on any such things (would make build/distr/name conflicts etc
															// more complex). So in main, if you ever plan to use these, then load resource and assign
															// to this member.
		#endif

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

	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream& sink);
		override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		nonvirtual	const Led_DIB*	GetDIBData () const;

	private:
		Led_DIB*	fDIBData;
};










/*
@CONFIGVAR:		qURLStyleMarkerNewDisplayMode
@DESCRIPTION:
		<p>The old display mode was somewhat idiosyncratic, and the new one is more like how other browsers
	display URLs. But keep the old one available in case some prefer it (at least for 3.0).</p>
		<p>Turn ON by default.</p>
 */
#ifndef	qURLStyleMarkerNewDisplayMode
	#define	qURLStyleMarkerNewDisplayMode	1
#endif






/*
@CLASS:			StandardURLStyleMarker
@BASES:			@'SimpleEmbeddedObjectStyleMarker'
@DESCRIPTION:	<p>A URL object. Double click on this to open the URL.</p>
*/
class	StandardURLStyleMarker : public SimpleEmbeddedObjectStyleMarker {
	private:
		typedef	SimpleEmbeddedObjectStyleMarker	inherited;

	public:
		static	const	Led_ClipFormat			kURLDClipFormat;
		#if		qWindows
			static	const	Led_ClipFormat		kWin32URLClipFormat;
		#endif
		static	const	Led_PrivateEmbeddingTag	kEmbeddingTag;

		StandardURLStyleMarker (const Led_URLD& urlData);
		~StandardURLStyleMarker ();

	public:
		static	SimpleEmbeddedObjectStyleMarker*	mk (const char* embeddingTag, const void* data, size_t len);
		static	SimpleEmbeddedObjectStyleMarker*	mk (ReaderFlavorPackage& flavorPackage);

#if		!qURLStyleMarkerNewDisplayMode
	public:
		#if		qMacOS
			static	Led_Picture**	sURLPict;	// Must be set externally by user of this class before we ever build one of these
												// objects, or an assert error.
												// Reason for this design is we need access to some pict resource, but we don't want
												// Led to depend on any such things (would make build/distr/name conflicts etc
												// more complex). So in main, if you ever plan to use these, then load resource and assign
												// to this member.
		#elif	qWindows
			static	const Led_DIB*	sURLPict;
		#endif
#endif

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

	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream& sink);
		override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		override	bool	HandleOpen ();

	public:
		override	vector<PrivateCmdNumber>	GetCmdNumbers () const;
		override	bool						IsCmdEnabled (PrivateCmdNumber cmd) const;

	public:
		nonvirtual	const Led_URLD&	GetURLData () const;
		nonvirtual	void			SetURLData (const Led_URLD& urlData);
	private:
		Led_URLD	fURLData;

	protected:
		nonvirtual	Led_tString				GetDisplayString () const;
		nonvirtual	Led_FontSpecification	GetDisplayFont (const RunElement& runElement) const;
};





#if		qMacOS || qWindows
class	StandardMacPictureWithURLStyleMarker : public SimpleEmbeddedObjectStyleMarker {
	private:
		typedef	SimpleEmbeddedObjectStyleMarker	inherited;

	public:
		static	const	Led_ClipFormat			kClipFormats[];
		static	const	size_t					kClipFormatCount;
		static	const	Led_PrivateEmbeddingTag	kEmbeddingTag;
		static	const	Led_PrivateEmbeddingTag	kOld1EmbeddingTag;

	public:
		StandardMacPictureWithURLStyleMarker (const Led_Picture* pictData, size_t picSize, const Led_URLD& urlData);
		~StandardMacPictureWithURLStyleMarker ();

	public:
		static	SimpleEmbeddedObjectStyleMarker*	mk (const char* embeddingTag, const void* data, size_t len);
		static	SimpleEmbeddedObjectStyleMarker*	mk (ReaderFlavorPackage& flavorPackage);

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

	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream& sink);
		override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		override	bool	HandleOpen ();

	public:
		override	vector<PrivateCmdNumber>	GetCmdNumbers () const;
		override	bool						IsCmdEnabled (PrivateCmdNumber cmd) const;

	private:
		Led_URLD	fURLData;

	public:
		nonvirtual	StandardMacPictureStyleMarker::PictureHandle	GetPictureHandle () const;
		nonvirtual	size_t				GetPictureByteSize () const;
		nonvirtual	const Led_URLD&		GetURLData () const;
		nonvirtual	void				SetURLData (const Led_URLD& urlData);

	private:
		StandardMacPictureStyleMarker::PictureHandle	fPictureHandle;
		#if		qWindows
			size_t										fPictureSize;
		#endif
};
#endif




class	StandardDIBWithURLStyleMarker : public SimpleEmbeddedObjectStyleMarker {
	private:
		typedef	SimpleEmbeddedObjectStyleMarker	inherited;

	public:
		static	const	Led_ClipFormat			kClipFormats[];
		static	const	size_t					kClipFormatCount;
		static	const	Led_PrivateEmbeddingTag	kEmbeddingTag;

	public:
		StandardDIBWithURLStyleMarker (const Led_DIB* dibData, const Led_URLD& urlData);
		~StandardDIBWithURLStyleMarker ();

	public:
		static	SimpleEmbeddedObjectStyleMarker*	mk (const char* embeddingTag, const void* data, size_t len);
		static	SimpleEmbeddedObjectStyleMarker*	mk (ReaderFlavorPackage& flavorPackage);

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

	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream& sink);
		override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		override	bool	HandleOpen ();

	public:
		override	vector<PrivateCmdNumber>	GetCmdNumbers () const;
		override	bool						IsCmdEnabled (PrivateCmdNumber cmd) const;

	public:
		nonvirtual	const Led_DIB*		GetDIBData () const;
	private:
		Led_DIB*	fDIBData;

	public:
		nonvirtual	const Led_URLD&		GetURLData () const;
		nonvirtual	void				SetURLData (const Led_URLD& urlData);
	private:
		Led_URLD	fURLData;
};





/*
@CLASS:			StandardUnknownTypeStyleMarker
@BASES:			@'SimpleEmbeddedObjectStyleMarker'
@DESCRIPTION:	<p>Use to represent visually embeddings which we've read from a file, and want to keep track of, but
			don't know how to display.</p>
*/
class	StandardUnknownTypeStyleMarker : public SimpleEmbeddedObjectStyleMarker {
	private:
		typedef	SimpleEmbeddedObjectStyleMarker	inherited;

	public:
		static	const	Led_PrivateEmbeddingTag	kDefaultEmbeddingTag;

	public:
		StandardUnknownTypeStyleMarker (Led_ClipFormat format, const char* embeddingTag, const void* unknownTypeData, size_t nBytes, const Led_DIB* dib = NULL);
		~StandardUnknownTypeStyleMarker ();

	public:
		#if		qMacOS
			static	Led_Picture**	sUnknownPict;	// Must be set externally by user of this class before we ever build one of these
													// objects, or an assert error.
													// Reason for this design is we need access to some pict resource, but we don't want
													// Led to depend on any such things (would make build/distr/name conflicts etc
													// more complex). So in main, if you ever plan to use these, then load resource and assign
													// to this member.
		#elif	qWindows
			static	const Led_DIB*	sUnknownPict;
		#endif

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

	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream& sink);
		override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		nonvirtual	Led_TWIPS_Point	GetShownSize () const;
		nonvirtual	void			SetShownSize (Led_TWIPS_Point size);
		nonvirtual	Led_TWIPS_Point	CalcDefaultShownSize ();
		static		Led_TWIPS_Point	CalcStaticDefaultShownSize ();
	private:
		Led_TWIPS_Point	fShownSize;

	public:
		nonvirtual	const void*	GetData () const;
		nonvirtual	size_t		GetDataLength () const;

	private:
		void*					fData;
		size_t					fLength;
		Led_ClipFormat			fFormat;
		Led_PrivateEmbeddingTag	fEmbeddingTag;
		auto_ptr<Led_DIB>		fDisplayDIB;
};







/*
@METHOD:		InsertEmbeddingForExistingSentinal
@DESCRIPTION:	<p>Utility method to insert the given embedding (@'SimpleEmbeddedObjectStyleMarker') into the given
			@'TextStore', at a given position in the text. To use this routine, the sentinal character must already
			be present. Use @'AddEmbedding' if the sentinal has not yet been added.</p>
*/
void	InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding);

/*
@METHOD:		AddEmbedding
@DESCRIPTION:	<p>Utility method to insert the given embedding (@'SimpleEmbeddedObjectStyleMarker') into the given
			@'TextStore', at a given position in the text. This routine, adds a sentinal character at that position. If
			the sentinal character is already there, use @'InsertEmbeddingForExistingSentinal'.</p>
				<p>Actually, this function isn't strictly needed. But there is alot of mumbo-jumbo that needs to be
			done when adding embeddings (e.g. DoAboutToUpdateCalls etc), and this packages up all those things and
			avoids you from having todo them in a bunch of places.</p>
*/
void	AddEmbedding (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding);









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
// class EmbeddedObjectCreatorRegistry::Assoc
	inline	Led_ClipFormat	EmbeddedObjectCreatorRegistry::Assoc::GetIthFormat (size_t i) const
		{
			Led_Assert (fFormatTagCount >= 1);
			Led_Require (i < fFormatTagCount);
			return (fFormatTagCount == 1)? fFormatTag: fFormatTags[i];
		}

// class EmbeddedObjectCreatorRegistry
	inline	EmbeddedObjectCreatorRegistry::EmbeddedObjectCreatorRegistry ():
		fAssocList ()
		{
		}
	inline	EmbeddedObjectCreatorRegistry&	EmbeddedObjectCreatorRegistry::Get ()
		{
			if (sThe == NULL) {
				sThe = new EmbeddedObjectCreatorRegistry ();
			}
			return *sThe;
		}
	inline	void	EmbeddedObjectCreatorRegistry::AddAssoc (Assoc assoc)
		{
			fAssocList.push_back (assoc);
		}
	inline	void	EmbeddedObjectCreatorRegistry::AddAssoc (const char* embeddingTag,
														SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len)
													)
		{
			Assoc	assoc;
			assoc.fFormatTagCount = 0;
			memcpy (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag));
			assoc.fReadFromMemory = memReader;
			assoc.fReadFromFlavorPackage = NULL;
			AddAssoc (assoc);
		}
	inline	void	EmbeddedObjectCreatorRegistry::AddAssoc (Led_ClipFormat clipFormat, const char* embeddingTag,
														SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
														SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage)
													)
		{
			Assoc	assoc;
			assoc.fFormatTag = clipFormat;
			assoc.fFormatTagCount = 1;
			memcpy (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag));
			assoc.fReadFromMemory = memReader;
			assoc.fReadFromFlavorPackage = packageReader;
			AddAssoc (assoc);
		}
	inline	void	EmbeddedObjectCreatorRegistry::AddAssoc (const Led_ClipFormat* clipFormats, size_t clipFormatCount, const char* embeddingTag,
														SimpleEmbeddedObjectStyleMarker* (*memReader) (const char* embeddingTag, const void* data, size_t len),
														SimpleEmbeddedObjectStyleMarker* (*packageReader) (ReaderFlavorPackage& flavorPackage)
													)
		{
			Assoc	assoc;
			assoc.fFormatTags = clipFormats;
			assoc.fFormatTagCount = clipFormatCount;
			memcpy (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag));
			assoc.fReadFromMemory = memReader;
			assoc.fReadFromFlavorPackage = packageReader;
			AddAssoc (assoc);
		}
	inline	const vector<EmbeddedObjectCreatorRegistry::Assoc>&	EmbeddedObjectCreatorRegistry::GetAssocList () const
		{
			return fAssocList;
		}
	inline	void	EmbeddedObjectCreatorRegistry::SetAssocList (const vector<Assoc>& assocList)
		{
			fAssocList = assocList;
		}





//	class	SimpleEmbeddedObjectStyleMarker
	/*
	@METHOD:		SimpleEmbeddedObjectStyleMarker::GetCommandNames
	@DESCRIPTION:	<p>Returns command name for each of the user-visible commands produced by this module.</p>
			<p>See also @'TextInteractor::CommandNames'.</p>
	*/
	inline	const SimpleEmbeddedObjectStyleMarker::CommandNames&	SimpleEmbeddedObjectStyleMarker::GetCommandNames ()
		{
			return sCommandNames;
		}
	/*
	@METHOD:		SimpleEmbeddedObjectStyleMarker::SetCommandNames
	@DESCRIPTION:	<p>See @'SimpleEmbeddedObjectStyleMarker::GetCommandNames'.</p>
	*/
	inline	void	SimpleEmbeddedObjectStyleMarker::SetCommandNames (const SimpleEmbeddedObjectStyleMarker::CommandNames& cmdNames)
		{
			sCommandNames = cmdNames;
		}





#if		qMacOS || qWindows
// class StandardMacPictureStyleMarker
	inline	StandardMacPictureStyleMarker::PictureHandle	StandardMacPictureStyleMarker::GetPictureHandle () const
		{
			Led_EnsureNotNil (fPictureHandle);
			return fPictureHandle;
		}
	inline	size_t	StandardMacPictureStyleMarker::GetPictureByteSize () const
		{
			#if		qMacOS
				return ::GetHandleSize (Handle (fPictureHandle));
			#elif	qWindows
				return fPictureSize;	// cannot use ::GlobalSize () since that sometimes returns result larger than
										// actual picture size (rounds up)
			#endif
		}
#endif


// class StandardDIBStyleMarker
	inline	const Led_DIB*	StandardDIBStyleMarker::GetDIBData () const	
		{
			Led_EnsureNotNil (fDIBData);
			return (fDIBData);
		}




#if		qMacOS || qWindows
// class StandardMacPictureWithURLStyleMarker
	inline	StandardMacPictureStyleMarker::PictureHandle	StandardMacPictureWithURLStyleMarker::GetPictureHandle () const
		{
			Led_EnsureNotNil (fPictureHandle);
			return fPictureHandle;
		}
	inline	size_t	StandardMacPictureWithURLStyleMarker::GetPictureByteSize () const
		{
			#if		qMacOS
				return ::GetHandleSize (Handle (fPictureHandle));
			#elif	qWindows
				return fPictureSize;	// cannot use ::GlobalSize () since that sometimes returns result larger than
										// actual picture size (rounds up)
			#endif
		}
#endif



// class StandardDIBWithURLStyleMarker
	inline	const Led_DIB*	StandardDIBWithURLStyleMarker::GetDIBData () const	
		{
			Led_EnsureNotNil (fDIBData);
			return (fDIBData);
		}



// class StandardUnknownTypeStyleMarker
	/*
	@METHOD:		StandardUnknownTypeStyleMarker::GetShownSize
	@DESCRIPTION:	<p>Return the size in TWIPS
				of this embeddings display. Defaults to some size appropriate for the picture drawn. But sometimes
				(like in reading RTF files which contain size annotations), we select an appropriate size.</p>
					<p>See @'StandardUnknownTypeStyleMarker::SetShownSize'
	*/
	inline	Led_TWIPS_Point	StandardUnknownTypeStyleMarker::GetShownSize () const
		{
			return fShownSize;
		}
	inline	const void*	StandardUnknownTypeStyleMarker::GetData () const
		{
			return fData;
		}
	inline	size_t	StandardUnknownTypeStyleMarker::GetDataLength () const
		{
			return fLength;
		}

#if		qLedUsesNamespaces
}
#endif

#endif	/*__StyledTextEmbeddedObjects_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
