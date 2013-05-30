/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "Adapters.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Hash;



// Robert Jenkins' 32 bit integer hash function
//
////			@todo	FIND SOURCE/COPYRIGHT ATTRIBUTE - NOT SURE WHERE I GOT THIS?
//
//
uint32_t  Adapapter<Hasher<uint32_t, Algorithms::Jenkins>>::Hash (uint32_t data2Hash)
{
    uint32_t a = data2Hash;
    a = (a + 0x7ed55d16) + (a << 12);
    a = (a ^ 0xc761c23c) ^ (a >> 19);
    a = (a + 0x165667b1) + (a << 5);
    a = (a + 0xd3a2646c) ^ (a << 9);
    a = (a + 0xfd7046c5) + (a << 3);
    a = (a ^ 0xb55a4f09) ^ (a >> 16);
    return a;
}

