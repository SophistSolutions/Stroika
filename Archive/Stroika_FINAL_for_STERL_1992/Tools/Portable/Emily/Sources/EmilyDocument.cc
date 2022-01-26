/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EmilyDocument.cc,v 1.7 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyDocument.cc,v $
 *		Revision 1.7  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *
 *
 *
 */





#include	<fstream.h>

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Resources.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"PathNameSelecters.hh"
#include	"WindowPlane.hh"

#include	"CommandNumbers.hh"

#include	"EmilyApplication.hh"
#include	"EmilyWindow.hh"
#include	"EmilyDocument.hh"





/*
 ********************************************************************************
 ************************************ EmilyDocument *****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	Implement (Iterator, EmDcPtr);
	Implement (Collection, EmDcPtr);
	Implement (AbSequence, EmDcPtr);
	Implement (Array, EmDcPtr);
	Implement (Sequence_Array, EmDcPtr);
	Implement (Sequence, EmDcPtr);
#endif

EmilyDocument::EmilyDocument ():
	FileBasedDocument (1),
	fEmilyWindow (Nil),
	fPrinter (Nil)
{
	fPrinter = new Printer ();
	fEmilyWindow = new EmilyWindow (*this);
	EmilyApplication::Get ().AddDocument (this);
#if		qMacOS
	SetFileType ('TEXT');
#endif
}

EmilyDocument::~EmilyDocument ()
{
	EmilyApplication::Get ().RemoveDocument (this);
}

void	EmilyDocument::DoSetupMenus ()
{
	EnableCommand (ePageSetup);
	EnableCommand (ePrint);
	EnableCommand (ePrintOne);
	FileBasedDocument::DoSetupMenus ();
}

Boolean	EmilyDocument::DoCommand (const CommandSelection& selection)
{
	CommandNumber commandNumber = selection.GetCommandNumber ();
	switch (commandNumber) {
		case ePageSetup:
			AssertNotNil (fPrinter);
			fPrinter->DoPageSetup ();
			return (True);

		case ePrint:
		case ePrintOne:
			fPrinter->Print (GetWindow (), Boolean (commandNumber == ePrint));
			return (True);

		default:
			break;
	}
	return (FileBasedDocument::DoCommand (selection));
}

static	String	GetFileNameWithoutSuffix (const String& fileName, const String& suffix)
{
	const	CollectionSize	suffixLen		=	suffix.GetLength ();
	CollectionSize			fileNameLen		=	fileName.GetLength ();
	if (fileNameLen > suffixLen) {
		String	fileSuffix	=	fileName.SubString (fileNameLen-suffixLen + 1);
		if (tolower (fileSuffix) == tolower (suffix)) {
			return (fileName.SubString (1, fileNameLen-suffixLen));
		}
	}
	return (fileName);
}

PathName	EmilyDocument::GetHeaderFilePathName ()
{
	const PathName	docPath = GetPathName ();

	PathName	bestPath = FileSystem::Get ().GetParentDirectory (docPath);

	Try {
		PathName newBestPath = FileSystem::Get ().GetParentDirectory (bestPath) + PathComponent ("Headers");
		if (FileSystem::Get ().Exists (newBestPath)) {
			bestPath = newBestPath;
		}
	}
	Catch () {
	}
	PathComponent	finalSuffix	=	PathComponent (GetFileNameWithoutSuffix (GetPrintName (), 
				EmilyApplication::Get ().GetDataSuffix ()) + EmilyApplication::Get ().GetHeaderSuffix ()
			);
	return (bestPath + finalSuffix);
}

PathName	EmilyDocument::GetSourceFilePathName ()
{
	const PathName	docPath = GetPathName ();

	PathName	bestPath = FileSystem::Get ().GetParentDirectory (docPath);
	
	Try {
		PathName newBestPath = FileSystem::Get ().GetParentDirectory (bestPath) + "Sources";
		if (FileSystem::Get ().Exists (newBestPath)) {
			bestPath = newBestPath;
		}
	}
	Catch () {
	}
	
	return (bestPath + PathComponent (GetFileNameWithoutSuffix (GetPrintName (), 
		EmilyApplication::Get ().GetDataSuffix ()) + EmilyApplication::Get ().GetSourceSuffix ()));
}

#if qMacOS
	//  moved out of class cuz of q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	MyGetter : public OldPathNameGetter {
		public:
			MyGetter (UInt32 fileType, const PathName& p):
				OldPathNameGetter (kPlainFileTypeSet, p),
				fAnyType (GetKeyBoard ().GetKey (KeyBoard::eOptionKey)),
				fType (fileType)
			{
			}

			override	Boolean	ShowFile (const PathName& pathName);
		private:
			Boolean	fAnyType;
			UInt32	fType;
	};
			Boolean	MyGetter::ShowFile (const PathName& pathName)
			{
				if (fAnyType) {
					return (True);
				}
				Try {
					if (FileSystem::Get_MacOS ().GetType (pathName) != fType) {
						return (False);
					}
				}
				Catch () {
					return (True);	// should be more exact catch, probably is a directory
				}
				
				// check suffix is correct
				const	String			kSuffix			=	EmilyApplication::Get ().GetDataSuffix ();
				const	CollectionSize	kSuffixLen		=	kSuffix.GetLength ();
				String					fileName		=	pathName.GetFileName ().GetName ();
				CollectionSize			fileNameLen		=	fileName.GetLength ();
				if (fileNameLen <= kSuffixLen) {
					return (False);
				}
				else {
					String	suffix	=	fileName.SubString (fileNameLen-kSuffixLen + 1);
					return (tolower (suffix) == tolower (kSuffix));
				}
			}
#endif

void	EmilyDocument::ChooseOldDocument ()
{
#if qMacOS
	/*
	 * By default, only allow selection of our kind of files, unless the option key is pressed,
	 * in which case we do no filtering. 
	 */
	MyGetter	oldFileGetter	=	MyGetter (GetFileType (), FileSystem::Get_MacOS ().GetCurrentSFGetFileDirectory () + PathComponent (GetPrintName ()));
	Try {
		oldFileGetter.Pose ();
		SetPathName (oldFileGetter.GetPathName ());
	}
	Catch () {
		oldFileGetter.MyGetter::~MyGetter ();
		_this_catch_->Raise ();
	}
