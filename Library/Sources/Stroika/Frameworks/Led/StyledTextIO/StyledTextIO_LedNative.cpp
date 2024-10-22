/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <cctype>
#include <climits>
#include <cstdio> // for a couple sprintf() calls - could pretty easily be avoided

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/LineEndings.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "Stroika/Frameworks/Led/StyledTextEmbeddedObjects.h"

#include "StyledTextIO_LedNative.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

using Memory::StackBuffer;

namespace {
    inline void OutputStandardToSinkStream_size_t_ (StyledTextIOWriter::SinkStream& sinkStream, size_t n)
    {
        uint32_t buf;
        SizeTToBuf (n, &buf);
        sinkStream.write (&buf, sizeof (buf));
    }
    inline unsigned long InputStandardFromSrcStream_ULONG (StyledTextIOReader::SrcStream& srcStream)
    {
        uint32_t buf;
        if (srcStream.read (&buf, sizeof (buf)) != sizeof (buf)) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        return (BufToUInt32 (&buf));
    }
}

/*
 * Basic file format is:
 *      MAGIC COOKIE
 *      LENGTH OF TEXT (ULONG)
 *      TEXT (variable length)
 *      LENGTH OF STYLE RUN LIST (number of elemenbts - stored as ULONG)
 *      Actual Style run elements, stored as PortableStyleRunData
 *      MAGIC COOKIE - AGAIN - JUST FOR GOOD LUCK!!!
 */
using LedFormatMagicCookie                                     = char[16];
const LedFormatMagicCookie kLedPartFormatVersion_4_MagicNumber = "\01Version4"; // always keep this magic cookie the same size
const LedFormatMagicCookie kLedPartFormatVersion_5_MagicNumber = "\01Version5"; // always keep this magic cookie the same size
const LedFormatMagicCookie kLedPartFormatVersion_6_MagicNumber = "\01Version6"; // always keep this magic cookie the same size

/*
 *  Version 4 font style info file format.
 */
struct PortableStyleRunData_Version4 {
    char          fFontName[256];
    unsigned char fItalic;
    unsigned char fBoldWeight; // use Windows definition here/4...
    enum {
        eBoldnessNormal = 400 / 4,
        eBoldnessBold   = 700 / 4
    };
    uint8_t  fUnderline;
    uint8_t  fUnused1_; // so we get good cross-platform / cross-compiler alignment
    uint16_t fFontSize; // note this size is a POINT-SIZE (not the tmHeight)
    uint16_t fUnused2_;
    uint32_t fLength;
};
inline void _DO_ALIGN_ASSERTS_Version4_ ()
{
    Assert (sizeof (PortableStyleRunData_Version4) == 268);
    Assert (offsetof (PortableStyleRunData_Version4, fItalic) == 256);
    Assert (offsetof (PortableStyleRunData_Version4, fBoldWeight) == 257);
    Assert (offsetof (PortableStyleRunData_Version4, fUnderline) == 258);
    Assert (offsetof (PortableStyleRunData_Version4, fFontSize) == 260);
    Assert (offsetof (PortableStyleRunData_Version4, fLength) == 264);
}
inline StyledInfoSummaryRecord mkInfoSummaryRecordFromPortData (const PortableStyleRunData_Version4& srcData)
{
    _DO_ALIGN_ASSERTS_Version4_ ();
    FontSpecification fsp;
    fsp.SetStyle_Plain (); // so all style valid bits set...
    fsp.SetStyle_Bold (srcData.fBoldWeight >= PortableStyleRunData_Version4::eBoldnessBold);
    fsp.SetStyle_Italic (!!srcData.fItalic);
    fsp.SetStyle_Underline (!!srcData.fUnderline);
    short fontSize = BufToUInt16 (&srcData.fFontSize);
    if (fontSize < 3 or fontSize > 128) {
        fontSize = 12; // a reasonable default for bogus/corrupt data
    }
    fsp.SetPointSize (fontSize);
    fsp.SetFontName (String::FromNarrowSDKString (srcData.fFontName).AsSDKString ());
    return (StyledInfoSummaryRecord (fsp, BufToUInt32 (&srcData.fLength)));
}

/*
 *  Version 5 font style info file format.
 *      <<Used for Led 2.1 release>>
 */
struct PortableStyleRunData_Version5 {
    unsigned char fThisRecordLength; // nbytes in this record - cuz fFontName len is variable...
    // note use of a char here effectively limits us to only font names of
    // a little over 200, but that should be more than enuf (limit on PC is 32,
    // and I don't know on mac).

    // Really wanted to just use bit-fields for these guys, but that is non-portable.
    // Sigh... I did that first and the code looks much more readable without all those
    // damned shifts and ors etc... LGP 960531
    enum BitFlagStyles {
        eBold = 0,
        eItalic,
        eUnderline,
        eOutline,
        eShadow,
        eCondensed,
        eExtended,
        eStrikeout
    };
    unsigned char  fStyleSet;  // or in BitFlagStyles
    unsigned short fPointSize; // note this size is a POINT-SIZE (not the tmHeight)
    uint32_t       fLength;
    char fFontName[256]; // to file, we really only write as many bytes as needed (no NUL char TERM - infer size from fThisRecordLength)

