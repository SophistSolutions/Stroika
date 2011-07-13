/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SchnauserDocument.cc,v 1.4 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SchnauserDocument.cc,v $
 *		Revision 1.4  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
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
#include	"ProjectWindow.hh"

#include	"SchnauserDocument.hh"






/*
 ********************************************************************************
 ************************** SchnauserDocument ***********************************
 ********************************************************************************
 */

Exception	SchnauserDocument::sUnknownFileFormat;

SchnauserDocument::SchnauserDocument ():
	FileBasedDocument (),
	fWindow (Nil)
{
	fWindow = new ProjectWindow (*this);
}

SchnauserDocument::~SchnauserDocument ()
{
	// window should be destroyed by now????
}

void	SchnauserDocument::DoNewState ()
{
	AssertNotNil (fWindow);
	fWindow->Select ();
}

void	SchnauserDocument::DoRead_ (class istream& from)
{
#if 0
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
#endif
}

void	SchnauserDocument::DoWrite_ (class ostream& to, int tabCount) const
{
#if 0
	AssertNotNil (fWindow);
	to << fWindow->GetPixelMap () << newline;
	fWindow->SetTitle (GetPrintName ());
#endif
}

FileOffset	SchnauserDocument::EstimatedFileSize ()
{
#if 0
	AssertNotNil (fWindow);
	return (fWindow->GetPixelMap ().GetImageSize ()*6);	// roughly each byte written as "0x22, "
#endif
}

void	SchnauserDocument::DoSetupMenus ()
{
	FileBasedDocument::DoSetupMenus ();
}

Boolean	SchnauserDocument::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
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

