/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Document.cc,v 1.10 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Document.cc,v $
 *		Revision 1.10  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.9  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.7  1992/07/16  05:31:48  lewis
 *		Keep a PathName always in FileBasedDocument - not PathName*. Use
 *		PathName::kBad to deal with untitled case.
 *
 *		Revision 1.6  1992/07/15  22:00:17  lewis
 *		Get code working again with new file stuff.
 *
 *		Revision 1.5  1992/07/14  19:52:18  lewis
 *		React to changes in file/pathname support, and also PathNameGetters::Pose no
 *		longer return value - they raise exceptions.
 *
 *		Revision 1.4  1992/07/10  22:36:02  lewis
 *		React to change in definitions in File.hh, and get rid of if's surrounding
 *		deletes - delete (Nil) OK.
 *
 *		Revision 1.2  1992/07/02  04:50:41  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.18  1992/05/19  10:32:24  sterling
 *		made DoWrite_ const
 *
 *		Revision 1.17  92/02/18  03:53:00  03:53:00  lewis (Lewis Pringle)
 *		Had to change type name of Version to SaveableVersion to avoid name conflict.
 *		
 *		Revision 1.16  1992/02/15  05:32:02  sterling
 *		got rid of references to ActiveItem
 *
 *		Revision 1.15  1992/01/31  16:42:32  sterling
 *		changed SaveChanges during Emily bootstrap
 *
 *		Revision 1.12  1992/01/10  14:49:26  lewis
 *		Enabled code for pathname selectors under unix.
 *
 *
 */



#include	"OSRenamePre.hh"

#if			qMacOS
#include	<Resources.h>
#endif		/*qMacOS*/

#include	"OSRenamePost.hh"

#include	"Config-Foundation.hh"

#include	<fstream.h>

#include	"Color.hh"
#include	"Debug.hh"
#include	"Format.hh"
#include	"KeyBoard.hh"

#include	"Dialog.hh"
#include	"StreamUtils.hh"

#include	"Application.hh"
#include	"PathNameSelecters.hh"
#include	"MenuOwner.hh"

#include	"Document.hh"



#include	"SaveChanges.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, DocumentPtr);
	Implement (Collection, DocumentPtr);
	Implement (AbBag, DocumentPtr);
	Implement (Array, DocumentPtr);
	Implement (Bag_Array, DocumentPtr);
	Implement (Bag, DocumentPtr);
#endif	/*!qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ************************************ Document **********************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable
Bag<DocumentPtr>	Document::sDocuments;
#else
Bag(DocumentPtr)	Document::sDocuments;
#endif


Document::Document ():
	WindowController ()
{
	sDocuments.Add (this);
}

Document::~Document ()
{
	sDocuments.Remove (this);
}

void	Document::HandleClose (Window& ofWindow)
{
	if (WouldCloseDocument (ofWindow)) {
		TryToClose ();
	}
	else {
		WindowController::HandleClose (ofWindow);
	}
}

Boolean	Document::WouldCloseDocument (Window& /*aWindow*/)
{
	return (True);
}

void	Document::Close ()
{
	Try {
		// force destruction of iterator before deleting this...
		ForEach (WindowPtr, it, MakeWindowIterator ()) {
			Window*	w	=	it.Current ();
			AssertNotNil (w);
			w->SetActive (not Window::kActive);
			w->SetVisible (not View::kVisible);
		}
		ForEach (WindowPtr, it2, MakeWindowIterator ()) {
			Window*	w	=	it2.Current ();
			AssertNotNil (w);
			delete w;
		}
		Assert (fWindows.GetLength () == 0);
	}
	Catch () {
		AssertNotReached ();		// failures not allowed during close
									// override TryToClose () if you wish to abort closing...
	}

	delete (this);					// dont reference any fields after this - yule be sorry!
}

Iterator(DocumentPtr)*	Document::MakeDocumentIterator ()
{
	return (sDocuments);
}








/*
 ********************************************************************************
 ********************************** DocumentDirtier *****************************
 ********************************************************************************
 */
DocumentDirtier::DocumentDirtier (Document& document):
	Command (CommandHandler::eNoCommand, kUndoable),
	fDocument (document)
{
}

void	DocumentDirtier::DoIt ()
{
	Command::DoIt ();
	fDocument.PlusChangeCount ();
}

void	DocumentDirtier::UnDoIt ()
{
	Command::UnDoIt ();
	fDocument.MinusChangeCount ();
}






/*
 ********************************************************************************
 ******************************** FileBasedDocument *****************************
 ********************************************************************************
 */
Exception	gUserCancelCloseException;

FileBasedDocument::FileBasedDocument (SaveableVersion maxVersion):
	Saveable (maxVersion),
#if		qMacOS
	fOpenResourceFork (False),
	fMacResFileRefNum (0),
#endif	/*qMacOS*/
#if		qMacOS
	fFileCreator ('????'),
	fFileType ('????'),
