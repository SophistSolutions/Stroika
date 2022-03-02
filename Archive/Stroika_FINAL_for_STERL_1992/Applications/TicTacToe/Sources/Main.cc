/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Main.cc,v 1.2 1992/09/01 17:58:36 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Main.cc,v $
 *		Revision 1.2  1992/09/01  17:58:36  sterling
 *		*** empty log message ***
 *
 *		Revision 1.6  1992/02/17  23:52:51  lewis
 *		Use new version support.
 *
 *		Revision 1.5  1992/01/22  06:10:47  lewis
 *		Use ToolkitInit.
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


#include	"Debug.hh"

#include	"TicTacToeApplication.hh"


int	main (int argc, const char* argv[])
{
	TicTacToeApplication (argc, argv).Run ();
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

