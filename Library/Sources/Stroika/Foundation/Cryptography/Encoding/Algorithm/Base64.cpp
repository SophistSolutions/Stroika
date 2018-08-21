/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
/*
 * Note - used PUBLIC DOMAIN http://sourceforge.net/projects/libb64/files/libb64/libb64/libb64-1.2.src.zip/download
 * code as a starting point.
 */
#include "../../../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../../Containers/Common.h"
#include "../../../DataExchange/BadFormatException.h"
#include "../../../Debug/Assertions.h"
#include "../../../Execution/Exceptions.h"
#include "../../../Memory/BLOB.h" // ONLY FOR QUICKHACK IMPL OF ENCODE...
#include "../../../Memory/SmallStackBuffer.h"

#include "Base64.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Encoding;
using namespace Stroika::Foundation::Cryptography::Encoding::Algorithm;
using namespace Stroika::Foundation::Memory;

/**
 *  IMPLEMENTATION NOTES:
 *
 *      The public domain (private) code in this file - is designed to operate in a STREAM mode. I've preserved that
 *  internally, evne though my current API doesn't work that way, because we will soon support a STREAM based API here,
 *  and that will fit perfectly.
 *
 *  @see    http://libb64.sourceforge.net/ - which explains the design of the routines, and the queer nested loop / switch
 *          construct.
 *
 *      -- LGP 2011-06-21
 */

/*
 ********************************************************************************
 *************************** Encoding::DecodeBase64 *****************************
 ********************************************************************************
 */
namespace {
    enum base64_decodestep_ {
        step_a,
        step_b,
        step_c,
        step_d
    };
    struct base64_decodestate_ {
        base64_decodestep_ step;
        Byte               plainchar;

        base64_decodestate_ ()
            : step (step_a)
            , plainchar ('\0')
        {
        }
    };
    int base64_decode_value_ (signed char value_in)
    {
        static constexpr signed char kDecoding[] = {62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
        value_in -= 43;
        if (value_in < 0 || static_cast<unsigned char> (value_in) >= NEltsOf (kDecoding)) {
            return -1;
        }
        Assert (0 <= value_in and static_cast<unsigned char> (value_in) < NEltsOf (kDecoding));
        return kDecoding[(int)value_in];
    }
    size_t base64_decode_block_ (const signed char* code_in, size_t length_in, Byte* plaintext_out, base64_decodestate_* state)
    {
        RequireNotNull (code_in);
        RequireNotNull (plaintext_out);

        const signed char* codechar  = code_in;
        Byte*              plainchar = plaintext_out;
        signed char        fragment  = '\0';

        *plainchar = state->plainchar;

        switch (state->step) {
            while (1) {
                case step_a:
                    do {
                        if (codechar == code_in + length_in) {
                            state->step      = step_a;
                            state->plainchar = *plainchar;
                            return plainchar - plaintext_out;
                        }
                        fragment = (signed char)base64_decode_value_ (*codechar++);
                    } while (fragment < 0);
                    *plainchar = (fragment & 0x03f) << 2;
                case step_b:
                    do {
                        if (codechar == code_in + length_in) {
                            state->step      = step_b;
                            state->plainchar = *plainchar;
                            return plainchar - plaintext_out;
                        }
                        fragment = (signed char)base64_decode_value_ (*codechar++);
                    } while (fragment < 0);
                    *plainchar++ |= (fragment & 0x030) >> 4;
                    *plainchar = (fragment & 0x00f) << 4;
                case step_c:
                    do {
                        if (codechar == code_in + length_in) {
                            state->step      = step_c;
                            state->plainchar = *plainchar;
                            return plainchar - plaintext_out;
                        }
                        fragment = (signed char)base64_decode_value_ (*codechar++);
                    } while (fragment < 0);
                    *plainchar++ |= (fragment & 0x03c) >> 2;
                    *plainchar = (fragment & 0x003) << 6;
                case step_d:
                    do {
                        if (codechar == code_in + length_in) {
                            state->step      = step_d;
                            state->plainchar = *plainchar;
                            return plainchar - plaintext_out;
                        }
                        fragment = (signed char)base64_decode_value_ (*codechar++);
                    } while (fragment < 0);
                    *plainchar++ |= (fragment & 0x03f);
            }
        }
        return plainchar - plaintext_out;
    }
}

Memory::BLOB Algorithm::DecodeBase64 (const Characters::String& s)
{
    //@todo - improve/fix this
    return DecodeBase64 (s.AsUTF8 ());
}

Memory::BLOB Algorithm::DecodeBase64 (const string& s)
{
    if (s.empty ()) {
        return Memory::BLOB ();
    }
    size_t                 dataSize1 = s.length ();
    SmallStackBuffer<Byte> buf1 (SmallStackBufferCommon::eUninitialized, dataSize1); // MUCH more than big enuf
    base64_decodestate_    state;
    size_t                 r = base64_decode_block_ (reinterpret_cast<const signed char*> (Containers::Start (s)), s.length (), buf1.begin (), &state);
    Assert (r <= dataSize1);
    return Memory::BLOB (buf1.begin (), buf1.begin () + r);
}

void Algorithm::DecodeBase64 (const string& s, const Streams::OutputStream<Byte>::Ptr& out)
{
    // QUICKIE implementation...
    Memory::BLOB tmp = DecodeBase64 (s);
    out.Write (tmp.begin (), tmp.end ());
}

/*
 ********************************************************************************
 ************************** Algorithm::EncodeBase64 *****************************
 ********************************************************************************
 */
namespace {
    enum base64_encodestep {
        step_A,
        step_B,
        step_C
    };
    struct base64_encodestate {
        base64_encodestep step;
        signed char       result;
        int               stepcount;
        LineBreak         fLineBreak;

