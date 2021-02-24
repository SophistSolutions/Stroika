/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "Jenkins.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Digest;

namespace {
    /*
     *  Implementation based on text from http://en.wikipedia.org/wiki/Jenkins_hash_function on 2013-05-30
     */
    inline void DoMore_ (uint32_t* hash2Update, const byte* from, const byte* to)
    {
        RequireNotNull (hash2Update);
        uint32_t hash = (*hash2Update);
        for (const byte* bi = from; bi != to; ++bi) {
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

Digester<Algorithm::Jenkins, uint32_t>::ReturnType Digester<Algorithm::Jenkins, uint32_t>::operator() (const Streams::InputStream<std::byte>::Ptr& from) const
{
    uint32_t hash = 0;
#if qDebug
    Algorithm::DigesterAlgorithm<Algorithm::Jenkins> test;
#endif
    while (true) {
        byte   buf[32 * 1024];
        size_t n = from.Read (std::begin (buf), std::end (buf));
        Assert (n <= sizeof (buf));
        if (n == 0) {
            break;
        }
#if qDebug
        test.Write (std::begin (buf), std::begin (buf) + n);
#endif
        DoMore_ (&hash, std::begin (buf), std::begin (buf) + n);
    }
    DoEnd_ (&hash);
#if qDebug
    Assert (test.Complete () == hash);
#endif
    return hash;
}

Digester<Algorithm::Jenkins, uint32_t>::ReturnType Digester<Algorithm::Jenkins, uint32_t>::operator() (const std::byte* from, const std::byte* to) const
{
    Require (from == to or from != nullptr);
    Require (from == to or to != nullptr);
    uint32_t hash = 0;
    DoMore_ (&hash, from, to);
    DoEnd_ (&hash);
#if qDebug
    {
        Algorithm::DigesterAlgorithm<Algorithm::Jenkins> test;
        test.Write (from, to);
        Assert (test.Complete () == hash);
    }
#endif
    return hash;
}

////////////NEW

void Algorithm::DigesterAlgorithm<Algorithm::Jenkins>::Write (const std::byte* start, const std::byte* end)
{
    DoMore_ (&fData_, start, end);
}

auto Algorithm::DigesterAlgorithm<Algorithm::Jenkins>::Complete () -> ReturnType
{
    DoEnd_ (&fData_);
    return fData_;
}
