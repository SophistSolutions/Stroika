/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "Jenkins.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Hash;







Hasher<uint32_t, Algorithms::Jenkins>::ReturnType  Hasher<uint32_t, Algorithms::Jenkins>::Hash (const Streams::BinaryInputStream& from)
{
    /*
     *  Implementation based on text from http://en.wikipedia.org/wiki/Jenkins_hash_function on 2013-05-30
     */
    uint32_t    hash    =   0;
    while (true) {
        Byte    buf[32 * 1024];
        size_t  n   =   from.Read (std::begin (buf), std::end (buf));
        if (n == 0) {
            break;
        }
        const Byte* bend = std::begin (buf) + n;
        for (const Byte* bi = std::begin (buf); bi != bend; ++bi) {
            hash += (*bi);
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

Hasher<uint32_t, Algorithms::Jenkins>::ReturnType  Hasher<uint32_t, Algorithms::Jenkins>::Hash (const Byte* from, const Byte* to)
{
    /*
     *  Implementation based on text from http://en.wikipedia.org/wiki/Jenkins_hash_function on 2013-05-30
     */
    uint32_t    hash    =   0;
    for (const Byte* bi = from; bi != to; ++bi) {
        hash += (*bi);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}
