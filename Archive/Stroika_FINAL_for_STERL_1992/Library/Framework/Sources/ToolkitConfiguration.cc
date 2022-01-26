/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ToolkitConfiguration.cc,v 1.2 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ToolkitConfiguration.cc,v $
 *		Revision 1.2  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacOS
#include	<GestaltEqu.h>
#include	<Memory.h>
#include	<OSUtils.h>
#include	<ToolUtils.h>
#include	<Traps.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"


#include	"ToolkitConfiguration.hh"




/*
 ********************************************************************************
 ****************************** ToolkitConfiguration ****************************
 ********************************************************************************
 */

ToolkitConfiguration::ToolkitConfiguration ():
	GDIConfiguration ()
{
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


