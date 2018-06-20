/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <mutex>
#include <set>

#include "../Characters/CString/Utilities.h"
#include "../Characters/Format.h"
#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Containers/Common.h"
#include "../Execution/Common.h"
#include "../Execution/Exceptions.h"
#include "../Execution/StringException.h"
#include "../Memory/SmallStackBuffer.h"

#include "CodePage.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

#ifndef qBuildInTableDrivenCodePageBuilderProc
#define qBuildInTableDrivenCodePageBuilderProc 0
#endif

/*
 *  Hack to build 'myiswalpha' and 'myiswpunct' for SPR#1220 (revised and moved here for SPR#1306 and class
 *  'CharacterProperties').
 */
#ifndef qBuildMemoizedISXXXBuilderProc
#define qBuildMemoizedISXXXBuilderProc 0
#endif

/*
 *  Use this to test my IsWXXX functions produce the right results. Only test under WinXP,
 *  since that is the reference they are copying (SPR#1229).
 */
#ifndef qTestMyISWXXXFunctions
#define qTestMyISWXXXFunctions 0
#endif

#if qBuildInTableDrivenCodePageBuilderProc || qBuildMemoizedISXXXBuilderProc
#include <fstream>
#endif

using namespace Characters;

#if qPlatform_Windows
namespace {
    inline const wchar_t*  SAFE_WIN_WCHART_CAST_ (const char16_t* t) { return reinterpret_cast<const wchar_t*> (t); }
    inline wchar_t*        SAFE_WIN_WCHART_CAST_ (char16_t* t) { return reinterpret_cast<wchar_t*> (t); }
    inline const char16_t* SAFE_WIN_WCHART_CAST_ (const wchar_t* t) { return reinterpret_cast<const char16_t*> (t); }
    inline char16_t*       SAFE_WIN_WCHART_CAST_ (wchar_t* t) { return reinterpret_cast<char16_t*> (t); }
}
#endif

namespace {
    // BASED on https://github.com/codebrainz/libutfxx/blob/master/utf/ConvertUTF.h
    // https://github.com/codebrainz/libutfxx/blob/master/utf/ConvertUTF.c
    namespace ConvertUTF_ {
        /*
         * Copyright 2001-2004 Unicode, Inc.
         *
         * Disclaimer
         *
         * This source code is provided as is by Unicode, Inc. No claims are
         * made as to fitness for any particular purpose. No warranties of any
         * kind are expressed or implied. The recipient agrees to determine
         * applicability of information provided. If this file has been
         * purchased on magnetic or optical media from Unicode, Inc., the
         * sole remedy for any claim will be exchange of defective media
         * within 90 days of receipt.
         *
         * Limitations on Rights to Redistribute This Code
         *
         * Unicode, Inc. hereby grants the right to freely use the information
         * supplied in this file in the creation of products supporting the
         * Unicode Standard, and to make copies of this file in any form
         * for internal or external distribution as long as this notice
         * remains attached.
         */

        /* ---------------------------------------------------------------------
            Conversions between UTF32, UTF-16, and UTF-8.  Header file.
            Several funtions are included here, forming a complete set of
            conversions between the three formats.  UTF-7 is not included
            here, but is handled in a separate source file.
            Each of these routines takes pointers to input buffers and output
            buffers.  The input buffers are const.
            Each routine converts the text between *sourceStart and sourceEnd,
            putting the result into the buffer between *targetStart and
            targetEnd. Note: the end pointers are *after* the last item: e.g.
            *(sourceEnd - 1) is the last item.
            The return result indicates whether the conversion was successful,
            and if not, whether the problem was in the source or target buffers.
            (Only the first encountered problem is indicated.)
            After the conversion, *sourceStart and *targetStart are both
            updated to point to the end of last text successfully converted in
            the respective buffers.
            Input parameters:
            sourceStart - pointer to a pointer to the source buffer.
                The contents of this are modified on return so that
                it points at the next thing to be converted.
            targetStart - similarly, pointer to pointer to the target buffer.
            sourceEnd, targetEnd - respectively pointers to the ends of the
                two buffers, for overflow checking only.
            These conversion functions take a ConversionFlags argument. When this
            flag is set to strict, both irregular sequences and isolated surrogates
            will cause an error.  When the flag is set to lenient, both irregular
            sequences and isolated surrogates are converted.
            Whether the flag is strict or lenient, all illegal sequences will cause
            an error return. This includes sequences such as: <F4 90 80 80>, <C0 80>,
            or <A0> in UTF-8, and values above 0x10FFFF in UTF-32. Conformant code
            must check for illegal sequences.
            When the flag is set to lenient, characters over 0x10FFFF are converted
            to the replacement character; otherwise (when the flag is set to strict)
            they constitute an error.
            Output parameters:
            The value "sourceIllegal" is returned from some routines if the input
            sequence is malformed.  When "sourceIllegal" is returned, the source
            value will point to the illegal value that caused the problem. E.g.,
            in UTF-8 when a sequence is malformed, it points to the start of the
            malformed sequence.
            Author: Mark E. Davis, 1994.
            Rev History: Rick McGowan, fixes & updates May 2001.
                 Fixes & updates, Sept 2001.
        ------------------------------------------------------------------------ */
        /* ---------------------------------------------------------------------
        Conversions between UTF32, UTF-16, and UTF-8. Source code file.
        Author: Mark E. Davis, 1994.
        Rev History: Rick McGowan, fixes & updates May 2001.
        Sept 2001: fixed const & error conditions per
        mods suggested by S. Parent & A. Lillich.
        June 2002: Tim Dodd added detection and handling of incomplete
        source sequences, enhanced error detection, added casts
        to eliminate compiler warnings.
        July 2003: slight mods to back out aggressive FFFE detection.
        Jan 2004: updated switches in from-UTF8 conversions.
        Oct 2004: updated to use UNI_MAX_LEGAL_UTF32 in UTF-32 conversions.
        See the header file "ConvertUTF.h" for complete documentation.
        ------------------------------------------------------------------------ */

        using UTF8  = uint8_t;
        using UTF16 = char16_t;
        using UTF32 = char32_t;
        /* Some fundamental constants */
        constexpr UTF32 UNI_REPLACEMENT_CHAR{(UTF32)0x0000FFFD};
        constexpr UTF32 UNI_MAX_BMP{(UTF32)0x0000FFFF};
        constexpr UTF32 UNI_MAX_UTF16{(UTF32)0x0010FFFF};
        constexpr UTF32 UNI_MAX_UTF32{(UTF32)0x7FFFFFFF};
        constexpr UTF32 UNI_MAX_LEGAL_UTF32{(UTF32)0x0010FFFF};

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

        constexpr int halfShift = 10; /* used for shifting by 10 bits */

        constexpr UTF32 halfBase = 0x0010000UL;
        constexpr UTF32 halfMask = 0x3FFUL;

        constexpr UTF32 UNI_SUR_HIGH_START{(UTF32)0xD800};
        constexpr UTF32 UNI_SUR_HIGH_END{(UTF32)0xDBFF};
        constexpr UTF32 UNI_SUR_LOW_START{(UTF32)0xDC00};
        constexpr UTF32 UNI_SUR_LOW_END{(UTF32)0xDFFF};

        ConversionResult ConvertUTF32toUTF16 (const UTF32** sourceStart, const UTF32* sourceEnd, UTF16** targetStart, UTF16* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const UTF32*     source = *sourceStart;
            UTF16*           target = *targetStart;
            while (source < sourceEnd) {
                UTF32 ch;
                if (target >= targetEnd) {
                    result = targetExhausted;
                    break;
                }
                ch = *source++;
                if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
                    /* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
                    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                        if (flags == strictConversion) {
                            --source; /* return to the illegal value itself */
                            result = sourceIllegal;
                            break;
                        }
                        else {
                            *target++ = UNI_REPLACEMENT_CHAR;
                        }
                    }
                    else {
                        *target++ = (UTF16)ch; /* normal case */
                    }
                }
                else if (ch > UNI_MAX_LEGAL_UTF32) {
                    if (flags == strictConversion) {
                        result = sourceIllegal;
                    }
                    else {
                        *target++ = UNI_REPLACEMENT_CHAR;
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
                    *target++ = (UTF16) ((ch >> halfShift) + UNI_SUR_HIGH_START);
                    *target++ = (UTF16) ((ch & halfMask) + UNI_SUR_LOW_START);
                }
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }

