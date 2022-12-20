/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <mutex>
#include <set>

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Characters/String.h"
#include "../Characters/StringBuilder.h"
#include "../Configuration/Common.h"
#include "../Containers/Common.h"
#include "../Execution/Common.h"
#include "../Execution/Exceptions.h"
#include "../Memory/StackBuffer.h"

#include "UTFConvert.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

/**
     *  BASED on
     *      https://github.com/codebrainz/libutfxx/blob/master/utf/ConvertUTF.h
     *      https://github.com/codebrainz/libutfxx/blob/master/utf/ConvertUTF.c
     *      http://docs.ros.org/lunar/api/rtabmap/html/ConvertUTF_8h_source.html,
     *
     *      but updated for C++.
     *
     *  Copyright 2001-2004 Unicode, Inc.
     * 
     *  Disclaimer
     * 
     *  This source code is provided as is by Unicode, Inc. No claims are
     *  made as to fitness for any particular purpose. No warranties of any
     *  kind are expressed or implied. The recipient agrees to determine
     *  applicability of information provided. If this file has been
     *  purchased on magnetic or optical media from Unicode, Inc., the
     *  sole remedy for any claim will be exchange of defective media
     *  within 90 days of receipt.
     * 
     *  Limitations on Rights to Redistribute This Code
     * 
     *  Unicode, Inc. hereby grants the right to freely use the information
     *  supplied in this file in the creation of products supporting the
     *  Unicode Standard, and to make copies of this file in any form
     *  for internal or external distribution as long as this notice
     *  remains attached.
     * 
     *
     *      Author: Mark E. Davis, 1994.
     *      Rev History: Rick McGowan, fixes & updates May 2001.
     *      Fixes & updates, Sept 2001.
     */
namespace {
    namespace UTFConvert_libutfxx_ {
        static constexpr char32_t UNI_REPLACEMENT_CHAR = (char32_t)0x0000FFFD;
        static constexpr char32_t UNI_MAX_BMP          = (char32_t)0x0000FFFF;
        static constexpr char32_t UNI_MAX_UTF16        = (char32_t)0x0010FFFF;
        static constexpr char32_t UNI_MAX_UTF32        = (char32_t)0x7FFFFFFF;
        static constexpr char32_t UNI_MAX_LEGAL_UTF32  = (char32_t)0x0010FFFF;

        enum ConversionResult {
            conversionOK,    /* conversion successful */
            sourceExhausted, /* partial character in source, but hit end */
            targetExhausted, /* insuff. room in target for conversion */
            sourceIllegal    /* source sequence is illegal/malformed */
        };
        enum ConversionFlags {
            strictConversion = 0,
            lenientConversion
        };

        UTFConverter::ConversionResults cvt_ (ConversionResult cr)
        {
            switch (cr) {
                case conversionOK:
                    return UTFConverter::ConversionResults::ok;
                case sourceExhausted:
                    return UTFConverter::ConversionResults::sourceExhausted;
                case targetExhausted:
                    RequireNotReached (); // API doesn't allow this
                    return UTFConverter::ConversionResults::sourceIllegal;
                case sourceIllegal:
                    return UTFConverter::ConversionResults::sourceIllegal;
                default:
                    RequireNotReached (); // API doesn't allow this
                    return UTFConverter::ConversionResults::sourceIllegal;
            }
        }

        // Most of the comparisons we need to do in this code are with unsigned characters, but we want the public
        // API to use UTF8=char, since that will work best with calling code (e.g. class basic_string<char>).
        using UTF8_ = unsigned char;

        constexpr int halfShift = 10; /* used for shifting by 10 bits */

        constexpr char32_t halfBase = 0x0010000UL;
        constexpr char32_t halfMask = 0x3FFUL;

        constexpr char32_t UNI_SUR_HIGH_START{(char32_t)0xD800};
        constexpr char32_t UNI_SUR_HIGH_END{(char32_t)0xDBFF};
        constexpr char32_t UNI_SUR_LOW_START{(char32_t)0xDC00};
        constexpr char32_t UNI_SUR_LOW_END{(char32_t)0xDFFF};

