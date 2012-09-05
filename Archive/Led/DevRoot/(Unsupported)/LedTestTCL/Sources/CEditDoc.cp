/******************************************************************************
	CEditDoc.c
	
	Document methods for a tiny editor.
		
	Copyright © 1989 Symantec Corporation. All rights reserved.

 ******************************************************************************/

#include	<Resources.h>

#include "Global.h"
#include "Commands.h"
#include "CApplication.h"
#include "CBartender.h"
#include "CDataFile.h"
#include "CResFile.h"
#include "CDecorator.h"
#include "CDesktop.h"
#include "CError.h"
#include "CPanorama.h"
#include "CScrollPane.h"
#include "TBUtilities.h"
#include "CEditDoc.h"
#include "CEditPane.h"
#include "CWindow.h"

#define	WINDculture		500		/* Resource ID for WIND template */

extern	CApplication *gApplication;	/* The application */
extern	CBartender	*gBartender;	/* The menu handling object */
extern	CDecorator	*gDecorator;	/* Window dressing object	*/
extern	CDesktop	*gDesktop;		/* The enclosure for all windows */
extern	CBureaucrat	*gGopher;		/* The current boss in the chain of command */
extern	OSType		gSignature;		/* The application's signature */
extern	CError		*gError;		/* The error handling object */

#if		!qUseBuiltinTE
	#include	"StyledTextIO.hh"
#endif



// So I can test it!!!- Later - add popup to save file dialog for type, and detect type on read,
// so we can read all types...

#define	qTmpHackForceLedFormatFiles	0

#ifndef	qTmpHackForceLedFormatFiles
	#define	qTmpHackForceLedFormatFiles	0
#endif




// For now, define duplicated here and in CEditDoc.cpp
#define	kPickInputFileFormat_AlertID	9493





/***
 * IEditDoc
 *
 *	This is your document's initialization method.
 *	If your document has its own instance variables, initialize
 *	them here.
 *	The least you need to do is invoke the default method.
 *
 ***/
 
void CEditDoc::IEditDoc(CApplication *aSupervisor, Boolean printable)

{
	CDocument::IDocument(aSupervisor, printable);
}


/***
 * NewFile
 *
 *	When the user chooses New from the File menu, the CreateDocument()
 *	method in your Application class will send a newly created document
 *	this message. This method needs to create a new window, ready to
 *	work on a new document.
 *
 *	Since this method and the OpenFile() method share the code for creating
 *	the window, you should use an auxiliary window-building method.
 *
 ***/
void CEditDoc::NewFile(void)

{
		/**
		 **	BuildWindow() is the method that
		 **	does the work of creating a window.
		 **	It's parameter should be the data that
		 **	you want to display in the window.
		 **	Since this is a new window, there's nothing
		 **	to display.
		 **
		 **/

	BuildWindow (NULL, NULL, NULL);

		/**
		 **	Send the window a Select() message to make
		 **	it the active window.
		 **/
	
	itsWindow->Select();
}


/***
 * OpenFile
 *
 *	When the user chooses OpenÉ from the File menu, the OpenDocument()
 *	method in your Application class will let the user choose a file
 *	and then send a newly created document this message. The information
 *	about the file is in the SFReply record.
 *
 *	In this method, you need to open the file and display its contents
 *	in a window. This method uses the auxiliary window-building method.
 *
 ***/


#if		!qUseBuiltinTE
struct	popupPrivateData {
	MenuHandle	mHandle;	/* the popup menu handle */
	short		mID;       	/* the popup menu ID */
};
inline	MenuHandle GetPopUpMenuHandle(ControlHandle thisControl)
	{
		popupPrivateData** theMenuData = (popupPrivateData**)(*thisControl)->contrlData;
		return((*theMenuData)->mHandle);
	}
inline	ControlHandle SnatchHandle (DialogPtr thebox, short theGetItem)
	{
	    short itemtype;
	    Rect itemrect;
	    Handle thandle;
	    GetDItem(thebox, theGetItem, &itemtype, &thandle, &itemrect);
	    return((ControlHandle)thandle);
	}
class	MyReader : public StyledTextIOReader_CheckFileAndQueryUserDesiredFormat {
	public:
		MyReader (SrcStream_* srcStream, SinkStream_* sinkStream, const char* fileName):
			StyledTextIOReader_CheckFileAndQueryUserDesiredFormat (false, srcStream, sinkStream),
			fFileName (fileName)
			{
				Led_AssertNotNil (fileName);
			}

		static	bool					sFirstTime;
		static	Led_Array<const char*> 	sFormats;
		static	int						sPopupItemChosen;

