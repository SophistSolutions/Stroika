/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Sound__
#define	__Sound__

/*
 * $Header: /fuji/lewis/RCS/Sound.hh,v 1.2 1992/07/01 03:57:21 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Sound.hh,v $
 *		Revision 1.2  1992/07/01  03:57:21  lewis
 *		Got rid of unneeded String.hh include.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"Exception.hh"

// may need some exceptions here?
//extern	Exception			gSOUNDException;



extern	void	Beep ();

class	Sound { 
	public:
		//		should be able to construct from resources (name and id) and from data, and
		//		empty ones (no args)
		//		also, somewhere in this file to interface to sound input (e.g. from users)
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Sound__*/
