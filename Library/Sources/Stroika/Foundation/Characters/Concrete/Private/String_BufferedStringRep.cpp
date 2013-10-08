/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "String_BufferedStringRep.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete::Private;






/*
 ********************************************************************************
 ********************** BufferedStringRep ::_Rep ********************************
 ********************************************************************************
 */


void    BufferedStringRep ::_Rep::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
    Require (index >= 0);
    Require (index <= GetLength ());
    Require (srcStart < srcEnd);

    // cannot insert pointer FROM THIS REP! Doing so would be buggy! MAYBE I need to handle this with special case logic - before I do the resize?
    //      -- LGP 2011-12-04
    Assert (not (_fStart <= reinterpret_cast<const wchar_t*> (srcStart) and reinterpret_cast<const wchar_t*> (srcStart) <= _fEnd));

    size_t      origLen     =   GetLength ();
    size_t      amountToAdd =   (srcEnd - srcStart);
    SetLength (origLen + amountToAdd);
    size_t      newLen      =   origLen + amountToAdd;
    Assert (newLen == GetLength ());
    wchar_t*    gapStart    =   _PeekStart () + index;
    // make space for insertion
    (void)::memmove (gapStart + amountToAdd, gapStart, (origLen - index) * sizeof (wchar_t));
    // now copy in new characters
    (void)::memcpy (gapStart, srcStart, amountToAdd * sizeof (wchar_t));
    Ensure (_fStart <= _fEnd);
}
void        BufferedStringRep ::_Rep::SetLength (size_t newLength)
{
    if (newLength < capacity ()) {
        // We COULD shrink the capacity at this point, but not critical right now...
    }
    else {
        ReserveAtLeast_ (newLength);
    }
    _fEnd = _fStart + newLength;    // we don't bother doing anything to added/removed characters
    Ensure (GetLength () == newLength);
}
const wchar_t*  BufferedStringRep ::_Rep::c_str_peek () const noexcept
{
    size_t  len =   GetLength ();
    if (len < fCapacity_) {
        const_cast<wchar_t*> (_fStart)[len] = '\0';     // Cheaper to always set than to check, and maybe set
        return _fStart;
    }
    return nullptr;
}
const wchar_t*      BufferedStringRep ::_Rep::c_str_change ()
{
    ReserveAtLeast_ (GetLength () + 1);
    return c_str_peek ();
}
