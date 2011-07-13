/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/QuickHelp.cc,v 1.4 1992/09/11 19:47:11 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: QuickHelp.cc,v $
 *		Revision 1.4  1992/09/11  19:47:11  sterling
 *		*** empty log message ***
 *
 *		Revision 1.3  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/04/08  15:25:59  lewis
 *		Workaround qMPW_STATIC_CONSTRUCTOR_COMPLEX_INITIALIZER_NYI_BUG.
 *
 *		
 *
 */


#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Balloons.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"OSConfiguration.hh"

#include	"QuickHelp.hh"



#if		qMacToolkit
#if		qMPW_STATIC_CONSTRUCTOR_COMPLEX_INITIALIZER_NYI_BUG
inline	Boolean x () { return (OSConfiguration ().HasHelpManager ()); }
Boolean	QuickHelp::sHelpEnabled = x ();
#else
Boolean	QuickHelp::sHelpEnabled = OSConfiguration ().HasHelpManager ();
#endif
#else
Boolean	QuickHelp::sHelpEnabled = False;
#endif





/*
 ********************************************************************************
 ************************************* QuickHelp ********************************
 ********************************************************************************
 */
QuickHelp::QuickHelp ():
	fHelp (kEmptyString)
{
}
		
Boolean	QuickHelp::GetHelpEnabled () const
{
#if		qMacToolkit
	return Boolean (sHelpEnabled and ::HMGetBalloons ());
#endif	/*Toolkit*/
}

void	QuickHelp::SetHelpEnabled (Boolean enabled)
{
	sHelpEnabled = enabled;
}

String	QuickHelp::GetHelp () const
{
	return (fHelp);
}

void	QuickHelp::SetHelp (const String& help)
{
	fHelp = help;
}

void	QuickHelp::SetHelp (const char* help)
{
	AssertNotNil (help);
	fHelp = help;
}
		
void	QuickHelp::ShowHelp (const Point& globalWhere, const Region& globalRegion)
{
#if		qMacToolkit
	Require (GetHelpEnabled ());
	Require (OSConfiguration ().HasHelpManager ());

	osPoint	osWhere;
	os_cvt (globalWhere, osWhere);
	HMMessageRecord	helpRecord;

	UInt8	helpLength		=	UInt8 (GetHelp ().GetLength ());	// create temporary of type
																	// UInt8 since MPW 3.2 headers
																	// accidendtally declared as hmmString
																	// as array of 256 signed chars, and that
																	// means must be careful when treating first
																	// byte as a length byte...
	helpRecord.hmmHelpType = khmmString;
	GetHelp ().ToPStringTrunc (helpRecord.u.hmmString, sizeof (helpRecord.u.hmmString));

	OSErr err = ::HMShowBalloon (&helpRecord, osWhere, NULL, NULL, 0, 0, kHMRegularWindow);
	Assert ((err == noErr) or (err == hmBalloonAborted));
#endif	/*qMacOS*/
}

void	QuickHelp::HideHelp ()
{
#if		qMacOS
	if (OSConfiguration ().HasHelpManager ()) {
		::HMRemoveBalloon ();
	}
#endif	/*Toolkit*/
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