        ConversionResult ConvertUTF16toUTF32 (const UTF16** sourceStart, const UTF16* sourceEnd, UTF32** targetStart, UTF32* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const UTF16*     source = *sourceStart;
            UTF32*           target = *targetStart;
            UTF32            ch, ch2;
            while (source < sourceEnd) {
                const UTF16* oldSource = source; /*  In case we have to back up because of target overflow. */
                ch                     = *source++;
                /* If we have a surrogate pair, convert to UTF32 first. */
                if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
                    /* If the 16 bits following the high surrogate are in the source buffer... */
                    if (source < sourceEnd) {
                        ch2 = *source;
                        /* If it's a low surrogate, convert to UTF32. */
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
                if (target >= targetEnd) {
                    source = oldSource; /* Back up source pointer! */
                    result = targetExhausted;
                    break;
                }
                *target++ = ch;
            }
            *sourceStart = source;
            *targetStart = target;
#ifdef CVTUTF_DEBUG
            if (result == sourceIllegal) {
                fprintf (stderr, "ConvertUTF16toUTF32 illegal seq 0x%04x,%04x\n", ch, ch2);
                fflush (stderr);
            }
#endif
            return result;
        }

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
        * Magic values subtracted from a buffer value during UTF8 conversion.
        * This table contains as many values as there might be trailing bytes
        * in a UTF-8 sequence.
        */
        constexpr UTF32 offsetsFromUTF8[6] = {0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL};

        /*
         * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
         * into the first byte, depending on how many bytes follow.  There are
         * as many entries in this table as there are UTF-8 sequence types.
         * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
         * for *legal* UTF-8 will be 4 or fewer bytes total.
         */
        constexpr UTF8 firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

        /* The interface converts a whole buffer to avoid function-call overhead.
         * Constants have been gathered. Loops & conditionals have been removed as
         * much as possible for efficiency, in favor of drop-through switches.
         * (See "Note A" at the bottom of the file for equivalent code.)
         * If your compiler supports it, the "isLegalUTF8" call can be turned
         * into an inline function.
         */

        ConversionResult ConvertUTF16toUTF8 (const UTF16** sourceStart, const UTF16* sourceEnd, UTF8** targetStart, UTF8* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const UTF16*     source = *sourceStart;
            UTF8*            target = *targetStart;
            while (source < sourceEnd) {
                UTF32          ch;
                unsigned short bytesToWrite = 0;
                const UTF32    byteMask     = 0xBF;
                const UTF32    byteMark     = 0x80;
                const UTF16*   oldSource    = source; /* In case we have to back up because of target overflow. */
                ch                          = *source++;
                /* If we have a surrogate pair, convert to UTF32 first. */
                if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
                    /* If the 16 bits following the high surrogate are in the source buffer... */
                    if (source < sourceEnd) {
                        UTF32 ch2 = *source;
                        /* If it's a low surrogate, convert to UTF32. */
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
                if (ch < (UTF32)0x80) {
                    bytesToWrite = 1;
                }
                else if (ch < (UTF32)0x800) {
                    bytesToWrite = 2;
                }
                else if (ch < (UTF32)0x10000) {
                    bytesToWrite = 3;
                }
                else if (ch < (UTF32)0x110000) {
                    bytesToWrite = 4;
                }
                else {
                    bytesToWrite = 3;
                    ch           = UNI_REPLACEMENT_CHAR;
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
                        *--target = (UTF8) ((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 3:
                        *--target = (UTF8) ((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 2:
                        *--target = (UTF8) ((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 1:
                        *--target = (UTF8) (ch | firstByteMark[bytesToWrite]);
                }
                target += bytesToWrite;
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }

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
        bool isLegalUTF8 (const UTF8* source, int length)
        {
            UTF8        a;
            const UTF8* srcptr = source + length;
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

        /*
         * Exported function to return whether a UTF-8 sequence is legal or not.
         * This is not used here; it's just exported.
         */
        bool isLegalUTF8Sequence (const UTF8* source, const UTF8* sourceEnd)
        {
            int length = trailingBytesForUTF8[*source] + 1;
            if (source + length > sourceEnd) {
                return false;
            }
            return isLegalUTF8 (source, length);
        }

        ConversionResult ConvertUTF8toUTF16 (const UTF8** sourceStart, const UTF8* sourceEnd, UTF16** targetStart, UTF16* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const UTF8*      source = *sourceStart;
            UTF16*           target = *targetStart;
            while (source < sourceEnd) {
                UTF32          ch               = 0;
                unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
                if (source + extraBytesToRead >= sourceEnd) {
                    result = sourceExhausted;
                    break;
                }
                /* Do this check whether lenient or strict */
                if (!isLegalUTF8 (source, extraBytesToRead + 1)) {
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

                if (target >= targetEnd) {
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
                        *target++ = (UTF16)ch; /* normal case */
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
                    *target++ = (UTF16) ((ch >> halfShift) + UNI_SUR_HIGH_START);
                    *target++ = (UTF16) ((ch & halfMask) + UNI_SUR_LOW_START);
                }
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }

        ConversionResult ConvertUTF32toUTF8 (const UTF32** sourceStart, const UTF32* sourceEnd, UTF8** targetStart, UTF8* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const UTF32*     source = *sourceStart;
            UTF8*            target = *targetStart;
            while (source < sourceEnd) {
                UTF32          ch;
                unsigned short bytesToWrite = 0;
                const UTF32    byteMask     = 0xBF;
                const UTF32    byteMark     = 0x80;
                ch                          = *source++;
                if (flags == strictConversion) {
                    /* UTF-16 surrogate values are illegal in UTF-32 */
                    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                        --source; /* return to the illegal value itself */
                        result = sourceIllegal;
                        break;
                    }
                }
                /*
                 * Figure out how many bytes the result will require. Turn any
                 * illegally large UTF32 things (> Plane 17) into replacement chars.
                 */
                if (ch < (UTF32)0x80) {
                    bytesToWrite = 1;
                }
                else if (ch < (UTF32)0x800) {
                    bytesToWrite = 2;
                }
                else if (ch < (UTF32)0x10000) {
                    bytesToWrite = 3;
                }
                else if (ch <= UNI_MAX_LEGAL_UTF32) {
                    bytesToWrite = 4;
                }
                else {
                    bytesToWrite = 3;
                    ch           = UNI_REPLACEMENT_CHAR;
                    result       = sourceIllegal;
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
                        *--target = (UTF8) ((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 3:
                        *--target = (UTF8) ((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 2:
                        *--target = (UTF8) ((ch | byteMark) & byteMask);
                        ch >>= 6;
                    case 1:
                        *--target = (UTF8) (ch | firstByteMark[bytesToWrite]);
                }
                target += bytesToWrite;
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }

        ConversionResult ConvertUTF8toUTF32 (const UTF8** sourceStart, const UTF8* sourceEnd, UTF32** targetStart, UTF32* targetEnd, ConversionFlags flags)
        {
            ConversionResult result = conversionOK;
            const UTF8*      source = *sourceStart;
            UTF32*           target = *targetStart;
            while (source < sourceEnd) {
                UTF32          ch               = 0;
                unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
                if (source + extraBytesToRead >= sourceEnd) {
                    result = sourceExhausted;
                    break;
                }
                /* Do this check whether lenient or strict */
                if (!isLegalUTF8 (source, extraBytesToRead + 1)) {
                    result = sourceIllegal;
                    break;
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

                if (target >= targetEnd) {
                    source -= (extraBytesToRead + 1); /* Back up the source pointer! */
                    result = targetExhausted;
                    break;
                }
                if (ch <= UNI_MAX_LEGAL_UTF32) {
                    /*
                     * UTF-16 surrogate values are illegal in UTF-32, and anything
                     * over Plane 17 (> 0x10FFFF) is illegal.
                     */
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
                        *target++ = ch;
                    }
                }
                else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
                    result    = sourceIllegal;
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            }
            *sourceStart = source;
            *targetStart = target;
            return result;
        }

        /* ---------------------------------------------------------------------
            Note A.
            The fall-through switches in UTF-8 reading code save a
            temp variable, some decrements & conditionals.  The switches
            are equivalent to the following loop:
            {
                int tmpBytesToRead = extraBytesToRead+1;
                do {
                ch += *source++;
                --tmpBytesToRead;
                if (tmpBytesToRead) ch <<= 6;
                } while (tmpBytesToRead > 0);
            }
            In UTF-8 writing code, the switches on "bytesToWrite" are
            similarly unrolled loops.
           --------------------------------------------------------------------- */
    }
}

/*
 ********************************************************************************
 **************************** Characters::GetCharsetString **********************
 ********************************************************************************
 */
wstring Characters::GetCharsetString (CodePage cp)
{
    switch (cp) {
        case kCodePage_UTF7:
            return L"UTF-7";
        case kCodePage_UTF8:
            return L"UTF-8";
        default:
            return Characters::CString::Format (L"CodePage %d", cp);
    }
}

namespace {
    template <CodePage CODEPAGE>
    class TableDrivenCodePageConverter_ {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    };

    template <>
    class TableDrivenCodePageConverter_<kCodePage_ANSI> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
    template <>
    class TableDrivenCodePageConverter_<kCodePage_MAC> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
    template <>
    class TableDrivenCodePageConverter_<kCodePage_PC> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
    template <>
    class TableDrivenCodePageConverter_<kCodePage_PCA> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
    template <>
    class TableDrivenCodePageConverter_<kCodePage_GREEK> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
    template <>
    class TableDrivenCodePageConverter_<kCodePage_Turkish> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
    template <>
    class TableDrivenCodePageConverter_<kCodePage_HEBREW> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
    template <>
    class TableDrivenCodePageConverter_<kCodePage_ARABIC> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);

    private:
        static const char16_t kMap[256];
    };
}

namespace {

#if qBuildInTableDrivenCodePageBuilderProc
    static void WriteCodePageTable (CodePage codePage);

    struct DoRunIt {
        DoRunIt ()
        {
            //WriteCodePageTable (kCodePage_ANSI);
            //WriteCodePageTable (kCodePage_MAC);
            //WriteCodePageTable (kCodePage_PC);
            //WriteCodePageTable (kCodePage_PCA);
            //WriteCodePageTable (kCodePage_GREEK);
            //WriteCodePageTable (kCodePage_Turkish);
            //WriteCodePageTable (kCodePage_HEBREW);
            //WriteCodePageTable (kCodePage_ARABIC);
        }
    } gRunIt;
#endif

#if qBuildMemoizedISXXXBuilderProc
    template <class FUNCTION>
    void WriteMemoizedIsXXXProc (FUNCTION function, const string& origFunctionName, const string& functionName)
    {
        ofstream outStream ("IsXXXProc.txt");

        outStream << "bool      " << functionName << " (wchar_t c)\n";
        outStream << "{\n";

        outStream << "\t// ********** " << functionName << " (AUTOGENERATED memoize of " << origFunctionName << " - " << __DATE__ << ") ***********\n";
        outStream << "\t// Hack for SPR#1220 and SPR#1306\n";

        // SPR#1308 - the generated if/then skipchain can be somewhat long, and therefore inefficient.
        // This is needlessly so. In principle - we could break up the long skipchain into many smaller ones
        // nested hierarchically. This would involve first pre-computing the entire list of regions, and
        // then generating the if/then code recursively since before you can generate INNER code you need
        // to know the full range of codepoints contained in all contained ifs.
        //
        // Anyhow - there is a trivial implementation that gets us most of the speedup we seek - since most
        // characters looked up fall in the 0..256 range. So - just handle that specially.
        //

        const wchar_t kBoundaryForSpecialLoop1 = 255;
        const wchar_t kBoundaryForSpecialLoop2 = 5000;

        for (int j = 0; j <= 2; ++j) {

            if (j == 0) {
                outStream << "\tif (c < " << int(kBoundaryForSpecialLoop1) << ") {\n";
            }
            else if (j == 1) {
                outStream << "\telse if (c < " << int(kBoundaryForSpecialLoop2) << ") {\n";
            }
            else {
                outStream << "\telse {\n";
            }

            outStream << "\t\tif (";
            bool   firstTime         = true;
            bool   hasLastTrue       = false;
            size_t firstRangeIdxTrue = 0;
            size_t startLoop         = 0;
            size_t endLoop           = 0;
            if (j == 0) {
                endLoop = kBoundaryForSpecialLoop1;
            }
            else if (j == 1) {
                startLoop = kBoundaryForSpecialLoop1;
                endLoop   = kBoundaryForSpecialLoop2;
            }
            else {
                startLoop = kBoundaryForSpecialLoop2;
                endLoop   = 256 * 256;
            }
            for (size_t i = startLoop; i < endLoop; ++i) {
                bool isT = function (static_cast<wchar_t> (i));

                if (((not isT) or (i + 1 == endLoop)) and hasLastTrue) {
                    // then emit the range...
                    if (not firstTime) {
                        outStream << "\t\t\t||";
                    }
                    size_t rangeEnd = isT ? i : i - 1;
                    outStream << "(" << firstRangeIdxTrue << " <= c && c <= " << rangeEnd << ")";
                    firstTime = false;
                    outStream << "\n";
                    hasLastTrue = false;
                }
                if (isT and not hasLastTrue) {
                    firstRangeIdxTrue = i;
                    hasLastTrue       = true;
                }
            }
            if (firstTime) {
                outStream << "false";
            }

            outStream << "\t\t\t) {\n";
            outStream << "\t\t\t#if             qTestMyISWXXXFunctions\n";
            outStream << "\t\t\tAssert (" << origFunctionName << "(c));\n";
            outStream << "\t\t\t#endif\n";
            outStream << "\t\t\treturn true;\n";
            outStream << "\t\t}\n";

            outStream << "\t}\n";
        }
        outStream << "\t#if             qTestMyISWXXXFunctions\n";
        outStream << "\tAssert (!" << origFunctionName << "(c));\n";
        outStream << "\t#endif\n";
        outStream << "\treturn false;\n";
        outStream << "}\n";
        outStream << "\n";
    }

    struct DoRunIt {
        DoRunIt ()
        {
            WriteMemoizedIsXXXProc (iswalpha, "iswalpha", "CharacterProperties::IsAlpha_M");
            //WriteMemoizedIsXXXProc (iswalnum, "iswalnum", "CharacterProperties::Ialnum_M");
            //WriteMemoizedIsXXXProc (iswpunct, "iswpunct", "CharacterProperties::IsPunct_M");
            //WriteMemoizedIsXXXProc (iswspace, "iswspace", "CharacterProperties::IsSpace_M");
            //WriteMemoizedIsXXXProc (iswcntrl, "iswcntrl", "CharacterProperties::IsCntrl_M");
            //WriteMemoizedIsXXXProc (iswdigit, "iswdigit", "CharacterProperties::IsDigit_M");
        }
    } gRunIt;
#endif

#if qTestMyISWXXXFunctions
    class MyIsWXXXTesterFunctions {
    public:
        MyIsWXXXTesterFunctions ()
        {
            for (wchar_t c = 0; c < 0xffff; ++c) {
                Assert (CharacterProperties::IsAlpha_M (c) == !!iswalpha (c));
                Assert (CharacterProperties::IsAlnum_M (c) == !!iswalnum (c));
                Assert (CharacterProperties::IsPunct_M (c) == !!iswpunct (c));
                Assert (CharacterProperties::IsSpace_M (c) == !!iswspace (c));
                Assert (CharacterProperties::IsCntrl_M (c) == !!iswcntrl (c));
                Assert (CharacterProperties::IsDigit_M (c) == !!iswdigit (c));
            }
        }
    } sMyIsWXXXTesterFunctions;
#endif
}

/*
 ********************************************************************************
 ************* Characters::MapSBUnicodeTextWithMaybeBOMToUNICODE ****************
 ********************************************************************************
 */
void Characters::MapSBUnicodeTextWithMaybeBOMToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
    RequireNotNull (outChars);
    RequireNotNull (outCharCnt);
    [[maybe_unused]] size_t      outBufSize = *outCharCnt;
    CodePagesGuesser::Confidence confidence = CodePagesGuesser::Confidence::eLow;
    CodePage                     cp         = CodePagesGuesser ().Guess (inMBChars, inMBCharCnt, &confidence, nullptr);
    if (confidence <= CodePagesGuesser::Confidence::eLow) {
        cp = kCodePage_UTF8;
    }
    CodePageConverter cpCvt (cp, CodePageConverter::eHandleBOM);
    cpCvt.MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
    Ensure (*outCharCnt <= outBufSize);
}

/*
 ********************************************************************************
 ******************************** CodePageConverter *****************************
 ********************************************************************************
 */
size_t CodePageConverter::MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const
{
    size_t resultSize;
    switch (fCodePage) {
        case kCodePage_ANSI:
            resultSize = inCharCnt * 1;
            break;
        case kCodePage_MAC:
            resultSize = inCharCnt * 1;
            break;
        case kCodePage_PC:
            resultSize = inCharCnt * 1;
            break;
        case kCodePage_PCA:
            resultSize = inCharCnt * 1;
            break;
        case kCodePage_SJIS:
            resultSize = inCharCnt * 2;
            break;
        case kCodePage_UTF7:
            resultSize = inCharCnt * 6;
            break; // ITHINK thats right... BOM appears to be 5 chars long? LGP 2001-09-11
        case kCodePage_UTF8:
            resultSize = UTF8Converter ().MapFromUNICODE_QuickComputeOutBufSize (inChars, inCharCnt);
        default:
            resultSize = inCharCnt * 8;
            break; // I THINK that should always be enough - but who knows...
    }
    if (GetHandleBOM ()) {
        switch (fCodePage) {
            case kCodePage_UNICODE_WIDE:
            case kCodePage_UNICODE_WIDE_BIGENDIAN: {
                // BOM (byte order mark)
                resultSize += 2;
            } break;
            case kCodePage_UTF7: {
                resultSize += 5; // for BOM
            } break;
            case kCodePage_UTF8: {
                resultSize += 3; // BOM (byte order mark)
            }
        }
    }
    return resultSize;
}

void CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const
{
    Require (inMBCharCnt == 0 or inMBChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);

    if (GetHandleBOM ()) {
        size_t bytesToStrip = 0;
        if (CodePagesGuesser ().Guess (inMBChars, inMBCharCnt, nullptr, &bytesToStrip) == fCodePage) {
            Assert (inMBCharCnt >= bytesToStrip);
            inMBChars += bytesToStrip;
            inMBCharCnt -= bytesToStrip;
        }
    }
    switch (fCodePage) {
        case kCodePage_ANSI:
            TableDrivenCodePageConverter_<kCodePage_ANSI>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_MAC:
            TableDrivenCodePageConverter_<kCodePage_MAC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_PC:
            TableDrivenCodePageConverter_<kCodePage_PC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_PCA:
            TableDrivenCodePageConverter_<kCodePage_PCA>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_GREEK:
            TableDrivenCodePageConverter_<kCodePage_GREEK>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_Turkish:
            TableDrivenCodePageConverter_<kCodePage_Turkish>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_HEBREW:
            TableDrivenCodePageConverter_<kCodePage_HEBREW>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_ARABIC:
            TableDrivenCodePageConverter_<kCodePage_ARABIC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_UNICODE_WIDE: {
            const wchar_t* inWChars   = reinterpret_cast<const wchar_t*> (inMBChars);
            size_t         inWCharCnt = (inMBCharCnt / sizeof (wchar_t));
            *outCharCnt               = inWCharCnt;
            (void)::memcpy (outChars, inWChars, inWCharCnt * sizeof (wchar_t));
        } break;
        case kCodePage_UNICODE_WIDE_BIGENDIAN: {
            const wchar_t* inWChars   = reinterpret_cast<const wchar_t*> (inMBChars);
            size_t         inWCharCnt = (inMBCharCnt / sizeof (wchar_t));
            *outCharCnt               = inWCharCnt;
            for (size_t i = 0; i < inWCharCnt; ++i) {
                wchar_t c = inWChars[i];
                // byteswap
                c           = ((c & 0xff) << 8) + (c >> 8);
                outChars[i] = c;
            }
        } break;
        case kCodePage_UTF8: {
            UTF8Converter ().MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
        } break;
        default: {
#if qPlatform_Windows
            Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapToUNICODE (inMBChars, inMBCharCnt, SAFE_WIN_WCHART_CAST_ (outChars), outCharCnt);
#else
            Execution::Throw (CodePageNotSupportedException (fCodePage));
#endif
        } break;
    }

#if qDebug && qPlatform_Windows && 0
    // Assure my baked tables (and UTF8 converters) perform the same as the builtin Win32 API
    {
        size_t                    tstCharCnt = *outCharCnt;
        SmallStackBuffer<wchar_t> tstBuf (*outCharCnt);
        Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapToUNICODE (inMBChars, inMBCharCnt, tstBuf, &tstCharCnt);
        Assert (tstCharCnt == *outCharCnt);
        Assert (memcmp (tstBuf, outChars, sizeof (wchar_t) * tstCharCnt) == 0);
    }
#endif
}

void CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const
{
    // Not really right - but hopefully adquate for starters -- LGP 2011-09-06
    SmallStackBuffer<char16_t> tmpBuf (*outCharCnt);
    MapToUNICODE (inMBChars, inMBCharCnt, tmpBuf, outCharCnt);
    for (size_t i = 0; i < *outCharCnt; ++i) {
        outChars[i] = tmpBuf[i];
    }
}

void CodePageConverter::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    Require (inCharCnt == 0 or inChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);

    [[maybe_unused]] size_t outBufferSize = *outCharCnt;
#if qDebug && qPlatform_Windows
    size_t countOfBOMCharsAdded = 0; // just for the Windows debug check at the end
#endif

    switch (fCodePage) {
        case kCodePage_ANSI:
            TableDrivenCodePageConverter_<kCodePage_ANSI>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_MAC:
            TableDrivenCodePageConverter_<kCodePage_MAC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_PC:
            TableDrivenCodePageConverter_<kCodePage_PC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_PCA:
            TableDrivenCodePageConverter_<kCodePage_PCA>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case kCodePage_UNICODE_WIDE: {
            if (*outCharCnt >= 2) {
                wchar_t* outWBytes    = reinterpret_cast<wchar_t*> (outChars);
                size_t   outByteCount = (inCharCnt * sizeof (wchar_t));
                if (GetHandleBOM ()) {
                    outWBytes++;       // skip BOM
                    outByteCount -= 2; // subtract for BOM
                }
                *outCharCnt = outByteCount;
                if (GetHandleBOM ()) {
                    outChars[0] = '\xff';
                    outChars[1] = '\xfe';
#if qDebug && qPlatform_Windows
                    countOfBOMCharsAdded = 2;
#endif
                }
                (void)::memcpy (outWBytes, inChars, inCharCnt * sizeof (wchar_t));
            }
            else {
                *outCharCnt = 0;
            }
        } break;
        case kCodePage_UNICODE_WIDE_BIGENDIAN: {
            if (*outCharCnt >= 2) {
                wchar_t* outWBytes    = reinterpret_cast<wchar_t*> (outChars);
                size_t   outByteCount = (inCharCnt * sizeof (wchar_t));
                if (GetHandleBOM ()) {
                    outWBytes++;       // skip BOM
                    outByteCount -= 2; // subtract for BOM
                }
                *outCharCnt = outByteCount;
                if (GetHandleBOM ()) {
                    outChars[0] = '\xfe';
                    outChars[1] = '\xff';
#if qDebug && qPlatform_Windows
                    countOfBOMCharsAdded = 2;
#endif
                }
                for (size_t i = 0; i < inCharCnt; ++i) {
                    wchar_t c = inChars[i];
                    // byteswap
                    c            = ((c & 0xff) << 8) + (c >> 8);
                    outWBytes[i] = c;
                }
            }
            else {
                *outCharCnt = 0;
            }
        } break;
#if qPlatform_Windows
        case kCodePage_UTF7: {
            char*  useOutChars     = outChars;
            size_t useOutCharCount = *outCharCnt;
            if (GetHandleBOM ()) {
                if (*outCharCnt >= 5) {
                    useOutChars += 5; // skip BOM
                    useOutCharCount -= 5;
                    outChars[0] = 0x2b;
                    outChars[1] = 0x2f;
                    outChars[2] = 0x76;
                    outChars[3] = 0x38;
                    outChars[4] = 0x2d;
#if qDebug && qPlatform_Windows
                    countOfBOMCharsAdded = 5;
#endif
                }
                else {
                    useOutCharCount = 0;
                }
            }
            Characters::Platform::Windows::PlatformCodePageConverter (kCodePage_UTF7).MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt, useOutChars, &useOutCharCount);
            if (GetHandleBOM ()) {
                if (*outCharCnt >= 5) {
                    useOutCharCount += 5;
                }
            }
            *outCharCnt = useOutCharCount;
        } break;
#endif
        case kCodePage_UTF8: {
            char*  useOutChars     = outChars;
            size_t useOutCharCount = *outCharCnt;
            if (GetHandleBOM ()) {
                if (*outCharCnt >= 3) {
                    useOutChars += 3; // skip BOM
                    useOutCharCount -= 3;
                    reinterpret_cast<unsigned char*> (outChars)[0] = 0xef;
                    reinterpret_cast<unsigned char*> (outChars)[1] = 0xbb;
                    reinterpret_cast<unsigned char*> (outChars)[2] = 0xbf;
#if qDebug && qPlatform_Windows
                    countOfBOMCharsAdded = 3;
#endif
                }
                else {
                    useOutCharCount = 0;
                }
            }
            UTF8Converter ().MapFromUNICODE (inChars, inCharCnt, useOutChars, &useOutCharCount);
            if (GetHandleBOM ()) {
                if (*outCharCnt >= 3) {
                    useOutCharCount += 3;
                }
            }
            *outCharCnt = useOutCharCount;
        } break;
        default: {
#if qPlatform_Windows
            Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt, outChars, outCharCnt);
#else
            Execution::Throw (CodePageNotSupportedException (fCodePage));
#endif
        }
    }

#if qDebug && qPlatform_Windows
    // Assure my baked tables perform the same as the builtin Win32 API
    {
        size_t                 win32TstCharCnt = outBufferSize;
        SmallStackBuffer<char> win32TstBuf (win32TstCharCnt);

        Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt, win32TstBuf, &win32TstCharCnt);

        // SPR#0813 (and SPR#1277) - assert this produces the right result OR a '?' character -
        // used for bad conversions. Reason is cuz for characters that don't map - our table and
        // the system table can differ in how they map depending on current OS code page.
        Assert ((win32TstCharCnt + countOfBOMCharsAdded) == *outCharCnt or outChars[0] == '?');
        Assert (memcmp (win32TstBuf, outChars + countOfBOMCharsAdded, win32TstCharCnt) == 0 or outChars[0] == '?');
    }
#endif
}

void CodePageConverter::MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    // Not really right - but hopefully adquate for starters -- LGP 2011-09-06
    SmallStackBuffer<char16_t> tmpBuf (*outCharCnt);
    for (size_t i = 0; i < inCharCnt; ++i) {
        tmpBuf[i] = inChars[i];
    }
    MapFromUNICODE (tmpBuf, inCharCnt, outChars, outCharCnt);
}

/*
 ********************************************************************************
 ***************** TableDrivenCodePageConverter_<kCodePage_ANSI> ****************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_ANSI>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0x20ac,
    0x81,
    0x201a,
    0x192,
    0x201e,
    0x2026,
    0x2020,
    0x2021,
    0x2c6,
    0x2030,
    0x160,
    0x2039,
    0x152,
    0x8d,
    0x17d,
    0x8f,
    0x90,
    0x2018,
    0x2019,
    0x201c,
    0x201d,
    0x2022,
    0x2013,
    0x2014,
    0x2dc,
    0x2122,
    0x161,
    0x203a,
    0x153,
    0x9d,
    0x17e,
    0x178,
    0xa0,
    0xa1,
    0xa2,
    0xa3,
    0xa4,
    0xa5,
    0xa6,
    0xa7,
    0xa8,
    0xa9,
    0xaa,
    0xab,
    0xac,
    0xad,
    0xae,
    0xaf,
    0xb0,
    0xb1,
    0xb2,
    0xb3,
    0xb4,
    0xb5,
    0xb6,
    0xb7,
    0xb8,
    0xb9,
    0xba,
    0xbb,
    0xbc,
    0xbd,
    0xbe,
    0xbf,
    0xc0,
    0xc1,
    0xc2,
    0xc3,
    0xc4,
    0xc5,
    0xc6,
    0xc7,
    0xc8,
    0xc9,
    0xca,
    0xcb,
    0xcc,
    0xcd,
    0xce,
    0xcf,
    0xd0,
    0xd1,
    0xd2,
    0xd3,
    0xd4,
    0xd5,
    0xd6,
    0xd7,
    0xd8,
    0xd9,
    0xda,
    0xdb,
    0xdc,
    0xdd,
    0xde,
    0xdf,
    0xe0,
    0xe1,
    0xe2,
    0xe3,
    0xe4,
    0xe5,
    0xe6,
    0xe7,
    0xe8,
    0xe9,
    0xea,
    0xeb,
    0xec,
    0xed,
    0xee,
    0xef,
    0xf0,
    0xf1,
    0xf2,
    0xf3,
    0xf4,
    0xf5,
    0xf6,
    0xf7,
    0xf8,
    0xf9,
    0xfa,
    0xfb,
    0xfc,
    0xfd,
    0xfe,
    0xff,
};

void TableDrivenCodePageConverter_<kCodePage_ANSI>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_ANSI>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter_<kCodePage_MAC> **************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_MAC>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0xc4,
    0xc5,
    0xc7,
    0xc9,
    0xd1,
    0xd6,
    0xdc,
    0xe1,
    0xe0,
    0xe2,
    0xe4,
    0xe3,
    0xe5,
    0xe7,
    0xe9,
    0xe8,
    0xea,
    0xeb,
    0xed,
    0xec,
    0xee,
    0xef,
    0xf1,
    0xf3,
    0xf2,
    0xf4,
    0xf6,
    0xf5,
    0xfa,
    0xf9,
    0xfb,
    0xfc,
    0x2020,
    0xb0,
    0xa2,
    0xa3,
    0xa7,
    0x2022,
    0xb6,
    0xdf,
    0xae,
    0xa9,
    0x2122,
    0xb4,
    0xa8,
    0x2260,
    0xc6,
    0xd8,
    0x221e,
    0xb1,
    0x2264,
    0x2265,
    0xa5,
    0xb5,
    0x2202,
    0x2211,
    0x220f,
    0x3c0,
    0x222b,
    0xaa,
    0xba,
    0x2126,
    0xe6,
    0xf8,
    0xbf,
    0xa1,
    0xac,
    0x221a,
    0x192,
    0x2248,
    0x2206,
    0xab,
    0xbb,
    0x2026,
    0xa0,
    0xc0,
    0xc3,
    0xd5,
    0x152,
    0x153,
    0x2013,
    0x2014,
    0x201c,
    0x201d,
    0x2018,
    0x2019,
    0xf7,
    0x25ca,
    0xff,
    0x178,
    0x2044,
    0x20ac,
    0x2039,
    0x203a,
    0xfb01,
    0xfb02,
    0x2021,
    0xb7,
    0x201a,
    0x201e,
    0x2030,
    0xc2,
    0xca,
    0xc1,
    0xcb,
    0xc8,
    0xcd,
    0xce,
    0xcf,
    0xcc,
    0xd3,
    0xd4,
    0xf8ff,
    0xd2,
    0xda,
    0xdb,
    0xd9,
    0x131,
    0x2c6,
    0x2dc,
    0xaf,
    0x2d8,
    0x2d9,
    0x2da,
    0xb8,
    0x2dd,
    0x2db,
    0x2c7,
};

void TableDrivenCodePageConverter_<kCodePage_MAC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_MAC>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter_<kCodePage_PC> ***************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_PC>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0xc7,
    0xfc,
    0xe9,
    0xe2,
    0xe4,
    0xe0,
    0xe5,
    0xe7,
    0xea,
    0xeb,
    0xe8,
    0xef,
    0xee,
    0xec,
    0xc4,
    0xc5,
    0xc9,
    0xe6,
    0xc6,
    0xf4,
    0xf6,
    0xf2,
    0xfb,
    0xf9,
    0xff,
    0xd6,
    0xdc,
    0xa2,
    0xa3,
    0xa5,
    0x20a7,
    0x192,
    0xe1,
    0xed,
    0xf3,
    0xfa,
    0xf1,
    0xd1,
    0xaa,
    0xba,
    0xbf,
    0x2310,
    0xac,
    0xbd,
    0xbc,
    0xa1,
    0xab,
    0xbb,
    0x2591,
    0x2592,
    0x2593,
    0x2502,
    0x2524,
    0x2561,
    0x2562,
    0x2556,
    0x2555,
    0x2563,
    0x2551,
    0x2557,
    0x255d,
    0x255c,
    0x255b,
    0x2510,
    0x2514,
    0x2534,
    0x252c,
    0x251c,
    0x2500,
    0x253c,
    0x255e,
    0x255f,
    0x255a,
    0x2554,
    0x2569,
    0x2566,
    0x2560,
    0x2550,
    0x256c,
    0x2567,
    0x2568,
    0x2564,
    0x2565,
    0x2559,
    0x2558,
    0x2552,
    0x2553,
    0x256b,
    0x256a,
    0x2518,
    0x250c,
    0x2588,
    0x2584,
    0x258c,
    0x2590,
    0x2580,
    0x3b1,
    0xdf,
    0x393,
    0x3c0,
    0x3a3,
    0x3c3,
    0xb5,
    0x3c4,
    0x3a6,
    0x398,
    0x3a9,
    0x3b4,
    0x221e,
    0x3c6,
    0x3b5,
    0x2229,
    0x2261,
    0xb1,
    0x2265,
    0x2264,
    0x2320,
    0x2321,
    0xf7,
    0x2248,
    0xb0,
    0x2219,
    0xb7,
    0x221a,
    0x207f,
    0xb2,
    0x25a0,
    0xa0,
};

void TableDrivenCodePageConverter_<kCodePage_PC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_PC>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter_<kCodePage_PCA> **************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_PCA>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0xc7,
    0xfc,
    0xe9,
    0xe2,
    0xe4,
    0xe0,
    0xe5,
    0xe7,
    0xea,
    0xeb,
    0xe8,
    0xef,
    0xee,
    0xec,
    0xc4,
    0xc5,
    0xc9,
    0xe6,
    0xc6,
    0xf4,
    0xf6,
    0xf2,
    0xfb,
    0xf9,
    0xff,
    0xd6,
    0xdc,
    0xf8,
    0xa3,
    0xd8,
    0xd7,
    0x192,
    0xe1,
    0xed,
    0xf3,
    0xfa,
    0xf1,
    0xd1,
    0xaa,
    0xba,
    0xbf,
    0xae,
    0xac,
    0xbd,
    0xbc,
    0xa1,
    0xab,
    0xbb,
    0x2591,
    0x2592,
    0x2593,
    0x2502,
    0x2524,
    0xc1,
    0xc2,
    0xc0,
    0xa9,
    0x2563,
    0x2551,
    0x2557,
    0x255d,
    0xa2,
    0xa5,
    0x2510,
    0x2514,
    0x2534,
    0x252c,
    0x251c,
    0x2500,
    0x253c,
    0xe3,
    0xc3,
    0x255a,
    0x2554,
    0x2569,
    0x2566,
    0x2560,
    0x2550,
    0x256c,
    0xa4,
    0xf0,
    0xd0,
    0xca,
    0xcb,
    0xc8,
    0x131,
    0xcd,
    0xce,
    0xcf,
    0x2518,
    0x250c,
    0x2588,
    0x2584,
    0xa6,
    0xcc,
    0x2580,
    0xd3,
    0xdf,
    0xd4,
    0xd2,
    0xf5,
    0xd5,
    0xb5,
    0xfe,
    0xde,
    0xda,
    0xdb,
    0xd9,
    0xfd,
    0xdd,
    0xaf,
    0xb4,
    0xad,
    0xb1,
    0x2017,
    0xbe,
    0xb6,
    0xa7,
    0xf7,
    0xb8,
    0xb0,
    0xa8,
    0xb7,
    0xb9,
    0xb3,
    0xb2,
    0x25a0,
    0xa0,
};

void TableDrivenCodePageConverter_<kCodePage_PCA>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_PCA>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 ******************* TableDrivenCodePageConverter_<kCodePage_GREEK> *************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_GREEK>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0x20ac,
    0x81,
    0x201a,
    0x192,
    0x201e,
    0x2026,
    0x2020,
    0x2021,
    0x88,
    0x2030,
    0x8a,
    0x2039,
    0x8c,
    0x8d,
    0x8e,
    0x8f,
    0x90,
    0x2018,
    0x2019,
    0x201c,
    0x201d,
    0x2022,
    0x2013,
    0x2014,
    0x98,
    0x2122,
    0x9a,
    0x203a,
    0x9c,
    0x9d,
    0x9e,
    0x9f,
    0xa0,
    0x385,
    0x386,
    0xa3,
    0xa4,
    0xa5,
    0xa6,
    0xa7,
    0xa8,
    0xa9,
    0xf8f9,
    0xab,
    0xac,
    0xad,
    0xae,
    0x2015,
    0xb0,
    0xb1,
    0xb2,
    0xb3,
    0x384,
    0xb5,
    0xb6,
    0xb7,
    0x388,
    0x389,
    0x38a,
    0xbb,
    0x38c,
    0xbd,
    0x38e,
    0x38f,
    0x390,
    0x391,
    0x392,
    0x393,
    0x394,
    0x395,
    0x396,
    0x397,
    0x398,
    0x399,
    0x39a,
    0x39b,
    0x39c,
    0x39d,
    0x39e,
    0x39f,
    0x3a0,
    0x3a1,
    0xf8fa,
    0x3a3,
    0x3a4,
    0x3a5,
    0x3a6,
    0x3a7,
    0x3a8,
    0x3a9,
    0x3aa,
    0x3ab,
    0x3ac,
    0x3ad,
    0x3ae,
    0x3af,
    0x3b0,
    0x3b1,
    0x3b2,
    0x3b3,
    0x3b4,
    0x3b5,
    0x3b6,
    0x3b7,
    0x3b8,
    0x3b9,
    0x3ba,
    0x3bb,
    0x3bc,
    0x3bd,
    0x3be,
    0x3bf,
    0x3c0,
    0x3c1,
    0x3c2,
    0x3c3,
    0x3c4,
    0x3c5,
    0x3c6,
    0x3c7,
    0x3c8,
    0x3c9,
    0x3ca,
    0x3cb,
    0x3cc,
    0x3cd,
    0x3ce,
    0xf8fb,
};

void TableDrivenCodePageConverter_<kCodePage_GREEK>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_GREEK>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 **************** TableDrivenCodePageConverter_<kCodePage_Turkish> ***************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_Turkish>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0x20ac,
    0x81,
    0x201a,
    0x192,
    0x201e,
    0x2026,
    0x2020,
    0x2021,
    0x2c6,
    0x2030,
    0x160,
    0x2039,
    0x152,
    0x8d,
    0x8e,
    0x8f,
    0x90,
    0x2018,
    0x2019,
    0x201c,
    0x201d,
    0x2022,
    0x2013,
    0x2014,
    0x2dc,
    0x2122,
    0x161,
    0x203a,
    0x153,
    0x9d,
    0x9e,
    0x178,
    0xa0,
    0xa1,
    0xa2,
    0xa3,
    0xa4,
    0xa5,
    0xa6,
    0xa7,
    0xa8,
    0xa9,
    0xaa,
    0xab,
    0xac,
    0xad,
    0xae,
    0xaf,
    0xb0,
    0xb1,
    0xb2,
    0xb3,
    0xb4,
    0xb5,
    0xb6,
    0xb7,
    0xb8,
    0xb9,
    0xba,
    0xbb,
    0xbc,
    0xbd,
    0xbe,
    0xbf,
    0xc0,
    0xc1,
    0xc2,
    0xc3,
    0xc4,
    0xc5,
    0xc6,
    0xc7,
    0xc8,
    0xc9,
    0xca,
    0xcb,
    0xcc,
    0xcd,
    0xce,
    0xcf,
    0x11e,
    0xd1,
    0xd2,
    0xd3,
    0xd4,
    0xd5,
    0xd6,
    0xd7,
    0xd8,
    0xd9,
    0xda,
    0xdb,
    0xdc,
    0x130,
    0x15e,
    0xdf,
    0xe0,
    0xe1,
    0xe2,
    0xe3,
    0xe4,
    0xe5,
    0xe6,
    0xe7,
    0xe8,
    0xe9,
    0xea,
    0xeb,
    0xec,
    0xed,
    0xee,
    0xef,
    0x11f,
    0xf1,
    0xf2,
    0xf3,
    0xf4,
    0xf5,
    0xf6,
    0xf7,
    0xf8,
    0xf9,
    0xfa,
    0xfb,
    0xfc,
    0x131,
    0x15f,
    0xff,
};

void TableDrivenCodePageConverter_<kCodePage_Turkish>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_Turkish>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 **************** TableDrivenCodePageConverter_<kCodePage_HEBREW> ***************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_HEBREW>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0x20ac,
    0x81,
    0x201a,
    0x192,
    0x201e,
    0x2026,
    0x2020,
    0x2021,
    0x2c6,
    0x2030,
    0x8a,
    0x2039,
    0x8c,
    0x8d,
    0x8e,
    0x8f,
    0x90,
    0x2018,
    0x2019,
    0x201c,
    0x201d,
    0x2022,
    0x2013,
    0x2014,
    0x2dc,
    0x2122,
    0x9a,
    0x203a,
    0x9c,
    0x9d,
    0x9e,
    0x9f,
    0xa0,
    0xa1,
    0xa2,
    0xa3,
    0x20aa,
    0xa5,
    0xa6,
    0xa7,
    0xa8,
    0xa9,
    0xd7,
    0xab,
    0xac,
    0xad,
    0xae,
    0xaf,
    0xb0,
    0xb1,
    0xb2,
    0xb3,
    0xb4,
    0xb5,
    0xb6,
    0xb7,
    0xb8,
    0xb9,
    0xf7,
    0xbb,
    0xbc,
    0xbd,
    0xbe,
    0xbf,
    0x5b0,
    0x5b1,
    0x5b2,
    0x5b3,
    0x5b4,
    0x5b5,
    0x5b6,
    0x5b7,
    0x5b8,
    0x5b9,
    0x5ba,
    0x5bb,
    0x5bc,
    0x5bd,
    0x5be,
    0x5bf,
    0x5c0,
    0x5c1,
    0x5c2,
    0x5c3,
    0x5f0,
    0x5f1,
    0x5f2,
    0x5f3,
    0x5f4,
    0xf88d,
    0xf88e,
    0xf88f,
    0xf890,
    0xf891,
    0xf892,
    0xf893,
    0x5d0,
    0x5d1,
    0x5d2,
    0x5d3,
    0x5d4,
    0x5d5,
    0x5d6,
    0x5d7,
    0x5d8,
    0x5d9,
    0x5da,
    0x5db,
    0x5dc,
    0x5dd,
    0x5de,
    0x5df,
    0x5e0,
    0x5e1,
    0x5e2,
    0x5e3,
    0x5e4,
    0x5e5,
    0x5e6,
    0x5e7,
    0x5e8,
    0x5e9,
    0x5ea,
    0xf894,
    0xf895,
    0x200e,
    0x200f,
    0xf896,
};

void TableDrivenCodePageConverter_<kCodePage_HEBREW>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_HEBREW>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 ******************* TableDrivenCodePageConverter_<kCodePage_ARABIC> ************
 ********************************************************************************
 */
const char16_t TableDrivenCodePageConverter_<kCodePage_ARABIC>::kMap[256] = {
    0x0,
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8,
    0x9,
    0xa,
    0xb,
    0xc,
    0xd,
    0xe,
    0xf,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2a,
    0x2b,
    0x2c,
    0x2d,
    0x2e,
    0x2f,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    0x3a,
    0x3b,
    0x3c,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4a,
    0x4b,
    0x4c,
    0x4d,
    0x4e,
    0x4f,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5a,
    0x5b,
    0x5c,
    0x5d,
    0x5e,
    0x5f,
    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6a,
    0x6b,
    0x6c,
    0x6d,
    0x6e,
    0x6f,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7a,
    0x7b,
    0x7c,
    0x7d,
    0x7e,
    0x7f,
    0x20ac,
    0x67e,
    0x201a,
    0x192,
    0x201e,
    0x2026,
    0x2020,
    0x2021,
    0x2c6,
    0x2030,
    0x679,
    0x2039,
    0x152,
    0x686,
    0x698,
    0x688,
    0x6af,
    0x2018,
    0x2019,
    0x201c,
    0x201d,
    0x2022,
    0x2013,
    0x2014,
    0x6a9,
    0x2122,
    0x691,
    0x203a,
    0x153,
    0x200c,
    0x200d,
    0x6ba,
    0xa0,
    0x60c,
    0xa2,
    0xa3,
    0xa4,
    0xa5,
    0xa6,
    0xa7,
    0xa8,
    0xa9,
    0x6be,
    0xab,
    0xac,
    0xad,
    0xae,
    0xaf,
    0xb0,
    0xb1,
    0xb2,
    0xb3,
    0xb4,
    0xb5,
    0xb6,
    0xb7,
    0xb8,
    0xb9,
    0x61b,
    0xbb,
    0xbc,
    0xbd,
    0xbe,
    0x61f,
    0x6c1,
    0x621,
    0x622,
    0x623,
    0x624,
    0x625,
    0x626,
    0x627,
    0x628,
    0x629,
    0x62a,
    0x62b,
    0x62c,
    0x62d,
    0x62e,
    0x62f,
    0x630,
    0x631,
    0x632,
    0x633,
    0x634,
    0x635,
    0x636,
    0xd7,
    0x637,
    0x638,
    0x639,
    0x63a,
    0x640,
    0x641,
    0x642,
    0x643,
    0xe0,
    0x644,
    0xe2,
    0x645,
    0x646,
    0x647,
    0x648,
    0xe7,
    0xe8,
    0xe9,
    0xea,
    0xeb,
    0x649,
    0x64a,
    0xee,
    0xef,
    0x64b,
    0x64c,
    0x64d,
    0x64e,
    0xf4,
    0x64f,
    0x650,
    0xf7,
    0x651,
    0xf9,
    0x652,
    0xfb,
    0xfc,
    0x200e,
    0x200f,
    0x6d2,
};

void TableDrivenCodePageConverter_<kCodePage_ARABIC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void TableDrivenCodePageConverter_<kCodePage_ARABIC>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}

/*
 ********************************************************************************
 ********************************** UTF8Converter *******************************
 ********************************************************************************
 */
void UTF8Converter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const
{
    Require (inMBCharCnt == 0 or inMBChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);
    using namespace ConvertUTF_;
    const UTF8*                       sourceStart = reinterpret_cast<const UTF8*> (inMBChars);
    const UTF8*                       sourceEnd   = sourceStart + inMBCharCnt;
    UTF16*                            targetStart = outChars;
    UTF16*                            targetEnd   = outChars + *outCharCnt;
    constexpr ConversionFlags         kFlag_      = lenientConversion;
    [[maybe_unused]] ConversionResult tmp         = ConvertUTF8toUTF16 (&sourceStart, sourceEnd, &targetStart, targetEnd, kFlag_);
    *outCharCnt                                   = targetStart - outChars;
}

void UTF8Converter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const
{
    using namespace ConvertUTF_;
    const UTF8*                       sourceStart = reinterpret_cast<const UTF8*> (inMBChars);
    const UTF8*                       sourceEnd   = sourceStart + inMBCharCnt;
    UTF32*                            targetStart = outChars;
    UTF32*                            targetEnd   = outChars + *outCharCnt;
    constexpr ConversionFlags         kFlag_      = lenientConversion;
    [[maybe_unused]] ConversionResult tmp         = ConvertUTF8toUTF32 (&sourceStart, sourceEnd, &targetStart, targetEnd, kFlag_);
    *outCharCnt                                   = targetStart - outChars;
}

void UTF8Converter::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    using namespace ConvertUTF_;
    const UTF16*                      sourceStart = reinterpret_cast<const UTF16*> (inChars);
    const UTF16*                      sourceEnd   = sourceStart + inCharCnt;
    UTF8*                             targetStart = reinterpret_cast<UTF8*> (outChars);
    UTF8*                             targetEnd   = targetStart + *outCharCnt;
    constexpr ConversionFlags         kFlag_      = lenientConversion;
    [[maybe_unused]] ConversionResult tmp         = ConvertUTF16toUTF8 (&sourceStart, sourceEnd, &targetStart, targetEnd, kFlag_);
    *outCharCnt                                   = targetStart - reinterpret_cast<UTF8*> (outChars);
}

void UTF8Converter::MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    using namespace ConvertUTF_;
    const UTF32*                      sourceStart = reinterpret_cast<const UTF32*> (inChars);
    const UTF32*                      sourceEnd   = sourceStart + inCharCnt;
    UTF8*                             targetStart = reinterpret_cast<UTF8*> (outChars);
    UTF8*                             targetEnd   = targetStart + *outCharCnt;
    constexpr ConversionFlags         kFlag_      = lenientConversion;
    [[maybe_unused]] ConversionResult tmp         = ConvertUTF32toUTF8 (&sourceStart, sourceEnd, &targetStart, targetEnd, kFlag_);
    *outCharCnt                                   = targetStart - reinterpret_cast<UTF8*> (outChars);
}

/*
 ********************************************************************************
 ********************************** CodePagesInstalled **************************
 ********************************************************************************
 */
namespace {
#if qPlatform_Windows
    shared_ptr<set<CodePage>> s_EnumCodePagesProc_Accumulator_;
    BOOL FAR PASCAL EnumCodePagesProc_ (LPTSTR lpCodePageString)
    {
        s_EnumCodePagesProc_Accumulator_->insert (_ttoi (lpCodePageString));
        return (1);
    }
#endif
}

CodePagesInstalled::CodePagesInstalled ()
    : fCodePages_ ()
{
    Assert (fCodePages_.size () == 0);

    shared_ptr<set<CodePage>> accum = make_shared<set<CodePage>> ();
#if qPlatform_Windows
    static mutex sCritSec_;
    {
        [[maybe_unused]] auto&& critSec = lock_guard{sCritSec_};
        Assert (s_EnumCodePagesProc_Accumulator_.get () == nullptr);
        s_EnumCodePagesProc_Accumulator_ = accum;
        ::EnumSystemCodePages (EnumCodePagesProc_, CP_INSTALLED);
        s_EnumCodePagesProc_Accumulator_.reset ();
    }
#endif
    // Add these 'fake' code pages - which I believe are always available, but never listed by EnumSystemCodePages()
    accum->insert (kCodePage_UNICODE_WIDE);
    accum->insert (kCodePage_UNICODE_WIDE_BIGENDIAN);
    accum->insert (kCodePage_UTF8);
    fCodePages_ = vector<CodePage> (accum->begin (), accum->end ());
}

/*
 ********************************************************************************
 ********************************** CodePagesGuesser ****************************
 ********************************************************************************
 */
CodePage CodePagesGuesser::Guess (const void* input, size_t nBytes, Confidence* confidence, size_t* bytesFromFrontToStrip)
{
    if (confidence != nullptr) {
        *confidence = Confidence::eLow;
    }
    if (bytesFromFrontToStrip != nullptr) {
        *bytesFromFrontToStrip = 0;
    }
    if (nBytes >= 2) {
        unsigned char c0 = reinterpret_cast<const unsigned char*> (input)[0];
        unsigned char c1 = reinterpret_cast<const unsigned char*> (input)[1];
        if (c0 == 0xff and c1 == 0xfe) {
            if (confidence != nullptr) {
                *confidence = Confidence::eHigh;
            }
            if (bytesFromFrontToStrip != nullptr) {
                *bytesFromFrontToStrip = 2;
            }
            return kCodePage_UNICODE_WIDE;
        }
        if (c0 == 0xfe and c1 == 0xff) {
            if (confidence != nullptr) {
                *confidence = Confidence::eHigh;
            }
            if (bytesFromFrontToStrip != nullptr) {
                *bytesFromFrontToStrip = 2;
            }
            return kCodePage_UNICODE_WIDE_BIGENDIAN;
        }
        if (nBytes >= 3) {
            unsigned char c2 = reinterpret_cast<const unsigned char*> (input)[2];
            if (c0 == 0xef and c1 == 0xbb and c2 == 0xbf) {
                if (confidence != nullptr) {
                    *confidence = Confidence::eHigh;
                }
                if (bytesFromFrontToStrip != nullptr) {
                    *bytesFromFrontToStrip = 3;
                }
                return kCodePage_UTF8;
            }
        }
        if (nBytes >= 5) {
            unsigned char c2 = reinterpret_cast<const unsigned char*> (input)[2];
            unsigned char c3 = reinterpret_cast<const unsigned char*> (input)[3];
            unsigned char c4 = reinterpret_cast<const unsigned char*> (input)[4];
            if (c0 == 0x2b and c1 == 0x2f and c2 == 0x76 and c3 == 0x38 and c4 == 0x2d) {
                if (confidence != nullptr) {
                    *confidence = Confidence::eHigh;
                }
                if (bytesFromFrontToStrip != nullptr) {
                    *bytesFromFrontToStrip = 5;
                }
                return kCodePage_UTF7;
            }
        }
    }

    /*
     * Final ditch efforts if we don't recognize any prefix.
     */
    if (confidence != nullptr) {
        *confidence = Confidence::eLow;
    }
    return Characters::GetDefaultSDKCodePage ();
}

/*
 ********************************************************************************
 ****************************** CodePagePrettyNameMapper ************************
 ********************************************************************************
 */
CodePagePrettyNameMapper::CodePageNames CodePagePrettyNameMapper::sCodePageNames_ = CodePagePrettyNameMapper::MakeDefaultCodePageNames ();

CodePagePrettyNameMapper::CodePageNames CodePagePrettyNameMapper::MakeDefaultCodePageNames ()
{
    CodePageNames codePageNames;
    codePageNames.fUNICODE_WIDE           = L"UNICODE {wide characters}";
    codePageNames.fUNICODE_WIDE_BIGENDIAN = L"UNICODE {wide characters - big endian}";
    codePageNames.fANSI                   = L"ANSI (1252)";
    codePageNames.fMAC                    = L"MAC (2)";
    codePageNames.fPC                     = L"IBM PC United States code page (437)";
    codePageNames.fSJIS                   = L"Japanese SJIS {932}";
    codePageNames.fUTF7                   = L"UNICODE {UTF-7}";
    codePageNames.fUTF8                   = L"UNICODE {UTF-8}";
    codePageNames.f850                    = L"Latin I - MS-DOS Multilingual (850)";
    codePageNames.f851                    = L"Latin II - MS-DOS Slavic (850)";
    codePageNames.f866                    = L"Russian - MS-DOS (866)";
    codePageNames.f936                    = L"Chinese {Simplfied} (936)";
    codePageNames.f949                    = L"Korean (949)";
    codePageNames.f950                    = L"Chinese {Traditional} (950)";
    codePageNames.f1250                   = L"Eastern European Windows (1250)";
    codePageNames.f1251                   = L"Cyrilic (1251)";
    codePageNames.f10000                  = L"Roman {Macintosh} (10000)";
    codePageNames.f10001                  = L"Japanese {Macintosh} (10001)";
    codePageNames.f50220                  = L"Japanese JIS (50220)";
    return codePageNames;
}

wstring CodePagePrettyNameMapper::GetName (CodePage cp)
{
    switch (cp) {
        case kCodePage_UNICODE_WIDE:
            return sCodePageNames_.fUNICODE_WIDE;
        case kCodePage_UNICODE_WIDE_BIGENDIAN:
            return sCodePageNames_.fUNICODE_WIDE_BIGENDIAN;
        case kCodePage_ANSI:
            return sCodePageNames_.fANSI;
        case kCodePage_MAC:
            return sCodePageNames_.fMAC;
        case kCodePage_PC:
            return sCodePageNames_.fPC;
        case kCodePage_SJIS:
            return sCodePageNames_.fSJIS;
        case kCodePage_UTF7:
            return sCodePageNames_.fUTF7;
        case kCodePage_UTF8:
            return sCodePageNames_.fUTF8;
        case 850:
            return sCodePageNames_.f850;
        case 851:
            return sCodePageNames_.f851;
        case 866:
            return sCodePageNames_.f866;
        case 936:
            return sCodePageNames_.f936;
        case 949:
            return sCodePageNames_.f949;
        case 950:
            return sCodePageNames_.f950;
        case 1250:
            return sCodePageNames_.f1250;
        case 1251:
            return sCodePageNames_.f1251;
        case 10000:
            return sCodePageNames_.f10000;
        case 10001:
            return sCodePageNames_.f10001;
        case 50220:
            return sCodePageNames_.f50220;
        default: {
            return Characters::CString::Format (L"%d", cp);
        }
    }
}

/*
 ********************************************************************************
 *********************** Characters::WideStringToNarrow *************************
 ********************************************************************************
 */
namespace {
    void PortableWideStringToNarrow_ (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
    {
        RequireNotNull (intoResult);
        Require (wsStart <= wsEnd);
        size_t            inSize = wsEnd - wsStart;
        CodePageConverter cc (codePage);
        // this grossly overestimates size - which is a problem for the RESIZE below!!! COULD pointlessly run out of memroy and intitialize data to good values...
        size_t outSizeBuf = cc.MapFromUNICODE_QuickComputeOutBufSize (wsStart, inSize);
        intoResult->resize (outSizeBuf);
        size_t actualOutSize = 0;
        if (inSize != 0) {
            actualOutSize = outSizeBuf;
            cc.MapFromUNICODE (wsStart, inSize, Containers::Start (*intoResult), &actualOutSize);
            if (intoResult->size () != actualOutSize) {
                // shrink
                Assert (intoResult->size () > actualOutSize);
                intoResult->resize (actualOutSize);
            }
        }
    }
}
void Characters::WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
{
    RequireNotNull (intoResult);
    Require (wsStart <= wsEnd);

#if qPlatform_Windows
    Platform::Windows::WideStringToNarrow (wsStart, wsEnd, codePage, intoResult);
#else
    PortableWideStringToNarrow_ (wsStart, wsEnd, codePage, intoResult);
#endif
}

/*
 ********************************************************************************
 *********************** Characters::NarrowStringToWide *************************
 ********************************************************************************
 */
namespace {
    void PortableNarrowStringToWide_ (const char* sStart, const char* sEnd, int codePage, wstring* intoResult)
    {
        RequireNotNull (intoResult);
        Require (sStart <= sEnd);
        size_t            inSize = sEnd - sStart;
        CodePageConverter cc (codePage);
        // this grossly overestimates size - which is a problem for the RESIZE below!!! COULD pointlessly run out of memroy and intitialize data to good values...
        size_t outSizeBuf = cc.MapToUNICODE_QuickComputeOutBufSize (sStart, inSize);
        intoResult->resize (outSizeBuf);
        size_t actualOutSize = 0;
        if (inSize != 0) {
            actualOutSize = outSizeBuf;
            cc.MapToUNICODE (sStart, inSize, Containers::Start (*intoResult), &actualOutSize);
            if (intoResult->size () != actualOutSize) {
                // shrink
                Assert (intoResult->size () > actualOutSize);
                intoResult->resize (actualOutSize);
            }
        }
    }
}
void Characters::NarrowStringToWide (const char* sStart, const char* sEnd, int codePage, wstring* intoResult)
{
    RequireNotNull (intoResult);
    Require (sStart <= sEnd);
#if qPlatform_Windows
    Platform::Windows::NarrowStringToWide (sStart, sEnd, codePage, intoResult);
#else
    PortableNarrowStringToWide_ (sStart, sEnd, codePage, intoResult);
#endif
}

/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
wstring Characters::MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end)
{
    Require (start <= end);
    if (start == end) {
        return wstring ();
    }
    else {
        size_t                    outBufSize = end - start;
        SmallStackBuffer<wchar_t> wideBuf (outBufSize);
        size_t                    outCharCount = outBufSize;
        MapSBUnicodeTextWithMaybeBOMToUNICODE (start, end - start, wideBuf, &outCharCount);
        Assert (outCharCount <= outBufSize);
        if (outCharCount == 0) {
            return wstring ();
        }

        // The wideBuf may be NUL-terminated or not (depending on whether the input was NUL-terminated or not).
        // Be sure to construct the resuting string with the right end-of-string pointer (the length doesn't include
        // the NUL-char)
        return wstring (wideBuf, wideBuf[outCharCount - 1] == '\0' ? (outCharCount - 1) : outCharCount);
    }
}

/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
vector<Byte> Characters::MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP)
{
    CodePageConverter      cpc (useCP, CodePageConverter::eHandleBOM);
    size_t                 outCharCount = cpc.MapFromUNICODE_QuickComputeOutBufSize (start, end - start);
    SmallStackBuffer<char> buf (outCharCount);
    cpc.MapFromUNICODE (start, end - start, buf, &outCharCount);
    const Byte* bs = reinterpret_cast<const Byte*> (static_cast<const char*> (buf));
    return vector<Byte> (bs, bs + outCharCount);
}

/*
 ********************************************************************************
 *********************** Characters::LookupCodeConverter ************************
 ********************************************************************************
 */
namespace {
    // From https://en.wikipedia.org/wiki/ISO/IEC_8859-1 - "ISO-8859-1 was incorporated as the first 256 code points of ISO/IEC 10646 and Unicode.
    struct codecvt_iso10646_ : std::codecvt<wchar_t, char, std::mbstate_t> {
        virtual result do_in (std::mbstate_t& _State, const char* _First1, const char* _Last1, const char*& _Mid1, wchar_t* _First2, wchar_t* _Last2, wchar_t*& _Mid2) const
        {
            // Convert 'bytes' to wchar_t using utf8 converter. Since first 256 code points the same, valid ISO-8859-1 will map to the right unicode
            // @todo - trim badly converted bytes (>256) to errors
            result tmp = kUTF82wchar_tConverter_.in (_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            // @see https://stroika.atlassian.net/browse/STK-274
            if (tmp == error) {
                tmp = ok;
            }
            return tmp;
        }
        virtual result do_out (std::mbstate_t& _State, const wchar_t* _First1, const wchar_t* _Last1, const wchar_t*& _Mid1, char* _First2, char* _Last2, char*& _Mid2) const
        {
            result tmp = kUTF82wchar_tConverter_.out (_State, _First1, _Last1, _Mid1, _First2, _Last2, _Mid2);
            // @see https://stroika.atlassian.net/browse/STK-274
            if (tmp == error) {
                tmp = ok;
            }
            return tmp;
        }
        static codecvt_utf8<wchar_t> kUTF82wchar_tConverter_;
    };
    codecvt_utf8<wchar_t> codecvt_iso10646_::kUTF82wchar_tConverter_;
}
namespace Stroika {
    namespace Foundation {
        namespace Characters {
            template <>
            const std::codecvt<wchar_t, char, std::mbstate_t>& LookupCodeConverter (const String& charset)
            {
                // https://svn.apache.org/repos/asf/stdcxx/trunk/examples/include/codecvte.h almost works for ISO 8859-1 but I cannot use it (license)
                if (charset.Equals (L"utf-8", CompareOptions::eCaseInsensitive)) {
                    static const codecvt_utf8<wchar_t> kConverter_; // safe to keep static because only read-only const methods used
                    return kConverter_;
                }
                else if (charset.Equals (L"ISO-8859-1", CompareOptions::eCaseInsensitive)) {
                    static const codecvt_iso10646_ kConverter_; // safe to keep static because only read-only const methods used
                    return kConverter_;
                }
                Execution::Throw (Execution::StringException (L"charset not supported"));
            }
        }
    }
}