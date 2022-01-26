/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PathNameSelecters.cc,v 1.8 1992/09/11 19:46:51 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PathNameSelecters.cc,v $
 *		Revision 1.8  1992/09/11  19:46:51  sterling
 *		Get rid of calls to peek.
 *
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  05:32:33  lewis
 *		Clean PathName stuff up after further imporvments to PathName
 *		stuff. Use PathName overloads directly, and no longer refer to HFSPathComponent
 *		directly.
 *
 *		Revision 1.3  1992/07/15  22:01:28  lewis
 *		Hack use of HFSPathComponent further- dont use its string arg - make pathname
 *		with first part HFSPathComponent for direcotyr, and simple path compoennt
 *		for name - will cleanup pathname stuff to make this easier then come back and
 *		fix here - but works for now.
 *
 *		Revision 1.2  1992/07/14  19:55:08  lewis
 *		Lots of changes - mostly cosmetic - and changes to the usage of the file
 *		stuff dicated by changes in File.hh - also, make Pose () raise exception
 *		on cancel instead of returning Boolean. This needs more work - we must
 *		define our own excpetion here, but for now, hack and use externed one from
 *		Document.cc
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.14  1992/04/20  14:27:39  lewis
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *
 *		Revision 1.13  92/04/02  13:07:29  13:07:29  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.12  92/03/10  13:12:06  13:12:06  lewis (Lewis Pringle)
 *		Support new args to ProcessKey (in eventhandle subclasses).
 *		
 *		Revision 1.11  1992/03/10  00:33:37  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.10  1992/02/24  07:51:16  lewis
 *		Properly set dialog title under motif for pathname pickers.
 *
 *		Revision 1.9  1992/01/28  20:00:34  lewis
 *		Misc cleanups.
 *
 *		Revision 1.8  1992/01/26  15:23:26  lewis
 *		Fixed bug with sfgetfile boxes for motif where they used the main window shell. Now we create our own.
 *		Also, changed title to message field from "sloppy joe".
 *
 *		Revision 1.7  1992/01/10  14:52:21  lewis
 *		Implemented pathname selectors for motif.
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Files.h>
#include	<StandardFile.h>
#include	<SysEqu.h>
#elif	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/FileSB.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"



#include	"Debug.hh"

#include	"Dialog.hh"
#include	"StreamUtils.hh"

#include	"EventManager.hh"

#include	"PathNameSelecters.hh"




#if		qXmToolkit
class	FileSelectionBoxEventHandler : public EventHandler {
	public:
		FileSelectionBoxEventHandler (osWidget* fileSelectionBox, const PathName& pathName, const String& dlogTitle);
		~FileSelectionBoxEventHandler ();

		nonvirtual	Boolean	Pose ();

		nonvirtual	PathName	GetPathName () const;

		override	void	ProcessMousePress (const osEventRecord& eventRecord, const Point& where, Boolean isDown,
											   const KeyBoard& keyBoardState, UInt8 clickCount);
		override	void	ProcessMouseMoved (const osEventRecord& eventRecord, const Point& newLocation,
											   const KeyBoard& keyBoardState, Region& newMouseRegion);
		override	void	ProcessKey (const osEventRecord& eventRecord, KeyBoard::KeyCode keyCode, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState);


		static	void	OKCallBack (osWidget* w, char* client_data, char* call_data);
		static	void	CancelCallBack (osWidget* w, char* client_data, char* call_data);

	private:
		osWidget*	fFileSelectionBox;
		Boolean		fOKHit;
		PathName	fPathName;
};
#endif	/*qXmToolkit*/





extern	Exception	gUserCancelCloseException;	// reference exception from Document.cc - this is just a temp
												// hack - find a better place for this...





/*
 ********************************************************************************
 ***************************** AbstractNewPathNameGetter ************************
 ********************************************************************************
 */
AbstractNewPathNameGetter::AbstractNewPathNameGetter ()
{
}





/*
 ********************************************************************************
 ***************************** AbstractOldPathNameGetter ************************
 ********************************************************************************
 */
