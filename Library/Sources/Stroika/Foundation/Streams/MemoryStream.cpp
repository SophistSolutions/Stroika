/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>

#include "../Execution/Common.h"
#include "../Execution/Exceptions.h"
#include "../Memory/BlockAllocated.h"
#include "../Traversal/Iterator.h"

#include "MemoryStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;
using Memory::Byte;

/*
 ********************************************************************************
 ***************************** Streams::MemoryStream ****************************
 ********************************************************************************
 */
template <>
template <>
Memory::BLOB MemoryStream<Byte>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsVector ();
}

template <>
template <>
Memory::BLOB MemoryStream<uint8_t>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsVector ();
}

template <>
template <>
string MemoryStream<Byte>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsString ();
}

template <>
template <>
string MemoryStream<uint8_t>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsString ();
}

template <>
template <>
Characters::String MemoryStream<Characters::Character>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    auto        tmp = rep.AsVector ();
    return String (Containers::Start (tmp), Containers::End (tmp));
}