    static size_t NameLenFromRecordLen (unsigned char len)
    {
        size_t xtra = offsetof (PortableStyleRunData_Version5, fFontName);
        Assert (len > xtra);
        return static_cast<unsigned char> (len - xtra);
    }
    static unsigned char RecordLenFromNameLen (size_t len)
    {
        if (len > 255) {
            Execution::Throw (Execution::Exception{L"RecordLenFromNameLen too long"sv});
        }
        size_t xtra = offsetof (PortableStyleRunData_Version5, fFontName);
        return (unsigned char)min (xtra + size_t (len), size_t (256));
    }
};
inline void _DO_ALIGN_ASSERTS_Version5_ ()
{
    Assert (offsetof (PortableStyleRunData_Version5, fThisRecordLength) == 0);
    Assert (offsetof (PortableStyleRunData_Version5, fStyleSet) == 1);
    Assert (offsetof (PortableStyleRunData_Version5, fPointSize) == 2);
    Assert (offsetof (PortableStyleRunData_Version5, fLength) == 4);
    Assert (offsetof (PortableStyleRunData_Version5, fFontName) == 8);
    //Assert (offsetof (PortableStyleRunData_Version5, fFontName) == 16);
    //Assert (sizeof (PortableStyleRunData_Version5) == 16+256);
    Assert (sizeof (PortableStyleRunData_Version5) == 8 + 256);
}
inline PortableStyleRunData_Version5 mkPortableStyleRunData_Version5 (const StyledInfoSummaryRecord& isr)
{
    _DO_ALIGN_ASSERTS_Version5_ ();
    PortableStyleRunData_Version5 data;
    (void)::memset (&data, 0, sizeof data); // A nice feature of data file formats, is that if you make no changes to the content
    // and write it again, they 'diff' equal. Even though this memset isn't needed for
    // accuracy, its nice todo anyhow, and pretty cheap - LGP 960531
    string fontName = String::FromSDKString (isr.GetFontName ()).AsNarrowSDKString ();
    Characters::CString::Copy (data.fFontName, Memory::NEltsOf (data.fFontName), fontName.c_str ());
    data.fThisRecordLength = data.RecordLenFromNameLen (fontName.length ());

    data.fStyleSet |= isr.GetStyle_Bold () ? (1 << data.eBold) : 0;
    data.fStyleSet |= isr.GetStyle_Italic () ? (1 << data.eItalic) : 0;
    data.fStyleSet |= isr.GetStyle_Underline () ? (1 << data.eUnderline) : 0;
// note these set to zero in memset above so no need to set on other platforms
#if qStroika_Foundation_Common_Platform_Windows
    data.fStyleSet |= isr.GetStyle_Strikeout () ? (1 << data.eStrikeout) : 0;
#endif
    UInt16ToBuf (isr.GetPointSize (), &data.fPointSize);
    SizeTToBuf (isr.fLength, &data.fLength);
    return data;
}
inline StyledInfoSummaryRecord mkInfoSummaryRecordFromPortData (const PortableStyleRunData_Version5& srcData)
{
    _DO_ALIGN_ASSERTS_Version5_ ();
    FontSpecification fsp;
    fsp.SetStyle_Plain (); // so all style valid bits set...
    fsp.SetStyle_Bold (!!(srcData.fStyleSet & (1 << srcData.eBold)));
    fsp.SetStyle_Italic (!!(srcData.fStyleSet & (1 << srcData.eItalic)));
    fsp.SetStyle_Underline (!!(srcData.fStyleSet & (1 << srcData.eUnderline)));
#if qStroika_Foundation_Common_Platform_Windows
    fsp.SetStyle_Strikeout (!!(srcData.fStyleSet & (1 << srcData.eStrikeout)));
#endif
    short fontSize = BufToUInt16 (&srcData.fPointSize);
    if (fontSize < 3 or fontSize > 128) {
        fontSize = 12; // a reasonable default for bogus/corrupt data
    }
    fsp.SetPointSize (fontSize);
    {
        size_t fontNameLen = srcData.NameLenFromRecordLen (srcData.fThisRecordLength);
        fsp.SetFontName (String::FromNarrowSDKString (string{srcData.fFontName, fontNameLen}).AsSDKString ());
    }
    return (StyledInfoSummaryRecord (fsp, BufToUInt32 (&srcData.fLength)));
}

/*
 *  Version 6 font style info file format.
 *      <<Introduced for Led 2.2a2, 970616 - to add color & sub/superscript support>>
 */
struct PortableStyleRunData_Version6 {
    unsigned char fThisRecordLength; // nbytes in this record - cuz fFontName len is variable...
    // note use of a char here effectively limits us to only font names of
    // a little over 200, but that should be more than enuf (limit on PC is 32,
    // and I don't know on mac).

