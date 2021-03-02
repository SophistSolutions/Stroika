/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 *
 * Copyright(c) 2004-2008 by Paul Hsieh
 *      PARTS OF THIS CODE MAYBE COVERED BY http://www.gnu.org/licenses/lgpl-2.1.txt - See
 *      http://www.azillionmonkeys.com/qed/hash.html
 */
#include "../../../StroikaPreComp.h"

#include "../../../Configuration/Endian.h"

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
    inline uint16_t get16bits_ (const byte* p)
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

/*
 ********************************************************************************
 ************ Algorithm::DigesterAlgorithm<Algorithm::SuperFastHash> ************
 ********************************************************************************
 */
void Algorithm::DigesterAlgorithm<Algorithm::SuperFastHash>::Write (const std::byte* start, const std::byte* end)
{
    Require (not fCompleted_);
    /*
     *  Require() here cuz of following cast.
     *  NB: apparently broken if large data input! > 4gig on 64bit machine.
     *  But this still produces a reasonable hashed result, and this misfeature
     *  of ignoring higher order bits appears implied by the reference algorithm
     *  on http://www.azillionmonkeys.com/qed/hash.html
     */
    size_t len = static_cast<size_t> (end - start);
    Require (len < numeric_limits<uint32_t>::max ());

    fRemainder_ += static_cast<uint32_t> (len);
    fRemainder_ &= 0x3; // old code just did fRemainder_ = len & 3, but we now get len in bits and peices
    Assert (0 <= fRemainder_ && fRemainder_ <= 3);

    const byte* data = start;

    len >>= 2;

    auto hash = fHash_;
    /* Main loop */
    for (; len > 0; len--) {
        hash += get16bits_ (data);
        uint32_t tmp = (get16bits_ (data + 2) << 11) ^ hash;
        hash         = (hash << 16) ^ tmp;
        data += 2 * sizeof (uint16_t);
        hash += hash >> 11;
    }
    fHash_ = hash;
    Assert (0 <= fRemainder_ and fRemainder_ <= 3);
    memcpy (&fFinalBytes_, data, fRemainder_);
}

auto Algorithm::DigesterAlgorithm<Algorithm::SuperFastHash>::Complete () -> ReturnType
{
#if qDebug
    Require (not fCompleted_);
    fCompleted_ = true;
#endif
    const byte* data = &fFinalBytes_[0];
    auto        hash = fHash_;
    /* Handle end cases */
    switch (fRemainder_) {
        case 3:
            hash += get16bits_ (data);
            hash ^= hash << 16;
            hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
            hash += hash >> 11;
            break;
        case 2:
            hash += get16bits_ (data);
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