AbstractOldPathNameGetter::AbstractOldPathNameGetter ()
{
}





/*
 ********************************************************************************
 ******************************* NewPathNameGetter_MacUI ************************
 ********************************************************************************
 */

NewPathNameGetter_MacUI::NewPathNameGetter_MacUI ():
	AbstractNewPathNameGetter ()
{
}







/*
 ********************************************************************************
 ******************************* OldPathNameGetter_MacUI ************************
 ********************************************************************************
 */

OldPathNameGetter_MacUI::OldPathNameGetter_MacUI ():
	AbstractOldPathNameGetter ()
{
}






#if		qMacToolkit

/*
 ********************************************************************************
 **************************** NewPathNameGetter_MacUI_Native ********************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable
NewPathNameGetter_MacUI_Native::NewPathNameGetter_MacUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName):
#else
NewPathNameGetter_MacUI_Native::NewPathNameGetter_MacUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName):
#endif
	NewPathNameGetter_MacUI (),
	fFileTypes (fileTypes),
	fPathName (pathName),
	fMessage ("Choose New ")
{
	if (fileTypes == kDirectoryFileTypeSet) {
		SetMessage (GetMessage () + "Directory");
	}
	else {
		SetMessage (GetMessage () + "File");
	}
}

#if		qRealTemplatesAvailable
const AbSet<FileSystem::FileType>&	NewPathNameGetter_MacUI_Native::GetFileTypes () const
#else
const AbSet(FileTypeX)&				NewPathNameGetter_MacUI_Native::GetFileTypes () const
#endif
{
	return (fFileTypes);
}

#if		qRealTemplatesAvailable
void	NewPathNameGetter_MacUI_Native::SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes)
#else
void	NewPathNameGetter_MacUI_Native::SetFileTypes (const AbSet(FileTypeX)& fileTypes)
#endif
{
	fFileTypes = fileTypes;
}

PathName	NewPathNameGetter_MacUI_Native::GetPathName () const
{
	return (fPathName);
}

void	NewPathNameGetter_MacUI_Native::SetPathName (const PathName& pathName)
{
	fPathName = pathName;
}

void	NewPathNameGetter_MacUI_Native::Pose ()
{
	osPoint		dlogWhere	= {20, 20};
	SFTypeList	typeList;
	SFReply		reply;	

	osStr255	origName;
	fPathName.GetFileName ().GetName ().ToPStringTrunc (origName, sizeof (origName));

	osStr255	message;
	GetMessage ().ToPStringTrunc (message, sizeof (message));

	::SFPutFile (dlogWhere, message, origName, NULL, &reply);
	if (reply.good) {
// not great- maybe we should use some File utility to map from wdrefnum to vrefnum/dirID
		if (GetFileTypes () == kDirectoryFileTypeSet) {
			// hack til we munge sfgetfile to add "CURRENT-DIR" button, etc.. like MPW
			fPathName = PathName (-*(short*)SFSaveDisk, *(long *)CurDirStore);
		}
		else {
			fPathName = PathName (-*(short*)SFSaveDisk, *(long *)CurDirStore, String (&reply.fName[1], reply.fName[0]));
		}
	}
	else {
		gUserCancelCloseException.Raise ();
	}
}

String	NewPathNameGetter_MacUI_Native::GetMessage () const
{
	return (fMessage);
}

void	NewPathNameGetter_MacUI_Native::SetMessage (const String& message)
{
	fMessage = message;
}








/*
 ********************************************************************************
 **************************** OldPathNameGetter_MacUI_Native ********************
 ********************************************************************************
 */

OldPathNameGetter_MacUI_Native*	OldPathNameGetter_MacUI_Native::sCurrenGetter	=	Nil;
#if		qRealTemplatesAvailable
OldPathNameGetter_MacUI_Native::OldPathNameGetter_MacUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName):
#else
OldPathNameGetter_MacUI_Native::OldPathNameGetter_MacUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName):
#endif
	OldPathNameGetter_MacUI (),
	fFileTypes (fileTypes),
	fPathName (pathName),
	fMessage ("Choose Old")
{
	if (fileTypes == kDirectoryFileTypeSet) {
		SetMessage (GetMessage () + "Directory");
	}
	else {
		SetMessage (GetMessage () + "File");
	}
}

