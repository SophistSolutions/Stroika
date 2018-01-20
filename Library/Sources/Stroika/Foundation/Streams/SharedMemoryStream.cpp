/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>

#include "../Execution/Common.h"
#include "../Execution/Exceptions.h"
#include "../Memory/BlockAllocated.h"
#include "../Traversal/Iterator.h"

#include "SharedMemoryStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;
using Memory::Byte;

/*
 ********************************************************************************
 ************************ Streams::SharedMemoryStream ***************************
 ********************************************************************************
 */
template <>
template <>
Memory::BLOB SharedMemoryStream<Byte>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsVector ();
}

template <>
template <>
string SharedMemoryStream<Byte>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsString ();
}

template <>
template <>
Characters::String SharedMemoryStream<Characters::Character>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    auto        tmp = rep.AsVector ();
    return String (Containers::Start (tmp), Containers::End (tmp));
}
