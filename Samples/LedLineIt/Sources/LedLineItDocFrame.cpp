/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#include "LedLineItView.h"
#include "Resource.h"

#include "LedLineItDocFrame.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ******************************** LedLineItDocFrame *****************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE (LedLineItDocFrame, CMDIChildWnd)
BEGIN_MESSAGE_MAP (LedLineItDocFrame, LedLineItDocFrame::inherited)
ON_WM_CREATE ()
END_MESSAGE_MAP ()

int LedLineItDocFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
    RequireNotNull (lpCreateStruct);

    if (inherited::OnCreate (lpCreateStruct) == -1) {
        return -1;
    }

    /*
     *  When we are creating the first MDI child window, start out with it maximized. No point in
     *  wasting all that space! I dunno why MDI doesn't do this by default?
     */
    CMDIFrameWnd* owningFrame = GetMDIFrame ();
    RequireNotNull (owningFrame);
    if (owningFrame->MDIGetActive () == NULL) {
        ::SetWindowLong (m_hWnd, GWL_STYLE, GetStyle () | WS_MAXIMIZE);
    }

    return 0;
}

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
