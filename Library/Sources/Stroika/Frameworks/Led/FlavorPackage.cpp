/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/LineEndings.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "Config.h"

#if qPlatform_Windows
#include <fcntl.h>
#include <io.h>
#include <shellapi.h>
#elif qStroika_FeatureSupported_XWindows
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "Marker.h"
#include "TextStore.h"

#include "FlavorPackage.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 *************************** FlavorPackageExternalizer **************************
 ********************************************************************************
 */

TextStore* FlavorPackageExternalizer::PeekAtTextStore () const
{
    return &fTextStore;
}

/*
@METHOD:        FlavorPackageExternalizer::ExternalizeFlavors
@DESCRIPTION:   <p>Externalize to the given @'FlavorPackageExternalizer::WriterFlavorPackage' the text in the
    given range. This can be for a Drag&Drop package, a ClipBoard package, or whatever
    (see @'FlavorPackageExternalizer::ExternalizeBestFlavor').</p>
*/
void FlavorPackageExternalizer::ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    ExternalizeFlavor_TEXT (flavorPackage, from, to);
}

/*
@METHOD:        FlavorPackageExternalizer::ExternalizeBestFlavor
@DESCRIPTION:   <p>Externalize to the given @'FlavorPackageExternalizer::WriterFlavorPackage' the text in the
    given range. This can be for a Drag&Drop package, a ClipBoard package, or whatever
    (see @'FlavorPackageExternalizer::ExternalizeFlavors').</p>
*/
void FlavorPackageExternalizer::ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    ExternalizeFlavor_TEXT (flavorPackage, from, to);
}

/*
@METHOD:        FlavorPackageExternalizer::ExternalizeFlavor_TEXT
@DESCRIPTION:   <p>Externalize to the given @'FlavorPackageExternalizer::WriterFlavorPackage' the text in the
    given range. This can be for a Drag&Drop package, a ClipBoard package, or whatever
    (see @'FlavorPackageExternalizer::ExternalizeFlavors' and @'FlavorPackageExternalizer::ExternalizeBestFlavor').</p>
        <p>This method externalizes in the native OS text format (with any embedded NUL-characters
    in the text eliminated).</p>
*/
void FlavorPackageExternalizer::ExternalizeFlavor_TEXT (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    size_t start = from;
    size_t end   = to;
    Require (start >= 0);
    Require (end <= GetTextStore ().GetEnd ());
    Require (start <= end);
    size_t length = end - start;
#if qPlatform_Windows
    Memory::StackBuffer<Led_tChar> buf{2 * length + 1}; //CRLF
#else
    Memory::StackBuffer<Led_tChar> buf{length};
#endif
    if (length != 0) {
        Memory::StackBuffer<Led_tChar> buf2{length};
        GetTextStore ().CopyOut (start, length, buf2.data ());
#if qStroika_FeatureSupported_XWindows
        length = Characters::NLToNative<Led_tChar> (buf2, length, buf, length);
#elif qPlatform_Windows
        length = Characters::NLToNative<Led_tChar> (buf2.data (), length, buf.data (), 2 * length + 1);
#endif
    }

    // At least for MS-Windows - NUL-chars in the middle of the text just cause confusion in other
    // apps. Don't know about Mac. But I doubt they help any there either. Since for this stuff
    // we aren't going todo the right thing for sentinels ANYHOW - we may as well eliminate them
    // (or any other NUL-chars)
    length = Led_SkrunchOutSpecialChars (buf.data (), length, '\0');

#if qPlatform_Windows
    buf[length] = '\0'; // Windows always expects CF_TEXT to be NUL char terminated
    length++;           // so AddFlavorData() writes out the NUL-byte
#endif
    flavorPackage.AddFlavorData (kTEXTClipFormat, length * sizeof (Led_tChar), buf.data ());
}

/*
 ********************************************************************************
 *************************** FlavorPackageInternalizer **************************
 ********************************************************************************
 */
TextStore* FlavorPackageInternalizer::PeekAtTextStore () const
{
    return &fTextStore;
}

bool FlavorPackageInternalizer::InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    if (InternalizeFlavor_FILE (flavorPackage, from, to)) {
        return true;
    }
    else if (InternalizeFlavor_TEXT (flavorPackage, from, to)) {
        return true;
    }
    return false;
}

