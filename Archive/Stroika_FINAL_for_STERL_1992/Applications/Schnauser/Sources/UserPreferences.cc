/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/UserPreferences.cc,v 1.3 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: UserPreferences.cc,v $
 *		Revision 1.3  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/30  04:00:31  lewis
 *		Initial revision
 *
 *
 *
 */




#include	<fstream.h>

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Dialog.hh"

#include	"UserPreferences.hh"






/*
 ********************************************************************************
 *********************************** UserPreferences ****************************
 ********************************************************************************
 */

#include	"UserPreferencesDLog.hh"
typedef	UserPreferencesDLogX	UserPreferencesDLog;

const	String	kPrefsFileName	=
#if		qMacOS
	"Schnauzer Preferences"
#elif	qUnixOS
	".SchnauzerPrefs"			// <= 14 characters - sysV
#else
	"Schnauzer Preferences"
#endif
	;

UserPreferences	UserPreferences::sUserPreferences;

UserPreferences::UserPreferences ():
	fOnOpen (eNewProject)
{
}

Boolean	UserPreferences::DisplayDialog ()
{
	UserPreferences		prefs	=	GetPrefs ();
	UserPreferencesDLog	dlgView;

	Dialog				d = Dialog (&dlgView, &dlgView, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (Nil, View::eNoUpdate);

	if (d.Pose ()) {
//		copy dlog params to prefs...
		SetPrefs (prefs);
		return (True);
	}
	return (False);
}

const	UserPreferences&	UserPreferences::GetPrefs ()
{
	return (sUserPreferences);
}

void	UserPreferences::SetPrefs (const UserPreferences& newPrefs)
{
	sUserPreferences = newPrefs;
}

void	UserPreferences::Save ()
{
	int fd = open (GetPreferencesFolder () + kPrefsFileName, O_WRONLY | O_CREAT);
	ofstream	outFile = int (fd);
	UserPreferences	prefs	=	GetPrefs ();
#if		qMPW_CFRONT_2_1_FUNNY_STREAM_TRY_TO_OVERLOAD_BUG
	((ostream&)outFile) << int (prefs.fOnOpen) << newline;
#else
	outFile << int (prefs.fOnOpen) << newline;
#endif
	::close (fd);
}

void	UserPreferences::Restore ()
{
	Try {
		int fd = open (GetPreferencesFolder () + kPrefsFileName, O_RDONLY);
		ifstream	inFile = int (fd);
		UserPreferences	prefs	=	GetPrefs ();
		int a = 0;
		inFile >> a;
		if (a >= 0 and a <= 2) {
			prefs.fOnOpen = a;
		}
		SetPrefs (prefs);
		::close (fd);
	}
	Catch () {
		// For now ignore errors, and just assume we didn't find the file - oh well...
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

