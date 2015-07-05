/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>

#include    "../Execution/Common.h"
#include    "../Execution/Exceptions.h"
#include    "../Memory/BlockAllocated.h"
#include    "../Traversal/Iterator.h"

#include    "MemoryStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;

using   Characters::Character;
using   Characters::String;
using   Memory::Byte;


/*
 ********************************************************************************
 ************************** Streams::MemoryStream *******************************
 ********************************************************************************
 */
template    <>
template    <>
Memory::BLOB   MemoryStream<Byte>::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), Rep_);
    const Rep_&    rep =   *dynamic_cast<const Rep_*> (_GetRep ().get ());
    return rep.AsVector ();
}

template    <>
template    <>
string   MemoryStream<Byte>::As () const
{
    RequireNotNull (_GetRep ().get ());
    AssertMember (_GetRep ().get (), Rep_);
    const Rep_&    rep =   *dynamic_cast<const Rep_*> (_GetRep ().get ());
    return rep.AsString ();
}


template    <>
template    <>
Characters::String   MemoryStream<Characters::Character>::As () const
{
    RequireNotNull (_GetRep ().get ());
    const Rep_&    rep =   *reinterpret_cast<const Rep_*> (_GetRep ().get ());
    auto tmp = rep.AsVector ();
    return String (Containers::Start (tmp), Containers::End (tmp));
}

