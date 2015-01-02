/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "BinaryTiedStreams.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;





/*
 ********************************************************************************
 ************************* Streams::BinaryTiedStreams ***************************
 ********************************************************************************
 */
BinaryTiedStreams::BinaryTiedStreams (const BinaryStream::_SharedIRep& inOutRep)
    : BinaryInputStream (std::dynamic_pointer_cast<BinaryInputStream::_IRep> (inOutRep))
    , BinaryOutputStream (std::dynamic_pointer_cast<BinaryOutputStream::_IRep> (inOutRep))
{
    RequireMember (inOutRep.get (), BinaryInputStream::_IRep);
    RequireMember (inOutRep.get (), BinaryOutputStream::_IRep);
}

BinaryTiedStreams::BinaryTiedStreams (const BinaryInputStream& in, const BinaryOutputStream& out)
    : BinaryInputStream (in)
    , BinaryOutputStream (out)
{
}