        /*
         * Magic values subtracted from a buffer value during UTF8 conversion.
         * This table contains as many values as there might be trailing bytes
         * in a UTF-8 sequence.
         */
        constexpr char32_t offsetsFromUTF8[6] = {0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL};

        /*
         * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
         * into the first byte, depending on how many bytes follow.  There are
         * as many entries in this table as there are UTF-8 sequence types.
         * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
         * for *legal* UTF-8 will be 4 or fewer bytes total.
         */
        constexpr UTF8_ firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

        /*
         * Index into the table below with the first byte of a UTF-8 sequence to
         * get the number of trailing bytes that are supposed to follow it.
         * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
         * left as-is for anyone who may want to do such conversion, which was
         * allowed in earlier algorithms.
         */
        constexpr char trailingBytesForUTF8[256] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

        /*
         * Utility routine to tell whether a sequence of bytes is legal UTF-8.
         * This must be called with the length pre-determined by the first byte.
         * If not calling this from ConvertUTF8to*, then the length can be set by:
         *  length = trailingBytesForUTF8[*source]+1;
         * and the sequence is illegal right away if there aren't that many bytes
         * available.
         * If presented with a length > 4, this returns false.  The Unicode
         * definition of UTF-8 goes up to 4-byte sequences.
         */
        bool isLegalUTF8_ (const UTF8_* source, int length)
        {
            UTF8_        a;
            const UTF8_* srcptr = source + length;
            switch (length) {
                default:
                    return false;
                /* Everything else falls through when "true"... */
                case 4:
                    if ((a = (*--srcptr)) < 0x80 || a > 0xBF)
                        return false;
                case 3:
                    if ((a = (*--srcptr)) < 0x80 || a > 0xBF)
                        return false;
                case 2:
                    if ((a = (*--srcptr)) > 0xBF)
                        return false;

                    switch ((unsigned char)*source) {
                        /* no fall-through in this inner switch */
                        case 0xE0:
                            if (a < 0xA0)
                                return false;
                            break;
                        case 0xED:
                            if (a > 0x9F)
                                return false;
                            break;
                        case 0xF0:
                            if (a < 0x90)
                                return false;
                            break;
                        case 0xF4:
                            if (a > 0x8F)
                                return false;
                            break;
                        default:
                            if (a < 0x80)
                                return false;
                    }

                case 1:
                    if (*source >= 0x80 && *source < 0xC2)
                        return false;
            }
            if (*source > 0xF4)
                return false;
            return true;
        }

        inline ConversionResult ConvertUTF16toUTF8_ (const char16_t** sourceStart, const char16_t* sourceEnd, char** targetStart, char* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const char16_t*  source = *sourceStart;
            UTF8_*           target = reinterpret_cast<UTF8_*> (*targetStart);
            while (source < sourceEnd) {
                char32_t           ch;
                unsigned short     bytesToWrite = 0;
                constexpr char32_t byteMask     = 0xBF;
                constexpr char32_t byteMark     = 0x80;
                const char16_t*    oldSource    = source; /* In case we have to back up because of target overflow. */
                ch                              = *source++;
                /* If we have a surrogate pair, convert to char32_t first. */
                if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) [[unlikely]] {
                    /* If the 16 bits following the high surrogate are in the source buffer... */
                    if (source < sourceEnd) [[likely]] {
                        char32_t ch2 = *source;
                        /* If it's a low surrogate, convert to char32_t. */
                        if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                            ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
                            ++source;
                        }
                        else if (flags == strictConversion) { /* it's an unpaired high surrogate */
                            --source;                         /* return to the illegal value itself */
                            result = sourceIllegal;
                            break;
                        }
                    }
                    else {        /* We don't have the 16 bits following the high surrogate. */
                        --source; /* return to the high surrogate */
                        result = sourceExhausted;
                        break;
                    }
                }
                else if (flags == strictConversion) {
                    /* UTF-16 surrogate values are illegal in UTF-32 */
                    if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                        --source; /* return to the illegal value itself */
                        result = sourceIllegal;
                        break;
                    }
                }
                /* Figure out how many bytes the result will require */
                if (ch < (char32_t)0x80) {
                    bytesToWrite = 1;
                }
                else if (ch < (char32_t)0x800) {
                    bytesToWrite = 2;
                }
                else if (ch < (char32_t)0x10000) {
                    bytesToWrite = 3;
                }
                else if (ch < (char32_t)0x110000) {
                    bytesToWrite = 4;
                }
                else {
                    bytesToWrite = 3;
                    ch           = UNI_REPLACEMENT_CHAR;
                }

