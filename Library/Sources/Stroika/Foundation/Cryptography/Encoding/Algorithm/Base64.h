/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Base64_h_
#define _Stroika_Foundation_Cryptography_Base64_h_ 1

#include "../../../StroikaPreComp.h"

#include <string>
#include <vector>

#include "../../../Characters/String.h"
#include "../../../Configuration/Common.h"
#include "../../../Memory/BLOB.h"
#include "../../../Streams/InputStream.h"
#include "../../../Streams/OutputStream.h"

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

namespace Stroika::Foundation::Cryptography::Encoding::Algorithm::Base64 {

    Memory::BLOB Decode (span<const char> s);
    Memory::BLOB Decode (const string& s);
    Memory::BLOB Decode (const u8string& s);
    Memory::BLOB Decode (const Characters::String& s);
    void         Decode (const string& s, const Streams::OutputStream::Ptr<byte>& out);

    enum class LineBreak : uint8_t {
        eLF_LB,
        eCRLF_LB,
        eAuto_LB = eCRLF_LB
    };
    struct Options {
        LineBreak fLineBreak{LineBreak::eAuto_LB};
    };

    /**
     *  \note BLOB overload redundant, but provided since slight performance tweak (@todo maybe replace with span<byte> for that case/reason)
     */
    string Encode (const Streams::InputStream::Ptr<byte>& from, const Options& o = {});
    string Encode (const Memory::BLOB& from, const Options& o = {});

}

namespace Stroika::Foundation::Cryptography::Encoding::Algorithm {

    [[deprecated ("Since Stroika v3.0d5 use Base64::Decode")]] inline Memory::BLOB DecodeBase64 (span<const char> s)
    {
        return Base64::Decode (s);
    }
    [[deprecated ("Since Stroika v3.0d5 use Base64::Decode")]] inline Memory::BLOB DecodeBase64 (const string& s)
    {
        return Base64::Decode (s);
    }
    [[deprecated ("Since Stroika v3.0d5 use Base64::Decode")]] inline Memory::BLOB DecodeBase64 (const u8string& s)
    {
        return Base64::Decode (s);
    }
    [[deprecated ("Since Stroika v3.0d5 use Base64::Decode")]] inline Memory::BLOB DecodeBase64 (const Characters::String& s)
    {
        return Base64::Decode (s);
    }
    [[deprecated ("Since Stroika v3.0d5 use Base64::Decode")]] inline void DecodeBase64 (const string& s, const Streams::OutputStream::Ptr<byte>& out)
    {
        Base64::Decode (s, out);
    }

    using Base64::LineBreak; // deprecated
    [[deprecated ("Since Stroika v3.0d5 use Base64::Encode")]] inline string EncodeBase64 (const Streams::InputStream::Ptr<byte>& from,
                                                                                           LineBreak lb = LineBreak::eAuto_LB)
    {
        return Base64::Encode (from, Base64::Options{.fLineBreak = lb});
    }
    [[deprecated ("Since Stroika v3.0d5 use Base64::Encode")]] inline string EncodeBase64 (const Memory::BLOB& from, LineBreak lb = LineBreak::eAuto_LB)
    {
        return Base64::Encode (from, Base64::Options{.fLineBreak = lb});
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_Base64_h_*/
