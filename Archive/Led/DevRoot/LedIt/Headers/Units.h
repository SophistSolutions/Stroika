/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Units_h__
#define	__Units_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/Units.h,v 1.4 2002/05/06 21:31:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Units.h,v $
 *	Revision 1.4  2002/05/06 21:31:00  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:10  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:44  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */
#include	<afxwin.h>

#include	"LedSupport.h"

#include	"LedItConfig.h"



class Unit
{
public:
	int m_nTPU;
	int m_nSmallDiv;	// small divisions - small line displayed
	int m_nMediumDiv;	// medium divisions - large line displayed
	int m_nLargeDiv;	// large divisions - numbers displayed
	int m_nMinMove;		// minimum tracking movements
	BOOL m_bSpaceAbbrev; // put space before abbreviation
	CString m_strAbbrev;// cm, pt, pi, ", in, inch, inches

	Unit() {}
	Unit(int nTPU, int nSmallDiv, int nMediumDiv, int nLargeDiv, 
		int nMinMove, const char* nAbbrev, BOOL bSpaceAbbrev);

public:
	int	GetTPU () const { return m_nTPU; };
};

class	StandardUnits {
	public:
		enum { kPrimaryNumUnits = 4 };
		enum { kNumUnits = 7 };
		static	const	Unit	kUnits[kNumUnits];

	public:
		static	Unit	GetCurrentUnits ();
};


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif	/*__Units_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