bool FlavorPackageInternalizer::InternalizeFlavor_TEXT (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    if (flavorPackage.GetFlavorAvailable_TEXT ()) {
        size_t         length     = flavorPackage.GetFlavorSize (kTEXTClipFormat);
        Led_ClipFormat textFormat = kTEXTClipFormat;
        Memory::StackBuffer<char> buf{length * sizeof (Led_tChar)}; // data read from flavor package is just an array of bytes (not Led_tChar)
        // but allocate enuf space for converting TO UNICODE - in case of
        // qWorkAroundWin95BrokenUNICODESupport workaround below - we may
        // want to copy UNICODE chars in there instead.
        length = flavorPackage.ReadFlavorData (textFormat, length, buf.data ());

        Led_tChar* buffp   = reinterpret_cast<Led_tChar*> (static_cast<char*> (buf)); // INTERPRET array of bytes as Led_tChars
        size_t     nTChars = length / sizeof (Led_tChar);
#if qPlatform_Windows
        if (nTChars > 0) {
            // On Windows - CF_TEXT always GUARANTEED to be NUL-terminated, and the
            // length field is often wrong (rounded up to some chunk size, with garbage
            // text at the end...
            nTChars = min (nTChars, Led_tStrlen (buffp)); // do min in case clip data corrupt
        }
#endif

        size_t start = from;
        size_t end   = to;
        Require (start <= end);

        nTChars = Characters::NormalizeTextToNL<Led_tChar> (buffp, nTChars, buffp, nTChars);
        GetTextStore ().Replace (start, end, buffp, nTChars);
        return true;
    }
    else {
        return false;
    }
}

bool FlavorPackageInternalizer::InternalizeFlavor_FILE (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    // For now, we ignore any files beyond the first one (Mac&PC)...LGP 960522
    if (flavorPackage.GetFlavorAvailable (kFILEClipFormat)) {
        size_t                    fileSpecBufferLength = flavorPackage.GetFlavorSize (kFILEClipFormat);
        Memory::StackBuffer<char> fileSpecBuffer{fileSpecBufferLength};
        fileSpecBufferLength = flavorPackage.ReadFlavorData (kFILEClipFormat, fileSpecBufferLength, fileSpecBuffer.data ());

// Unpack the filename
#if qPlatform_Windows
        TCHAR realFileName[_MAX_PATH + 1];
        {
            HDROP hdrop = (HDROP)::GlobalAlloc (GMEM_FIXED, fileSpecBufferLength);
            Execution::ThrowIfNull (hdrop);
            (void)::memcpy (hdrop, fileSpecBuffer.data (), fileSpecBufferLength);
            size_t nChars = ::DragQueryFile (hdrop, 0, nullptr, 0);
            Verify (::DragQueryFile (hdrop, 0, realFileName, static_cast<UINT> (nChars + 1)) == nChars);
            ::GlobalFree (hdrop);
        }
        Led_ClipFormat suggestedClipFormat = kBadClipFormat; // no guess - examine text later
#else
        char realFileName[1000]; // use MAX_PATH? ??? NO - GET REAL SIZE !!! X-TMP-HACK-LGP991213
        realFileName[0]                    = '\0';
        Led_ClipFormat suggestedClipFormat = kBadClipFormat; // no guess - examine text later
#endif

        return InternalizeFlavor_FILEData (realFileName, &suggestedClipFormat, nullopt, from, to);
    }
    else {
        return false;
    }
}

bool FlavorPackageInternalizer::InternalizeFlavor_FILEData (filesystem::path fileName, Led_ClipFormat* suggestedClipFormat,
                                                            optional<CodePage> suggestedCodePage, size_t from, size_t to)
{
    Memory::BLOB b       = IO::FileSystem::FileInputStream::New (filesystem::path (fileName)).ReadAll ();
    const byte*  fileBuf = b.begin ();
    size_t       fileLen = b.size ();

    InternalizeFlavor_FILEGuessFormatsFromName (fileName, suggestedClipFormat, suggestedCodePage);

    // If we still don't have a good guess of the clip format, then peek at the data, and
    // try to guess...
    InternalizeFlavor_FILEGuessFormatsFromStartOfData (suggestedClipFormat, suggestedCodePage, fileBuf, fileBuf + fileLen);

    // By this point, we've read in the file data, and its stored in fileReader. And, we've been given guesses,
    // and adjusted our guesses about the format (without looking at the data). Now just
    // internalize the bytes according to our format guesses.
    return InternalizeFlavor_FILEDataRawBytes (suggestedClipFormat, suggestedCodePage, from, to, fileBuf, fileLen);
}

void FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (filesystem::path fileName, Led_ClipFormat* suggestedClipFormat,
                                                                            [[maybe_unused]] optional<CodePage> suggestedCodePage)
{
#if qPlatform_Windows
    if (suggestedClipFormat != nullptr and *suggestedClipFormat == kBadClipFormat) {
        TCHAR drive[_MAX_DRIVE];
        TCHAR dir[_MAX_DIR];
        TCHAR fname[_MAX_FNAME];
        TCHAR ext[_MAX_EXT];
        ::_tsplitpath_s (fileName.native ().c_str (), drive, dir, fname, ext);
        if (::_tcsicmp (ext, Led_SDK_TCHAROF (".txt")) == 0) {
            *suggestedClipFormat = kTEXTClipFormat;
        }
    }
#endif
}

void FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (Led_ClipFormat*                     suggestedClipFormat,
                                                                                   [[maybe_unused]] optional<CodePage> suggestedCodePage,
                                                                                   const byte* /*fileStart*/, const byte* /*fileEnd*/
)
{
    if (suggestedClipFormat != nullptr) {
        if (*suggestedClipFormat == kBadClipFormat) {
            // Then try guessing some different formats. Since this module only knows of one, and thats a last ditch
            // default - we don't venture a guess here. But - other formats - like RTF and HTML - have distinctive headers
            // than can be scanned for to see if we are using that format data
        }
    }
}

bool FlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes (Led_ClipFormat* suggestedClipFormat, optional<CodePage> suggestedCodePage,
                                                                    size_t from, size_t to, const void* rawBytes, size_t nRawBytes)
{
    /*
     *  If suggesedFormat UNKNOWN - treat as text.
     *
     *  If its TEXT, read it in here directly (using the suggested codePage).
     *
     *  If its anything else - then create a temporary, fake memory-based SourcePackage, and re-internalize.
     */
    Led_ClipFormat cf = (suggestedClipFormat == nullptr or *suggestedClipFormat == kBadClipFormat) ? kTEXTClipFormat : *suggestedClipFormat;
    if (cf != kTEXTClipFormat) {
        ReadWriteMemBufferPackage package;
        package.AddFlavorData (cf, nRawBytes, rawBytes);
        if (InternalizeBestFlavor (package, from, to)) {
            return true;
        }
    }

    /*
     *  If either the suggestedClipFormat was TEXT or it was something else we didn't understand - then just
     *  import that contents as if it was plain text
     *
     *  We COULD just vector to InternalizeBestFlavor above for the TEXT case - but then we'd lose the passed information about the
     *  preferred code page - so just do the read/replace here...
     */
    span<const byte> rawByteSpan{reinterpret_cast<const byte*> (rawBytes), nRawBytes};
#if qPlatform_Windows
    CodeCvt<Led_tChar> converter{&rawByteSpan, CodeCvt<Led_tChar>{suggestedCodePage.value_or (CP_ACP)}};
#else
    CodeCvt<Led_tChar> converter{&rawByteSpan, CodeCvt<Led_tChar>{locale{}}};
#endif
    size_t                         outCharCnt = converter.ComputeTargetCharacterBufferSize (rawByteSpan);
    Memory::StackBuffer<Led_tChar> fileData2{outCharCnt};
    auto                           charsRead = converter.Bytes2Characters (&rawByteSpan, span{fileData2}).size ();
    charsRead = Characters::NormalizeTextToNL<Led_tChar> (fileData2.data (), charsRead, fileData2.data (), charsRead);
    GetTextStore ().Replace (from, to, fileData2.data (), charsRead);
    return true;
}

#if qStroika_Frameworks_Led_SupportClipboard
/*
 ********************************************************************************
 *************************** ReaderClipboardFlavorPackage ***********************
 ********************************************************************************
 */
#if qStroika_FeatureSupported_XWindows
map<Led_ClipFormat, vector<char>> ReaderClipboardFlavorPackage::sPrivateClipData;
#endif

bool ReaderClipboardFlavorPackage::GetFlavorAvailable (Led_ClipFormat clipFormat) const
{
#if qStroika_FeatureSupported_XWindows
    map<Led_ClipFormat, vector<char>>::const_iterator i = sPrivateClipData.find (clipFormat);
    return (i != sPrivateClipData.end ());
#else
    return Led_ClipboardObjectAcquire::FormatAvailable (clipFormat);
#endif
}

size_t ReaderClipboardFlavorPackage::GetFlavorSize (Led_ClipFormat clipFormat) const
{
#if qStroika_FeatureSupported_XWindows
    map<Led_ClipFormat, vector<char>>::const_iterator i = sPrivateClipData.find (clipFormat);
    if (i == sPrivateClipData.end ()) {
        return 0;
    }
    else {
        return i->second.size ();
    }
#else
    Led_ClipboardObjectAcquire clip (clipFormat);
    if (clip.GoodClip ()) {
        return clip.GetDataLength ();
    }
    else {
        return 0;
    }
#endif
}

