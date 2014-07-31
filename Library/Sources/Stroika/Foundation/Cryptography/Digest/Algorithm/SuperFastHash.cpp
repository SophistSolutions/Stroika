/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 *
 * Copyright(c) 2004-2008 by Paul Hsieh
 *      PARTS OF THIS CODE MAYBE COVERED BY http://www.gnu.org/licenses/lgpl-2.1.txt - See
 *      http://www.azillionmonkeys.com/qed/hash.html
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/BLOB.h"

#include    "SuperFastHash.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Hash;



// @todo - reimplement using inline function and test for endian!!!
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                      +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif




namespace {
    /*
     *  Implementation based on text from http://www.azillionmonkeys.com/qed/hash.html on 2014-07-28
     *
     *  NOTE - I tried to implement in a way that could be windowed, just reading it bits at a time,
     *  but the trouble is that the initial value of the hash is the length, and since the BinaryInputStream
     *  isn't necessarily seekable, we cannot compute its length.
     */
}





Hasher<uint32_t, Algorithm::SuperFastHash>::ReturnType  Hasher<uint32_t, Algorithm::SuperFastHash>::Hash (const Streams::BinaryInputStream& from)
{
    Memory::BLOB    b = from.ReadAll ();
    return Hasher<uint32_t, Algorithm::SuperFastHash>::Hash (b.begin (), b.end ());
}

Hasher<uint32_t, Algorithm::SuperFastHash>::ReturnType  Hasher<uint32_t, Algorithm::SuperFastHash>::Hash (const Byte* from, const Byte* to)
{
    Require (from == to or from != nullptr);
    Require (from == to or to != nullptr);

    if (from == to) {
        return 0;
    }

    uint32_t len = to - from;

    const Byte* data = from;

    uint32_t hash = len;
    uint32_t tmp;
    int rem;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (; len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data + 2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2 * sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3:
            hash += get16bits (data);
            hash ^= hash << 16;
            hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
            hash += hash >> 11;
            break;
        case 2:
            hash += get16bits (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1:
            hash += (signed char) * data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}
