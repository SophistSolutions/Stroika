/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(WIN32)
#include <afxwin.h>
#endif

#include "LedItResources.h"

#include "Units.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 *********************************** Unit ***************************************
 ********************************************************************************
 */
Unit::Unit (int nTPU, int nSmallDiv, int nMediumDiv, int nLargeDiv,
            int nMinMove, const char* nAbbrev, BOOL bSpaceAbbrev)
{
    m_nTPU         = nTPU;
    m_nSmallDiv    = nSmallDiv;
    m_nMediumDiv   = nMediumDiv;
    m_nLargeDiv    = nLargeDiv;
    m_nMinMove     = nMinMove;
    m_strAbbrev    = nAbbrev;
    m_bSpaceAbbrev = bSpaceAbbrev;
}

/*
 ********************************************************************************
 *********************************** StandardUnits ******************************
 ********************************************************************************
 */
const char IDS_INCH1_ABBREV[]                              = "\"\"";
const char IDS_INCH2_ABBREV[]                              = "in";
const char IDS_INCH3_ABBREV[]                              = "inch";
const char IDS_INCH4_ABBREV[]                              = "inches";
const char IDS_CM_ABBREV[]                                 = "cm";
const char IDS_POINT_ABBREV[]                              = "pt";
const char IDS_PICA_ABBREV[]                               = "pi";
const Unit StandardUnits::kUnits[StandardUnits::kNumUnits] = {
    //  TPU,    SmallDiv,   MedDiv, LargeDiv,   MinMove,    szAbbrev,           bSpace
    Unit (1440, 180, 720, 1440, 90, IDS_INCH1_ABBREV, FALSE), //inches
    Unit (568, 142, 284, 568, 142, IDS_CM_ABBREV, TRUE),      //centimeters
    Unit (20, 120, 720, 720, 100, IDS_POINT_ABBREV, TRUE),    //points
    Unit (240, 240, 1440, 1440, 120, IDS_PICA_ABBREV, TRUE),  //picas
    Unit (1440, 180, 720, 1440, 90, IDS_INCH2_ABBREV, FALSE), //in
    Unit (1440, 180, 720, 1440, 90, IDS_INCH3_ABBREV, FALSE), //inch
    Unit (1440, 180, 720, 1440, 90, IDS_INCH4_ABBREV, FALSE)  //inches
};

Unit StandardUnits::GetCurrentUnits ()
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
