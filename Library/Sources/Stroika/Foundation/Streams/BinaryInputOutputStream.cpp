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
    RequireNotNull (GetInputRep ().get ());
    RequireNotNull (GetOutputRep ().get ());
}
