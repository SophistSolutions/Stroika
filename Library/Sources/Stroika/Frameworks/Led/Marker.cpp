/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "TextStore.h"

#include "Marker.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ************************************ MarkerOwner *******************************
 ********************************************************************************
 */
/*
@METHOD:        MarkerOwner::FindNextCharacter
@DESCRIPTION:   <p>Return the associated @'TextStore::FindNextCharacter' ().</p>
*/
size_t MarkerOwner::FindNextCharacter (size_t afterPos) const
{
    return GetTextStore ().FindNextCharacter (afterPos);
}

/*
@METHOD:        MarkerOwner::FindPreviousCharacter
@DESCRIPTION:   <p>Return the associated @'TextStore::FindPreviousCharacter' ().</p>
*/
size_t MarkerOwner::FindPreviousCharacter (size_t beforePos) const
{
    return GetTextStore ().FindPreviousCharacter (beforePos);
}

/*
@METHOD:        MarkerOwner::GetLength
@DESCRIPTION:   <p>Return the associated @'TextStore::GetLength' ().</p>
*/
size_t MarkerOwner::GetLength () const
{
    return (GetTextStore ().GetLength ());
}

/*
@METHOD:        MarkerOwner::GetEnd
@DESCRIPTION:   <p>Return the associated @'TextStore::GetEnd' ().</p>
*/
size_t MarkerOwner::GetEnd () const
{
    return (GetTextStore ().GetEnd ());
}

/*
@METHOD:        MarkerOwner::CopyOut
@DESCRIPTION:   <p>Calls the associated @'TextStore::CopyOut' ().</p>
*/
void MarkerOwner::CopyOut (size_t from, size_t count, Led_tChar* buffer) const
{
    GetTextStore ().CopyOut (from, count, buffer);
}

#if qMultiByteCharacters
void MarkerOwner::Assert_CharPosDoesNotSplitCharacter (size_t charPos) const
{
#if qDebug
    GetTextStore ().Assert_CharPosDoesNotSplitCharacter (charPos);
#endif
}
#endif

/*
 ********************************************************************************
 ************************************ TempMarker ********************************
 ********************************************************************************
 */
TempMarker::TempMarker (TextStore& ts, size_t start, size_t end)
    : inherited ()
    , fTextStore (ts)
    , fMarker ()
{
    Require (start <= end);
    GetTextStore ().AddMarkerOwner (this);
    try {
        GetTextStore ().AddMarker (&fMarker, start, end - start, this);
    }
    catch (...) {
        GetTextStore ().RemoveMarkerOwner (this);
        throw;
    }
}

TempMarker::~TempMarker ()
{
    try {
        GetTextStore ().RemoveMarker (&fMarker);
    }
    catch (...) {
    }
    try {
        GetTextStore ().RemoveMarkerOwner (this);
    }
    catch (...) {
    }
}

TextStore* TempMarker::PeekAtTextStore () const
{
    return &fTextStore;
}
