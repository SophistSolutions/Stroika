/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#ifndef __Units_h__
#define __Units_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxwin.h>

#include "Stroika/Frameworks/Led/Support.h"

#include "LedItConfig.h"

class Unit {
public:
    int     m_nTPU{};
    int     m_nSmallDiv{};    // small divisions - small line displayed
    int     m_nMediumDiv{};   // medium divisions - large line displayed
    int     m_nLargeDiv{};    // large divisions - numbers displayed
    int     m_nMinMove{};     // minimum tracking movements
    BOOL    m_bSpaceAbbrev{}; // put space before abbreviation
    CString m_strAbbrev{};    // cm, pt, pi, ", in, inch, inches

    Unit () = default;
    Unit (int nTPU, int nSmallDiv, int nMediumDiv, int nLargeDiv,
          int nMinMove, const char* nAbbrev, BOOL bSpaceAbbrev);

public:
    int GetTPU () const { return m_nTPU; };
};

class StandardUnits {
public:
    enum { kPrimaryNumUnits = 4 };
    enum { kNumUnits = 7 };
    static const Unit kUnits[kNumUnits];

public:
    static Unit GetCurrentUnits ();
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*__Units_h__*/
