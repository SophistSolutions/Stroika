/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "TextStore.h"

#include "Marker.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ********************************** MarkerOwner *********************************
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
    return GetTextStore ().GetLength ();
}

/*
@METHOD:        MarkerOwner::GetEnd
@DESCRIPTION:   <p>Return the associated @'TextStore::GetEnd' ().</p>
*/
size_t MarkerOwner::GetEnd () const
{
    return GetTextStore ().GetEnd ();
}

/*
@METHOD:        MarkerOwner::CopyOut
@DESCRIPTION:   <p>Calls the associated @'TextStore::CopyOut' ().</p>
*/
void MarkerOwner::CopyOut (size_t from, size_t count, Led_tChar* buffer) const
{
    GetTextStore ().CopyOut (from, count, buffer);
}

/*
 ********************************************************************************
 ************************************ TempMarker ********************************
 ********************************************************************************
 */
TempMarker::TempMarker (TextStore& ts, size_t start, size_t end)
    : fTextStore{ts}
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
