/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__Units_h__
#define	__Units_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/Units.h,v 1.2 1997/12/24 04:46:26 lewis Exp $
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
 *	Revision 1.2  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  1997/12/24  04:07:11  lewis
 *	Initial revision
 *
 *
 *
 *	<<Based on MFC / MSDEVSTUDIO 5.0 Sample Wordpad - LGP 971212>>
 *
 *
 */
#include	<afxwin.h>

#include	"LedSupport.h"

#include	"LedItConfig.h"



class CUnit
{
public:
	int m_nTPU;
	int m_nSmallDiv;	// small divisions - small line displayed
	int m_nMediumDiv;	// medium divisions - large line displayed
	int m_nLargeDiv;	// large divisions - numbers displayed
	int m_nMinMove;		// minimum tracking movements
	BOOL m_bSpaceAbbrev; // put space before abbreviation
	CString m_strAbbrev;// cm, pt, pi, ", in, inch, inches

	CUnit() {}
	CUnit(int nTPU, int nSmallDiv, int nMediumDiv, int nLargeDiv, 
		int nMinMove, const char* nAbbrev, BOOL bSpaceAbbrev);

public:
	int	GetTPU () const { return m_nTPU; };
};

class	StandardUnits {
	public:
		enum { kPrimaryNumUnits = 4 };
		enum { kNumUnits = 7 };
		static	const	CUnit	kUnits[kNumUnits];

	public:
		static	CUnit	GetCurrentUnits ();
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

