/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Translater.cc,v 1.6 1992/12/07 03:05:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Translater.cc,v $
 *		Revision 1.6  1992/12/07  03:05:28  lewis
 *		Renamed Catch/Try to lower case.
 *
 *		Revision 1.5  1992/09/26  22:24:08  lewis
 *		Ported to A/UX and gcc 2.2.1.
 *
 *		Revision 1.4  1992/09/08  16:53:45  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/07/14  20:10:37  lewis
 *		Changes for new filesystem code. PathName::kBad, and use FileSystem::Get().Open..
 *
 *
 */

#include	<fstream.h>
#include	<strstream.h>

#if			qUnixOS
#include	<unistd.h>
#endif

#include	"Debug.hh"
#include	"File.hh"
#include	"StreamUtils.hh"

#include	"CPlusLexer.hh"

#include	"Translater.hh"







/*
 ********************************************************************************
 ******************************** Translater ************************************
 ********************************************************************************
 */

Translater::Translater ():
	fInFile (Nil),
	fOutFile (Nil)
{
}

void	Translater::TranslateFile (const PathName& srcFile, const PathName& dstFile)
{
	Exception*	e	=	Nil;
	{
		int	ifd = FileSystem::Get ().Open (srcFile, O_RDONLY);
		ifstream	inFile = int (ifd);
		int	ofd = 0;
		ofstream	outFile;
		ostrstream	outFileX;
		if (dstFile != PathName::kBad) {
#if		qMacOS
			ofd = FileSystem::Get_MacOS ().Open (dstFile, O_WRONLY | O_CREAT | O_TRUNC, 'MPS ', 'TEXT');
#else
			ofd = FileSystem::Get ().Open (dstFile, O_WRONLY | O_CREAT | O_TRUNC);
#endif
#if		qGCC_LibgPlusPlusIOStreamAttatchMissingBug
			outFile.rdbuf ()->attach (ofd);
#else
			outFile.attach (ofd);
#endif
			fOutFile = &outFile;
		}
		else {
			fOutFile = &outFileX;
		}
	
		fInFile = &inFile;
		CPlusLexer		lexer	(inFile); 
		try {
			CPlusLexer::Token			t		=	lexer.GetNextToken ();
		
			typedef CPlusLexer::Token CFrontHack;
			for (; t != CFrontHack::kEOF; ) {
				HandleToken (t);
				t = lexer.GetNextToken ();
			}
		}
		catch () {
			e = _this_catch_;
		}
	
		fInFile = Nil;
		fOutFile = Nil;
		outFile.flush ();
		::close (ifd);					// not sure needed???
		if (ofd != 0) {
			::close (ofd);				// not sure needed???
		}
	}
	if (e != Nil) {
		e->Raise ();
	}
}

void	Translater::HandleToken (const CPlusLexer::Token& t)
{
	_HandleToken (t);
}

void	Translater::_HandleToken (const CPlusLexer::Token& t)
{
	_GetOut () << t.fText;
}

istream&	Translater::_GetIn () const
{
	AssertNotNil (fInFile);
	return (*fInFile);
}

ostream&	Translater::_GetOut () const
{
	AssertNotNil (fOutFile);
	return (*fOutFile);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