#if		qRealTemplatesAvailable
const AbSet<FileSystem::FileType>&	OldPathNameGetter_MacUI_Native::GetFileTypes () const
#else
const AbSet(FileTypeX)&				OldPathNameGetter_MacUI_Native::GetFileTypes () const
#endif
{
	return (fFileTypes);
}

#if		qRealTemplatesAvailable
void	OldPathNameGetter_MacUI_Native::SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes)
#else
void	OldPathNameGetter_MacUI_Native::SetFileTypes (const AbSet(FileTypeX)& fileTypes)
#endif
{
	fFileTypes = fileTypes;
}

PathName	OldPathNameGetter_MacUI_Native::GetPathName () const
{
	return (fPathName);
}

void	OldPathNameGetter_MacUI_Native::SetPathName (const PathName& pathName)
{
	fPathName = pathName;
}

// hack for dir selector - fix later
static	SFReply	gDirReply;
static	Boolean	CurDirValid;
static	long	MyCurDir;
void	OldPathNameGetter_MacUI_Native::Pose ()
{
	const	kGetDirectoryDLOG	=	1002;
	osPoint		dlogWhere	= {20, 20};
	SFTypeList	typeList;
	SFReply&	reply		=	gDirReply;	

	osStr255	message;
	GetMessage ().ToPStringTrunc (message, sizeof (message));

// clean up a bit - always do the filter version
	if (GetFileTypes () == kDirectoryFileTypeSet) {
		Assert (sCurrenGetter == Nil);
		sCurrenGetter = this;

		CurDirValid = False;
		::SFPGetFile (dlogWhere,			/* location */
			(ConstStr255Param) Nil,		/* vestigial string */
			(FileFilterProcPtr) FileFilterProc,/* fileFilter */
			(short) -1,						/* numtypes; -1 means all */
			typeList,						/* array to types to show */
			(DlgHookProcPtr) FoldersOnly_GetDirHook,	/* dlgHook */
			&gDirReply,						/* record for returned values */
			kGetDirectoryDLOG,
			(ModalFilterProcPtr) Nil);

		Assert (sCurrenGetter == this);
		sCurrenGetter = Nil;

		Assert (not reply.good);
		if (CurDirValid) {
			fPathName = PathName (-*(short *)SFSaveDisk, MyCurDir);
			return;
		}
		else {
			gUserCancelCloseException.Raise ();
		}
	}
	else {
		Assert (sCurrenGetter == Nil);
		sCurrenGetter = this;

		::SFGetFile (dlogWhere, message, FileFilterProc, -1, typeList, Nil, &reply);
		Assert (sCurrenGetter == this);
		sCurrenGetter = Nil;
		if (reply.good) {
// not great- maybe we should use some File utility to map from wdrefnum to vrefnum/dirID
			if (GetFileTypes () == kDirectoryFileTypeSet) {
				// hack til we munge sfgetfile to add "CURRENT-DIR" button, etc.. like MPW
				fPathName = PathName (-*(short*)SFSaveDisk, *(long *)CurDirStore);
			}
			else {
				fPathName = PathName (-*(short*)SFSaveDisk, *(long *)CurDirStore, String (&reply.fName[1], reply.fName[0]));
			}
			return;
		}
		else {
			gUserCancelCloseException.Raise ();
		}
	}
}

String	OldPathNameGetter_MacUI_Native::GetMessage () const
{
	return (fMessage);
}

void	OldPathNameGetter_MacUI_Native::SetMessage (const String& message)
{
	fMessage = message;
}

