/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "String_ReadWriteRep.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete::Private;




/*
 ********************************************************************************
 **************************** ReadOnlyRep::_Rep *********************************
 ********************************************************************************
 */
#if 0
void    ReadWriteRep::_Rep::RemoveAll ()
{
    Assert (_fStart <= _fEnd);
    _fEnd = _fStart;
}
#endif

void    ReadWriteRep::_Rep::SetAt (Character item, size_t index)
{
    Assert (_fStart <= _fEnd);
    Require (index < GetLength ());
    _PeekStart ()[index] = item.As<wchar_t> ();
}

void    ReadWriteRep::_Rep::RemoveAt (size_t from, size_t to)
{
    Require (from <= to);
    Require (to <= GetLength ());
    Assert (_fStart <= _fEnd);
    wchar_t*    lhs =   &_PeekStart () [from];
    size_t      amountToRemove  =   to - from;
    wchar_t*    rhs =   &lhs [amountToRemove];
    for (size_t i = (_fEnd - _fStart) - from - amountToRemove; i > 0; i--) {
        *lhs++ = *rhs++;
    }
    _fEnd -= amountToRemove;
    Ensure (_fStart <= _fEnd);
}
