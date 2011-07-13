/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SimpleAboutBox.cc,v 1.3 1992/09/01 15:58:05 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SimpleAboutBox.cc,v $
 *		Revision 1.3  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/02/17  23:17:34  lewis
 *		Use new version support - new class instead of application.
 *
 *
 *
 */

#include	"Version.hh"

#include	"Alert.hh"
#include	"Dialog.hh"

#include	"SimpleAboutBox.hh"




//#include	"SimpleAboutBox.dlg"



/*
 ********************************************************************************
 *************************** DisplaySimpleAboutBox ******************************
 ********************************************************************************
 */

void	DisplaySimpleAboutBox ()
{
	String	appName	=	kApplicationVersion.GetLongVersionString ();
	if (appName == kEmptyString) {
		appName = "about application <vers resource not found>";
	}
// try appending stroika version info by default
	appName += String ("\n") + kStroikaVersion.GetLongVersionString ();

	Alert a (appName, AbstractPushButton::kOKLabel);
	a.SetFont (&kApplicationFont);		// looks better...
	a.Pose ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

