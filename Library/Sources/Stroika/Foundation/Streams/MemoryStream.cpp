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