size_t ReaderClipboardFlavorPackage::ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const
{
#if qStroika_FeatureSupported_XWindows
    map<Led_ClipFormat, vector<char>>::const_iterator i = sPrivateClipData.find (clipFormat);
    if (i == sPrivateClipData.end ()) {
        return 0;
    }
    else {
        size_t copyNBytes = min (bufSize, i->second.size ());
        (void)::memcpy (buf, Traversal::Iterator2Pointer (i->second.begin ()), copyNBytes);
        Ensure (copyNBytes <= bufSize);
        return copyNBytes;
    }
#else
    Led_ClipboardObjectAcquire clip{clipFormat};
    if (clip.GoodClip ()) {
        size_t copyNBytes = min (bufSize, clip.GetDataLength ());
        (void)::memcpy (buf, clip.GetData (), copyNBytes);
        Ensure (copyNBytes <= bufSize);
        return copyNBytes;
    }
    else {
        return 0;
    }
#endif
}
#endif

#if qStroika_Frameworks_Led_SupportClipboard
/*
 ********************************************************************************
 **************************** WriterClipboardFlavorPackage **********************
 ********************************************************************************
 */
void WriterClipboardFlavorPackage::AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)
{
#if qPlatform_Windows
    // NOTE: FOR THE PC - it is assumed all this happens  in the context of an open/close clipboard
    // done in the Led_MFC class overrides of OnCopyCommand_Before/OnCopyCommand_After
    HANDLE h = ::GlobalAlloc (GHND | GMEM_MOVEABLE, bufSize);
    Execution::ThrowIfNull (h);
    {
        char* clipBuf = (char*)::GlobalLock (h);
        if (clipBuf == nullptr) {
            ::GlobalFree (h);
            Execution::Throw (bad_alloc{});
        }
        memcpy (clipBuf, buf, bufSize);
        ::GlobalUnlock (h);
    }
    if (::SetClipboardData (clipFormat, h) == nullptr) {
        DWORD err = ::GetLastError ();
        ThrowIfErrorHRESULT (MAKE_HRESULT (SEVERITY_ERROR, FACILITY_WIN32, err));
    }
#elif qStroika_FeatureSupported_XWindows
    ReaderClipboardFlavorPackage::sPrivateClipData.insert (map<Led_ClipFormat, vector<char>>::value_type (
        clipFormat, vector<char> (reinterpret_cast<const char*> (buf), reinterpret_cast<const char*> (buf) + bufSize)));
#endif
}
#endif

/*
 ********************************************************************************
 ***************************** ReadWriteMemBufferPackage ************************
 ********************************************************************************
 */
ReadWriteMemBufferPackage::ReadWriteMemBufferPackage ()
    : ReaderFlavorPackage{}
    , WriterFlavorPackage{}
    , fPackages{}
{
    // Tuned to the MWERKS CWPro1 STL implementation. If you don't call vector::reserve () it uses 361 (pagesize/size(T) apx)
    // which means this is HUGE. And since we keep several of these (one per char typed for undo), it helps mem usage
    // alot to keep this down.
    //
    // Even though tuned to Mac code, should work pretty well in any case.

    //fPackages.SetSlotsAlloced (1);
    // HAVENT TESTED WITH STLLIB - BUT Probably a good idea here too - LGP 980923
    fPackages.reserve (1);
}

ReadWriteMemBufferPackage::~ReadWriteMemBufferPackage ()
{
}

bool ReadWriteMemBufferPackage::GetFlavorAvailable (Led_ClipFormat clipFormat) const
{
    for (size_t i = 0; i < fPackages.size (); i++) {
        if (fPackages[i].fFormat == clipFormat) {
            return true;
        }
    }
    return false;
}

size_t ReadWriteMemBufferPackage::GetFlavorSize (Led_ClipFormat clipFormat) const
{
    for (size_t i = 0; i < fPackages.size (); i++) {
        if (fPackages[i].fFormat == clipFormat) {
            return fPackages[i].fData.size ();
        }
    }
    Assert (false);
    return 0;
}

size_t ReadWriteMemBufferPackage::ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const
{
    for (size_t i = 0; i < fPackages.size (); i++) {
        if (fPackages[i].fFormat == clipFormat) {
            size_t copyNBytes = min (bufSize, fPackages[i].fData.size ());
            memcpy (buf, Traversal::Iterator2Pointer (fPackages[i].fData.begin ()), copyNBytes);
            Ensure (copyNBytes <= bufSize);
            return copyNBytes;
        }
    }
    Assert (false);
    return 0;
}

void ReadWriteMemBufferPackage::AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)
{
    PackageRecord pr;
    pr.fFormat     = clipFormat;
    const char* cb = reinterpret_cast<const char*> (buf);
    pr.fData       = vector<char> (cb, cb + bufSize);
    fPackages.push_back (pr);
}
