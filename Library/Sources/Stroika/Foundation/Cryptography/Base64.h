/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Base64_h_
#define _Stroika_Foundation_Cryptography_Base64_h_  1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../Configuration/Common.h"



/**
 * TODO:
 *
 *
 *      @todo   Redo API - to use Stream API (and document you can use BLOB().As<BinaryInputStream>()
 *
 *      @todo   Do API for EncodeBase64 () templated on iterator (or using Stroika iterator???) of bytes.
 *
 *      @todo   EncodeBase64 should return STROIKA string, or BLOB - not std::string? Or maybe std::string
 *              best - but document why - cuz always ascii data so more compact?? No - not good reason cuz
 *              we can construct a SUBTYPE of String that takes advantage of it.
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {

            vector<Byte>    DecodeBase64 (const string& s);

enum class  LineBreak : uint8_t { eLF_LB, eCRLF_LB, eAuto_LB = eCRLF_LB };
            string          EncodeBase64 (const Byte* start, const Byte* end, LineBreak lb = LineBreak::eAuto_LB);
            string          EncodeBase64 (const vector<Byte>& b, LineBreak lb = LineBreak::eAuto_LB);

        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_Base64_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
