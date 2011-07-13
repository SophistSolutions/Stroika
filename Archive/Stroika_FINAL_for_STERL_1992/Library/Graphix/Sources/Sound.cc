/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Sound.cc,v 1.2 1992/09/01 15:36:53 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Sound.cc,v $
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.9  1992/04/02  11:35:00  lewis
 *		Use gDisplay instead of Tablet::GetOSDisplay ().
 *
 *		Revision 1.7  1992/01/22  04:16:51  lewis
 *		Use Tablet::GetOSDisplay ().
 *
 *
 */

#include	<string.h>

#include	"OSRenamePre.hh"

#if			qMacOS

#include	<Errors.h>
#include	<Sound.h>
#include	<OSUtils.h>

#elif		qXGDI

#include	<X11/Xlib.h>

#endif		/*GDI*/

#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"Memory.hh"
#include	"OSConfiguration.hh"

#include	"Sound.hh"




/*
 ********************************************************************************
 ************************************* Beep *************************************
 ********************************************************************************
 */

void	Beep ()
{
#if		qMacOS
	::SysBeep (1);
#elif	qUnixOS
	extern	osDisplay*	gDisplay;
	::XBell (gDisplay, 100);
#endif
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