    // Really wanted to just use bit-fields for these guys, but that is non-portable.
    // Sigh... I did that first and the code looks much more readable without all those
    // damned shifts and ors etc... LGP 960531
    enum BitFlagStyles {
        eBold = 0,
        eItalic,
        eUnderline,
        eOutline,
        eShadow,
        eCondensed,
        eExtended,
        eStrikeout,
        eSubscript,
        eSuperscript
    };
    uint16_t fStyleSet;  // or in BitFlagStyles
    uint16_t fPointSize; // note this size is a POINT-SIZE (not the tmHeight)
    uint16_t fColor[3];
    uint32_t fLength;
    char     fFontName[256]; // to file, we really only write as many bytes as needed (no NUL char TERM - infer size from fThisRecordLength)

    static size_t NameLenFromRecordLen (unsigned char len)
    {
        size_t xtra = offsetof (PortableStyleRunData_Version6, fFontName);
        Assert (len > xtra);
        return static_cast<unsigned char> (len - xtra);
    }
    static unsigned char RecordLenFromNameLen (size_t len)
    {
        if (len > 255) {
            Execution::Throw (Execution::Exception{L"RecordLenFromNameLen too long"sv});
        }
        size_t xtra = offsetof (PortableStyleRunData_Version6, fFontName);
        return (unsigned char)min (xtra + size_t (len), size_t (256));
    }
};
inline void _DO_ALIGN_ASSERTS_Version6_ ()
{
    Assert (offsetof (PortableStyleRunData_Version6, fThisRecordLength) == 0);
    Assert (offsetof (PortableStyleRunData_Version6, fStyleSet) == 2);
    Assert (offsetof (PortableStyleRunData_Version6, fPointSize) == 4);
    Assert (offsetof (PortableStyleRunData_Version6, fColor) == 6);
    Assert (offsetof (PortableStyleRunData_Version6, fLength) == 12);
    Assert (offsetof (PortableStyleRunData_Version6, fFontName) == 16);
    Assert (sizeof (PortableStyleRunData_Version6) == 16 + 256);
}
inline PortableStyleRunData_Version6 mkPortableStyleRunData_Version6 (const StyledInfoSummaryRecord& isr)
{
    _DO_ALIGN_ASSERTS_Version6_ ();
    PortableStyleRunData_Version6 data;
    (void)::memset (&data, 0, sizeof data); // A nice feature of data file formats, is that if you make no changes to the content
    // and write it again, they 'diff' equal. Even though this memset isn't needed for
    // accuracy, its nice todo anyhow, and pretty cheap - LGP 960531
    string fontName = String::FromSDKString (isr.GetFontName ()).AsNarrowSDKString ();
    Characters::CString::Copy (data.fFontName, Memory::NEltsOf (data.fFontName), fontName.c_str ());
    data.fThisRecordLength = data.RecordLenFromNameLen (fontName.length ());

    data.fStyleSet |= isr.GetStyle_Bold () ? (1 << data.eBold) : 0;
    data.fStyleSet |= isr.GetStyle_Italic () ? (1 << data.eItalic) : 0;
    data.fStyleSet |= isr.GetStyle_Underline () ? (1 << data.eUnderline) : 0;
// note these set to zero in memset above so no need to set on other platforms
#if qStroika_Foundation_Common_Platform_Windows
    data.fStyleSet |= isr.GetStyle_Strikeout () ? (1 << data.eStrikeout) : 0;
#endif
    data.fStyleSet |= (isr.GetStyle_SubOrSuperScript () == FontSpecification::eSubscript) ? (1 << data.eSubscript) : 0;
    data.fStyleSet |= (isr.GetStyle_SubOrSuperScript () == FontSpecification::eSuperscript) ? (1 << data.eSuperscript) : 0;
    UInt16ToBuf (isr.GetTextColor ().GetRed (), &data.fColor[0]);
    UInt16ToBuf (isr.GetTextColor ().GetGreen (), &data.fColor[1]);
    UInt16ToBuf (isr.GetTextColor ().GetBlue (), &data.fColor[2]);
    UInt16ToBuf (isr.GetPointSize (), &data.fPointSize);
    SizeTToBuf (isr.fLength, &data.fLength);
    return data;
}
inline StyledInfoSummaryRecord mkInfoSummaryRecordFromPortData (const PortableStyleRunData_Version6& srcData)
{
    _DO_ALIGN_ASSERTS_Version6_ ();
    FontSpecification fsp;
    fsp.SetStyle_Plain (); // so all style valid bits set...
    fsp.SetStyle_Bold (!!(srcData.fStyleSet & (1 << srcData.eBold)));
    fsp.SetStyle_Italic (!!(srcData.fStyleSet & (1 << srcData.eItalic)));
    fsp.SetStyle_Underline (!!(srcData.fStyleSet & (1 << srcData.eUnderline)));
#if qStroika_Foundation_Common_Platform_Windows
    fsp.SetStyle_Strikeout (!!(srcData.fStyleSet & (1 << srcData.eStrikeout)));
#endif
    if (srcData.fStyleSet & (1 << srcData.eSubscript)) {
        fsp.SetStyle_SubOrSuperScript (FontSpecification::eSubscript);
    }
    if (srcData.fStyleSet & (1 << srcData.eSuperscript)) {
        fsp.SetStyle_SubOrSuperScript (FontSpecification::eSuperscript);
    }

    fsp.SetTextColor (Color (BufToUInt16 (&srcData.fColor[0]), BufToUInt16 (&srcData.fColor[1]), BufToUInt16 (&srcData.fColor[2])));

    short fontSize = BufToUInt16 (&srcData.fPointSize);
    if (fontSize < 3 or fontSize > 128) {
        fontSize = 12; // a reasonable default for bogus/corrupt data
    }
    fsp.SetPointSize (fontSize);
    {
        size_t fontNameLen = srcData.NameLenFromRecordLen (srcData.fThisRecordLength);
        fsp.SetFontName (String::FromNarrowSDKString (string{srcData.fFontName, fontNameLen}).AsSDKString ());
    }
    return (StyledInfoSummaryRecord (fsp, BufToUInt32 (&srcData.fLength)));
}