        base64_encodestate (LineBreak lb)
            : step (step_A)
            , result (0)
            , stepcount (0)
            , fLineBreak (lb)
        {
        }
    };

    signed char base64_encode_value_ (signed char value_in)
    {
        const signed char BASE64_CHARS_[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        Assert (NEltsOf (BASE64_CHARS_) == (2 * 26 + 10 + 2 + 1));
        if (value_in > 63) {
            return '=';
        }
        return BASE64_CHARS_[(int)value_in];
    }

    size_t base64_encode_block_ (const Byte* plaintext_in, size_t length_in, signed char* code_out, base64_encodestate* state)
    {
        const int CHARS_PER_LINE = 76;

        const Byte*       plainchar    = plaintext_in;
        const Byte* const plaintextend = plaintext_in + length_in;
        signed char*      codechar     = code_out;
        signed char       result       = state->result;

        signed char fragment = '\0';
        switch (state->step) {
            while (1) {
                case step_A:
                    if (plainchar == plaintextend) {
                        state->result = result;
                        state->step   = step_A;
                        return codechar - code_out;
                    }
                    fragment    = *plainchar++;
                    result      = (fragment & 0x0fc) >> 2;
                    *codechar++ = base64_encode_value_ (result);
                    result      = (fragment & 0x003) << 4;
                case step_B:
                    if (plainchar == plaintextend) {
                        state->result = result;
                        state->step   = step_B;
                        return codechar - code_out;
                    }
                    fragment = *plainchar++;
                    result |= (fragment & 0x0f0) >> 4;
                    *codechar++ = base64_encode_value_ (result);
                    result      = (fragment & 0x00f) << 2;
                case step_C:
                    if (plainchar == plaintextend) {
                        state->result = result;
                        state->step   = step_C;
                        return codechar - code_out;
                    }
                    fragment = *plainchar++;
                    result |= (fragment & 0x0c0) >> 6;
                    *codechar++ = base64_encode_value_ (result);
                    result      = (fragment & 0x03f) >> 0;
                    *codechar++ = base64_encode_value_ (result);

                    ++(state->stepcount);
                    if (state->stepcount == CHARS_PER_LINE / 4) {
                        switch (state->fLineBreak) {
                            case LineBreak::eLF_LB:
                                *codechar++ = '\n';
                                break;
                            case LineBreak::eCRLF_LB:
                                *codechar++ = '\r';
                                *codechar++ = '\n';
                                break;
                        }
                        state->stepcount = 0;
                    }
            }
        }
        return codechar - code_out;
    }
    inline size_t base64_encode_blockend_ (signed char* code_out, base64_encodestate* state)
    {
        signed char* codechar = code_out;
        switch (state->step) {
            case step_B:
                *codechar++ = base64_encode_value_ (state->result);
                *codechar++ = '=';
                *codechar++ = '=';
                break;
            case step_C:
                *codechar++ = base64_encode_value_ (state->result);
                *codechar++ = '=';
                break;
            case step_A:
                break;
        }
        return static_cast<size_t> (codechar - code_out);
    }
}

string Algorithm::EncodeBase64 (const Streams::InputStream<Byte>::Ptr& from, LineBreak lb)
{
#if 0
    // Use look doing multiple base64_encode_block_() calls!
#elif 1
    // quick hack impl
    Memory::BLOB bytes = Streams::InputStream<Byte>::Ptr (from).ReadAll ();
    const Byte*  start = bytes.begin ();
    const Byte*  end   = bytes.end ();
    Require (start == end or start != nullptr);
    Require (start == end or end != nullptr);
    base64_encodestate state (lb);
    size_t             srcLen  = end - start;
    size_t             bufSize = 4 * srcLen;
    Assert (bufSize >= srcLen); // no overflow!
    SmallStackBuffer<signed char> data (SmallStackBufferCommon::eUninitialized, bufSize);
    size_t                        mostBytesCopied = base64_encode_block_ (start, srcLen, data.begin (), &state);
    size_t                        extraBytes      = base64_encode_blockend_ (data.begin () + mostBytesCopied, &state);
    size_t                        totalBytes      = mostBytesCopied + extraBytes;
    Assert (totalBytes <= bufSize);
    return string (data.begin (), data.begin () + totalBytes);
#else
    Require (start == end or start != nullptr);
    Require (start == end or end != nullptr);

    base64_encodestate state (lb);
    size_t             srcLen  = end - start;
    size_t             bufSize = 4 * srcLen;
    Assert (bufSize >= srcLen); // no overflow!
    SmallStackBuffer<char> data (SmallStackBufferCommon::eUninitialized, bufSize);
    size_t                 mostBytesCopied = base64_encode_block_ (start, srcLen, data.begin (), &state);
    size_t                 extraBytes      = base64_encode_blockend_ (data.begin () + mostBytesCopied, &state);
    size_t                 totalBytes      = mostBytesCopied + extraBytes;
    Assert (totalBytes <= bufSize);
    return string (data.begin (), data.begin () + totalBytes);
#endif
}

string Algorithm::EncodeBase64 (const Memory::BLOB& from, LineBreak lb)
{
    const Byte* start = from.begin ();
    const Byte* end   = from.end ();
    Require (start == end or start != nullptr);
    Require (start == end or end != nullptr);
    base64_encodestate state (lb);
    size_t             srcLen  = end - start;
    size_t             bufSize = 4 * srcLen;
    Assert (bufSize >= srcLen); // no overflow!
    SmallStackBuffer<signed char> data (SmallStackBufferCommon::eUninitialized, bufSize);
    size_t                        mostBytesCopied = base64_encode_block_ (start, srcLen, data.begin (), &state);
    size_t                        extraBytes      = base64_encode_blockend_ (data.begin () + mostBytesCopied, &state);
    size_t                        totalBytes      = mostBytesCopied + extraBytes;
    Assert (totalBytes <= bufSize);
    return string (data.begin (), data.begin () + totalBytes);
}
