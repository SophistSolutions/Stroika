/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

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
#if !qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
#endif
}

SimpleLedWordProcessor::~SimpleLedWordProcessor ()
{
#if !qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
    SetCommandHandler (NULL);
    SpecifyTextStore (NULL);
#endif
}

#if qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
#if defined(_MFC_VER)
void SimpleLedWordProcessor::OnInitialUpdate ()
{
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
    inherited::OnInitialUpdate ();
}

void SimpleLedWordProcessor::PostNcDestroy ()
{
    SetCommandHandler (NULL);
    SpecifyTextStore (NULL);
    inherited::PostNcDestroy ();
}
#elif defined(_WIN32)
LRESULT SimpleLedWordProcessor::OnCreate_Msg (LPCREATESTRUCT createStruct)
{
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
    return inherited::OnCreate_Msg (createStruct);
}

void SimpleLedWordProcessor::OnNCDestroy_Msg ()
{
    SetCommandHandler (NULL);
    SpecifyTextStore (NULL);
    inherited::OnNCDestroy_Msg ();
}
#endif
#endif

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
#if !qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
    SetScrollBarType (h, eScrollBarAlways);
    SetScrollBarType (v, eScrollBarAlways);
#endif
}

SimpleLedLineEditor::~SimpleLedLineEditor ()
{
#if !qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
    SpecifyTextStore (NULL);
#endif
}

#if qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
#if defined(_MFC_VER)
void SimpleLedLineEditor::OnInitialUpdate ()
{
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
    inherited::OnInitialUpdate ();
}

void SimpleLedLineEditor::PostNcDestroy ()
{
    SetCommandHandler (NULL);
    SpecifyTextStore (NULL);
    inherited::PostNcDestroy ();
}
#elif defined(_WIN32)
LRESULT SimpleLedLineEditor::OnCreate_Msg (LPCREATESTRUCT createStruct)
{
    SpecifyTextStore (&fTextStore);
    SetCommandHandler (&fCommandHandler);
    return inherited::OnCreate_Msg (createStruct);
}

void SimpleLedLineEditor::OnNCDestroy_Msg ()
{
    SetCommandHandler (NULL);
    SpecifyTextStore (NULL);
    inherited::OnNCDestroy_Msg ();
}
#endif
#endif

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

#if qPlatform_Windows && defined(_MFC_VER)
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

#if qPlatform_Windows && defined(_MFC_VER)
IMPLEMENT_DYNCREATE (LedDialogText, CView)
BEGIN_MESSAGE_MAP (LedDialogText, LedDialogText::inherited)
ON_WM_MOUSEACTIVATE ()
END_MESSAGE_MAP ()
#endif
