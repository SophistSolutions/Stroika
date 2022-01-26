/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Main.cc,v 1.3 1992/09/01 17:42:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Main.cc,v $
 *		Revision 1.3  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *
 *
 */




/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include	"ToolkitInit.hh"
#pragma	push
#pragma	force_active	on
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

#include	"SchnauserApplication.hh"



int	main (int argc, const char* argv[])
{
	SchnauserApplication (argc, argv).Run ();
	return (0);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***




