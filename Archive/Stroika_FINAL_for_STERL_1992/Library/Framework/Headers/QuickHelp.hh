/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__QuickHelp__
#define	__QuickHelp__

/*
 * $Header: /fuji/lewis/RCS/QuickHelp.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *			Need to do a abtract, native and portable implementation here!!!
 *
 * Notes:
 *
 * Changes:
 *	$Log: QuickHelp.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.6  92/02/21  19:50:34  19:50:34  lewis (Lewis Pringle)
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *		
 *
 *
 */

#include	"String.hh"

#include	"Region.hh"

#include	"Config-Framework.hh"






class	QuickHelp {
	public:
		QuickHelp ();
		
		nonvirtual	Boolean	GetHelpEnabled () const;
		virtual		void	SetHelpEnabled (Boolean enabled);
		
		virtual		String	GetHelp () const;
		virtual		void	SetHelp (const String& help);
		virtual		void	SetHelp (const char* help);

		virtual		void	ShowHelp (const Point& globalWhere, const Region& globalRegion);
		virtual		Boolean	TrackHelp (const Point& cursorAt, Region& helpRegion) = Nil;
		
		static		void	HideHelp ();

	private:
		String	fHelp;
		static	Boolean	sHelpEnabled;
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__QuickHelp__*/