/*
 ********************************************************************************
 ******************* StyledTextIOReader_LedNativeFileFormat *********************
 ********************************************************************************
 */
StyledTextIOReader_LedNativeFileFormat::StyledTextIOReader_LedNativeFileFormat (SrcStream* srcStream, SinkStream* sinkStream)
    : StyledTextIOReader (srcStream, sinkStream)
{
}

void StyledTextIOReader_LedNativeFileFormat::Read ()
{
    _DO_ALIGN_ASSERTS_Version4_ (); // check here for all versions, just so we don't wait too long to discover
    _DO_ALIGN_ASSERTS_Version5_ (); // a problem reading old files (I don't really test that).

    LedFormatMagicCookie cookie;
    if (GetSrcStream ().read (cookie, sizeof (cookie)) != sizeof (cookie)) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    bool isVersion4   = memcmp (cookie, kLedPartFormatVersion_4_MagicNumber, sizeof (kLedPartFormatVersion_4_MagicNumber)) == 0;
    bool isVersion5   = memcmp (cookie, kLedPartFormatVersion_5_MagicNumber, sizeof (kLedPartFormatVersion_5_MagicNumber)) == 0;
    bool isVersion6   = memcmp (cookie, kLedPartFormatVersion_6_MagicNumber, sizeof (kLedPartFormatVersion_6_MagicNumber)) == 0;
    bool isFormatGood = isVersion4 or isVersion5 or isVersion6;
    if (not isFormatGood) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    if (isVersion4) {
        Read_Version4 (cookie);
    }
    if (isVersion5) {
        Read_Version5 (cookie);
    }
    if (isVersion6) {
        Read_Version6 (cookie);
    }
    GetSinkStream ().EndOfBuffer ();
}

bool StyledTextIOReader_LedNativeFileFormat::QuickLookAppearsToBeRightFormat ()
{
    SrcStreamSeekSaver savePos (GetSrcStream ());

    LedFormatMagicCookie cookie;
    if (GetSrcStream ().read (cookie, sizeof (cookie)) != sizeof (cookie)) {
        return false;
    }
    bool isVersion4   = memcmp (cookie, kLedPartFormatVersion_4_MagicNumber, sizeof (kLedPartFormatVersion_4_MagicNumber)) == 0;
    bool isVersion5   = memcmp (cookie, kLedPartFormatVersion_5_MagicNumber, sizeof (kLedPartFormatVersion_5_MagicNumber)) == 0;
    bool isVersion6   = memcmp (cookie, kLedPartFormatVersion_6_MagicNumber, sizeof (kLedPartFormatVersion_6_MagicNumber)) == 0;
    bool isFormatGood = isVersion4 or isVersion5 or isVersion6;
    return isFormatGood;
}