		override	size_t	UserPickFormat (const Led_Array<const char*>& formats)
			{
				sFirstTime = true;
				sFormats = formats;
				
				Str255	tmp;
				tmp[0] = ::strlen (fFileName);
				memcpy (&tmp[1], fFileName, tmp[0]);
				::ParamText (tmp, "\p", "\p", "\p");
				::PositionDialog ('ALRT', kPickInputFileFormat_AlertID);
				::InitCursor();
				#if		defined (powerc)
					static	RoutineDescriptor myDlgProcHookRD = BUILD_ROUTINE_DESCRIPTOR (uppModalFilterProcInfo, DlgFilterProc);
					::Alert (kPickInputFileFormat_AlertID, &myDlgProcHookRD);
				#else
					::Alert (kPickInputFileFormat_AlertID, DlgFilterProc);
				#endif

				return qLedFirstIndex + (sPopupItemChosen-1);
			}
	const char* fFileName;

	static	pascal Boolean	DlgFilterProc (DialogPtr dp, EventRecord* theEvent, short* itemHit)
	{
		Handle	h;
		short	i;
		Rect	r;

		::GetDialogItem (dp, 5, &i, &h, &r);
		if (sFirstTime) {
			MenuHandle		mm	=	GetPopUpMenuHandle ((ControlHandle)h);
			for (size_t i = 0; i < sFormats.GetLength (); i++) {
				Str255	tmp;
				tmp[0] = strlen (sFormats[i + qLedFirstIndex]);
				memcpy (&tmp[1], sFormats[i + qLedFirstIndex], tmp[0]);
				AppendMenu (mm, "\pHiMom");
				SetItem (mm, i+1, tmp);
			}
			::SetControlMinimum ((ControlHandle)h, 1);
			::SetControlMaximum ((ControlHandle)h, i);
			::SetControlValue ((ControlHandle)h, 1);
			sFirstTime = false;
		}
		else {
			sPopupItemChosen = ::GetControlValue ((ControlHandle)h);
		}

		return StdFilterProc (dp, theEvent, itemHit);
	}

};
bool	MyReader::sFirstTime;
Led_Array<const char*> 	MyReader::sFormats;
int	MyReader::sPopupItemChosen;
#endif





void CEditDoc::OpenFile(SFReply *macSFReply)

{
	CDataFile	*theFile;
	Handle		theData;
	Str63		theName;

		/**
		 ** Create a file and send it a SFSpecify()
		 **	message to set up the name, volume, and
		 **	directory.
		 **
		 **/

	theFile = new(CDataFile);

		/**
		 **	Be sure to set the instance variable
		 **	so other methods can use the file if they
		 **	need to. This is especially important if
		 **	you leave the file open in this method.
		 **	If you close the file after reading it, you
		 **	should be sure to set itsFile to NULL.
		 **
		 **/

	itsFile = theFile;

	theFile->IDataFile();
	theFile->SFSpecify(macSFReply);
	

		/**
		 **	Send the file an Open() message to
		 **	open it. You can use the ReadSome() or
		 **	ReadAll() methods to get the contents of the file.
		 **
		 **/
	try_ {
		theFile->Open(fsRdWrPerm);
	}
	catch_all_ () {
		theFile->Open(fsRdPerm);
	}
	end_try_;

#if		qUseBuiltinTE
	if (theFile->GetLength() > 10240L)
	{
		ParamText( "\pCan't open a file this big.", "\p", "\p", "\p");
		PositionDialog('ALRT', 128);
		InitCursor();
		Alert(128, NULL);
		
		delete this;
		return;
	}
#endif

    theData = theFile->ReadAll();     /* ReadAll() creates the handle */


	StScrpHandle	styleInfo	=	NULL;
	#if		qStyledTextEdit
		try {
			CResFile	resFile;
			resFile.SFSpecify(macSFReply);
			resFile.Open (fsRdPerm);
			resFile.MakeCurrent ();
			styleInfo = (StScrpHandle)::Get1Resource ('styl', 128);
			if (styleInfo != NULL) {
				::DetachResource ((Handle)styleInfo);
			}
		}
		catch (...) {
			// ignore any errors - if no res fork, no style data, thats all
		}
	#endif


	try_ {
		itsFile->GetName(theName);
		char	cFileName[256];
		memcpy (cFileName, &theName[1], theName[0]);
		cFileName[theName[0]] = '\0';
		BuildWindow (theData, styleInfo, cFileName);
	}
	catch_all_() {
		theFile->Close ();
		if (theData != NULL) {
			DisposHandle(theData);
			theData = NULL;
		}
	#if		qStyledTextEdit
		::DisposeHandle((Handle) styleInfo);
		styleInfo = NULL;
	#endif
		throw_same_ ();
	}
	end_try_;

		/**
		 **	In your application, you'll probably store
		 **	the data in some form as an instance variable
		 **	in your document class. For this example, there's
		 **	no need to save it, so we'll get rid of it.
		 **
		 **/

	#if		qStyledTextEdit
		::DisposeHandle((Handle) styleInfo);
	#endif
	DisposHandle(theData);

		/**
		 **	In this implementation, we leave the file
		 **	open. You might want to close it after
		 **	you've read in all the data.
		 **
		 **/

	itsFile->GetName(theName);
	itsWindow->SetTitle(theName);
	itsWindow->Select();			/* Don't forget to make the window active */
}



