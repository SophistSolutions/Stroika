/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEditorDocument.cc,v 1.4 1992/09/08 18:02:17 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditorDocument.cc,v $
 *		Revision 1.4  1992/09/08  18:02:17  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:55:46  sterling
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/07/21  18:40:40  sterling
 *		reflect File changes
 *
 *		
 *
 *
 */

#include "Debug.hh"
#include "StreamUtils.hh"

#include "TextEdit.hh"

#include "TextEditorDocument.hh"
#include "TextEditorWindow.hh"


/*
 ********************************************************************************
 ********************************* TextEditorDocument ***************************
 ********************************************************************************
 */

TextEditorDocument::TextEditorDocument ():
	FileBasedDocument (),
	fTEWin (Nil)
{
	fTEWin = new TextEditorWindow (*this);
}

TextEditorDocument::~TextEditorDocument ()
{
}

void	TextEditorDocument::DoNewState ()
{
	AssertNotNil (fTEWin);
	fTEWin->SetTitle (GetPrintName ());
	fTEWin->Select ();
}

void	TextEditorDocument::DoRead_ (class istream& from)
{
#if 0
const PathName	docPath = GetPathName ();
gDebugStream << " docPath = " << docPath << newline;
Assert (FileSystem::Get ().Exists (docPath));
FileSystem::Get ().GetSize (docPath);
#endif
	AssertNotNil (fTEWin);
	String	inStr	=	kEmptyString;
	while (from) {
		char bigBuf [1024];
		bigBuf [0] = 0;
		from.read (bigBuf, sizeof (bigBuf)-1);
		Assert (from.gcount () < sizeof (bigBuf));
		bigBuf [from.gcount ()] = 0;
		inStr += bigBuf;
	}
	fTEWin->GetTE ().SetText (inStr);

	fTEWin->SetTitle (GetPrintName ());
	fTEWin->Select ();
}

void	TextEditorDocument::DoWrite_ (class ostream& to, int /*tabCount*/) const
{
	AssertNotNil (fTEWin);
	Assert (to.good ());
	to << fTEWin->GetTE ().GetText ();
	Assert (to.good ());
	fTEWin->SetTitle (GetPrintName ());
}

off_t	TextEditorDocument::EstimatedFileSize ()
{
	AssertNotNil (fTEWin);
	return (fTEWin->GetTE ().GetLength ());
}