void StyledTextIOReader_LedNativeFileFormat::Read_Version4 (const char* cookie)
{
    // quickie text read implementation - could loop, but turns out multiple calls to AppendText
    // is slow (at least under OpenDoc - see why later and see if we can do piecewise read,
    // so we don't need large contiguos buffer. This is OK though ...
    size_t totalTextLength = 0;
    {
        uint32_t lenAsBuf;
        if (GetSrcStream ().read (&lenAsBuf, sizeof (lenAsBuf)) != sizeof (lenAsBuf)) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        totalTextLength = BufToUInt32 (&lenAsBuf);
    }
    const size_t kMaxBufSize = 64 * 1024 * 1024; // not a REAL limit - just a sanity check input data not garbage,
    // and so we don't get errors from our malloc for asking for huge number
    // (OpenDoc 1.0 does this).
    if (totalTextLength > kMaxBufSize) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    {
        StackBuffer<char> buf{Memory::eUninitialized, totalTextLength};
        if (GetSrcStream ().read (buf.data (), totalTextLength) != totalTextLength) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        size_t                 nChars = totalTextLength;
        StackBuffer<Led_tChar> unicodeText{Memory::eUninitialized, nChars};
#if qStroika_Foundation_Common_Platform_Windows
        auto text = CodeCvt<Led_tChar>{static_cast<CodePage> (CP_ACP)}.Bytes2Characters (as_bytes (span{buf}), span{unicodeText});
#else
        auto text = CodeCvt<Led_tChar>{locale{}}.Bytes2Characters (as_bytes (span{buf}), span{unicodeText});
#endif
        GetSinkStream ().AppendText (text.data (), text.size (), nullptr);
    }

    // Read Style Runs
    {
        size_t nStyleRuns = InputStandardFromSrcStream_ULONG (GetSrcStream ());
        if (nStyleRuns > totalTextLength + 1) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        vector<StyledInfoSummaryRecord> styleRunInfo;
        {
            StackBuffer<PortableStyleRunData_Version4> portableStyleRuns{Memory::eUninitialized, nStyleRuns};
            if (GetSrcStream ().read (portableStyleRuns.data (), nStyleRuns * sizeof (PortableStyleRunData_Version4)) !=
                nStyleRuns * sizeof (PortableStyleRunData_Version4)) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            for (size_t i = 0; i < nStyleRuns; ++i) {
                styleRunInfo.push_back (mkInfoSummaryRecordFromPortData (portableStyleRuns[i]));
            }
        }
        GetSinkStream ().ApplyStyle (0, totalTextLength, styleRunInfo);
    }

    // Read Embeddings
#if qStroika_Frameworks_Led_SupportGDI
    {
        size_t nEmbeddings = InputStandardFromSrcStream_ULONG (GetSrcStream ());
        if (nEmbeddings > totalTextLength) { // sanity check
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }

        for (size_t i = 0; i < nEmbeddings; ++i) {
            size_t whereAt      = InputStandardFromSrcStream_ULONG (GetSrcStream ());
            size_t howManyBytes = InputStandardFromSrcStream_ULONG (GetSrcStream ());

            size_t justBeforeEmbedding = GetSrcStream ().current_offset ();

            Led_PrivateEmbeddingTag tag;
            if (howManyBytes < sizeof (tag)) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            if (GetSrcStream ().read (tag, sizeof (tag)) != sizeof (tag)) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            SimpleEmbeddedObjectStyleMarker* embedding = InternalizeEmbedding (tag, howManyBytes - sizeof (tag));
            if (embedding == NULL) {
                // something unknown. In the future, we will cons up a special embedding for unknown
                // items so they will appear in the text, and still get written back out later correctly.
                // But SKIPPING THEM will have todo for now...
                GetSrcStream ().seek_to (justBeforeEmbedding + howManyBytes);
            }
            else {
                GetSinkStream ().InsertEmbeddingForExistingSentinel (embedding, whereAt);
            }
        }
    }
#endif

    // check for extra cookie at the end...
    LedFormatMagicCookie endCookie;
    if (GetSrcStream ().read (endCookie, sizeof (endCookie)) != sizeof (endCookie)) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    if (memcmp (endCookie, cookie, sizeof (endCookie)) != 0) {
        // maybe should warn - maybe an error? Who knows - we've read so much - its a shame to fail to open
        // just cuz of this...
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
}

void StyledTextIOReader_LedNativeFileFormat::Read_Version5 (const char* cookie)
{
    // quickie text read implementation - could loop, but turns out multiple calls to AppendText
    // is slow (at least under OpenDoc - see why later and see if we can do piecewise read,
    // so we don't need large contiguos buffer. This is OK though ...
    size_t totalTextLength = 0;
    {
        uint32_t lenAsBuf;
        if (GetSrcStream ().read (&lenAsBuf, sizeof (lenAsBuf)) != sizeof (lenAsBuf)) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        totalTextLength = BufToUInt32 (&lenAsBuf);
    }
    const size_t kMaxBufSize = 64 * 1024 * 1024; // not a REAL limit - just a sanity check input data not garbage,
    // and so we don't get errors from our malloc for asking for huge number
    // (OpenDoc 1.0 does this).
    if (totalTextLength > kMaxBufSize) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    {
        StackBuffer<char> buf{Memory::eUninitialized, totalTextLength};
        if (GetSrcStream ().read (buf.data (), totalTextLength) != totalTextLength) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        size_t                 nChars = totalTextLength;
        StackBuffer<Led_tChar> unicodeText{Memory::eUninitialized, nChars};
#if qStroika_Foundation_Common_Platform_Windows
        auto text = CodeCvt<Led_tChar>{static_cast<CodePage> (CP_ACP)}.Bytes2Characters (as_bytes (span{buf}), span{unicodeText});
#else
        auto text = CodeCvt<Led_tChar>{locale{}}.Bytes2Characters (as_bytes (span{buf}), span{unicodeText});
#endif
        GetSinkStream ().AppendText (text.data (), text.size (), nullptr);
    }

    // Read Style Runs
    {
        size_t            howManyBytesOfStyleInfo = InputStandardFromSrcStream_ULONG (GetSrcStream ());
        StackBuffer<char> portableStyleRunsBuffer{Memory::eUninitialized, howManyBytesOfStyleInfo};
        if (GetSrcStream ().read (portableStyleRunsBuffer.data (), howManyBytesOfStyleInfo) != howManyBytesOfStyleInfo) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }

        size_t offsetInText = 0;
        size_t i            = 0;
        for (; i < howManyBytesOfStyleInfo;) {
            const PortableStyleRunData_Version5* thisBucket = (PortableStyleRunData_Version5*)(((char*)portableStyleRunsBuffer) + i);
            if (i + thisBucket->fThisRecordLength > howManyBytesOfStyleInfo) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            StyledInfoSummaryRecord isr = mkInfoSummaryRecordFromPortData (*thisBucket);

            vector<StyledInfoSummaryRecord> list;
            list.push_back (isr);
            GetSinkStream ().ApplyStyle (offsetInText, offsetInText + isr.fLength, list); // silly this API REQUIRES a list...

            offsetInText += isr.fLength;
            i += thisBucket->fThisRecordLength;
        }
        if (i != howManyBytesOfStyleInfo) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
    }

#if qStroika_Frameworks_Led_SupportGDI
    // Read Embeddings
    {
        size_t nEmbeddings = InputStandardFromSrcStream_ULONG (GetSrcStream ());
        if (nEmbeddings > totalTextLength) { // sanity check
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }

        for (size_t i = 0; i < nEmbeddings; ++i) {
            size_t whereAt      = InputStandardFromSrcStream_ULONG (GetSrcStream ()) - 1;
            size_t howManyBytes = InputStandardFromSrcStream_ULONG (GetSrcStream ());

            size_t justBeforeEmbedding = GetSrcStream ().current_offset ();

            Led_PrivateEmbeddingTag tag;
            if (howManyBytes < sizeof (tag)) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            if (GetSrcStream ().read (tag, sizeof (tag)) != sizeof (tag)) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            SimpleEmbeddedObjectStyleMarker* embedding = InternalizeEmbedding (tag, howManyBytes - sizeof (tag));
            if (embedding == nullptr) {
                // something unknown. In the future, we will cons up a special embedding for unknown
                // items so they will appear in the text, and still get written back out later correctly.
                // But SKIPPING THEM will have todo for now...
                GetSrcStream ().seek_to (justBeforeEmbedding + howManyBytes);
            }
            else {
                GetSinkStream ().InsertEmbeddingForExistingSentinel (embedding, whereAt);
            }
        }
    }
#endif

    // check for extra cookie at the end...
    LedFormatMagicCookie endCookie;
    if (GetSrcStream ().read (endCookie, sizeof (endCookie)) != sizeof (endCookie)) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    if (memcmp (endCookie, cookie, sizeof (endCookie)) != 0) {
        // maybe should warn - maybe an error? Who knows - we've read so much - its a shame to fail to open
        // just cuz of this...
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
}

void StyledTextIOReader_LedNativeFileFormat::Read_Version6 (const char* cookie)
{
    // quickie text read implementation - could loop, but turns out multiple calls to AppendText
    // is slow (at least under OpenDoc - see why later and see if we can do piecewise read,
    // so we don't need large contiguos buffer. This is OK though ...
    size_t totalTextLength = 0;
    {
        uint32_t lenAsBuf;
        if (GetSrcStream ().read (&lenAsBuf, sizeof (lenAsBuf)) != sizeof (lenAsBuf)) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        totalTextLength = BufToUInt32 (&lenAsBuf);
    }
    const size_t kMaxBufSize = 64 * 1024 * 1024; // not a REAL limit - just a sanity check input data not garbage,
    // and so we don't get errors from our malloc for asking for huge number
    // (OpenDoc 1.0 does this).
    if (totalTextLength > kMaxBufSize) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    {
        StackBuffer<char> buf{Memory::eUninitialized, totalTextLength};
        if (GetSrcStream ().read (buf.data (), totalTextLength) != totalTextLength) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        size_t                 nChars = totalTextLength;
        StackBuffer<Led_tChar> unicodeText{Memory::eUninitialized, nChars};
#if qStroika_Foundation_Common_Platform_Windows
        auto text = CodeCvt<Led_tChar>{static_cast<CodePage> (CP_ACP)}.Bytes2Characters (as_bytes (span{buf}), span{unicodeText});
#else
        auto text = CodeCvt<Led_tChar>{locale{}}.Bytes2Characters (as_bytes (span{buf}), span{unicodeText});
#endif
        GetSinkStream ().AppendText (text.data (), text.size (), nullptr);
    }

    // Read Style Runs
    {
        size_t            howManyBytesOfStyleInfo = InputStandardFromSrcStream_ULONG (GetSrcStream ());
        StackBuffer<char> portableStyleRunsBuffer{Memory::eUninitialized, howManyBytesOfStyleInfo};
        if (GetSrcStream ().read (portableStyleRunsBuffer.data (), howManyBytesOfStyleInfo) != howManyBytesOfStyleInfo) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }

        size_t offsetInText = 0;
        size_t i            = 0;
        for (; i < howManyBytesOfStyleInfo;) {
            const PortableStyleRunData_Version6* thisBucket = (PortableStyleRunData_Version6*)(((char*)portableStyleRunsBuffer) + i);
            if (i + thisBucket->fThisRecordLength > howManyBytesOfStyleInfo) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            StyledInfoSummaryRecord isr = mkInfoSummaryRecordFromPortData (*thisBucket);

            vector<StyledInfoSummaryRecord> list;
            list.push_back (isr);
            GetSinkStream ().ApplyStyle (offsetInText, offsetInText + isr.fLength, list); // silly this API REQUIRES a list...

            offsetInText += isr.fLength;
            i += thisBucket->fThisRecordLength;
        }
        if (i != howManyBytesOfStyleInfo) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
    }

#if qStroika_Frameworks_Led_SupportGDI
    // Read Embeddings
    {
        size_t nEmbeddings = InputStandardFromSrcStream_ULONG (GetSrcStream ());
        if (nEmbeddings > totalTextLength) { // sanity check
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }

        for (size_t i = 0; i < nEmbeddings; ++i) {
            size_t whereAt      = InputStandardFromSrcStream_ULONG (GetSrcStream ());
            size_t howManyBytes = InputStandardFromSrcStream_ULONG (GetSrcStream ());

            size_t justBeforeEmbedding = GetSrcStream ().current_offset ();

            Led_PrivateEmbeddingTag tag;
            if (howManyBytes < sizeof (tag)) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            if (GetSrcStream ().read (tag, sizeof (tag)) != sizeof (tag)) {
                Execution::Throw (DataExchange::BadFormatException::kThe);
            }
            SimpleEmbeddedObjectStyleMarker* embedding = InternalizeEmbedding (tag, howManyBytes - sizeof (tag));
            if (embedding == NULL) {
                // something unknown. In the future, we will cons up a special embedding for unknown
                // items so they will appear in the text, and still get written back out later correctly.
                // But SKIPPING THEM will have todo for now...
                GetSrcStream ().seek_to (justBeforeEmbedding + howManyBytes);
            }
            else {
                GetSinkStream ().InsertEmbeddingForExistingSentinel (embedding, whereAt);
            }
        }
    }
#endif

    // check for extra cookie at the end...
    LedFormatMagicCookie endCookie;
    if (GetSrcStream ().read (endCookie, sizeof (endCookie)) != sizeof (endCookie)) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
    if (memcmp (endCookie, cookie, sizeof (endCookie)) != 0) {
        // maybe should warn - maybe an error? Who knows - we've read so much - its a shame to fail to open
        // just cuz of this...
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }
}

