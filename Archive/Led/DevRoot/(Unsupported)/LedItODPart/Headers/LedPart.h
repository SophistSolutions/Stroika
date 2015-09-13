/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Headers/LedPart.h,v 2.10 1996/12/13 18:09:57 lewis Exp $
 *
 * Description:
 *		LedPart OpenDoc part editor class definition.
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedPart.h,v $
 *	Revision 2.10  1996/12/13 18:09:57  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.9  1996/12/05  21:11:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/09/01  15:44:28  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/03/16  19:29:17  lewis
 *	OnEnterFindStringCommand
 *
 *	Revision 2.6  1996/02/05  05:03:39  lewis
 *	Fixed up menubar stuff abit based on DR4Final CD.
 *	Find dialog support
 *	Preferences dialog support.
 *	qUseResFileHackToEnableBaloonHelp.
 *
 *	Revision 2.5  1996/01/22  05:49:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/01/04  00:22:03  lewis
 *	Massive cleanups - including using od_ for OD method calls (SOM), and
 *	losing Environement* crap.
 *
 *	Revision 2.3  1995/12/15  03:59:28  lewis
 *	Massive diffs.
 *	Changes including fPrinter support (primitive)
 *	DragDrop initiation, and DragWIthin support.
 *	fShapeList now owned by LedContent object
 *	much more...
 *
 *	Revision 2.2  1995/12/13  06:10:00  lewis
 *	LOTSA cleanups.
 *	starting using new od_ convention for stuff called from SOM.
 *	Losing Environemt* args.
 *	Lost CSelection class.
 *	Much more.
 *
 *	Revision 2.1  1995/12/06  02:00:23  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *	Revision 2.0  1995/11/25  00:39:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1995/11/02  22:45:43  lewis
 *	Support for shared font style database.
 *	Revised alot of the Internalize/Externalize code (1/2?)
 *
 *	Revision 2.1  1995/10/09  23:04:18  lewis
 *	Large amounts of changes. Lots of cleanups. Got scrollbars mostly
 *	wroking, and setport update problems fixed and mutliple linked frames,
 *	and cleanued up commands and supported font menus.
 *
 * Revision 2.0  1995/09/06  21:05:39  lewis
 * *** empty log message ***
 *
 *
 *
 *
 */
#ifndef __LedPart_hh__
#define __LedPart_hh__	1

#include	<Types.h>

#include	<ODTypes.h>


#include	"StyledTextImager.h"
#include	"LedSupport.h"


class	ODFacet;
class	ODCanvas;
class	ODFocusSet;
class	ODFrame;
class	ODMenuBar;
class	ODShape;
class	ODStorageUnit;
class	ODWindow;
class	ODSession;
class	ODDraft;
class	ODTypeList;
class	ODLinkSource;
class	ODLink;
class	ODStorageUnitView;
class	ODDragItemIterator;
class	ODObjectSpec;
class	ODExtension;

struct	WindowProperties;
class	FrameEditor;
class	Embedding;
class	CCommand;
class	CloneInfo;
class	COrderedList;
class	LedContent;
class	CPrinter;



/*
 *	Based on email from Jens <jens_alfke@powertalk.apple.com> and
 *	Troy Gaul <tgaul@apple.com>, Feb 1, 1996, and notes on the OpenDoc mailing
 *	list, it appears the new - approved way of doing this is to
 *	save/restore lib resources state on AdjustMenus/HandleEvent.
 */
#ifndef	qUseResFileHackToEnableBaloonHelp
	#define	qUseResFileHackToEnableBaloonHelp	1
#endif



class LedPart {
	public:
		LedPart();
		~LedPart();
		
		nonvirtual	void	od_InitPart (ODStorageUnit* storageUnit, ODPart* partWrapper);
		nonvirtual	void	od_InitPartFromStorage (ODStorageUnit* storageUnit, ODPart* partWrapper);

	// OpenDoc extensions
	public:
		nonvirtual	bool			od_HasExtension (ODType extensionName);
		nonvirtual	void			od_ReleaseExtension (ODExtension* extension);
		nonvirtual	ODExtension*	od_AcquireExtension (ODType extensionName);

	public:
		nonvirtual	ODPart*	GetODPart () const;