/***
 * BuildWindow
 *
 *	This is the auxiliary window-building method that the
 *	NewFile() and OpenFile() methods use to create a window.
 *
 *	In this implementation, the argument is the data to display.
 *
 ***/

void	CEditDoc::BuildWindow (Handle theData, StScrpHandle styleInfo, const char* fileName)

{
	CScrollPane		*theScrollPane;
	CEditPane		*theMainPane;
//	Rect			margin;

		/**
		 **	First create the window and initialize
		 **	it. The first argument is the resource ID
		 **	of the window. The second argument specifies
		 **	whether the window is a floating window.
		 **	The third argument is the window's enclosure; it
		 **	should always be gDesktop. The last argument is
		 **	the window's supervisor in the Chain of Command;
		 **	it should always be the Document object.
		 **
		 **/

	itsWindow = new(CWindow);
	itsWindow->IWindow(WINDculture, FALSE, gDesktop, this);
	
		/**
		 **	After you create the window, you can use the
		 **	SetSizeRect() message to set the windowÕs maximum
		 **	and minimum size. Be sure to set the max & min
		 **	BEFORE you send a PlaceNewWindow() message to the
		 **	decorator.
		 **
		 ** The default minimum is 100 by 100 pixels. The
		 **	default maximum is the bounds of GrayRgn() (The
		 **	entire display area on all screens.)
		 **
		 **/

#if		qLed_UseCharOffsetsForTCLScrollAPI
	theScrollPane = new Led_ScrollPane (itsWindow, this, 10, 10, 0, 0, sizELASTIC, sizELASTIC, TRUE, TRUE, TRUE);
#else
	theScrollPane = new(CScrollPane);


		/**
		 **	You can initialize a scroll pane two ways:
		 **		1. You can specify all the values
		 **		   right in your code, like this.
		 **		2. You can create a ScPn resource and
		 **		   initialize the pane from the information
		 **		   in the resource.
		 **
		 **/

	theScrollPane->IScrollPane(itsWindow, this, 10, 10, 0, 0,
								sizELASTIC, sizELASTIC,
								TRUE, TRUE, TRUE);
#endif

		/**
		 **	The FitToEnclFrame() method makes the
		 **	scroll pane be as large as its enclosure.
		 **	In this case, the enclosure is the window,
		 **	so the scroll pane will take up the entire
		 **	window.
		 **
		 **/

	theScrollPane->FitToEnclFrame(TRUE, TRUE);


		/**
		 **	itsMainPane is the document's focus
		 **	of attention. Some of the standard
		 **	classes (particularly CPrinter) rely
		 **	on itsMainPane pointing to the main
		 **	pane of your window.
		 **
		 **	itsGopher specifies which object
		 **	should become the gopher. By default
		 **	the document becomes the gopher. ItÕs
		 **	likely that your main pane handles commands
		 **	so youÕll almost want to set itsGopher
		 **	to point to the same object as itsMainPane.
		 **
		 **	Note that the main pane is the
		 **	panorama in the scroll pane and not
		 **	the scroll pane itself.
		 **
		 **/

/**
 **	The IEditPane method automatically
 **	fits the pane to the enclosure and
 **	gives us a little margin.
 **
 **/
	theMainPane = new CEditPane (theScrollPane, this);
	itsMainPane = theMainPane;
	itsGopher = theMainPane;


		/**
		 **	Send the scroll pane an InstallPanorama()
		 **	to associate our pane with the scroll pane.
		 **
		 **/

	theScrollPane->InstallPanorama(theMainPane);

	#if		qUseBuiltinTE
		if (theData)
			theMainPane->SetTextHandle(theData);
	#else

		if (theData != NULL) {
			HLock (theData);
			StyledTextIOSrcStream_Memory					source (*theData, ::GetHandleSize (theData), Handle (styleInfo));
			StyledTextIOSinkStream_StandardStyledTextImager	sink (theMainPane);
			#if		qTmpHackForceLedFormatFiles
				StyledTextIOReader_LedNativeFileFormat	textReader (&source, &sink);
			#else
				MyReader	textReader (&source, &sink, fileName);
			#endif
			textReader.Read ();
			HUnlock (theData);
		}
	#endif



		/**
		 **	The Decorator is a global object that takes care
		 **	of placing and sizing windows on the screen.
		 **	You don't have to use it.
		 **
		 **/

	gDecorator->PlaceNewWindow(itsWindow);
}



