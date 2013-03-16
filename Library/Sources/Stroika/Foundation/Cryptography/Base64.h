/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Base64_h_
#define _Stroika_Foundation_Cryptography_Base64_h_  1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/BLOB.h"
#include    "../Streams/BinaryInputStream.h"
#include    "../Streams/BinaryOutputStream.h"



/**
 *  \file
 *
 * TODO:
 *
 *      @todo   EncodeBase64 should return STROIKA string, or BLOB - not std::string? Or maybe std::string
 *              best - but document why - cuz always ascii data so more compact?? No - not good reason cuz
 *              we can construct a SUBTYPE of String that takes advantage of it.
 *
 *      @todo   DecodeBase64() should have an overload taking BinaryInputStream, and EncodeBase64()
 *              should have an overload with BinaryOutputStream (keeping exsting overloads).
 *
 *      @todo   Tons todo optimizing this implementation (to not use temporary objects and
 *              avoiding copying).
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {

            Memory::BLOB    DecodeBase64 (const string& s);
            void            DecodeBase64 (const string& s, Streams::BinaryOutputStream out);

            enum class  LineBreak : uint8_t { eLF_LB, eCRLF_LB, eAuto_LB = eCRLF_LB };
            string          EncodeBase64 (const Streams::BinaryInputStream& from, LineBreak lb = LineBreak::eAuto_LB);

        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_Base64_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
