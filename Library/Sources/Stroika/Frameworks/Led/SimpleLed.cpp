/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <stdlib.h>
#include <string.h>

#include "SimpleLed.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ***************************** SimpleLedWordProcessor ***************************
 ********************************************************************************
 */
SimpleLedWordProcessor::SimpleLedWordProcessor ()
    : inherited ()
    , fCommandHandler (kMaxUndoLevels)
    , fTextStore ()
{
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
}

SimpleLedWordProcessor::~SimpleLedWordProcessor ()
{
    SetCommandHandler (NULL);
    SpecifyTextStore (NULL);
}

#if defined(_MFC_VER)
IMPLEMENT_DYNCREATE (SimpleLedWordProcessor, CView)
BEGIN_MESSAGE_MAP (SimpleLedWordProcessor, SimpleLedWordProcessor::inherited)
END_MESSAGE_MAP ()
#endif

/*
 ********************************************************************************
 ******************************** SimpleLedLineEditor ***************************
 ********************************************************************************
 */
SimpleLedLineEditor::SimpleLedLineEditor ()
    : inherited ()
    , fCommandHandler (kMaxUndoLevels)
    , fTextStore ()
{
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
    SetScrollBarType (h, eScrollBarAlways);
    SetScrollBarType (v, eScrollBarAlways);
}

SimpleLedLineEditor::~SimpleLedLineEditor ()
{
    SpecifyTextStore (NULL);
}

#if defined(_MFC_VER)
IMPLEMENT_DYNCREATE (SimpleLedLineEditor, CView)
BEGIN_MESSAGE_MAP (SimpleLedLineEditor, SimpleLedLineEditor::inherited)
END_MESSAGE_MAP ()
#endif

/*
 ********************************************************************************
 ********************************* LedDialogText ********************************
 ********************************************************************************
 */

LedDialogText::LedDialogText ()
    : inherited ()
{
}

#if qStroika_Platform_Windows && defined(_MFC_VER)
void LedDialogText::PostNcDestroy ()
{
    // Don't auto-delete ourselves!
    CWnd::PostNcDestroy ();
}
int LedDialogText::OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    // Don't do CView::OnMouseActiveate() cuz that assumes our parent is a frame window,
    // and tries to make us the current view in the document...
    int nResult = CWnd::OnMouseActivate (pDesktopWnd, nHitTest, message);
    return nResult;
}
#endif

#if qStroika_Platform_Windows && defined(_MFC_VER)
IMPLEMENT_DYNCREATE (LedDialogText, CView)
BEGIN_MESSAGE_MAP (LedDialogText, LedDialogText::inherited)
ON_WM_MOUSEACTIVATE ()
END_MESSAGE_MAP ()
#endif
