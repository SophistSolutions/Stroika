/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "Jenkins.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Hash;



namespace {
    /*
     *  Implementation based on text from http://en.wikipedia.org/wiki/Jenkins_hash_function on 2013-05-30
     */
    void    DoMore_(uint32_t* hash2Update, const Byte* from, const Byte* to)
    {
        RequireNotNull (hash2Update);
        uint32_t    hash    =   (*hash2Update);
        for (const Byte* bi = from; bi != to; ++bi) {
            hash += (*bi);
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        (*hash2Update) = hash;
    }
    void    DoEnd_(uint32_t* hash2Update)
    {
        RequireNotNull (hash2Update);
        uint32_t    hash    =   (*hash2Update);
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        (*hash2Update) = hash;
    }
}





Hasher<uint32_t, Algorithms::Jenkins>::ReturnType  Hasher<uint32_t, Algorithms::Jenkins>::Hash (const Streams::BinaryInputStream& from)
{
    uint32_t    hash    =   0;
    while (true) {
        Byte    buf[32 * 1024];
        size_t  n   =   from.Read (std::begin (buf), std::end (buf));
        Assert (n <= sizeof (buf));
        if (n == 0) {
            break;
        }
        DoMore_ (&hash, std::begin (buf), std::begin (buf) + n);
    }
    DoEnd_ (&hash);
    return hash;
}

Hasher<uint32_t, Algorithms::Jenkins>::ReturnType  Hasher<uint32_t, Algorithms::Jenkins>::Hash (const Byte* from, const Byte* to)
{
    uint32_t    hash    =   0;
    DoMore_ (&hash, from, to);
    DoEnd_ (&hash);
    return hash;
}
