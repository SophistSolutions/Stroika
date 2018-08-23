/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "Jenkins.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Digest;

namespace {
    /*
     *  Implementation based on text from http://en.wikipedia.org/wiki/Jenkins_hash_function on 2013-05-30
     */
    inline void DoMore_ (uint32_t* hash2Update, const Byte* from, const Byte* to)
    {
        RequireNotNull (hash2Update);
        uint32_t hash = (*hash2Update);
        for (const Byte* bi = from; bi != to; ++bi) {
            hash += to_integer<uint8_t> (*bi);
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        (*hash2Update) = hash;
    }
    inline void DoEnd_ (uint32_t* hash2Update)
    {
        RequireNotNull (hash2Update);
        uint32_t hash = (*hash2Update);
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        (*hash2Update) = hash;
    }
}

Digester<Algorithm::Jenkins, uint32_t>::ReturnType Digester<Algorithm::Jenkins, uint32_t>::ComputeDigest (const Streams::InputStream<Byte>::Ptr& from)
{
    uint32_t hash = 0;
    while (true) {
        Byte   buf[32 * 1024];
        size_t n = from.Read (std::begin (buf), std::end (buf));
        Assert (n <= sizeof (buf));
        if (n == 0) {
            break;
        }
        DoMore_ (&hash, std::begin (buf), std::begin (buf) + n);
    }
    DoEnd_ (&hash);
    return hash;
}

Digester<Algorithm::Jenkins, uint32_t>::ReturnType Digester<Algorithm::Jenkins, uint32_t>::ComputeDigest (const Byte* from, const Byte* to)
{
    Require (from == to or from != nullptr);
    Require (from == to or to != nullptr);
    uint32_t hash = 0;
    DoMore_ (&hash, from, to);
    DoEnd_ (&hash);
    return hash;
}

Digester<Algorithm::Jenkins, uint32_t>::ReturnType Digester<Algorithm::Jenkins, uint32_t>::ComputeDigest (const Memory::BLOB& from)
{
    return ComputeDigest (from.begin (), from.end ());
}
