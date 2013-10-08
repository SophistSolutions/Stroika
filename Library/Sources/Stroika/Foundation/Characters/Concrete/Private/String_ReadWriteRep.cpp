/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
void    ReadWriteRep::_Rep::RemoveAll ()
{
    Assert (_fStart <= _fEnd);
    _fEnd = _fStart;
}

void    ReadWriteRep::_Rep::SetAt (Character item, size_t index)
{
    Assert (_fStart <= _fEnd);
    Require (index < GetLength ());
    _PeekStart ()[index] = item.As<wchar_t> ();
}

void    ReadWriteRep::_Rep::RemoveAt (size_t index, size_t amountToRemove)
{
    Assert (_fStart <= _fEnd);
    Require (index + amountToRemove <= GetLength ());
    wchar_t*    lhs =   &_PeekStart () [index];
    wchar_t*    rhs =   &lhs [amountToRemove];
    for (size_t i = (_fEnd - _fStart) - index - amountToRemove; i > 0; i--) {
        *lhs++ = *rhs++;
    }
    _fEnd -= amountToRemove;
    Ensure (_fStart <= _fEnd);
}