Boolean	OldPathNameGetter_MacUI_Native::ShowFile (const PathName& pathName)
{
	if (GetFileTypes () == kDirectoryFileTypeSet) {
		/*
		 * Normally, folders are ALWAYS shown, and aren't even passed to
		 * this file filter for judgement. Under such circumstances, it is
		 * only necessary to blindly return TRUE (allow no files whatsoever).
		 * However, Standard File is not documented in such a manner, and
		 * this feature may not be TRUE in the future. Therefore, we DO check
		 * to see if the entry passed to us describes a file or a directory.
		 */
		return Boolean (FileSystem::Get ().GetFileType (pathName) == FileSystem::eDirectoryFileType);
	}
	else {
		return (True);			// by default allow anything - override this to provide your restrictions
	}
}

pascal	unsigned char	OldPathNameGetter_MacUI_Native::FileFilterProc (ParamBlockRec* paramBlockPtr)
{
	AssertNotNil (paramBlockPtr);

	String 		theName = String (&paramBlockPtr->fileParam.ioNamePtr[1], paramBlockPtr->fileParam.ioNamePtr[0]);
	PathName	pn	=	PathName (-*(short*)SFSaveDisk, *(long *)CurDirStore, theName);

	AssertNotNil (sCurrenGetter);

	Boolean	showFile	=	False;
	Try {
		showFile = sCurrenGetter->ShowFile (pn);
	}
	Catch () {
#if		qDebug
		DebugMessage ("exception while checking if file should be shown!");
#endif	/*qDebug*/
	}
	return (!showFile);
}

pascal	short		OldPathNameGetter_MacUI_Native::FoldersOnly_GetDirHook (short item, struct osGrafPort* dialogPtr)
{
// code snagged origally from: 
	// 	Apple Macintosh Developer Technical Support
	// 
	// 	Standard File Sample Application
	// 
	// 	StdFile
	// 
	// 	StdFile.c	-	C Source
	// 
	// 	Copyright © 1989 Apple Computer, Inc.
	// 	All rights reserved.
	// 
	// 	Versions:	
	// 				1.00				04/89
	osStr255	messageTitle;
	short		kind;
	osHandle	h;
	osRect		r;

	const	getDirButton 	=11;
	const 	getDirNowButton =12;

	switch (item) {
		case getDirButton:
			if (gDirReply.fType != 0) {
				MyCurDir = gDirReply.fType;
				CurDirValid = True;
				return(getCancel);
			};
			break;
			
		case getDirNowButton:
			MyCurDir = *(long *) CurDirStore;
			CurDirValid = True;
			return(getCancel);
	};
	return (item);	/* By default, return the item passed to us. */
}

#endif	/*qMacToolkit*/










/*
 ********************************************************************************
 ******************************* NewPathNameGetter_MotifUI *********************
 ********************************************************************************
 */

NewPathNameGetter_MotifUI::NewPathNameGetter_MotifUI ()
{
}






/*
 ********************************************************************************
 ******************************* OldPathNameGetter_MotifUI *********************
 ********************************************************************************
 */

OldPathNameGetter_MotifUI::OldPathNameGetter_MotifUI ()
{
}







#if		qXmToolkit


