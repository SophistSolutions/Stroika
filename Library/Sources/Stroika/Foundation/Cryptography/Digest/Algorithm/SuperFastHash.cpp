/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 *
 * Copyright(c) 2004-2008 by Paul Hsieh
 *      PARTS OF THIS CODE MAYBE COVERED BY http://www.gnu.org/licenses/lgpl-2.1.txt - See
 *      http://www.azillionmonkeys.com/qed/hash.html
 */
#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Endian.h"
#include "../../../Memory/BLOB.h"

#include "SuperFastHash.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Digest;

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__TURBOC__)
#define get16bits_ref_(d) (*((const uint16_t*)(d)))
#endif
#if !defined(get16bits_ref_)
#define get16bits_ref_(d) ((((uint32_t) (((const uint8_t*)(d))[1])) << 8) + (uint32_t) (((const uint8_t*)(d))[0]))
#endif

namespace {
    inline uint16_t get16bits (const byte* p)
    {
        RequireNotNull (p);
        uint16_t result;
        switch (Configuration::GetEndianness ()) {
            case Configuration::Endian::eLittle:
                result = *reinterpret_cast<const uint16_t*> (p);
                break;
            case Configuration::Endian::eBig:
                result = (to_integer<uint16_t> (p[1]) << 8) + to_integer<uint16_t> (p[0]);
                break;
            default:
                AssertNotReached ();
                result = 0;
        }
        Ensure (get16bits_ref_ (p) == result);
        return result;
    }
}

namespace {
    /*
     *  Implementation based on text from http://www.azillionmonkeys.com/qed/hash.html on 2014-07-28
     *
     *  NOTE - I tried to implement in a way that could be windowed, just reading it bits at a time,
     *  but the trouble is that the initial value of the hash is the length, and since the BinaryInputStream
     *  isn't necessarily seekable, we cannot compute its length.
     */
}

Digester<Algorithm::SuperFastHash, uint32_t>::ReturnType Digester<Algorithm::SuperFastHash, uint32_t>::ComputeDigest (const Streams::InputStream<byte>::Ptr& from)
{
    Memory::BLOB b = from.ReadAll ();
    return Digester<Algorithm::SuperFastHash, uint32_t>::ComputeDigest (b.begin (), b.end ());
}

Digester<Algorithm::SuperFastHash, uint32_t>::ReturnType Digester<Algorithm::SuperFastHash, uint32_t>::ComputeDigest (const byte* from, const byte* to)
{
    Require (from == to or from != nullptr);
    Require (from == to or to != nullptr);

    if (from == to) {
        return 0;
    }

    size_t len = static_cast<size_t> (to - from);

    const byte* data = from;

    /*
     *  Require() here cuz of following cast.
     *  NB: apparently broken if large data input! > 4gig on 64bit machine.
     *  But this still produces a reasonable hashed result, and this misfeature
     *  of ignoring higher order bits appears implied by the reference algorithm
     *  on http://www.azillionmonkeys.com/qed/hash.html
     */
    Require (len < numeric_limits<uint32_t>::max ());
    uint32_t hash = static_cast<uint32_t> (len);

    uint32_t tmp;
    int      rem;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (; len > 0; len--) {
        hash += get16bits (data);
        tmp  = (get16bits (data + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        data += 2 * sizeof (uint16_t);
        hash += hash >> 11;
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
            hash += (signed char)*data;
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

Digester<Algorithm::SuperFastHash, uint32_t>::ReturnType Digester<Algorithm::SuperFastHash, uint32_t>::ComputeDigest (const Memory::BLOB& from)
{
    return ComputeDigest (from.begin (), from.end ());
}