#endif	/*qMacOS*/
	fPrintName (kEmptyString),
	fFilePathName (PathName::kBad),
	fKeepsFileOpen (True)		// good default since then others cannot munge under us...
{
#if		qMacOS
	SetFileCreator (Application::Get ().GetApplicationCreator ());
#endif	/*qMacOS*/
}

FileBasedDocument::~FileBasedDocument ()
{
}

void	FileBasedDocument::DoSetupMenus ()
{
	Document::DoSetupMenus ();
	EnableCommand (eSave, GetDirty ());
	EnableCommand (eSaveAs, True);
	EnableCommand (eRevertToSaved, Boolean (GetDirty () and (GetPathName () != PathName::kBad)));
}

Boolean	FileBasedDocument::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case	eSave: {
			Save ();
		}
		break;
		case	eSaveAs: {
			ChooseNewDocument ();
			Save ();
		}
		break;
		case	eRevertToSaved: {
			Read ();
		}
		break;
		default: {
			return (Document::DoCommand (selection));
		}
		break;
	}
}

const PathName&	FileBasedDocument::GetPathName () const
{
	return (fFilePathName);
}

void	FileBasedDocument::SetPathName (const PathName& pathName)
{
	fFilePathName = pathName;
	fPrintName = pathName.GetFileName ().GetName ();
}

void	FileBasedDocument::Save ()
{
	if (GetPathName () == PathName::kBad) {
		/*
		 * Put up dialog querying user for a pathname, and if all goes well, save into it.
		 */
		ChooseNewDocument ();		// user can raise exception here...
	}

	/*
	 * Now do real save code.  Open file, and hook stream to it and call DoWrite ().
	 */
	Exception*	theException	=	Nil;
#if		qMacOS
	int	fd = FileSystem::Get_MacOS ().Open (GetPathName (), O_WRONLY | O_TRUNC | O_CREAT, GetFileCreator (), GetFileType ());
#else
	int	fd = FileSystem::Get ().Open (GetPathName (), O_WRONLY | O_TRUNC | O_CREAT);
#endif
	Try {
#if		qMacOS
		if (GetOpenResourceFork ()) {
			int	resFD	=	FileSystem::Get_MacOS ().OpenResFile (GetPathName (), O_RDWR | O_CREAT);
			Assert (resFD > 0);		// IM (vol 1) says so...
			fMacResFileRefNum = resFD;
		}
#endif	/*qMacOS*/
		ofstream	outFile = int (fd);
		Assert (outFile);
		Try {
			DoWrite (outFile);
		}
		Catch () {
			outFile.close ();
			outFile.ofstream::~ofstream ();
			_this_catch_->Raise ();
		}
		Assert (outFile);
		outFile.close ();
	}
	Catch () {
		theException = _this_catch_;
	}
	::close (fd);					// not sure needed???

#if		qMacOS
	if (fMacResFileRefNum != 0) {
		::CloseResFile (fMacResFileRefNum);
		 Assert (ResError () == noErr);			// really should do something here?
		fMacResFileRefNum = 0;
	}
#endif	/*qMacOS*/

	if (theException != Nil) {
		theException->Raise ();
	}
}

void	FileBasedDocument::SaveAs (const PathName& pathName)
{
	SetPathName (pathName);
	Save ();
}

void	FileBasedDocument::Read ()
{
	// not sure this is right - read/write may not be symetric...
	if (GetPathName () == PathName::kBad) {
		/*
		 * Put up dialog querying user for a pathname, and then call SaveAs with that pathname.
		 */
		ChooseOldDocument ();
	}

	/*
	 * Now do real read code.  Open file, and hook stream to it and call DoRead ().
	 */
	Exception*	theException	=	Nil;
	int	fd = FileSystem::Get ().Open (GetPathName (), O_RDONLY);
	Try {
#if		qMacOS
		if (GetOpenResourceFork ()) {
			int	resFD	=	FileSystem::Get_MacOS ().OpenResFile (GetPathName (), O_RDONLY);
			Assert (resFD > 0);		// IM (vol 1) says so...
			fMacResFileRefNum = resFD;
		}
#endif	/*qMacOS*/
		ifstream	inFile = fd;
		Assert (inFile);
		Try {
			DoRead (inFile);
		}
		Catch () {
			inFile.close ();
			inFile.ifstream::~ifstream ();
			_this_catch_->Raise ();
		}
		inFile.close ();
	}
	Catch () {
		theException = _this_catch_;
	}
	::close (fd);				// not sure why needed...
#if		qMacOS
	if (fMacResFileRefNum != 0) {
		::CloseResFile (fMacResFileRefNum);
		 Assert (ResError () == noErr);			// really should do something here?
		fMacResFileRefNum = 0;
	}
#endif	/*qMacOS*/

	if (theException != Nil) {
		theException->Raise ();
	}
}

void	FileBasedDocument::ReadFrom (const PathName& pathName)
{
	SetPathName (pathName);
	Read ();
}

