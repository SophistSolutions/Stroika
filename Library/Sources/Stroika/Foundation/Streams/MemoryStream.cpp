/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "MemoryStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;

/*
 ********************************************************************************
 ************************ Streams::MemoryStream::Ptr ****************************
 ********************************************************************************
 */
template <>
Memory::BLOB MemoryStream::Ptr<byte>::AsBLOB_ () const
{
    using Rep_      = MemoryStream::Private_::Rep_<byte>;
    const Rep_& rep = *Debug::UncheckedDynamicCast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsVector ();
}
template <>
string MemoryStream::Ptr<byte>::Asstring_ () const
{
    using Rep_      = MemoryStream::Private_::Rep_<byte>;
    const Rep_& rep = *Debug::UncheckedDynamicCast<const Rep_*> (&_GetRepConstRef ());
    return rep.AsString ();
}
template <>
Characters::String MemoryStream::Ptr<Characters::Character>::AsString_ () const
{
    using Rep_ = MemoryStream::Private_::Rep_<Characters::Character>;
    AssertMember (&_GetRepConstRef (), Rep_);
    const Rep_& rep = *dynamic_cast<const Rep_*> (&_GetRepConstRef ());
    auto        tmp = rep.AsVector ();
    return String{span{tmp}};
}
