/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>

#include "../Execution/Common.h"
#include "../Execution/Throw.h"
#include "../Memory/BlockAllocated.h"
#include "../Traversal/Iterator.h"

#include "MemoryStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;

/*
 ********************************************************************************
 ***************************** Streams::MemoryStream ****************************
 ********************************************************************************
 */
template <>
template <>
Memory::BLOB MemoryStream<byte>::Ptr::As () const
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
string MemoryStream<byte>::Ptr::As () const
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
