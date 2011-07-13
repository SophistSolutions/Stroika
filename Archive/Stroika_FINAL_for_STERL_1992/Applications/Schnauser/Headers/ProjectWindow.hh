/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ProjectWindow__
#define	__ProjectWindow__

/*
 * $Header: /fuji/lewis/RCS/ProjectWindow.hh,v 1.2 1992/09/01 17:40:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: ProjectWindow.hh,v $
 *		Revision 1.2  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"Window.hh"

#include	"SchnauserConfig.hh"


// Possably shouldnt inherit from window - just own one???

class	SchnauserDocument;
class	ProjectWindow : public Window {
	public:
		ProjectWindow (SchnauserDocument& myDocument);
		~ProjectWindow ();

	private:
		SchnauserDocument&	fDocument;
		class ProjectMainView;
		ProjectMainView*	fProjectMainView;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ProjectWindow__*/