		// -- Accessors --
		nonvirtual	ODBoolean	IsReadOnly();
	
		nonvirtual	ODSession*	GetSession() const;
		nonvirtual	ODDraft*	GetDraft() const;
		
		nonvirtual	COrderedList* GetShapeList() const;
		nonvirtual	COrderedList* GetEmbeddedFrames() const;
		nonvirtual	COrderedList* GetContentDisplayFrames () const;

		nonvirtual	ODFrame* GetFirstSourceFrame ();
		
		nonvirtual	ODULong		GetGroupCount() const;		//	Return the last GroupID generated by this part instance
		nonvirtual	ODULong		GetNewGroupID();			//	Return the next GroupID generated by this part instance.
			
		// -- Storage --
		
		nonvirtual	void		SetDirty();
		
		nonvirtual	void		od_Release ();
		nonvirtual	void		od_ReleaseAll ();
		nonvirtual	ODSize		od_Purge (ODSize size);
		nonvirtual	void		od_Externalize();
		nonvirtual	void		od_ClonePartInfo (ODDraftKey key, ODInfoType partInfo, ODStorageUnitView* storageUnitView, ODFrame* scopeFrame);
									
		nonvirtual	void		od_CloneInto (ODDraftKey key, ODStorageUnit* destinationSU, ODFrame* scopeFrame);
		nonvirtual	void		od_WritePartInfo (ODInfoType partInfo, ODStorageUnitView* storageUnitView);
		nonvirtual	ODInfoType	od_ReadPartInfo (ODFrame* frame, ODStorageUnitView* storageUnitView);
		nonvirtual	void		od_ExternalizeKinds (ODTypeList* kindset);
		nonvirtual	void		od_ChangeKind (ODType kind);
		
		// -- Promises --
		nonvirtual	void	od_FulfillPromise (ODStorageUnitView* promiseSUView);
		
		// -- Layout --
	public:
		nonvirtual	void	od_DisplayFrameAdded (ODFrame* frame);
		nonvirtual	void	od_DisplayFrameRemoved (ODFrame* frame);
		nonvirtual	void	od_DisplayFrameClosed (ODFrame* frame);
		nonvirtual	void	od_DisplayFrameConnected (ODFrame* frame);
		nonvirtual	void	od_AttachSourceFrame (ODFrame* frame, ODFrame* sourceFrame);
		nonvirtual	void	od_ViewTypeChanged (ODFrame* frame);
		nonvirtual	void	od_PresentationChanged (ODFrame* frame);
		nonvirtual	void	od_SequenceChanged (ODFrame* frame);
		nonvirtual	void	od_FrameShapeChanged (ODFrame* frame);
		nonvirtual	ODID	od_Open (ODFrame* frame);
		nonvirtual	ODWindow*	AcquireFramesWindow(ODFrame* frame);
			
		// -- Imaging --
		
		nonvirtual	void	od_Draw (ODFacet* facet, ODShape* invalidShape);
		nonvirtual	void	od_GeometryChanged (ODFacet* facet, ODBoolean clipShapeChanged, ODBoolean externalTransformChanged);
		nonvirtual	void	od_HighlightChanged (ODFacet* facet);
		nonvirtual	void	od_FacetAdded (ODFacet* facet);
		nonvirtual	void	od_FacetRemoved (ODFacet* facet);
		nonvirtual	void	od_CanvasChanged (ODFacet* facet);
		nonvirtual	void	od_CanvasUpdated (ODCanvas* canvas);
		
		// -- Activation --
		
		nonvirtual	bool	od_BeginRelinquishFocus (ODTypeToken focus, ODFrame* ownerFrame, ODFrame* proposedFrame);
		nonvirtual	void	od_CommitRelinquishFocus (ODTypeToken focus, ODFrame* ownerFrame, ODFrame* proposedFrame);
		nonvirtual	void	od_AbortRelinquishFocus (ODTypeToken focus, ODFrame* ownerFrame, ODFrame* proposedFrame);
		nonvirtual	void	od_FocusAcquired (ODTypeToken focus, ODFrame* ownerFrame);
		nonvirtual	void	od_FocusLost (ODTypeToken focus, ODFrame* ownerFrame);
		nonvirtual	void	RelinquishAllFoci (ODFrame* frame);
		nonvirtual	void	PartActivated (ODFrame*	frame );
		
