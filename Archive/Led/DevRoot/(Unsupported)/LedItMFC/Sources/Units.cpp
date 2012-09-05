/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/Units.cpp,v 1.3 1998/10/30 14:56:05 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Units.cpp,v $
 *	Revision 1.3  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 1.2  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1997/12/24  04:17:04  lewis
 *	Initial revision
 *
 *
 *
 *	<<Based on MFC / MSDEVSTUDIO 5.0 Sample Wordpad - LGP 971212>>
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"Resource.h"

#include	"Units.h"





/*
 ********************************************************************************
 *********************************** CUnit **************************************
 ********************************************************************************
 */
CUnit::CUnit(int nTPU, int nSmallDiv, int nMediumDiv, int nLargeDiv, 
		int nMinMove, const char* nAbbrev, BOOL bSpaceAbbrev)
{
	m_nTPU = nTPU;
	m_nSmallDiv = nSmallDiv;
	m_nMediumDiv = nMediumDiv;
	m_nLargeDiv = nLargeDiv;
	m_nMinMove = nMinMove;
	m_strAbbrev = nAbbrev;
	m_bSpaceAbbrev = bSpaceAbbrev;
}






/*
 ********************************************************************************
 *********************************** StandardUnits ******************************
 ********************************************************************************
 */
const	char	IDS_INCH1_ABBREV[]	=	"\"\"";
const	char	IDS_INCH2_ABBREV[]	=	"in";
const	char	IDS_INCH3_ABBREV[]	=	"inch";
const	char	IDS_INCH4_ABBREV[]	=	"inches";
const	char	IDS_CM_ABBREV[]	=		"cm";
const	char	IDS_POINT_ABBREV[]	=	"pt";
const	char	IDS_PICA_ABBREV[]	=	"pi";
const	CUnit StandardUnits::kUnits[StandardUnits::kNumUnits] =  {
	//	TPU, 	SmallDiv,	MedDiv,	LargeDiv,	MinMove,	szAbbrev,			bSpace
	CUnit(1440,	180,		720,	1440,		90,			IDS_INCH1_ABBREV,	FALSE),//inches
	CUnit(568,	142,		284,	568,		142,		IDS_CM_ABBREV,		TRUE),//centimeters
	CUnit(20,	120,		720,	720,		100,		IDS_POINT_ABBREV,	TRUE),//points
	CUnit(240,	240,		1440,	1440,		120,		IDS_PICA_ABBREV,	TRUE),//picas
	CUnit(1440,	180,		720,	1440,		90,			IDS_INCH2_ABBREV,	FALSE),//in
	CUnit(1440,	180,		720,	1440,		90,			IDS_INCH3_ABBREV,	FALSE),//inch
	CUnit(1440,	180,		720,	1440,		90,			IDS_INCH4_ABBREV,	FALSE)//inches
};


CUnit	StandardUnits::GetCurrentUnits ()
{
#if 0
	TCHAR buf[2];
	buf[0] = NULL;
	GetLocaleInfo(GetUserDefaultLCID(), LOCALE_IMEASURE, buf, 2);
	int nDefUnits = buf[0] == '1' ? 0 : 1;
	SetUnits(GetProfileInt(szSection, szUnits, nDefUnits));
#endif
	return kUnits[0];
}




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
