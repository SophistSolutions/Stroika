/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__UserPreferences__
#define	__UserPreferences__

/*
 * $Header: /fuji/lewis/RCS/UserPreferences.hh,v 1.1 1992/06/30 23:05:21 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: UserPreferences.hh,v $
 *		Revision 1.1  1992/06/30  23:05:21  lewis
 *		Initial revision
 *
# Revision 1.1  1992/06/30  03:57:05  lewis
# Initial revision
#
 *
 *
 *
 */

#include	"SchnauserConfig.hh"


class	UserPreferences {
	public:
		UserPreferences ();


	// What do we do on application startup - eNewProject is default
	public:
		enum OnOpen { eNoProject, eNewProject, eLastOpenedProjects };
		OnOpen	fOnOpen;



	// Display Preferences Dialog - return True if there were changes...
	public:
		static	Boolean	DisplayDialog ();

	// Peek/Update global preferences...
	public:
		static	const	UserPreferences&	GetPrefs ();
		static	void						SetPrefs (const UserPreferences& newPrefs);
	private:
		static	UserPreferences	sUserPreferences;


	// Save/Restore from Prefs File...
	public:
		static	void	Save ();
		static	void	Restore ();
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__UserPreferences__*/