void	FileBasedDocument::GenerateDefaultName (String& name)
{
	static	int	i	=	1;
	OStringStream	buf;
	buf << "Untitled" << i++;
	name = String (buf);
}

void	FileBasedDocument::ChooseNewDocument ()
{
	NewPathNameGetter	newFileGetter	=	NewPathNameGetter (kPlainFileTypeSet,
#if		qMacOS
		FileSystem::Get_MacOS ().GetCurrentSFGetFileDirectory () + PathComponent (GetPrintName ())
#else
		PathName::kDot + PathComponent (GetPrintName ())
#endif
		);
	Try {
		newFileGetter.Pose ();
		SetPathName (newFileGetter.GetPathName ());
	}
	Catch () {
		newFileGetter.NewPathNameGetter::~NewPathNameGetter ();
		_this_catch_->Raise ();
	}
}

#if		qMacToolkit
// q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	MyGetter : public OldPathNameGetter {
		public:
			MyGetter (UInt32 fileType, const PathName& p):
				OldPathNameGetter (kPlainFileTypeSet, p),
				fAnyType (GetKeyBoard ().GetKey (KeyBoard::eOptionKey)),
				fType (fileType)
				{
				}
			override	Boolean	ShowFile (const PathName& pathName)
				{
#if		qMacOS
					return Boolean (fAnyType or FileSystem::Get_MacOS ().GetType (pathName) == fType);
#endif	/*qMacOS*/
				}
			private:
				Boolean	fAnyType;
				UInt32	fType;
	};
#endif	/*OldPathNameGetter*/
void	FileBasedDocument::ChooseOldDocument ()
{
#if		qMacToolkit
	/*
	 * By default, only allow selection of our kind of files, unless the option key is pressed,
	 * in which case we do no filtering. This routine can easily be overridden to so whatever filtering
	 * is appropriate.
	 */
	MyGetter			oldFileGetter	=	MyGetter (GetFileType (), FileSystem::Get_MacOS ().GetCurrentSFGetFileDirectory () + PathComponent (GetPrintName ()));
#else
	OldPathNameGetter	oldFileGetter	=	OldPathNameGetter (kPlainFileTypeSet, PathName::kDot + PathComponent (GetPrintName ()));
#endif
	Try {
		oldFileGetter.Pose ();
		SetPathName (oldFileGetter.GetPathName ());
	}
	Catch () {
#if		qMacToolkit
		oldFileGetter.MyGetter::~MyGetter ();
#else
		oldFileGetter.OldPathNameGetter::~OldPathNameGetter ();
#endif
		_this_catch_->Raise ();
	}
}

String	FileBasedDocument::GetPrintName ()
{
	if (fPrintName == kEmptyString) {
		GenerateDefaultName (fPrintName);
	}
	return (fPrintName);
}

void	FileBasedDocument::TryToClose ()
{
	if (GetDirty ()) {
		/*
		 * Put up alert, and see if users wants to save - if yes, then save, else
		 * throw error.
		 */
		SaveChangesDialog	dlog (GetPrintName ());
		Try {
			dlog.Pose ();
			if (dlog.GetCancelled ()) {
				gUserCancelCloseException.Raise ();
			}
			if (dlog.GetSaveChanges ()) {
				Save ();
			}
		}
		Catch () {
			dlog.SaveChangesDialog::~SaveChangesDialog ();
			_this_catch_->Raise ();
		}
	}

	Close ();		// deletes this, be very carefull!
}

Boolean	FileBasedDocument::GetKeepsFileOpen () const
{
	return (fKeepsFileOpen);
}

void	FileBasedDocument::SetKeepsFileOpen (Boolean keepsFileOpen)
{
	if (fKeepsFileOpen != keepsFileOpen) {
		fKeepsFileOpen = keepsFileOpen;
		// if currently open, or not, update....  Maybe not a good time!!!!  Actually I think not..
	}
}

#if		qMacOS
Boolean	FileBasedDocument::GetOpenResourceFork () const
{
	return (fOpenResourceFork);
}

void	FileBasedDocument::SetOpenResourceFork (Boolean openResourceFork)
{
	fOpenResourceFork = openResourceFork;
}

short	FileBasedDocument::GetMacResFileRefNum () const
{
	Require (fMacResFileRefNum != 0);		// require cuz thats how we enforce that you
											// cannot call this except in the context of a DoWrite/DoRead
	return (fMacResFileRefNum);
}
#endif	/*qMacOS*/

#if		qMacOS
UInt32	FileBasedDocument::GetFileCreator () const
{
	return (fFileCreator);
}

void	FileBasedDocument::SetFileCreator (UInt32 creator)
{
	fFileCreator = creator;
}

UInt32	FileBasedDocument::GetFileType () const
{
	return (fFileType);
}

void	FileBasedDocument::SetFileType (UInt32 fileType)
{
	fFileType = fileType;
}
#endif	/*qMacOS*/

void	FileBasedDocument::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
}

void	FileBasedDocument::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

