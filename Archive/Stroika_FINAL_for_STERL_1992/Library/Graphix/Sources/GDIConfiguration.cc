/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GDIConfiguration.cc,v 1.2 1992/09/01 15:36:53 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: GDIConfiguration.cc,v $
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/02/08  04:50:25  lewis
 *		Moved some stuff here from OSConfiguration.
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

#include	"GDIConfiguration.hh"




/*
 ********************************************************************************
 ****************************** GDIConfiguration ********************************
 ********************************************************************************
 */

GDIConfiguration::GDIConfiguration ():
	OSConfiguration ()
{
}

#if		qMacGDI
Boolean	GDIConfiguration::ColorQDAvailable () const
{
	static	SysEnvRec	sSysEnvironsRec;
	static	Boolean		inited	=	False;
	if (not inited) {
		inited = True;
		(void)::SysEnvirons (curSysEnvVers, &sSysEnvironsRec);
	}

	/*
	 * Faster to check sys environs record, than to call gestalt.  -- actually really irreleveant since we can cache result!!!
	 * SWITCH TO GESTALT...
	 */
	return Boolean (sSysEnvironsRec.hasColorQD);
}

Boolean	GDIConfiguration::ColorQD32Available () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltQuickdrawVersion, &response);
	if (err != noErr) {
		// might be some other way to tell, but assume the worst, if Gestalt cannot tell 
		return (False);
	}
	return (Boolean (response >= gestalt32BitQD));
}

Boolean	GDIConfiguration::HasOutlineFonts () const
{
	long	response	=	0;
	OSErr	err			=	::Gestalt (gestaltFontMgrAttr, &response);
	if (err != noErr) {
		return (False);			// if no Gestalt, then not available
	}
	return (Boolean (!!((response & (1 << gestaltOutlineFonts)))));
}
#endif	/*qMacGDI*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


