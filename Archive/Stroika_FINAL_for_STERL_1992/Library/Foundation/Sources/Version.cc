/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Version.cc,v 1.4 1992/09/30 21:04:55 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Version.cc,v $
 *		Revision 1.4  1992/09/30  21:04:55  lewis
 *		Use "" instead of obsolete kEmptyString.
 *
 *		Revision 1.3  1992/09/05  13:38:48  lewis
 *		Rename NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.5  1992/03/30  14:54:05  lewis
 *		Work around bugs in CFront 21's implementation of nested class names - pretty broken.
 *
 *		Revision 1.4  92/03/30  14:25:54  14:25:54  lewis (Lewis Pringle)
 *		Ported to Borland C++.
 *		
 *		Revision 1.2  1992/02/19  01:37:28  lewis
 *		Ported to mac and added bugfix level.
 *
 *		Revision 1.1  1992/02/18  00:50:00  lewis
 *		Initial revision
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Files.h>
#include	<Resources.h>
#include	<SysEqu.h>
#endif	/*qMacToolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Version.hh"






/*
 ********************************************************************************
 ************************************** Version *********************************
 ********************************************************************************
 */

Version::Version (MajorVersion majorVersion, MinorVersion minorVersion, BugFixVersion bugFixVersion, Stage stage,
				 StageVersion stageVersion, const String& shortVersionString,
				 const String& longVersionString):
	   fMajorVersion (majorVersion),
	   fMinorVersion (minorVersion),
	   fBugFixVersion (bugFixVersion),
	   fStage (stage),
	   fStageVersion (stageVersion),
	   fShortVersionString (shortVersionString),
	   fLongVersionString (longVersionString)
{
}

#if		qMacOS
Version::Version (int resourceID):
	   fMajorVersion (0),
	   fMinorVersion (0),
	   fBugFixVersion (0),
	   fStage (eDevelopment),
	   fStageVersion (0),
	   fShortVersionString (""),
	   fLongVersionString ("")
{
	// code derived from:	Apple Macintosh Developer Technical Support
	//						Collection of Utilities for DTS Sample code
	short		savedResFile	=	::CurResFile ();
	osVersRec**	versionHandle	=	Nil;

	::UseResFile (*(short*)CurApRefNum);	// could use GetAppParms () instead of global reference
	versionHandle = (osVersRec**)::Get1Resource ('vers', resourceID);
	::UseResFile (savedResFile);

	AssertNotNil (versionHandle);	// add an exception???
	fMajorVersion = (*versionHandle)->numericVersion.majorRev;
	fMinorVersion = (*versionHandle)->numericVersion.minorRev;
	fBugFixVersion = (*versionHandle)->numericVersion.bugFixRev;
	switch ((*versionHandle)->numericVersion.bugFixRev) {
		case	developStage:	fStage = eDevelopment; break;
		case	alphaStage:		fStage = eAlpha; break;
		case	betaStage:		fStage = eBeta; break;
		case	finalStage:		fStage = eRelease; break;
	};
	fStageVersion = (*versionHandle)->numericVersion.nonRelRev;
	unsigned char*	shortVersionAt = (*versionHandle)->shortVersion;
	fShortVersionString = String (&shortVersionAt[1], shortVersionAt[0]);
	unsigned char*	longVersionAt = shortVersionAt + shortVersionAt[0] + 1;		// long version packed right after
	fLongVersionString = String (&longVersionAt[1], longVersionAt[0]);
}
#endif

#if		qCFront21
MajorVersion
#else
Version::MajorVersion
#endif
	Version::GetMajorVersion () const
{
	return (fMajorVersion);
}

void	Version::SetMajorVersion (MajorVersion majorVersion)
{
	fMajorVersion = majorVersion;
}

#if		qCFront21
MinorVersion
#else
Version::MinorVersion
#endif
	Version::GetMinorVersion () const
{
	return (fMinorVersion);
}

void	Version::SetMinorVersion (MinorVersion minorVersion)
{
	fMinorVersion = minorVersion;
}

#if		qCFront21
BugFixVersion
#else
Version::BugFixVersion
#endif
	Version::GetBugFixVersion () const
{
	return (fBugFixVersion);
}

void	Version::SetBugFixVersion (BugFixVersion bugFixVersion)
{
	fBugFixVersion = bugFixVersion;
}

Version::Stage	Version::GetStage () const
{
	return (fStage);
}

void	Version::SetStage (Stage stage)
{
	fStage = stage;
}

#if		qCFront21
StageVersion
#else
Version::StageVersion
#endif
	Version::GetStageVersion () const
{
	return (fStageVersion);
}

void	Version::SetStageVersion (StageVersion stageVersion)
{
	fStageVersion = stageVersion;
}

String	Version::GetShortVersionString () const
{
	return (fShortVersionString);
}

void	Version::SetShortVersionString (const String& shortVersionString)
{
	fShortVersionString = shortVersionString;
}

String	Version::GetLongVersionString () const
{
	return (fLongVersionString);
}

void	Version::SetLongVersionString (const String& longVersionString)
{
	fLongVersionString = longVersionString;
}





// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

