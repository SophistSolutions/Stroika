/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __FontMenu_hh__
#define __FontMenu_hh__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afx.h>

#include "Stroika/Frameworks/Led/Support.h"

#include "LedLineItConfig.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

class CMenu;
void FixupFontMenu (CMenu* fontMenu);

Led_SDK_String CmdNumToFontName (UINT cmdNum);

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*__FontMenu_hh__*/
