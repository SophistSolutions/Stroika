/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Execution/Exceptions.h"

#include "CharacterEncodingException.h"

#include "UTFConvert.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

namespace {
    // crazy - from https://en.cppreference.com/w/cpp/locale/codecvt
    template <typename FACET>
    struct deletable_facet_ : FACET {
        template <typename... Args>
        deletable_facet_ (Args&&... args)
            : FACET{forward<Args> (args)...}
        {
        }
        ~deletable_facet_ ()
        {
        }
    };
}

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
        // static constexpr char32_t UNI_REPLACEMENT_CHAR = (char32_t)0x0000FFFD;
        static constexpr char32_t UNI_MAX_BMP         = (char32_t)0x0000FFFF;
        static constexpr char32_t UNI_MAX_UTF16       = (char32_t)0x0010FFFF;
        static constexpr char32_t UNI_MAX_LEGAL_UTF32 = (char32_t)0x0010FFFF;

        enum ConversionResult {
            conversionOK,    /* conversion successful */
            sourceExhausted, /* partial character in source, but hit end */
            targetExhausted, /* insuff. room in target for conversion */
            sourceIllegal    /* source sequence is illegal/malformed */
        };

        UTFConvert::ConversionStatusFlag cvt_ (ConversionResult cr)
        {
            switch (cr) {
                case conversionOK:
                    return UTFConvert::ConversionStatusFlag::ok;
                case sourceExhausted:
                    return UTFConvert::ConversionStatusFlag::sourceExhausted;
                case targetExhausted:
                    RequireNotReached (); // API doesn't allow this
                    return UTFConvert::ConversionStatusFlag::sourceIllegal;
                case sourceIllegal:
                    return UTFConvert::ConversionStatusFlag::sourceIllegal;
                default:
                    RequireNotReached (); // API doesn't allow this
                    return UTFConvert::ConversionStatusFlag::sourceIllegal;
            }
        }

        constexpr int halfShift = 10; /* used for shifting by 10 bits */

        constexpr char32_t halfBase = 0x0010000UL;
        constexpr char32_t halfMask = 0x3FFUL;

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
        constexpr char8_t firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

        /*
         * Index into the table below with the first byte of a UTF-8 sequence to
         * get the number of trailing bytes that are supposed to follow it.
         * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
         * left as-is for anyone who may want to do such conversion, which was
         * allowed in earlier algorithms.
         */
        constexpr char trailingBytesForUTF8[256] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5};

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
        bool isLegalUTF8_ (const char8_t* source, int length)
        {
            char8_t        a;
            const char8_t* srcptr = source + length;
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

        inline ConversionResult ConvertUTF8toUTF16_ (const char8_t** sourceStart, const char8_t* sourceEnd, char16_t** targetStart,
                                                     char16_t* targetEnd, optional<char32_t> missingCharacterReplacement)
        {
            ConversionResult result     = conversionOK;
            const char8_t*   source     = *sourceStart;
            char16_t*        target     = *targetStart;
            auto             addMissing = [&] () {
                if (Character{*missingCharacterReplacement}.IsSurrogatePair ()) {
                    auto p    = Character{*missingCharacterReplacement}.GetSurrogatePair ();
                    *target++ = p.first;
                    *target++ = p.second;
                }
                else {
                    *target++ = static_cast<char16_t> (*missingCharacterReplacement);
                }
            };
            while (source < sourceEnd) {
                char32_t       ch               = 0;
                unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
                if (source + extraBytesToRead >= sourceEnd) {
                    result = sourceExhausted;
                    break;
                }
                if (!isLegalUTF8_ (source, extraBytesToRead + 1)) {
                    if (missingCharacterReplacement) {
                        AssertNotImplemented (); // @todo - not hard - but not done
                    }
                    else {
                        result = sourceIllegal;
                        break;
                    }
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
                    if (ch >= Character::kUNICODESurrogate_High_Start && ch <= Character::kUNICODESurrogate_Low_End) {
                        if (missingCharacterReplacement) {
                            addMissing ();
                        }
                        else {
                            source -= (extraBytesToRead + 1); /* return to the illegal value itself */
                            result = sourceIllegal;
                            break;
                        }
                    }
                    else {
                        *target++ = (char16_t)ch; /* normal case */
                    }
                }
                else if (ch > UNI_MAX_UTF16) {
                    if (missingCharacterReplacement) {
                        addMissing ();
                    }
                    else {
                        result = sourceIllegal;
                        source -= (extraBytesToRead + 1); /* return to the start */
                        break;                            /* Bail out; shouldn't continue */
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
                    *target++ = (char16_t)((ch >> halfShift) + Character::kUNICODESurrogate_High_Start);
                    *target++ = (char16_t)((ch & halfMask) + Character::kUNICODESurrogate_Low_Start);
                }
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }
        inline ConversionResult ConvertUTF16toUTF8_ (const char16_t** sourceStart, const char16_t* sourceEnd, char8_t** targetStart,
                                                     char8_t* targetEnd, optional<char32_t> missingCharacterReplacement)
        {
            ConversionResult result = conversionOK;
            const char16_t*  source = *sourceStart;
            char8_t*         target = *targetStart;
            while (source < sourceEnd) {
                char32_t           ch;
                unsigned short     bytesToWrite = 0;
                constexpr char32_t byteMask     = 0xBF;
                constexpr char32_t byteMark     = 0x80;
                const char16_t*    oldSource    = source; /* In case we have to back up because of target overflow. */
                ch                              = *source++;
                /* If we have a surrogate pair, convert to char32_t first. */
                if (ch >= Character::kUNICODESurrogate_High_Start && ch <= Character::kUNICODESurrogate_High_End) [[unlikely]] {
                    /* If the 16 bits following the high surrogate are in the source buffer... */
                    if (source < sourceEnd) [[likely]] {
                        char32_t ch2 = *source;
                        /* If it's a low surrogate, convert to char32_t. */
                        if (ch2 >= Character::kUNICODESurrogate_Low_Start && ch2 <= Character::kUNICODESurrogate_Low_End) {
                            ch = ((ch - Character::kUNICODESurrogate_High_Start) << halfShift) + (ch2 - Character::kUNICODESurrogate_Low_Start) + halfBase;
                            ++source;
                        }
                        else if (missingCharacterReplacement == nullopt) { /* it's an unpaired high surrogate */
                            --source;                                      /* return to the illegal value itself */
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
                else if (missingCharacterReplacement == nullopt) {
                    /* UTF-16 surrogate values are illegal in UTF-32 */
                    if (ch >= Character::kUNICODESurrogate_Low_Start && ch <= Character::kUNICODESurrogate_Low_End) {
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
                    Assert (missingCharacterReplacement.has_value ()); // I THINK sb caught above if 'strict' mode
                    ch = *missingCharacterReplacement;
                }

                target += bytesToWrite;
                if (target > targetEnd) {
                    source = oldSource; /* Back up source pointer! */
                    target -= bytesToWrite;
                    result = targetExhausted;
                    break;
                }
                switch (bytesToWrite) { /* note: everything falls through. */
                    case 4:
                        *--target = (char8_t)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 3:
                        *--target = (char8_t)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 2:
                        *--target = (char8_t)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 1:
                        *--target = (char8_t)(ch | firstByteMark[bytesToWrite]);
                }
                target += bytesToWrite;
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }
        DISABLE_COMPILER_MSC_WARNING_START (4701) // potentially uninitialized local variable 'ch' used (WRONG cuz if we get into loop, initialized
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\""); // potentially uninitialized local variable 'ch' used (WRONG cuz if we get into loop, initialized
        inline ConversionResult ConvertUTF16toUTF32_ (const char16_t** sourceStart, const char16_t* sourceEnd, char32_t** targetStart,
                                                      char32_t* targetEnd, optional<char32_t> missingCharacterReplacement)
        {
            ConversionResult result = conversionOK;
            const char16_t*  source = *sourceStart;
            char32_t*        target = *targetStart;
            char32_t         ch, ch2;
            while (source < sourceEnd) {
                const char16_t* oldSource = source; /*  In case we have to back up because of target overflow. */
                ch                        = *source++;
                /* If we have a surrogate pair, convert to UTF32 first. */
                if (ch >= Character::kUNICODESurrogate_High_Start && ch <= Character::kUNICODESurrogate_High_End) [[unlikely]] {
                    /* If the 16 bits following the high surrogate are in the source buffer... */
                    if (source < sourceEnd) {
                        ch2 = *source;
                        /* If it's a low surrogate, convert to UTF32. */
                        if (ch2 >= Character::kUNICODESurrogate_Low_Start && ch2 <= Character::kUNICODESurrogate_Low_End) {
                            ch = ((ch - Character::kUNICODESurrogate_High_Start) << halfShift) + (ch2 - Character::kUNICODESurrogate_Low_Start) + halfBase;
                            ++source;
                        }
                        else if (missingCharacterReplacement == nullopt) { /* it's an unpaired high surrogate */
                            --source;                                      /* return to the illegal value itself */
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
                else if (missingCharacterReplacement == nullopt) {
                    /* UTF-16 surrogate values are illegal in UTF-32 */
                    if (ch >= Character::kUNICODESurrogate_Low_Start && ch <= Character::kUNICODESurrogate_Low_End) {
                        --source; /* return to the illegal value itself */
                        result = sourceIllegal;
                        break;
                    }
                }
                if (target >= targetEnd) {
                    source = oldSource; /* Back up source pointer! */
                    result = targetExhausted;
                    break;
                }
                *target++ = ch;
            }
            *sourceStart = source;
            *targetStart = target;
            if (result == sourceIllegal) {
                DbgTrace (L"ConvertUTF16toUTF32 illegal seq 0x%04x,%04x\n", ch, ch2);
            }
            return result;
        }
        DISABLE_COMPILER_MSC_WARNING_END (4701)
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\""); // potentially uninitialized local variable 'ch' used (WRONG cuz if we get into loop, initialized
        inline ConversionResult ConvertUTF32toUTF16_ (const char32_t** sourceStart, const char32_t* sourceEnd, char16_t** targetStart,
                                                      char16_t* targetEnd, optional<char32_t> missingCharacterReplacement)
        {
            ConversionResult result     = conversionOK;
            const char32_t*  source     = *sourceStart;
            char16_t*        target     = *targetStart;
            auto             addMissing = [&] () {
                if (Character{*missingCharacterReplacement}.IsSurrogatePair ()) {
                    auto p    = Character{*missingCharacterReplacement}.GetSurrogatePair ();
                    *target++ = p.first;
                    *target++ = p.second;
                }
                else {
                    *target++ = static_cast<char16_t> (*missingCharacterReplacement);
                }
            };
            while (source < sourceEnd) {
                char32_t ch;
                if (target >= targetEnd) {
                    result = targetExhausted;
                    break;
                }
                ch = *source++;
                if (ch <= UNI_MAX_BMP) [[likely]] { /* Target is a character <= 0xFFFF */
                    /*   UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
                    if (ch >= Character::kUNICODESurrogate_High_Start && ch <= Character::kUNICODESurrogate_Low_End) [[unlikely]] {
                        if (missingCharacterReplacement == nullopt) {
                            --source; /* return to the illegal value itself */
                            result = sourceIllegal;
                            break;
                        }
                        else {
                            addMissing ();
                        }
                    }
                    else {
                        *target++ = (char16_t)ch; /* normal case */
                    }
                }
                else if (ch > UNI_MAX_LEGAL_UTF32) {
                    if (missingCharacterReplacement) {
                        addMissing ();
                    }
                    else {
                        result = sourceIllegal;
                    }
                }
                else {
                    /* target is a character in range 0xFFFF - 0x10FFFF. */
                    if (target + 1 >= targetEnd) {
                        --source; /* Back up source pointer! */
                        result = targetExhausted;
                        break;
                    }
                    ch -= halfBase;
                    *target++ = (char16_t)((ch >> halfShift) + Character::kUNICODESurrogate_High_Start);
                    *target++ = (char16_t)((ch & halfMask) + Character::kUNICODESurrogate_Low_Start);
                }
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }
        inline ConversionResult ConvertUTF8toUTF32_ (const char8_t** sourceStart, const char8_t* sourceEnd, char32_t** targetStart,
                                                     char32_t* targetEnd, optional<char32_t> missingCharacterReplacement)
        {
            ConversionResult result = conversionOK;
            const char8_t*   source = *sourceStart;
            char32_t*        target = *targetStart;
            while (source < sourceEnd) {
                char32_t       ch               = 0;
                unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
                if (source + extraBytesToRead >= sourceEnd) [[unlikely]] {
                    result = sourceExhausted;
                    break;
                }
                if (!isLegalUTF8_ (source, extraBytesToRead + 1)) {
                    if (missingCharacterReplacement) {
                        AssertNotImplemented (); // @todo - not hard - but not done
                    }
                    else {
                        result = sourceIllegal;
                        break;
                    }
                }
                /*
                 * The cases all fall through. See "Note A" below.
                 */
                switch (extraBytesToRead) {
                    case 5:
                        ch += *source++;
                        ch <<= 6;
                    case 4:
                        ch += *source++;
                        ch <<= 6;
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
                    source -= (extraBytesToRead + 1); /* Back up the source pointer! */
                    result = targetExhausted;
                    break;
                }
                if (ch <= UNI_MAX_LEGAL_UTF32) [[likely]] {
                    /*
                     * UTF-16 surrogate values are illegal in UTF-32, and anything
                     * over Plane 17 (> 0x10FFFF) is illegal.
                     */
                    if (ch >= Character::kUNICODESurrogate_High_Start && ch <= Character::kUNICODESurrogate_Low_End) {
                        if (missingCharacterReplacement) {
                            *target++ = *missingCharacterReplacement;
                        }
                        else {
                            source -= (extraBytesToRead + 1); /* return to the illegal value itself */
                            result = sourceIllegal;
                            break;
                        }
                    }
                    else {
                        *target++ = ch;
                    }
                }
                else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
                    if (missingCharacterReplacement) {
                        *target++ = *missingCharacterReplacement;
                    }
                    else {
                        result = sourceIllegal;
                        break;
                    }
                }
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }
        inline ConversionResult ConvertUTF32toUTF8_ (const char32_t** sourceStart, const char32_t* sourceEnd, char8_t** targetStart,
                                                     char8_t* targetEnd, optional<char32_t> missingCharacterReplacement)
        {
            ConversionResult result = conversionOK;
            const char32_t*  source = *sourceStart;
            char8_t*         target = *targetStart;
            while (source < sourceEnd) {
                char32_t       ch;
                unsigned short bytesToWrite = 0;
                const char32_t byteMask     = 0xBF;
                const char32_t byteMark     = 0x80;
                ch                          = *source++;
                if (missingCharacterReplacement == nullopt) {
                    /* UTF-16 surrogate values are illegal in UTF-32 */
                    if (ch >= Character::kUNICODESurrogate_High_Start && ch <= Character::kUNICODESurrogate_Low_End) [[unlikely]] {
                        --source; /* return to the illegal value itself */
                        result = sourceIllegal;
                        break;
                    }
                }
                /*
                 * Figure out how many bytes the result will require. Turn any
                 * illegally large UTF32 things (> Plane 17) into replacement chars.
                 */
                if (ch < (char32_t)0x80) {
                    bytesToWrite = 1;
                }
                else if (ch < (char32_t)0x800) {
                    bytesToWrite = 2;
                }
                else if (ch < (char32_t)0x10000) {
                    bytesToWrite = 3;
                }
                else if (ch <= UNI_MAX_LEGAL_UTF32) {
                    bytesToWrite = 4;
                }
                else {
                    if (missingCharacterReplacement) {
                        ch           = *missingCharacterReplacement;
                        bytesToWrite = 3; // @todo WRONG - must get right number for this character
                    }
                    else {
                        result = sourceIllegal;
                        break;
                    }
                }

                target += bytesToWrite;
                if (target > targetEnd) {
                    --source; /* Back up source pointer! */
                    target -= bytesToWrite;
                    result = targetExhausted;
                    break;
                }
                switch (bytesToWrite) { /* note: everything falls through. */
                    case 4:
                        *--target = (char8_t)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 3:
                        *--target = (char8_t)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 2:
                        *--target = (char8_t)((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 1:
                        *--target = (char8_t)(ch | firstByteMark[bytesToWrite]);
                }
                target += bytesToWrite;
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }

    }
}

namespace {
    namespace UTFConvert_codecvtSupport_ {
        inline UTFConvert::ConversionStatusFlag cvt_stdcodecvt_results_ (int i)
        {
            switch (i) {
                case codecvt_base::ok:
                    return UTFConvert::ConversionStatusFlag::ok;
                case codecvt_base::error:
                    return UTFConvert::ConversionStatusFlag::sourceIllegal;
                case codecvt_base::partial:
                    return UTFConvert::ConversionStatusFlag::sourceExhausted; // not quite - couldbe target exhuasted?
                case codecvt_base::noconv:
                    return UTFConvert::ConversionStatusFlag::sourceIllegal; // not quite
                default:
                    Assert (false);
                    return UTFConvert::ConversionStatusFlag::sourceIllegal;
            }
        }
        inline UTFConvert::ConversionStatusFlag ConvertUTF8toUTF16_codecvt_ (const char8_t** sourceStart, const char8_t* sourceEnd,
                                                                             char16_t** targetStart, char16_t* targetEnd)
        {
            static const deletable_facet_<codecvt<char16_t, char8_t, mbstate_t>> cvt;
            mbstate_t                                                            ignoredMBState{};
            const char8_t*                                                       sourceCursor = *sourceStart;
            char16_t*                                                            outCursor    = *targetStart;
            codecvt_base::result rr = cvt.in (ignoredMBState, *sourceStart, sourceEnd, sourceCursor, *targetStart, targetEnd, outCursor);
            *sourceStart            = reinterpret_cast<const char8_t*> (sourceCursor);
            *targetStart            = outCursor;
            return cvt_stdcodecvt_results_ (rr);
        }
        inline UTFConvert::ConversionStatusFlag ConvertUTF16toUTF8_codecvt_ (const char16_t** sourceStart, const char16_t* sourceEnd,
                                                                             char8_t** targetStart, char8_t* targetEnd)
        {
            static const deletable_facet_<codecvt<char16_t, char8_t, mbstate_t>> cvt;
            mbstate_t                                                            ignoredMBState{};
            const char16_t*                                                      sourceCursor = *sourceStart;
            char8_t*                                                             outCursor    = *targetStart;
            codecvt_base::result rr = cvt.out (ignoredMBState, *sourceStart, sourceEnd, sourceCursor, *targetStart, targetEnd, outCursor);
            *sourceStart            = reinterpret_cast<const char16_t*> (sourceCursor);
            *targetStart            = reinterpret_cast<char8_t*> (outCursor);
            return cvt_stdcodecvt_results_ (rr);
        }
        inline UTFConvert::ConversionStatusFlag ConvertUTF8toUTF32_codecvt_ (const char8_t** sourceStart, const char8_t* sourceEnd,
                                                                             char32_t** targetStart, char32_t* targetEnd)
        {
            static const deletable_facet_<codecvt<char32_t, char8_t, mbstate_t>> cvt;
            mbstate_t                                                            ignoredState{};

            const char8_t*       sourceCursor = *sourceStart;
            char32_t*            outCursor    = *targetStart;
            codecvt_base::result rr = cvt.in (ignoredState, *sourceStart, sourceEnd, sourceCursor, *targetStart, targetEnd, outCursor);
            *sourceStart            = reinterpret_cast<const char8_t*> (sourceCursor);
            *targetStart            = outCursor;
            return cvt_stdcodecvt_results_ (rr);
        }
        inline UTFConvert::ConversionStatusFlag ConvertUTF32toUTF8_codecvt_ (const char32_t** sourceStart, const char32_t* sourceEnd,
                                                                             char8_t** targetStart, char8_t* targetEnd)
        {
            static const deletable_facet_<codecvt<char32_t, char8_t, mbstate_t>> cvt;
            mbstate_t                                                            ignoredState{};
            const char32_t*                                                      sourceCursor = *sourceStart;
            char8_t*                                                             outCursor    = *targetStart;
            codecvt_base::result rr = cvt.out (ignoredState, *sourceStart, sourceEnd, sourceCursor, *targetStart, targetEnd, outCursor);
            *sourceStart            = reinterpret_cast<const char32_t*> (sourceCursor);
            *targetStart            = reinterpret_cast<char8_t*> (outCursor);
            return cvt_stdcodecvt_results_ (rr);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Characters::UTFConvert ***************************
 ********************************************************************************
 */
namespace {
    using ConversionResultWithStatus = Characters::UTFConvert::ConversionResultWithStatus;
    using ConversionStatusFlag       = Characters::UTFConvert::ConversionStatusFlag;

    template <typename IN_T, typename OUT_T, regular_invocable<const IN_T**, const IN_T*, OUT_T**, OUT_T*, optional<char32_t>> FUN2DO_REAL_WORK>
    inline auto ConvertQuietly_StroikaPortable_helper_ (optional<Character> invalidCharacterReplacement, span<const IN_T> source,
                                                        span<OUT_T> target, FUN2DO_REAL_WORK&& realWork) -> ConversionResultWithStatus
    {
        using namespace UTFConvert_libutfxx_;
        const IN_T* sourceStart = source.data ();
        const IN_T* sourceEnd   = sourceStart + source.size ();
        OUT_T*      targetStart = target.data ();
        OUT_T*      targetEnd   = targetStart + target.size ();
        // convert replacement character to target character set, and then pass that
        ConversionResult r =
            realWork (&sourceStart, sourceEnd, &targetStart, targetEnd,
                      invalidCharacterReplacement.has_value () ? invalidCharacterReplacement->As<char32_t> () : optional<char32_t>{});
        return ConversionResultWithStatus{
            {static_cast<size_t> (sourceStart - source.data ()), static_cast<size_t> (targetStart - target.data ())}, cvt_ (r)};
    }
}
auto UTFConvert::ConvertQuietly_StroikaPortable_ (optional<Character> invalidCharacterReplacement, span<const char8_t> source, span<char16_t> target)
    -> ConversionResultWithStatus
{
    return ConvertQuietly_StroikaPortable_helper_ (invalidCharacterReplacement, source, target, UTFConvert_libutfxx_::ConvertUTF8toUTF16_);
}
auto UTFConvert::ConvertQuietly_StroikaPortable_ (optional<Character> invalidCharacterReplacement, span<const char8_t> source, span<char32_t> target)
    -> ConversionResultWithStatus
{
    return ConvertQuietly_StroikaPortable_helper_ (invalidCharacterReplacement, source, target, UTFConvert_libutfxx_::ConvertUTF8toUTF32_);
}
auto UTFConvert::ConvertQuietly_StroikaPortable_ (optional<Character> invalidCharacterReplacement, span<const char16_t> source,
                                                  span<char32_t> target) -> ConversionResultWithStatus
{
    return ConvertQuietly_StroikaPortable_helper_ (invalidCharacterReplacement, source, target, UTFConvert_libutfxx_::ConvertUTF16toUTF32_);
}
auto UTFConvert::ConvertQuietly_StroikaPortable_ (optional<Character> invalidCharacterReplacement, span<const char32_t> source,
                                                  span<char16_t> target) -> ConversionResultWithStatus
{
    return ConvertQuietly_StroikaPortable_helper_ (invalidCharacterReplacement, source, target, UTFConvert_libutfxx_::ConvertUTF32toUTF16_);
}
auto UTFConvert::ConvertQuietly_StroikaPortable_ (optional<Character> invalidCharacterReplacement, span<const char32_t> source, span<char8_t> target)
    -> ConversionResultWithStatus
{
    return ConvertQuietly_StroikaPortable_helper_ (invalidCharacterReplacement, source, target, UTFConvert_libutfxx_::ConvertUTF32toUTF8_);
}
auto UTFConvert::ConvertQuietly_StroikaPortable_ (optional<Character> invalidCharacterReplacement, span<const char16_t> source, span<char8_t> target)
    -> ConversionResultWithStatus
{
    return ConvertQuietly_StroikaPortable_helper_ (invalidCharacterReplacement, source, target, UTFConvert_libutfxx_::ConvertUTF16toUTF8_);
}

namespace {
    template <typename IN_T, typename OUT_T, typename FUN2DO_REAL_WORK>
    inline auto ConvertQuietly_codeCvt_helper_ (span<const IN_T> source, const span<OUT_T> target, FUN2DO_REAL_WORK&& realWork) -> ConversionResultWithStatus
    {
        using namespace UTFConvert_codecvtSupport_;
        const IN_T*          sourceStart = reinterpret_cast<const IN_T*> (source.data ());
        const IN_T*          sourceEnd   = sourceStart + source.size ();
        OUT_T*               targetStart = reinterpret_cast<OUT_T*> (target.data ());
        OUT_T*               targetEnd   = targetStart + target.size ();
        ConversionStatusFlag r           = realWork (&sourceStart, sourceEnd, &targetStart, targetEnd);
        if (r == ConversionStatusFlag::ok) {
            return ConversionResultWithStatus{{static_cast<size_t> (sourceStart - reinterpret_cast<const IN_T*> (source.data ())),
                                               static_cast<size_t> (targetStart - reinterpret_cast<const OUT_T*> (target.data ()))},
                                              ConversionStatusFlag::ok};
        }
        else {
            return ConversionResultWithStatus{{0, 0}, r};
        }
    }
}
auto UTFConvert::ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char16_t> target) -> ConversionResultWithStatus
{
    return ConvertQuietly_codeCvt_helper_ (source, target, UTFConvert_codecvtSupport_::ConvertUTF8toUTF16_codecvt_);
}
auto UTFConvert::ConvertQuietly_codeCvt_ (span<const char16_t> source, span<char8_t> target) -> ConversionResultWithStatus
{
    return ConvertQuietly_codeCvt_helper_ (source, target, UTFConvert_codecvtSupport_::ConvertUTF16toUTF8_codecvt_);
}
auto UTFConvert::ConvertQuietly_codeCvt_ (span<const char8_t> source, span<char32_t> target) -> ConversionResultWithStatus
{
    return ConvertQuietly_codeCvt_helper_ (source, target, UTFConvert_codecvtSupport_::ConvertUTF8toUTF32_codecvt_);
}
auto UTFConvert::ConvertQuietly_codeCvt_ (span<const char32_t> source, span<char8_t> target) -> ConversionResultWithStatus
{
    return ConvertQuietly_codeCvt_helper_ (source, target, UTFConvert_codecvtSupport_::ConvertUTF32toUTF8_codecvt_);
}

void UTFConvert::Throw (ConversionStatusFlag cr, size_t errorAtSourceOffset)
{
    switch (cr) {
        case ConversionStatusFlag::sourceExhausted: {
            static const auto kException_ = Execution::RuntimeErrorException{"Invalid UNICODE source string (incomplete UTF character)"sv};
            Execution::Throw (kException_);
        }
        case ConversionStatusFlag::sourceIllegal: {
            Execution::Throw (CharacterEncodingException{CharacterEncodingException::eDecoding, errorAtSourceOffset, "UNICODE"sv});
        }
        default:
            AssertNotReached ();
    }
}
