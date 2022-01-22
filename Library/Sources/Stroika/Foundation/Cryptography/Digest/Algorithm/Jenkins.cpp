/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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

/*
 ********************************************************************************
 ***************** Algorithm::DigesterAlgorithm<Algorithm::Jenkins> *************
 ********************************************************************************
 */
void Algorithm::DigesterAlgorithm<Algorithm::Jenkins>::Write (const std::byte* start, const std::byte* end)
{
    Require (not fCompleted_);
    DoMore_ (&fData_, start, end);
}

auto Algorithm::DigesterAlgorithm<Algorithm::Jenkins>::Complete () -> ReturnType
{
#if qDebug
    Require (not fCompleted_);
    fCompleted_ = true;
#endif
    DoEnd_ (&fData_);
    return fData_;
}
