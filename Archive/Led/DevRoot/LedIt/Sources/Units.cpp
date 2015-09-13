/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Sources/Units.cpp,v 1.6 2003/03/21 15:00:10 lewis Exp $
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
 *	Revision 1.6  2003/03/21 15:00:10  lewis
 *	SPR#1368 - added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 1.5  2002/10/24 15:53:16  lewis
 *	SPR#0680- adjust the window margins so text looks a little better. Did for MacOS/XWin/Win -
 *	but only tested on Win so far
 *	
 *	Revision 1.4  2002/05/06 21:31:10  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:17  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:50  lewis
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
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#if		defined (WIN32)
	#include	<afxwin.h>
#endif

#include	"LedItResources.h"

#include	"Units.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



/*
 ********************************************************************************
 *********************************** Unit ***************************************
 ********************************************************************************
 */
Unit::Unit (int nTPU, int nSmallDiv, int nMediumDiv, int nLargeDiv, 
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
const	Unit StandardUnits::kUnits[StandardUnits::kNumUnits] =  {
	//	TPU, 	SmallDiv,	MedDiv,	LargeDiv,	MinMove,	szAbbrev,			bSpace
	Unit(1440,	180,		720,	1440,		90,			IDS_INCH1_ABBREV,	FALSE),//inches
	Unit(568,	142,		284,	568,		142,		IDS_CM_ABBREV,		TRUE),//centimeters
	Unit(20,	120,		720,	720,		100,		IDS_POINT_ABBREV,	TRUE),//points
	Unit(240,	240,		1440,	1440,		120,		IDS_PICA_ABBREV,	TRUE),//picas
	Unit(1440,	180,		720,	1440,		90,			IDS_INCH2_ABBREV,	FALSE),//in
	Unit(1440,	180,		720,	1440,		90,			IDS_INCH3_ABBREV,	FALSE),//inch
	Unit(1440,	180,		720,	1440,		90,			IDS_INCH4_ABBREV,	FALSE)//inches
};


Unit	StandardUnits::GetCurrentUnits ()
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