		// -- Event Handling --
		nonvirtual	bool	od_HandleEvent (ODEventData* event, ODFrame* frame, ODFacet* facet, ODEventInfo* eventInfo);
		nonvirtual	void	od_AdjustMenus (ODFrame* frame);

	/*
	 *	Command handling.
	 */
	public:
		nonvirtual	void	AdjustAboutBoxCommand ();
		nonvirtual	void	OnAboutBoxCommand (ODFrame* frame);

		nonvirtual	void	AdjustOpenSelectionCommand (ODFrame* frame);
		nonvirtual	void	OnOpenSelectionCommand (ODFrame* frame);

		nonvirtual	void	AdjustSelectAllCommand (ODFrame* frame);
		nonvirtual	void	OnSelectAllCommand (ODFrame* frame);

		nonvirtual	void	AdjustClearCommand (ODFrame* frame);
		nonvirtual	void	OnClearCommand (ODFrame* frame);

		nonvirtual	void	AdjustClipboardCommands (ODFrame* frame);
		nonvirtual	void	OnCutCommand (ODFrame* frame);
		nonvirtual	void	OnCopyCommand (ODFrame* frame);
		nonvirtual	void	OnPasteCommand (ODFrame* frame);
		nonvirtual	void	OnPasteAsCommand (ODFrame* frame);

		nonvirtual	void	AdjustGetPartInfoCommand (ODFrame* frame);
		nonvirtual	void	OnGetPartInfoCommand (ODFrame* frame);

		nonvirtual	void	AdjustPreferencesCommand (ODFrame* frame);
		nonvirtual	void	OnPreferencesCommand (ODFrame* frame);

		nonvirtual	void	AdjustViewAsWindowCommand (ODFrame* frame);
		nonvirtual	void	OnViewAsWindowCommand (ODFrame* frame);

	public:
		nonvirtual	void	AdjustFindEtcCommand (ODFrame* frame);
		nonvirtual	void	OnFindEtcCommand (ODFrame* frame, ODCommandID theMenuCommand);
		nonvirtual	void	OnEnterFindStringCommand (ODFrame* frame);
	private:
		char*	fLastSearchText;
		bool	fWrapSearch;
		bool	fWholeWordSearch;
		bool	fCaseSensativeSearch;

	public:
		nonvirtual	void	AdjustFontCommands (ODFrame* frame);
		nonvirtual	void	OnFontCommand (ODFrame* frame, ODCommandID theMenuCommand);


	public:
		nonvirtual	bool	od_RevealFrame (ODFrame* embeddedFrame, ODShape* revealShape);
		nonvirtual	void	od_EmbeddedFrameSpec (ODFrame* embeddedFrame, ODObjectSpec* spec);

	public:
		// -- Drag & Drop --
		nonvirtual	void			od_DropCompleted (ODPart* part, ODDropResult result);
		nonvirtual	ODDragResult	od_DragEnter(ODDragItemIterator* dragInfo, ODFacet* facet, ODPoint* where);
		nonvirtual	ODDragResult	od_DragWithin (ODDragItemIterator* dragInfo, ODFacet* facet, ODPoint* where);
		nonvirtual	void			od_DragLeave (ODFacet* facet, ODPoint*  where);
		nonvirtual	ODDropResult	od_Drop (ODDragItemIterator* dragInfo, ODFacet* facet, ODPoint*  where);
		
		
		// -- Commands --
		nonvirtual	void	ExecuteCommand (CCommand* command);
		nonvirtual	void	od_UndoAction (ODActionData* actionState);
		nonvirtual	void	od_RedoAction (ODActionData* actionState);
		nonvirtual	void	od_DisposeActionState (ODActionData* actionState, ODDoneState doneState);
		// Embedding		
		nonvirtual	ODShape*	od_AdjustBorderShape (ODFacet *embeddedFacet, ODShape* borderShape);
		nonvirtual	void	 	InvalidateActiveBorder(ODFrame *frame);
								