#if qStroika_Frameworks_Led_SupportGDI
SimpleEmbeddedObjectStyleMarker* StyledTextIOReader_LedNativeFileFormat::InternalizeEmbedding (Led_PrivateEmbeddingTag tag, size_t howManyBytes)
{
    StackBuffer<char> dataBuf{Memory::eUninitialized, howManyBytes};
    if (GetSrcStream ().read (dataBuf.data (), howManyBytes) != howManyBytes) {
        Execution::Throw (DataExchange::BadFormatException::kThe);
    }

    const vector<EmbeddedObjectCreatorRegistry::Assoc>& types = EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
    for (size_t i = 0; i < types.size (); ++i) {
        EmbeddedObjectCreatorRegistry::Assoc assoc = types[i];
        if (memcmp (assoc.fEmbeddingTag, tag, sizeof (assoc.fEmbeddingTag)) == 0) {
            AssertNotNull (assoc.fReadFromMemory);
            return (assoc.fReadFromMemory) (tag, dataBuf.data (), howManyBytes);
        }
    }
    return new StandardUnknownTypeStyleMarker{0, tag, dataBuf.data (), howManyBytes};
}
#endif

/*
 ********************************************************************************
 ******************* StyledTextIOWriter_LedNativeFileFormat *********************
 ********************************************************************************
 */
