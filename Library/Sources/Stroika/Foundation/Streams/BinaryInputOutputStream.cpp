/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "BinaryInputOutputStream.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;





/*
 ********************************************************************************
 ******************* Streams::BinaryInputOutputStream ***************************
 ********************************************************************************
 */
BinaryInputOutputStream::BinaryInputOutputStream (const BinaryStream::_SharedIRep& rep)
    : Streams::BinaryInputStream (dynamic_pointer_cast<BinaryInputStream::_IRep> (rep))
    , Streams::BinaryOutputStream (dynamic_pointer_cast<BinaryOutputStream::_IRep> (rep))
{
    RequireNotNull (dynamic_pointer_cast<BinaryInputStream::_IRep> (rep).get ());
    RequireNotNull (dynamic_pointer_cast<BinaryOutputStream::_IRep> (rep).get ());
}