		nonvirtual	void 		ClipEmbeddedFacets(ODFrame* containingFrame);
		nonvirtual	void 		ClipEmbeddedFacets(ODFacet *containingFacet);
		nonvirtual	void 		od_RemoveEmbeddedFrame (ODFrame* embeddedFrame);
		nonvirtual	ODShape*	od_RequestFrameShape (ODFrame* embeddedFrame, ODShape* frameShape);
		nonvirtual	ODFrame* 	od_RequestEmbeddedFrame (ODFrame* containingFrame, ODFrame* baseFrame,
												ODShape* frameShape, ODPart* embedPart, ODTypeToken viewType,
												ODTypeToken presentation, ODBoolean isOverlaid
											);
		nonvirtual	void	 	od_UsedShapeChanged (ODFrame* embeddedFrame);
						
		nonvirtual	Embedding*	ShapeForFrame (ODFrame* embeddedFrame);
		
		// Linking	OpenDoc API
		nonvirtual	void	 		od_EmbeddedFrameUpdated (ODFrame* frame, ODUpdateID updateID);
		nonvirtual	ODLinkSource*	od_CreateLink (ODByteArray* data);



	public:
		nonvirtual	CPrinter*	GetPrinter ();
		nonvirtual	void		Print (ODFrame* frame);
	private:
		CPrinter*		fPrinter;



	private:
		nonvirtual	void	Initialize (ODStorageUnit* storageUnit);
		nonvirtual	void	ReBuildMenuBar ();


	// private storage utils...
	private:
		nonvirtual	void		CheckAndAddProperties (ODStorageUnit* storageUnit);
		nonvirtual	void		CleanseContentProperty (ODStorageUnit* storageUnit);
		nonvirtual	void		InternalizeStateInfo (ODStorageUnit* storageUnit);
		nonvirtual	void		InternalizeContent(ODStorageUnit* storageUnit);
		nonvirtual	void		ExternalizeStateInfo (ODStorageUnit* storageUnit);
		nonvirtual	void		ExternalizeContent( ODStorageUnit* storageUnit, CloneInfo* cloneInfo);	


	// -- Event Handling --
	protected:
		nonvirtual	bool	HandleMenuEvent (ODEventData* event, ODFrame* frame);
		nonvirtual	bool	HandleMouseEvent (ODEventData* event, ODFacet* facet, ODEventInfo* eventInfo);
		nonvirtual	bool	HandleWindowEvent (ODEventData* event, ODFrame* frame);
		nonvirtual	void	HandleMouseDown (ODFacet* facet, ODEventData* event, ODEventInfo*	eventInfo );
		nonvirtual	bool	HandleMouseDownInEmbeddedFrame (ODFacet* facet, ODEventData* event, ODEventInfo* eventInfo);
		nonvirtual	bool	HandleMouseDownInEmbeddedFrameBorder (ODFacet* facet, ODEventData* event, ODEventInfo* eventInfo);
		nonvirtual	bool	HandleMouseUpActivate (ODFacet* facet);

	public:
		// -- Dialogs --
		nonvirtual	ODBoolean	PrepareForDialog (ODFrame* frame );
		nonvirtual	void		DoneWithDialog (ODFrame* frame );
		nonvirtual	bool		DoDialogBox (ODFrame* frame,
								  			ODSShort dialogID, ODUShort errorNumber = 0,
								  			const char* paramText1 = NULL,
								  const char* paramText2 = NULL
						);
		
		nonvirtual	void		DrawIconView (ODFacet* facet);
		nonvirtual	void		DrawThumbnailView (ODFacet* facet);
		nonvirtual	void		DrawFrameView (ODFacet* facet, ODShape* invalidShape);
		
		// -- Activation --
		
		nonvirtual	ODBoolean	ActivateFrame (ODFrame* frame);
		nonvirtual	void		WindowActivating (ODFrame* frame, ODBoolean activating);
		
		// -- Layout --
		