/*
 ********************************************************************************
 ************************** NewPathNameGetter_MotifUI_Native *******************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
NewPathNameGetter_MotifUI_Native::NewPathNameGetter_MotifUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName):
#else
NewPathNameGetter_MotifUI_Native::NewPathNameGetter_MotifUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName):
#endif
	NewPathNameGetter_MotifUI (),
	fFileTypes (fileTypes),
	fPathName (pathName),
	fMessage (kEmptyString)
{
	if (fileTypes == kDirectoryFileTypeSet) {
		SetMessage ("Choose New Directory");
	}
	else {
		SetMessage ("Choose New File");
	}
}

#if		qRealTemplatesAvailable
const AbSet<FileSystem::FileType>&	NewPathNameGetter_MotifUI_Native::GetFileTypes () const
#else
const AbSet(FileTypeX)&				NewPathNameGetter_MotifUI_Native::GetFileTypes () const
#endif
{
	return (fFileTypes);
}

#if		qRealTemplatesAvailable
void	NewPathNameGetter_MotifUI_Native::SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes)
#else
void	NewPathNameGetter_MotifUI_Native::SetFileTypes (const AbSet(FileTypeX)& fileTypes)
#endif
{
	fFileTypes = fileTypes;
}

PathName	NewPathNameGetter_MotifUI_Native::GetPathName () const
{
	return (fPathName);
}

void	NewPathNameGetter_MotifUI_Native::SetPathName (const PathName& pathName)
{
	fPathName = pathName;
}

void	NewPathNameGetter_MotifUI_Native::Pose ()
{
	extern	osWidget*		gShell;
	FileSelectionBoxEventHandler	eh	=
	  FileSelectionBoxEventHandler (::XmCreateFileSelectionDialog (gShell, "", Nil, 0), fPathName, GetMessage ());
	if (eh.Pose ()) {
		SetPathName (eh.GetPathName ());
		return;
	}
	else {
		gUserCancelCloseException.Raise ();
	}
}

String	NewPathNameGetter_MotifUI_Native::GetMessage () const
{
	return (fMessage);
}

void	NewPathNameGetter_MotifUI_Native::SetMessage (const String& message)
{
	fMessage = message;
}









/*
 ********************************************************************************
 **************************** OldPathNameGetter_MotifUI_Native *****************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable
OldPathNameGetter_MotifUI_Native::OldPathNameGetter_MotifUI_Native (const AbSet<FileSystem::FileType>&, const PathName& pathName):
#else
OldPathNameGetter_MotifUI_Native::OldPathNameGetter_MotifUI_Native (const AbSet(FileTypeX)& fileTypes, const PathName& pathName):
#endif
	OldPathNameGetter_MotifUI (),
	fFileTypes (fileTypes),
	fPathName (pathName),
	fMessage (kEmptyString)
{
	if (fileTypes == kDirectoryFileTypeSet) {
		SetMessage ("Choose Old Directory");
	}
	else {
		SetMessage ("Choose Old File");
	}
}

#if		qRealTemplatesAvailable
const AbSet<FileSystem::FileType>&	OldPathNameGetter_MotifUI_Native::GetFileTypes () const
#else
const AbSet(FileTypeX)&				OldPathNameGetter_MotifUI_Native::GetFileTypes () const
#endif
{
	return (fFileTypes);
}

#if		qRealTemplatesAvailable
void	OldPathNameGetter_MotifUI_Native::SetFileTypes (const AbSet<FileSystem::FileType>& fileTypes)
#else
void	OldPathNameGetter_MotifUI_Native::SetFileTypes (const AbSet(FileTypeX)& fileTypes)
#endif
{
	fFileTypes = fileTypes;
}

PathName	OldPathNameGetter_MotifUI_Native::GetPathName () const
{
	return (fPathName);
}

void	OldPathNameGetter_MotifUI_Native::SetPathName (const PathName& pathName)
{
	fPathName = pathName;
}

void	OldPathNameGetter_MotifUI_Native::Pose ()
{
	extern	osWidget*		gShell;
	FileSelectionBoxEventHandler	eh				=
	  FileSelectionBoxEventHandler (::XmCreateFileSelectionDialog (gShell, "", Nil, 0), fPathName, GetMessage ());
	if (eh.Pose ()) {
		SetPathName (eh.GetPathName ());
		return;
	}
	else {
		gUserCancelCloseException.Raise ();
	}
}

String	OldPathNameGetter_MotifUI_Native::GetMessage () const
{
	return (fMessage);
}

void	OldPathNameGetter_MotifUI_Native::SetMessage (const String& message)
{
	fMessage = message;
}

Boolean	OldPathNameGetter_MotifUI_Native::ShowFile (const PathName& /*pathName*/)
{
	return (True);			// by default allow anything - override this to provide your restrictions
}









/*
 ********************************************************************************
 **************************** FileSelctionBoxEventManager ***********************
 ********************************************************************************
 */