                target += bytesToWrite;
                if (target > reinterpret_cast<UTF8_*> (targetEnd)) {
                    source = oldSource; /* Back up source pointer! */
                    target -= bytesToWrite;
                    result = targetExhausted;
                    break;
                }
                switch (bytesToWrite) { /* note: everything falls through. */
                    case 4:
                        *--target = (UTF8_)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 3:
                        *--target = (UTF8_)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 2:
                        *--target = (UTF8_)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 1:
                        *--target = (UTF8_)(ch | firstByteMark[bytesToWrite]);
                }
                target += bytesToWrite;
            }
            *sourceStart = source;
            *targetStart = reinterpret_cast<char*> (target);
            return result;
        }
        inline ConversionResult ConvertUTF8toUTF16_ (const char** sourceStart, const char* sourceEnd, char16_t** targetStart, char16_t* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const UTF8_*     source = reinterpret_cast<const UTF8_*> (*sourceStart);
            char16_t*        target = *targetStart;
            while (source < reinterpret_cast<const UTF8_*> (sourceEnd)) {
                char32_t       ch               = 0;
                unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
                if (source + extraBytesToRead >= reinterpret_cast<const UTF8_*> (sourceEnd)) {
                    result = sourceExhausted;
                    break;
                }
                /* Do this check whether lenient or strict */
                if (!isLegalUTF8_ (source, extraBytesToRead + 1)) {
                    result = sourceIllegal;
                    break;
                }
                /*
             * The cases all fall through. See "Note A" below.
             */
                switch (extraBytesToRead) {
                    case 5:
                        ch += *source++;
                        ch <<= 6; /* remember, illegal UTF-8 */
                    case 4:
                        ch += *source++;
                        ch <<= 6; /* remember, illegal UTF-8 */
                    case 3:
                        ch += *source++;
                        ch <<= 6;
                    case 2:
                        ch += *source++;
                        ch <<= 6;
                    case 1:
                        ch += *source++;
                        ch <<= 6;
                    case 0:
                        ch += *source++;
                }
                ch -= offsetsFromUTF8[extraBytesToRead];

                if (target >= targetEnd) [[unlikely]] {
                    source -= (extraBytesToRead + 1); /* Back up source pointer! */
                    result = targetExhausted;
                    break;
                }
                if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
                    /* UTF-16 surrogate values are illegal in UTF-32 */
                    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                        if (flags == strictConversion) {
                            source -= (extraBytesToRead + 1); /* return to the illegal value itself */
                            result = sourceIllegal;
                            break;
                        }
                        else {
                            *target++ = UNI_REPLACEMENT_CHAR;
                        }
                    }
                    else {
                        *target++ = (char16_t)ch; /* normal case */
                    }
                }
                else if (ch > UNI_MAX_UTF16) {
                    if (flags == strictConversion) {
                        result = sourceIllegal;
                        source -= (extraBytesToRead + 1); /* return to the start */
                        break;                            /* Bail out; shouldn't continue */
                    }
                    else {
                        *target++ = UNI_REPLACEMENT_CHAR;
                    }
                }
                else {
                    /* target is a character in range 0xFFFF - 0x10FFFF. */
                    if (target + 1 >= targetEnd) {
                        source -= (extraBytesToRead + 1); /* Back up source pointer! */
                        result = targetExhausted;
                        break;
                    }
                    ch -= halfBase;
                    *target++ = (char16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
                    *target++ = (char16_t)((ch & halfMask) + UNI_SUR_LOW_START);
                }
            }
            *sourceStart = reinterpret_cast<const char*> (source);
            *targetStart = target;
            return result;
        }

    }
}

