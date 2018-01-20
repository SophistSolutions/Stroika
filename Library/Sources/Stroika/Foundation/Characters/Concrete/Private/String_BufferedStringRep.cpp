/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "String_BufferedStringRep.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Concrete::Private;

/*
 ********************************************************************************
 ********************** BufferedStringRep ::_Rep ********************************
 ********************************************************************************
 */
void BufferedStringRep::_Rep::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
    Require (index >= 0);
    Require (index <= _GetLength ());
    Require (srcStart < srcEnd);

    // cannot insert pointer FROM THIS REP! Doing so would be buggy! MAYBE I need to handle this with special case logic - before I do the resize?
    //      -- LGP 2011-12-04
    Assert (not(_fStart <= reinterpret_cast<const wchar_t*> (srcStart) and reinterpret_cast<const wchar_t*> (srcStart) <= _fEnd));

    size_t origLen     = _GetLength ();
    size_t amountToAdd = (srcEnd - srcStart);
    SetLength_ (origLen + amountToAdd);
    size_t newLen = origLen + amountToAdd;
    Assert (newLen == _GetLength ());
    wchar_t* gapStart = _PeekStart () + index;
    // make space for insertion
    if (origLen != index) {
        // if test un-needed, but appears to be optimization on MSVC2k13 - as its common we append
        // -- LGP 2013-10-08
        (void)::memmove (gapStart + amountToAdd, gapStart, (origLen - index) * sizeof (wchar_t));
    }
    // now copy in new characters
    (void)::memcpy (gapStart, srcStart, amountToAdd * sizeof (wchar_t));
    Ensure (_fStart <= _fEnd);
}

void BufferedStringRep::_Rep::SetLength_ (size_t newLength)
{
    ReserveAtLeast_ (newLength + 1);
    _fEnd                    = _fStart + newLength; // we don't bother doing anything to added/removed characters
    _PeekStart ()[newLength] = '\0';
    Ensure (_GetLength () == newLength);
}

const wchar_t* BufferedStringRep::_Rep::c_str_peek () const noexcept
{
    size_t len = _GetLength ();
    Ensure (_fStart[len] == '\0');
    return _fStart;
}
