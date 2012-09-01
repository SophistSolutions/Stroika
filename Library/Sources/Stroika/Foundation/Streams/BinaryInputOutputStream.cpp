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
    : BinaryStream (rep)
{
    RequireMember (rep.get (), _SharedInputIRep::element_type);
    RequireMember (rep.get (), _SharedOutputIRep::element_type);
    AssertNotNull (_GetInputRep ().get ());
    AssertNotNull (_GetOutputRep ().get ());
}