/***
 * DoSave
 *
 *	This method handles what happens when the user chooses Save from the
 *	File menu. This method should return TRUE if the file save was successful.
 *	If there is no file associated with the document, you should send a
 *	DoSaveFileAs() message.
 *
 ***/

Boolean CEditDoc::DoSave(void)

{
	Handle		theData;

	if (itsFile == NULL) {
		return(DoSaveFileAs());
	}
	else {
		StyledTextIOWriterSrcStream_StandardStyledTextImager	source (((CEditPane *)itsMainPane));
		StyledTextIOWriterSinkStream_Memory						sink;
		#if		qTmpHackForceLedFormatFiles
		StyledTextIOWriter_LedNativeFileFormat					textWriter (&source, &sink);
		#else
		StyledTextIOWriter_PlainText							textWriter (&source, &sink);
		#endif
		textWriter.Write ();
		((CDataFile *)itsFile)->SetMark (0, fsFromStart);
		((CDataFile *)itsFile)->WriteSome((char*)sink.PeekAtData (), sink.GetLength ());
		((CDataFile *)itsFile)->SetLength (sink.GetLength ());

		#if		qStyledTextEdit
			{
				CResFile	resFile;
				FSSpec		fsp;
				itsFile->GetFSSpec (&fsp);
				resFile.SpecifyFSSpec (&fsp);
				if (not resFile.HasResFork ()) {
					resFile.CreateNew ('LED ', 'TEXT');
				}
				resFile.Open (fsRdWrPerm);
				resFile.MakeCurrent ();

				StScrpHandle	macStyleHandle		=	 (StScrpHandle)::Get1Resource ('styl', 128);
				if (macStyleHandle != NULL) {
					::RmveResource (Handle (macStyleHandle));	// remove old styl resource
				}

				Led_Array<StandardStyledTextImager::InfoSummaryRecord>
											ledStyleRuns	=	((CEditPane *)itsMainPane)->GetStyleInfo (qLedFirstIndex, ((CEditPane *)itsMainPane)->GetLength ());
				size_t						nStyleRuns		=	ledStyleRuns.GetLength ();
				
				
				
				Led_Assert (offsetof (StScrpRec, scrpStyleTab) == sizeof (short));	// thats why we add sizeof (short)
				macStyleHandle	=	(StScrpHandle)::NewHandle (sizeof (short) + nStyleRuns*sizeof (ScrpSTElement));
				HLock (Handle (macStyleHandle));
				(*macStyleHandle)->scrpNStyles = nStyleRuns;
				((CEditPane *)itsMainPane)->Convert (ledStyleRuns, (*macStyleHandle)->scrpStyleTab);
				HUnlock (Handle (macStyleHandle));

				::AddResource (Handle (macStyleHandle), 'styl', 128, "\p");
				FailOSErr (ResError ());

			}
		#endif




		dirty = FALSE;					/* Document is no longer dirty		*/
		gBartender->DisableCmd(cmdSave);
		return(TRUE);					/* Save was successful				*/
	}
}


/***
 * DoSaveAs
 *
 *	This method handles what happens when the user chooses Save AsÉ from
 *	File menu. The default DoCommand() method for documents sends a DoSaveFileAs()
 *	message which displays a standard put file dialog and sends this message.
 *	The SFReply record contains all the information about the file you're about
 *	to create.
 *
 ***/

Boolean CEditDoc::DoSaveAs(SFReply *macSFReply)

{
		/**
		 **	If there's a file associated with this document
		 **	already, close it. The Dispose() method for files
		 **	sends a Close() message to the file before releasing
		 **	its memory.
		 **
		 **/
		 
	TCLForgetObject(itsFile);


		/**
		 **	Create a new file, and then save it normally.
		 **
		 **/

	itsFile = new(CDataFile);
	((CDataFile *)itsFile)->IDataFile();
	itsFile->SFSpecify(macSFReply);
	itsFile->CreateNew(gSignature, 'TEXT');
	itsFile->Open(fsRdWrPerm);
	
	itsWindow->SetTitle(macSFReply->fName);

	return( DoSave() );
}