StyledTextIOWriter_LedNativeFileFormat::StyledTextIOWriter_LedNativeFileFormat (SrcStream* srcStream, SinkStream* sinkStream)
    : StyledTextIOWriter (srcStream, sinkStream)
{
}

void StyledTextIOWriter_LedNativeFileFormat::Write ()
{
    Write_Version6 ();
}

void StyledTextIOWriter_LedNativeFileFormat::Write_Version6 ()
{
    // Write a magic cookie to identify this format
    write (kLedPartFormatVersion_6_MagicNumber, sizeof (kLedPartFormatVersion_6_MagicNumber));

    size_t totalTextLength = GetSrcStream ().GetTotalTextLength ();
    {
        // write a length-of-text count, and then the text
        StackBuffer<Led_tChar> buf{Memory::eUninitialized, totalTextLength};
        if (GetSrcStream ().readNTChars (buf.data (), totalTextLength) != totalTextLength) {
            Execution::Throw (DataExchange::BadFormatException::kThe);
        }
        size_t            nChars = totalTextLength * sizeof (wchar_t);
        StackBuffer<char> result{Memory::eUninitialized, nChars};
#if qStroika_Foundation_Common_Platform_Windows
        nChars = CodeCvt<Led_tChar>{static_cast<CodePage> (CP_ACP)}.Characters2Bytes (span{buf}, as_writable_bytes (span{result})).size ();
#else
        nChars = CodeCvt<Led_tChar>{locale{}}.Characters2Bytes (span{buf}, as_writable_bytes (span{result})).size ();
#endif
        {
            uint32_t encodedTL = 0;
            SizeTToBuf (nChars, &encodedTL);
            write (&encodedTL, sizeof (encodedTL));
        }
        write (result.data (), nChars);
    }

    // Write the style runs
    {
        vector<StyledInfoSummaryRecord> styleRunInfo = GetSrcStream ().GetStyleInfo (0, totalTextLength);

        size_t howManyBytes              = 0; // write place-holder, than then come back and patch this!
        size_t styleRunInfoSectionCursor = GetSinkStream ().current_offset ();
        OutputStandardToSinkStream_size_t_ (GetSinkStream (), howManyBytes);

        size_t styleRuns = styleRunInfo.size ();
        for (size_t i = 0; i < styleRuns; ++i) {
            PortableStyleRunData_Version6 data = mkPortableStyleRunData_Version6 (styleRunInfo[i]);
            write (&data, data.fThisRecordLength);
        }

        // Here we back-patch the new size of the embedded object, so we can read-over these things
        // without knowing about the object type (exp cross-platform).
        size_t here  = GetSinkStream ().current_offset ();
        howManyBytes = here - (styleRunInfoSectionCursor + 4); //+4 cuz don't count size marker length itself.
        GetSinkStream ().seek_to (styleRunInfoSectionCursor);
        OutputStandardToSinkStream_size_t_ (GetSinkStream (), howManyBytes);
        GetSinkStream ().seek_to (here); // back to where we left off...
    }

    // Write the embedded objects
#if qStroika_Frameworks_Led_SupportGDI
    {
        vector<SimpleEmbeddedObjectStyleMarker*> embeddings = GetSrcStream ().CollectAllEmbeddingMarkersInRange (0, totalTextLength);
        OutputStandardToSinkStream_size_t_ (GetSinkStream (), embeddings.size ());
        size_t markerPosOffset = GetSrcStream ().GetEmbeddingMarkerPosOffset ();
        for (size_t i = 0; i < embeddings.size (); ++i) {
            SimpleEmbeddedObjectStyleMarker* embedding = embeddings[i];
            // Write where embedding is located in text, relative to beginning of text (being internalized/extenralized)
            size_t whereAt = embedding->GetStart () - markerPosOffset;
            OutputStandardToSinkStream_size_t_ (GetSinkStream (), whereAt);

            // Note: this howManyBytes refers to the content-type specific portion.
            size_t howManyBytes        = 0; // write place-holder, than then come back and patch this!
            size_t embeddingSizeCursor = GetSinkStream ().current_offset ();
            OutputStandardToSinkStream_size_t_ (GetSinkStream (), howManyBytes);

            size_t embeddingCursor = GetSinkStream ().current_offset ();
            write (embedding->GetTag (), sizeof (Led_PrivateEmbeddingTag));
            ExternalizeEmbedding (embedding);

            // Here we back-patch the new size of the embedded object, so we can read-over these things
            // without knowing about the object type (exp cross-platform).
            size_t here  = GetSinkStream ().current_offset ();
            howManyBytes = here - embeddingCursor;
            GetSinkStream ().seek_to (embeddingSizeCursor);
            OutputStandardToSinkStream_size_t_ (GetSinkStream (), howManyBytes);
            GetSinkStream ().seek_to (here); // back to where we left off...
        }
    }
#endif

    // Write a magic cookie - just as a validation/sanity check on the format
    write (kLedPartFormatVersion_6_MagicNumber, sizeof (kLedPartFormatVersion_6_MagicNumber));
}

#if qStroika_Frameworks_Led_SupportGDI
void StyledTextIOWriter_LedNativeFileFormat::ExternalizeEmbedding (SimpleEmbeddedObjectStyleMarker* embedding)
{
    EmbeddingSinkStream embeddingSinkStream (GetSinkStream ());
    embedding->Write (embeddingSinkStream);
}
#endif