/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "WordWrappedTextInteractor.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ***************************** WordWrappedTextInteractor ************************
 ********************************************************************************
 */
WordWrappedTextInteractor::WordWrappedTextInteractor ()
    : inherited ()
{
}

/*
@METHOD:        WordWrappedTextInteractor::OnTypedNormalCharacter
@DESCRIPTION:   <p>Override @'TextInteractor::OnTypedNormalCharacter' to map 'shiftPressed' + NL to a soft-line break.</p>
*/
void WordWrappedTextInteractor::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
    if (theChar == '\n' and shiftPressed) {
        /*
         *  Map 'shiftPressed' + NL to a soft-line break, but shutoff controlchar checking since that would
         *  generate an exception for this character (SPR#1080).
         */
        bool savedSuppressFlag = GetSuppressTypedControlCharacters ();
        SetSuppressTypedControlCharacters (false);
        try {
            inherited::OnTypedNormalCharacter (WordWrappedTextImager::kSoftLineBreakChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
        }
        catch (...) {
            SetSuppressTypedControlCharacters (savedSuppressFlag);
            throw;
        }
        SetSuppressTypedControlCharacters (savedSuppressFlag);
    }
    else {
        inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
    }
}

void WordWrappedTextInteractor::SetTopRowInWindow (size_t newTopRow)
{
    PreScrollInfo preScrollInfo;
    PreScrollHelper (eDefaultUpdate, &preScrollInfo);
    WordWrappedTextImager::SetTopRowInWindow (newTopRow);
    PostScrollHelper (preScrollInfo);
}

void WordWrappedTextInteractor::SetTopRowInWindow (RowReference row)
{
    PreScrollInfo preScrollInfo;
    PreScrollHelper (eDefaultUpdate, &preScrollInfo);
    WordWrappedTextImager::SetTopRowInWindow (row);
    PostScrollHelper (preScrollInfo);
}

void WordWrappedTextInteractor::SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode)
{
    SetTopRowInWindow (GetRowReferenceContainingPosition (markerPos), updateMode);
}