#else
	FileBasedDocument::ChooseOldDocument ();
#endif
}

void	EmilyDocument::DoNewState ()
{
	GetWindow ().SetTitle (GetPrintName ());
	GetWindow ().SetVisible (Panel::kVisible);
}

void	EmilyDocument::DoRead_ (class istream& from)
{
	GetWindow ().DoRead (from);
	GetWindow ().SetTitle (GetPrintName ());
	GetWindow ().SetVisible (Panel::kVisible);
	
	int fd = 0;
	{
	Try {
#if qMacOS
		fd = FileSystem::Get_MacOS ().Open (GetHeaderFilePathName (), O_RDONLY, 
			Application::Get ().GetApplicationCreator (), 'TEXT');
#else
		fd = FileSystem::Get ().Open (GetHeaderFilePathName (), O_RDONLY);
#endif
	}
	Catch () {
		fd = 0;
	}
	}

	if (fd != 0) {
		ifstream	headerTo (fd);	
		GetWindow ().ReadHeaderFile (headerTo);
		headerTo.close ();
		::close (fd);
	}

	{
	Try {
#if qMacOS
		fd = FileSystem::Get_MacOS ().Open (GetSourceFilePathName (), O_RDONLY, 
			Application::Get ().GetApplicationCreator (), 'TEXT');
#else
		fd = FileSystem::Get ().Open (GetSourceFilePathName (), O_RDONLY);
#endif
	}
	Catch () {
		fd = 0;
	}
	}

	if (fd != 0) {
		ifstream	sourceTo (fd);	
		GetWindow ().ReadSourceFile (sourceTo);
		sourceTo.close ();
		::close (fd);
	}
}

#define	qProjectorFriendly	0
void	EmilyDocument::Save ()
{
#if		qMacOS && qProjectorFriendly
	// taken from MacAPP headers, is this documented anywhere???
	static	const	int	kProjectorResID	= 128;
	struct ProjectorRecord
	{
		Int32 checksum;
		Int32 locationID;
		Int16 version;
		Int16 checkoutState;	/* 0 = not checked out */
		Int8 branch;
		Int8 Modified;			/* 0 = readOnly, else modifiedReadOnly */
		Int32 unused;
		Int32 checkoutTime;
		Int32 modDate;
		Int32 etcetera;
	};

	if (GetPathName () != Nil) {
		int	resFD	=	0;
		Boolean	projectorLocked = False;
		Try {
			resFD = OpenResFile (*GetPathName (), O_RDWR);
			ProjectorRecord**	projectorRecord = (ProjectorRecord**) Get1Resource('ckid', kProjectorResID);
			if ((projectorRecord != Nil) and ((*projectorRecord)->checkoutState == 0) and ((*projectorRecord)->Modified == 0)) {
				projectorLocked = True;
			}
		}
		Catch () {
			// no problem, clearly isn't locked if there is no resource fork!!!
		}
		if (resFD != 0) {
			::CloseResFile (resFD);
		}
		if (projectorLocked) {
			FileSystem::sFileProjectorLocked.Raise ();
		}
	}
#endif

	FileBasedDocument::Save ();
}

void	EmilyDocument::DoWrite_ (class ostream& to, int /*tabCount*/) const
{
	Assert (to.good ());
	
	GetWindow ().DoWrite (to);
	Assert (to.good ());
	GetWindow ().SetTitle (GetPrintName ());

#if qMacOS
	int	fd = FileSystem::Get_MacOS ().Open (GetHeaderFilePathName (), O_WRONLY | O_TRUNC | O_CREAT, 
		Application::Get ().GetApplicationCreator (), 'TEXT');
#else
	int	fd = FileSystem::Get ().Open (GetHeaderFilePathName (), O_WRONLY | O_TRUNC | O_CREAT);
#endif

	ofstream	headerTo (fd);	
	GetWindow ().WriteHeaderFile (headerTo);
	headerTo.close ();
	::close (fd);

#if qMacOS
	fd = FileSystem::Get_MacOS ().Open (GetSourceFilePathName (), O_WRONLY | O_TRUNC | O_CREAT, 
		Application::Get ().GetApplicationCreator (), 'TEXT');
#else
	fd = FileSystem::Get ().Open (GetSourceFilePathName (), O_WRONLY | O_TRUNC | O_CREAT);
#endif

	ofstream	sourceTo (fd);	
	GetWindow ().WriteSourceFile (sourceTo);
	sourceTo.close ();
	::close (fd);
}

off_t	EmilyDocument::EstimatedFileSize ()
{
	return (100);		// could do better....
}

void	EmilyDocument::EditModeChanged (Boolean newEditMode)
{
	GetWindow ().EditModeChanged (newEditMode);
}

EmilyWindow&	EmilyDocument::GetWindow () 
{ 
	RequireNotNil (fEmilyWindow);
	return (*fEmilyWindow);	
}
