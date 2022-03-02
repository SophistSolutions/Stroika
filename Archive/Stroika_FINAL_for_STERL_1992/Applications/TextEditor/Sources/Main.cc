/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Main.cc,v 1.2 1992/09/01 17:55:46 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Main.cc,v $
 *		Revision 1.2  1992/09/01  17:55:46  sterling
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/02/18  00:22:12  lewis
 *		Use new version number support.
 *
 *
 *
 */

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#pragma	push
#pragma	force_active	on


#include	"ToolkitInit.hh"
static	ToolkitInit	sInit	=	ToolkitInit ();
#pragma	pop

#include	"Stroika-Globals.hh"		//	To get them inited real early cuz C++ does not guarantee
										//	order of init of static objects across .o files

#if		qMacToolkit
extern	const	Version	kApplicationVersion	=	Version (1);
#else
#include	"ApplicationVersion"
extern	const	Version	kApplicationVersion	=	Version (kMajorVersion, kMinorVersion, kBugFixVersion, kStage, kStageVersion,
														 kShortVersionString, kLongVersionString);
#endif

#include	"TextEditor.hh"



int	main (int argc, const char* argv[])
{
DebugMessage ("hi mom");
	TextEditorApplication (argc, argv).Run ();
	return (0);
}




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