FileSelectionBoxEventHandler::FileSelectionBoxEventHandler (osWidget* fileSelectionBox, const PathName& pathName, const String& dlogTitle):
	EventHandler (),
	fFileSelectionBox (fileSelectionBox),
	fOKHit (False),
	fPathName (pathName)
{
	RequireNotNil (fFileSelectionBox);
	::XtAddCallback (::XmFileSelectionBoxGetChild (fileSelectionBox, XmDIALOG_OK_BUTTON),
					 XmNactivateCallback, (XtCallbackProc)OKCallBack, (XtPointer)this);

	::XtAddCallback (::XmFileSelectionBoxGetChild (fileSelectionBox, XmDIALOG_CANCEL_BUTTON),
					 XmNactivateCallback, (XtCallbackProc)CancelCallBack, (XtPointer)this);

	char*	tmp = dlogTitle.ToCString ();
	XmString mStrTmp = XmStringCreate (tmp, XmSTRING_DEFAULT_CHARSET);
		Arg args[1];
		XtSetArg (args[0], XmNdialogTitle, mStrTmp);
		::XtSetValues (fileSelectionBox, args, 1);
	::XmStringFree (mStrTmp);
	delete (tmp);
}

FileSelectionBoxEventHandler::~FileSelectionBoxEventHandler ()
{
	::XtDestroyWidget (fFileSelectionBox);
}

PathName	FileSelectionBoxEventHandler::GetPathName () const
{
	return (fPathName);
}

void	FileSelectionBoxEventHandler::ProcessMousePress (const osEventRecord& eventRecord, const Point& /*where*/,
														 Boolean /*isDown*/, const KeyBoard& /*keyBoardState*/, UInt8 /*clickCount*/)
{
	// should check that were in the file selectiobox... or somehow make it modal..
	osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
	::XtDispatchEvent (&er);
}

void	FileSelectionBoxEventHandler::ProcessMouseMoved (const osEventRecord& eventRecord, const Point& /*newLocation*/,
														 const KeyBoard& /*keyBoardState*/, Region& /*newMouseRegion*/)
{
	osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
	::XtDispatchEvent (&er);
}

void	FileSelectionBoxEventHandler::ProcessKey (const osEventRecord& eventRecord, KeyBoard::KeyCode /*keyCode*/,
										Boolean /*isUp*/, const KeyBoard& /*keyBoardState*/, KeyComposeState& composeState)
{
	osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
	::XtDispatchEvent (&er);
	composeState = KeyComposeState ();		// reset to initial state, since if we're doing one of these WE are
											// not doing a key compose - if some xm routine is, they'll keep thier
											// own...
}

Boolean	FileSelectionBoxEventHandler::Pose ()
{
	Activate ();
	ReorderActiveList ();
	::XtManageChild (fFileSelectionBox);
	while (True) {
		EventManager::Get ().RunOnce ();
		if (not ::XtIsManaged (fFileSelectionBox)) {
			break;
		}
	}
	Deactivate ();
	return (fOKHit);
}

void	FileSelectionBoxEventHandler::OKCallBack (osWidget* /*w*/, char* client_data, char* /*call_data*/)
{
	FileSelectionBoxEventHandler*	eh	=	(FileSelectionBoxEventHandler*)client_data;
	eh->fOKHit = True;
// ug - what a mess to get the file name ---
	XmString s = Nil;
	Arg		arg;
	XtSetArg (arg, XmNtextString, &s);
	::XtGetValues (eh->fFileSelectionBox, &arg, 1);
	char *ss = Nil;
	if (!XmStringGetLtoR (s, XmSTRING_DEFAULT_CHARSET, &ss)) {
		AssertNotReached ();
	}
	eh->fPathName = String (ss);
	XtFree (ss);
	::XtUnmanageChild (eh->fFileSelectionBox);
}

void	FileSelectionBoxEventHandler::CancelCallBack (osWidget* /*w*/, char* client_data, char* /*call_data*/)
{
	FileSelectionBoxEventHandler*	eh	=	(FileSelectionBoxEventHandler*)client_data;
	eh->fOKHit = False;
	::XtUnmanageChild (eh->fFileSelectionBox);
}




#endif	/*Toolkit*/






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