namespace {
    namespace UTFConvert_codecvSupport_ {
       inline UTFConverter::ConversionResults cvt_stdcodecvt_results_ (int i)
        {
            using namespace UTFConvert;
            switch (i) {
                case std::codecvt_utf8_utf16<char16_t>::ok:
                    return UTFConverter::ConversionResults::ok;
                case std::codecvt_utf8_utf16<char16_t>::error:
                    return UTFConverter::ConversionResults::sourceIllegal;
                case std::codecvt_utf8_utf16<char16_t>::partial:
                    return UTFConverter::ConversionResults::sourceExhausted; // not quite - couldbe target exhuasted?
                case std::codecvt_utf8_utf16<char16_t>::noconv:
                    return UTFConverter::ConversionResults::sourceIllegal; // not quite
                default:
                    Assert (false);
                    return UTFConverter::ConversionResults::sourceIllegal;
            }
        }
        inline UTFConverter::ConversionResults ConvertUTF8toUTF16_codecvt_ (mbstate_t* multibyteConversionState, const char8_t** sourceStart, const char8_t* sourceEnd, char16_t** targetStart, char16_t* targetEnd)
        {
            DISABLE_COMPILER_MSC_WARNING_START (4996); // warning STL4020: std::codecvt<char16_t, char, mbstate_t> DEPRECATED
            // SIGH - DEPRECATED but ALSO more than twice as slow as my (lifted) implementation (not sure why - looks similar).
            //      --LGP 2022-12-17
            //  https://en.cppreference.com/w/cpp/locale/codecvt_utf8_utf16
            static const std::codecvt_utf8_utf16<char16_t>             cvt;
            mbstate_t                                                  state{};
            if (multibyteConversionState != nullptr) {
                state = *multibyteConversionState;
            }
            const char*                                                sourceCursor = reinterpret_cast<const char*> (*sourceStart);
            char16_t*                                                  outCursor = *targetStart;
            [[maybe_unused]] std::codecvt_utf8_utf16<char16_t>::result rr           = cvt.in (state,reinterpret_cast<const char*> ( *sourceStart), reinterpret_cast<const char*> (sourceEnd), sourceCursor, *targetStart, targetEnd, outCursor);
            *sourceStart                                                            = reinterpret_cast<const char8_t*> (sourceCursor);
            *targetStart                                                         = outCursor;
            if (multibyteConversionState != nullptr) {
                *multibyteConversionState = state;
            }
            return cvt_stdcodecvt_results_ (rr);
            DISABLE_COMPILER_MSC_WARNING_END (4996);
        }
        inline UTFConverter::ConversionResults ConvertUTF16toUTF8_codecvt_ (const char16_t** sourceStart, const char16_t* sourceEnd, char8_t** targetStart, char8_t* targetEnd)
        {
            DISABLE_COMPILER_MSC_WARNING_START (4996); // warning STL4020: std::codecvt<char16_t, char, mbstate_t> DEPRECATED
            // SIGH - DEPRECATED but ALSO more than twice as slow as my (lifted) implementation (not sure why - looks similar).
            //      --LGP 2022-12-17
            //  https://en.cppreference.com/w/cpp/locale/codecvt_utf8_utf16
            static const std::codecvt_utf8_utf16<char16_t>             cvt;
            mbstate_t                                                  state{};
            const char16_t*                                            sourceCursor = *sourceStart;
            char*                                                      outCursor    = reinterpret_cast<char*> (*targetStart);
            [[maybe_unused]] std::codecvt_utf8_utf16<char16_t>::result rr           = cvt.out (state, *sourceStart, sourceEnd, sourceCursor, reinterpret_cast<char*> (*targetStart), reinterpret_cast<char*> (targetEnd), outCursor);
            *sourceStart                                                            = reinterpret_cast<const char16_t*> (sourceCursor);
            *targetStart                                                            = reinterpret_cast<char8_t*> (outCursor);
            return cvt_stdcodecvt_results_ (rr);
            DISABLE_COMPILER_MSC_WARNING_END (4996);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Characters::UTFConverter *************************
 ********************************************************************************
 */
auto UTFConverter::ConvertQuietly_StroikaPortable_ (const span<const char8_t> source, const span<char16_t> target) -> tuple<ConversionResults, size_t, size_t>
{
    if (source.empty ()) {
        return make_tuple (ConversionResults::ok, 0, 0); // avoid dereferncing empty iterators
    }
    using namespace UTFConvert_libutfxx_;
    const char*      sourceStart = reinterpret_cast<const char*> (&*source.begin ());
    const char*      sourceEnd   = sourceStart + source.size ();
    char16_t*        targetStart = reinterpret_cast<char16_t*> (&*target.begin ());
    char16_t*        targetEnd   = targetStart + target.size ();
    ConversionResult r           = ConvertUTF8toUTF16_ (&sourceStart, sourceEnd, &targetStart, targetEnd, ConversionFlags::lenientConversion); // look at options
    return make_tuple (cvt_ (r), sourceStart - reinterpret_cast<const char*> (&*source.begin ()), targetStart - reinterpret_cast<const char16_t*> (&*target.begin ()));
}

auto UTFConverter::ConvertQuietly_StroikaPortable_ (const span<const char16_t> source, const span<char8_t> target) -> tuple<ConversionResults, size_t, size_t>
{
    if (source.empty ()) {
        return make_tuple (ConversionResults::ok, 0, 0); // avoid dereferncing empty iterators
    }
    using namespace UTFConvert_libutfxx_;
    const char*      sourceStart = reinterpret_cast<const char*> (&*source.begin ());
    const char*      sourceEnd   = sourceStart + source.size ();
    char16_t*        targetStart = reinterpret_cast<char16_t*> (&*target.begin ());
    char16_t*        targetEnd   = targetStart + target.size ();
    ConversionResult r           = ConvertUTF8toUTF16_ (&sourceStart, sourceEnd, &targetStart, targetEnd, ConversionFlags::lenientConversion); // look at options
    return make_tuple (cvt_ (r), sourceStart - reinterpret_cast<const char*> (&*source.begin ()), targetStart - reinterpret_cast<const char16_t*> (&*target.begin ()));
}

auto UTFConverter::ConvertQuietly_codeCvt_ (const span<const char8_t> source, const span<char16_t> target, mbstate_t* multibyteConversionState) -> tuple<ConversionResults, size_t, size_t>
{
    if (source.empty ()) {
        return make_tuple (ConversionResults::ok, 0, 0); // avoid dereferncing empty iterators
    }
    using namespace UTFConvert_codecvSupport_;
    const char8_t*    sourceStart = reinterpret_cast<const char8_t*> (&*source.begin ());
    const char8_t*    sourceEnd   = sourceStart + source.size ();
    char16_t*         targetStart = reinterpret_cast<char16_t*> (&*target.begin ());
    char16_t*         targetEnd   = targetStart + target.size ();
    ConversionResults r           = ConvertUTF8toUTF16_codecvt_ (multibyteConversionState , & sourceStart, sourceEnd, &targetStart, targetEnd);
    if (r == ConversionResults::ok) {
        return make_tuple (ConversionResults::ok, sourceStart - reinterpret_cast<const char8_t*> (&*source.begin ()), targetStart - reinterpret_cast<const char16_t*> (&*target.begin ()));
    }
    else {
        return make_tuple (r, 0, 0);
    }
}

auto UTFConverter::ConvertQuietly_codeCvt_ (const span<const char16_t> source, const span<char8_t> target) -> tuple<ConversionResults, size_t, size_t>
{
    if (source.empty ()) {
        return make_tuple (ConversionResults::ok, 0, 0); // avoid dereferncing empty iterators
    }
    using namespace UTFConvert_codecvSupport_;
    const char16_t*   sourceStart = reinterpret_cast<const char16_t*> (&*source.begin ());
    const char16_t*   sourceEnd   = sourceStart + source.size ();
    char8_t*          targetStart = reinterpret_cast<char8_t*> (&*target.begin ());
    char8_t*          targetEnd   = targetStart + target.size ();
    ConversionResults r           = ConvertUTF16toUTF8_codecvt_ (&sourceStart, sourceEnd, &targetStart, targetEnd);
    if (r == ConversionResults::ok) {
        return make_tuple (ConversionResults::ok, sourceStart - reinterpret_cast<const char16_t*> (&*source.begin ()), targetStart - reinterpret_cast<const char8_t*> (&*target.begin ()));
    }
    else {
        return make_tuple (r, 0, 0);
    }
}
