/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>

#include "../Debug/Cast.h"
#include "../Execution/Common.h"
#include "../Execution/Throw.h"
#include "../Memory/BlockAllocated.h"
#include "../Traversal/Iterator.h"

#include "SharedMemoryStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;

/*
 ********************************************************************************
 ************************ Streams::SharedMemoryStream ***************************
 ********************************************************************************
 */
template <>
template <>
Memory::BLOB SharedMemoryStream<byte>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *Debug::UncheckedDynamicCast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsVector ();
}

template <>
template <>
string SharedMemoryStream<byte>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *Debug::UncheckedDynamicCast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsString ();
}

template <>
template <>
Characters::String SharedMemoryStream<Characters::Character>::Ptr::As () const
{
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *Debug::UncheckedDynamicCast<const Rep_*> (&_GetRepConstRef ());
    auto        tmp = rep.AsVector ();
    #if qCompilerAndStdLib_spanOfContainer_Buggy
    return String{span{tmp.data (), tmp.size ()}};
    #else
    return String{span{tmp}};
    #endif
}
