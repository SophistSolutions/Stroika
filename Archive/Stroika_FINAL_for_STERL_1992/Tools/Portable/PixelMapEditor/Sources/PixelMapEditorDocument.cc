/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorDocument.cc,v 1.4 1992/09/08 16:52:13 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditorDocument.cc,v $
 *		Revision 1.4  1992/09/08  16:52:13  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:37:42  sterling
 *		*** empty log message ***
 *
 *		Revision 1.5  1992/05/20  00:29:38  lewis
 *		Added more Write methods to write other pixelmap formats.
 *
 *		Revision 1.3  92/04/29  12:37:47  12:37:47  lewis (Lewis Pringle)
 *		Fixup file IO.
 *		
 *		Revision 1.1  92/04/20  11:06:02  11:06:02  lewis (Lewis Pringle)
 *		Initial revision
 *
 *
 *
 */




#include	<fstream.h>

#include	"File.hh"
#include	"StreamUtils.hh"

#include	"PathNameSelecters.hh"

#include	"CodeGenUtils.hh"
#include	"PixelMapFormats.hh"

#include	"CommandNumbers.hh"
#include	"PixelMapEditorWindow.hh"

#include	"PixelMapEditorDocument.hh"




/*
 ********************************************************************************
 ************************** PixelMapEditorDocument ******************************
 ********************************************************************************
 */

Exception	PixelMapEditorDocument::sUnknownFileFormat;

PixelMapEditorDocument::PixelMapEditorDocument ():
	FileBasedDocument (),
	fWindow (Nil)
{
#if		qMacToolkit
	SetFileType ('TEXT');				// not sure how to set this properly...
#endif
	fWindow = new PixelMapEditorWindow (*this);
}

PixelMapEditorDocument::~PixelMapEditorDocument ()
{
	// window should be destroyed by now????
}

void	PixelMapEditorDocument::DoNewState ()
{
	AssertNotNil (fWindow);
	fWindow->Select ();
}

void	PixelMapEditorDocument::DoRead_ (class istream& from)
{
	PixelMap	pm;

// First try reading our format
	streampos	oldStreamPos	=	from.tellg ();
	from >> pm;
	if (from) {
		AssertNotNil (fWindow);
		fWindow->SetPixelMap (pm);
		fWindow->SetTitle (GetPrintName ());
		fWindow->Select ();
		return;
	}

// Then try XPM	
	from.clear ();
	from.seekg (oldStreamPos);	// reset and try new format
	String	name;
	ReadPixelMapFromXPM (from, pm, name);
	if (from) {
		AssertNotNil (fWindow);
		fWindow->SetPixelMap (pm);
		fWindow->SetTitle (GetPrintName ());
		fWindow->Select ();
		return;
	}

// Then try XPM2
	from.clear ();
	from.seekg (oldStreamPos);	// reset and try new format
	ReadPixelMapFromXPM2 (from, pm);
	if (from) {
		AssertNotNil (fWindow);
		fWindow->SetPixelMap (pm);
		fWindow->SetTitle (GetPrintName ());
		fWindow->Select ();
		return;
	}

	// If we get here, we've failed...
	sUnknownFileFormat.Raise ();
}

void	PixelMapEditorDocument::DoWrite_ (class ostream& to, int tabCount) const
{
	AssertNotNil (fWindow);
	to << fWindow->GetPixelMap () << newline;
	fWindow->SetTitle (GetPrintName ());
}

off_t	PixelMapEditorDocument::EstimatedFileSize ()
{
	AssertNotNil (fWindow);
	return (fWindow->GetPixelMap ().GetImageSize ()*6);	// roughly each byte written as "0x22, "
}

void	PixelMapEditorDocument::DoSetupMenus ()
{
	EnableCommand (eWriteCPlusPlus, True);
	EnableCommand (eWriteXPM, True);
	EnableCommand (eWriteXPM2, True);
	FileBasedDocument::DoSetupMenus ();
}

Boolean	PixelMapEditorDocument::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case	eWriteCPlusPlus: {
			NewPathNameGetter	getter (kPlainFileTypeSet, PathName::kBad);
			getter.SetMessage ("Output C++ Source to file:");
// getter.Pose () Could cause exception which would cause memory leak but too painful to test all the time.. - Really I should....
			getter.Pose ();
			int	fd	=	FileSystem::Get ().Open (getter.GetPathName (), O_WRONLY | O_TRUNC | O_CREAT);
			ofstream	out	=	fd;
			// Should let user configure name - but for now, just use filename...
			String shortName = getter.GetPathName ().GetFileName ().GetName ();
			if (shortName.IndexOf (".") != kBadSequenceIndex) {
				shortName = shortName.SubString (1, shortName.IndexOf (".")-1);
			}
			WriteAsPixelMapFromData (out, fWindow->GetPixelMap (), shortName);
			out.flush ();
			::close (fd);
		}
		break;
		case	eWriteXPM: {
			NewPathNameGetter	getter (kPlainFileTypeSet, PathName::kBad);
			getter.SetMessage ("Output XPM to file:");
// getter.Pose () Could cause exception which would cause memory leak but too painful to test all the time.. - Really I should....
			getter.Pose ();
			int	fd	=	FileSystem::Get ().Open (getter.GetPathName (), O_WRONLY | O_TRUNC | O_CREAT);
			ofstream	out	=	fd;
			// Should let user configure name - but for now, just use filename...
			String shortName = getter.GetPathName ().GetFileName ().GetName ();
			if (shortName.IndexOf (".") != kBadSequenceIndex) {
				shortName = shortName.SubString (1, shortName.IndexOf (".")-1);
			}
			WritePixelMapToXPM (out, fWindow->GetPixelMap (), shortName);
			out.flush ();
			::close (fd);
		}
		break;
		case	eWriteXPM2: {
			NewPathNameGetter	getter (kPlainFileTypeSet, PathName::kBad);
			getter.SetMessage ("Output XPM2 to file:");
// getter.Pose () Could cause exception which would cause memory leak but too painful to test all the time.. - Really I should....
			getter.Pose ();
			int	fd	=	FileSystem::Get ().Open (getter.GetPathName (), O_WRONLY | O_TRUNC | O_CREAT);
			ofstream	out	=	fd;
			// Should let user configure name - but for now, just use filename...
			WritePixelMapToXPM2 (out, fWindow->GetPixelMap ());
			out.flush ();
			::close (fd);
		}
		break;
		default: {
			return (FileBasedDocument::DoCommand (selection));
		}
		break;
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