		nonvirtual	ODWindow*	CreateWindow (ODFrame* frame, ODType frameType, WindowProperties* windowProperties);
		nonvirtual	void		CleanupWindow (ODFrame* frame);
		nonvirtual	WindowProperties*	GetDefaultWindowProperties (ODFrame*		sourceFrame, Rect*			windowRect );
		nonvirtual	WindowProperties*	GetSavedWindowProperties(ODFrame* frame);
		nonvirtual	Rect		CalcPartWindowSize (ODFrame* sourceFrame);
		nonvirtual	Rect		CalcPartWindowPosition(ODFrame* frame, Rect* partWindowBounds);
		nonvirtual	ODFacet*	GetActiveFacetForFrame(ODFrame* frame);
		nonvirtual	ODShape*	CalcNewUsedShape(ODFrame* frame);
		nonvirtual	void		UpdateFrame(ODFrame* frame, ODTypeToken view, ODShape* usedShape);
		nonvirtual	void		CleanupDisplayFrame(ODFrame* frame, ODBoolean frameRemoved);


	/*
	 *	Object linking support.
	 */
	public:
		nonvirtual	void	od_LinkUpdated (ODLink* odLink, ODUpdateID change);
		nonvirtual	void	od_RevealLink (ODLinkSource* linkSource);
		nonvirtual	bool	od_EditInLinkAttempted (ODFrame* frame);
		nonvirtual	void	od_LinkStatusChanged (ODFrame* frame);

	public: 
		nonvirtual	void	AddShape (Embedding* shape);
		nonvirtual	void	RemoveShape (Embedding* shape);


	/*
	 *	Keep track of what embeddings are currently displayed on the screen (and therefore
	 *	are internalized, and in memory, etc. And maybe need to be thrown away due
	 *	to scrolling.
	 */
	public:
		nonvirtual	void	NoteThatEmbeddingHasBeenDrawn (Embedding* e);
		nonvirtual	void	NoteThatEmbeddingHasNOTBeenDrawn (Embedding* e);

		nonvirtual	const Led_Array<Embedding*>&	GetOutstandingDrawnEmbeddings ();
	private:
		Led_Array<Embedding*>	fEmbeddingsLoaded;


	public:
		Led_Array<FrameEditor*>	fFrameEditors;
	
	private:
		// Since we are lazily internalizing display frames, 
		// this is actually a list of CDisplayFrameProxy
		// objects, rather than ODFrames.
		COrderedList*		fDisplayFrameProxies;
		
		ODBoolean			fDirty;
		ODPart*				fSelf;
		ODBoolean			fReadOnlyStorage;
		
		
		// Content data
		ODULong				fGroupCount;
		
		ODFacet*			fHilightedDragFacet;
		ODShape*			fActiveBorderShape;
		
		COrderedList* 		fEmbeddedFrames;
	
	public:
		LedContent* 		fPartContent;

	

	public:
		nonvirtual	TextStore_&										GetTextStore () const;
		nonvirtual	StandardStyledTextImager::SharedStyleDatabase&	GetStyleDatabase () const;


	public:
		nonvirtual	FrameEditor&	LookupFrameEditor (ODFrame* frame) const;	// ERROR if no corresponding editor to that frame...


	private:
		nonvirtual	bool	DisplayingAsFrameMode (ODFrame* frame) const;


	friend	class	FrameEditor;
};






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	<Frame.xh>

inline	ODPart*	LedPart::GetODPart () const
	{
		Led_EnsureNotNil (fSelf);
		return fSelf;
	}

inline	FrameEditor&	LedPart::LookupFrameEditor (ODFrame* frame) const	// ERROR if no corresponding editor to that frame...
	{
		Environment*	ev = ::somGetGlobalEnvironment();
		Led_AssertNotNil (ev);
	
		Led_RequireNotNil (frame);
		FrameEditor*	frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
		Led_RequireNotNil (frameInfo);
		return *frameInfo;
	}
inline	const Led_Array<Embedding*>&	LedPart::GetOutstandingDrawnEmbeddings ()
	{
		return fEmbeddingsLoaded;
	}
inline	ODBoolean LedPart::IsReadOnly()
	{
		return fReadOnlyStorage;
	}
inline	ODULong LedPart::GetGroupCount() const
	{
		return fGroupCount;
	}
inline	ODULong LedPart::GetNewGroupID()
	{
		fGroupCount++;
		return GetGroupCount();
	}



#endif	/*__LedPart_hh__*/


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
