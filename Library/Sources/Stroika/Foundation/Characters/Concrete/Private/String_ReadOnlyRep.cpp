/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Containers/Common.h"
#include    "../../../Debug/Assertions.h"
#include    "../../../Execution/Exceptions.h"

#include    "String_ReadOnlyRep.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete::Private;


using   Traversal::Iterator;
using   Traversal::IteratorOwnerID;




/*
 ********************************************************************************
 **************************** ReadOnlyRep::_Rep *********************************
 ********************************************************************************
 */
void    ReadOnlyRep::_Rep::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

void    ReadOnlyRep::_Rep::SetAt (Character item, size_t index)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

void    ReadOnlyRep::_Rep::RemoveAt (size_t from, size_t to)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

const wchar_t*  ReadOnlyRep::_Rep::c_str_peek () const  noexcept
{
    return nullptr;
}

const wchar_t*      ReadOnlyRep::_Rep::c_str_change ()
{
    Execution::DoThrow (UnsupportedFeatureException ());
}
