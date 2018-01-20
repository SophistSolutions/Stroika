/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#if qPlatform_MacOS
#include <QDOffscreen.h>
#include <TextEdit.h> // for Apple TE scrap format and TEContinuous etc compatability
#endif

#include "StandardStyledTextImager.h"

#include "StyledTextEmbeddedObjects.h"

/**
 *  @todo   Must fix to properly support 32-bit and 64-bit safety
 */
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4267)
#endif

#if qUseQuicktimeForWindows
#include "QTW.H"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

// Hack to free up registry data on program exit. No real point to this but
// to shutup MFC leak detector. That has SOME value, since it makes it easier to see REAL
// leaks.
static struct FooBarBlatzRegistryCleanupHack {
    ~FooBarBlatzRegistryCleanupHack ()
    {
        delete EmbeddedObjectCreatorRegistry::sThe;
        EmbeddedObjectCreatorRegistry::sThe = nullptr;
    }
} sOneOfThese;

#if qPlatform_MacOS || qPlatform_Windows
static void MacPictureDrawSegment (StandardMacPictureStyleMarker::PictureHandle pictureHandle,
                                   Led_Tablet                                   tablet,
                                   Led_Color foreColor, Led_Color backColor,
                                   const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
                                   const Led_Size& imageSize,
                                   const Led_Size& margin = kDefaultEmbeddingMargin) noexcept;
#endif
static void DIBDrawSegment (const Led_DIB* dib,
                            Led_Tablet     tablet,
                            Led_Color foreColor, Led_Color backColor,
                            const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
                            const Led_Size& imageSize,
                            const Led_Size& margin = kDefaultEmbeddingMargin) noexcept;
#if qPlatform_MacOS
static PixMap** MakePixMapFromDIB (const Led_DIB* dib);
#endif

struct UnsupportedFormat {
};

/*
 ********************************************************************************
 ************************** EmbeddedObjectCreatorRegistry ***********************
 ********************************************************************************
 */
EmbeddedObjectCreatorRegistry* EmbeddedObjectCreatorRegistry::sThe = nullptr;

void EmbeddedObjectCreatorRegistry::AddStandardTypes ()
{
#if qPlatform_MacOS || qPlatform_Windows
    AddAssoc (StandardMacPictureStyleMarker::kClipFormat, StandardMacPictureStyleMarker::kEmbeddingTag, &StandardMacPictureStyleMarker::mk, &StandardMacPictureStyleMarker::mk);
    AddAssoc (StandardDIBStyleMarker::kClipFormat, StandardDIBStyleMarker::kEmbeddingTag, &StandardDIBStyleMarker::mk, &StandardDIBStyleMarker::mk);
#endif

    AddAssoc (StandardURLStyleMarker::kURLDClipFormat, StandardURLStyleMarker::kEmbeddingTag, &StandardURLStyleMarker::mk, &StandardURLStyleMarker::mk);
#if qPlatform_Windows
    AddAssoc (StandardURLStyleMarker::kWin32URLClipFormat, StandardURLStyleMarker::kEmbeddingTag, &StandardURLStyleMarker::mk, &StandardURLStyleMarker::mk);
#endif

#if qPlatform_MacOS || qPlatform_Windows
    AddAssoc (StandardMacPictureWithURLStyleMarker::kClipFormats, StandardMacPictureWithURLStyleMarker::kClipFormatCount, StandardMacPictureWithURLStyleMarker::kEmbeddingTag, &StandardMacPictureWithURLStyleMarker::mk, &StandardMacPictureWithURLStyleMarker::mk);
    AddAssoc (StandardMacPictureWithURLStyleMarker::kOld1EmbeddingTag, &StandardMacPictureWithURLStyleMarker::mk);
    AddAssoc (StandardDIBWithURLStyleMarker::kClipFormats, StandardDIBWithURLStyleMarker::kClipFormatCount, StandardDIBWithURLStyleMarker::kEmbeddingTag, &StandardDIBWithURLStyleMarker::mk, &StandardDIBWithURLStyleMarker::mk);
#endif
}

bool EmbeddedObjectCreatorRegistry::Lookup (const char* embeddingTag, Assoc* result) const
{
    RequireNotNull (embeddingTag);
    RequireNotNull (result);
    const vector<EmbeddedObjectCreatorRegistry::Assoc>& types = GetAssocList ();
    for (size_t i = 0; i < types.size (); i++) {
        EmbeddedObjectCreatorRegistry::Assoc assoc = types[i];
        if (memcmp (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag)) == 0) {
            *result = assoc;
            return true;
        }
    }
    return false;
}

/*
 ********************************************************************************
 ********************* SimpleEmbeddedObjectStyleMarker **************************
 ********************************************************************************
 */
SimpleEmbeddedObjectStyleMarker::CommandNames SimpleEmbeddedObjectStyleMarker::sCommandNames = SimpleEmbeddedObjectStyleMarker::MakeDefaultCommandNames ();

SimpleEmbeddedObjectStyleMarker::SimpleEmbeddedObjectStyleMarker ()
    : inherited ()
{
}

int SimpleEmbeddedObjectStyleMarker::GetPriority () const
{
    return eEmbeddedObjectPriority;
}

Led_Distance SimpleEmbeddedObjectStyleMarker::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
{
    // Baseline for embeddings should be very bottom of the embedding (less the bottom margin)
    Require (from + 1 == to);
    return (MeasureSegmentHeight (imager, runElement, from, to) - 1 * kDefaultEmbeddingMargin.v);
}

void SimpleEmbeddedObjectStyleMarker::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) noexcept
{
    RequireNotNull (GetOwner ());
    RequireNotNull (GetOwner ()->PeekAtTextStore ());
    size_t newLength = GetLength ();
    if (newLength == 0) {
        GetOwner ()->PeekAtTextStore ()->RemoveMarker (this);
        delete this;
    }
    else if (newLength == 1) {
        inherited::DidUpdateText (updateInfo);
    }
    else {
        Assert (GetEnd () > 0); // cuz otherwise we couldn't have a non-zero length!
        GetOwner ()->PeekAtTextStore ()->SetMarkerStart (this, GetEnd () - 1);
        inherited::DidUpdateText (updateInfo);
    }
}

/*
@METHOD:        SimpleEmbeddedObjectStyleMarker::HandleClick
@DESCRIPTION:   <p>Called when a user clicks on the given embedding. 'clickedAt' is relative to the embedding itself.</p>
*/
bool SimpleEmbeddedObjectStyleMarker::HandleClick (Led_Point /*clickedAt*/, unsigned clickCount)
{
    if (clickCount == 2) {
        return HandleOpen ();
    }
    return true;
}

bool SimpleEmbeddedObjectStyleMarker::HandleOpen ()
{
    return true;
}

/*
@METHOD:        SimpleEmbeddedObjectStyleMarker::GetCmdNumbers
@DESCRIPTION:   <p>Return a list of all private commands (such as eOpenCmdNum) which this embedding object can support.
            See also @'SimpleEmbeddedObjectStyleMarker::IsCmdEnabled' (), @'SimpleEmbeddedObjectStyleMarker::GetCmdText' (),
            and @'SimpleEmbeddedObjectStyleMarker::DoCommand' () which also must be supported for each of those commands.</p>
*/
vector<SimpleEmbeddedObjectStyleMarker::PrivateCmdNumber> SimpleEmbeddedObjectStyleMarker::GetCmdNumbers () const
{
    return vector<PrivateCmdNumber> ();
}

/*
@METHOD:        SimpleEmbeddedObjectStyleMarker::IsCmdEnabled
@DESCRIPTION:   <p>For all commands specified in overrides of @'SimpleEmbeddedObjectStyleMarker::GetCmdNumbers' (ie all private commands
            supported) return whether or not that command is currently enabled.</p>
*/
bool SimpleEmbeddedObjectStyleMarker::IsCmdEnabled (PrivateCmdNumber /*cmd*/) const
{
    return false; // default to disabled - subclasses OVERRIDE SimpleEmbeddedObjectStyleMarker::GetCmdNumbers () && SimpleEmbeddedObjectStyleMarker::IsCmdEnabled ()
}

/*
@METHOD:        SimpleEmbeddedObjectStyleMarker::GetCmdText
@DESCRIPTION:   <p>For all commands specified in overrides of @'SimpleEmbeddedObjectStyleMarker::GetCmdNumbers' (ie all private commands
            supported) return the command name text.</p>
*/
Led_SDK_String SimpleEmbeddedObjectStyleMarker::GetCmdText (PrivateCmdNumber cmd)
{
    switch (cmd) {
        case eOpenCmdNum:
            return GetCommandNames ().fOpenCommandName;
        default:
            Assert (false);
            return Led_SDK_String ();
    }
}

/*
@METHOD:        SimpleEmbeddedObjectStyleMarker::DoCommand
@DESCRIPTION:   <p>For all commands specified in overrides of @'SimpleEmbeddedObjectStyleMarker::GetCmdNumbers' (ie all private commands
            supported) perform that command. This is invoked by the menu commands, etc (for example). The only command automatically
            handled by this implementation (@'SimpleEmbeddedObjectStyleMarker::DoCommand') is for eOpenCmdNum, and that just invokes
            @'SimpleEmbeddedObjectStyleMarker::HandleOpen' (). For other command numbers, the subclasser must handle the
            commands themselves.</p>
*/
void SimpleEmbeddedObjectStyleMarker::DoCommand (PrivateCmdNumber cmd)
{
    switch (cmd) {
        case eOpenCmdNum:
            HandleOpen ();
            break;
        default:
            Assert (false);
            break;
    }
}

SimpleEmbeddedObjectStyleMarker::CommandNames SimpleEmbeddedObjectStyleMarker::MakeDefaultCommandNames ()
{
    SimpleEmbeddedObjectStyleMarker::CommandNames cmdNames;
    cmdNames.fOpenCommandName = Led_SDK_TCHAROF ("Open Embedding");
    return cmdNames;
}

#if qPlatform_MacOS || qPlatform_Windows
/*
 ********************************************************************************
 ************************** StandardMacPictureStyleMarker ***********************
 ********************************************************************************
 */
#if qPlatform_MacOS
const Led_ClipFormat StandardMacPictureStyleMarker::kClipFormat = kPICTClipFormat;
#elif qPlatform_Windows
// Surprising, the QuickTime for windows 2.1 picture viewer doesn't appear to export (on copy) a
// native format rep of the picture. So I've no guess what the RIGHT arg is to RegisterClipboardFormat.
// So - this is as good a guess as any. At least then Led can xfer pictures among instances of itself.
// LGP 960429
const Led_ClipFormat StandardMacPictureStyleMarker::kClipFormat = ::RegisterClipboardFormat (_T ("Mac PICT Format"));
#endif
const Led_PrivateEmbeddingTag StandardMacPictureStyleMarker::kEmbeddingTag = "Pict2";

#if qPlatform_Windows
const Led_DIB* StandardMacPictureStyleMarker::sUnsupportedFormatPict = nullptr;
#endif

StandardMacPictureStyleMarker::StandardMacPictureStyleMarker (const Led_Picture* pictData, size_t picSize)
    : SimpleEmbeddedObjectStyleMarker ()
    , fPictureHandle (nullptr)
#if qPlatform_Windows
    , fPictureSize (0)
#endif
{
    RequireNotNull (pictData);
#if qPlatform_Windows
    RequireNotNull (sUnsupportedFormatPict);
#endif
#if qPlatform_MacOS
    fPictureHandle = (PictureHandle)Led_DoNewHandle (picSize);
#elif qPlatform_Windows
    fPictureSize   = picSize;
    fPictureHandle = ::GlobalAlloc (GMEM_MOVEABLE, picSize);
    Led_ThrowIfNull (fPictureHandle);
#endif
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    memcpy (locker.GetPointer (), pictData, picSize);
}

StandardMacPictureStyleMarker::~StandardMacPictureStyleMarker ()
{
    AssertNotNull (fPictureHandle);
#if qPlatform_MacOS
    ::DisposeHandle (Handle (fPictureHandle));
#else
    ::GlobalFree (fPictureHandle);
#endif
}

SimpleEmbeddedObjectStyleMarker* StandardMacPictureStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
    Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
    Led_Arg_Unused (embeddingTag);
    return (new StandardMacPictureStyleMarker ((Led_Picture*)data, len));
}

SimpleEmbeddedObjectStyleMarker* StandardMacPictureStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
    size_t                              length = flavorPackage.GetFlavorSize (kClipFormat);
    Memory::SmallStackBuffer<Led_tChar> buf (length);
    length = flavorPackage.ReadFlavorData (kClipFormat, length, buf);
    return (mk (kEmbeddingTag, buf, length));
}

void StandardMacPictureStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
                                                 size_t from, size_t to, const TextLayoutBlock& text,
                                                 const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    Assert (from + 1 == to);
    Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    MacPictureDrawSegment (GetPictureHandle (), tablet,
                           imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
                           drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetMacPictSize ((Led_Picture*)locker.GetPointer ()));
}

void StandardMacPictureStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
                                                         const Led_tChar* text,
                                                         Led_Distance*    distanceResults) const
{
    Assert (from + 1 == to);
    RequireNotNull (text);
    /*
     *  Though we generally require that:
     *
     *      Require (text[0] == kEmbeddingSentinalChar);
     *
     *  we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
     *  in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
     *  when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
     *  character here. We know the right width here anyhow.
     *      See SPR#0821.
     */
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    distanceResults[0] = Led_GetMacPictWidth ((Led_Picture*)locker.GetPointer ()) + 2 * kDefaultEmbeddingMargin.h;
}

Led_Distance StandardMacPictureStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
    Assert (from + 1 == to);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    return (Led_GetMacPictHeight ((Led_Picture*)locker.GetPointer ()) + 2 * kDefaultEmbeddingMargin.v);
}

void StandardMacPictureStyleMarker::Write (SinkStream& sink)
{
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    sink.write (locker.GetPointer (), GetPictureByteSize ());
}

void StandardMacPictureStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    flavorPackage.AddFlavorData (kClipFormat, GetPictureByteSize (), (Led_Picture*)locker.GetPointer ());
}

const char* StandardMacPictureStyleMarker::GetTag () const
{
    return kEmbeddingTag;
}
#endif

/*
 ********************************************************************************
 ************************** StandardDIBStyleMarker ******************************
 ********************************************************************************
 */
#if qPlatform_MacOS
// I don't know of any standard type for this, so just make one up...
// LGP 960429
const Led_ClipFormat StandardDIBStyleMarker::kClipFormat = 'DIB ';
#elif qPlatform_Windows
const Led_ClipFormat StandardDIBStyleMarker::kClipFormat = CF_DIB;
#elif qXWindows
const Led_ClipFormat StandardDIBStyleMarker::kClipFormat     = 666;    // X-TMP-HACK-LGP991214
#endif
const Led_PrivateEmbeddingTag StandardDIBStyleMarker::kEmbeddingTag = "DIB";

#if qPlatform_MacOS
Led_Picture** StandardDIBStyleMarker::sUnsupportedFormatPict = nullptr;
#endif

StandardDIBStyleMarker::StandardDIBStyleMarker (const Led_DIB* pictData)
    : SimpleEmbeddedObjectStyleMarker ()
    , fDIBData (nullptr)
{
#if qPlatform_MacOS
    RequireNotNull (sUnsupportedFormatPict); // see class declaration for descriptio
#endif
    RequireNotNull (pictData);
    fDIBData = Led_CloneDIB (pictData);
}

StandardDIBStyleMarker::~StandardDIBStyleMarker ()
{
    AssertNotNull (fDIBData);
    delete fDIBData;
}

SimpleEmbeddedObjectStyleMarker* StandardDIBStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
    Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
    Led_Arg_Unused (embeddingTag);
    if (len < 40) {
        // This is less than we need to peek and see size of DIB...
        Led_ThrowBadFormatDataException ();
    }

    size_t picSize = Led_GetDIBImageByteCount ((Led_DIB*)data);
    // It appears that sometimes (at least on NT 4.0 beta) that the length of the flavor package is longer
    // than we expect. I'm guessing this is some optional additional information MS now passes along.
    // The picts APPEAR to come out looking fine if we ignore this stuff of the end. So continue doing so...
    if (len != picSize) {
        // Set a breakpoint here if this worries you...
        if (len < picSize) {
            // This is definitely bad!!!
            Led_ThrowBadFormatDataException ();
        }
        else {
            // we'll just ignore the stuff off the end... Hope thats OK - LGP 960429
        }
    }
    return (new StandardDIBStyleMarker ((Led_DIB*)data));
}

SimpleEmbeddedObjectStyleMarker* StandardDIBStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
    size_t                              length = flavorPackage.GetFlavorSize (kClipFormat);
    Memory::SmallStackBuffer<Led_tChar> buf (length);
    length = flavorPackage.ReadFlavorData (kClipFormat, length, buf);
    return (mk (kEmbeddingTag, buf, length));
}

void StandardDIBStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text,
                                          const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    Assert (from + 1 == to);
    Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    DIBDrawSegment (fDIBData, tablet,
                    imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
                    drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetDIBImageSize (fDIBData));
}

void StandardDIBStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const
{
    Assert (from + 1 == to);
    RequireNotNull (text);
    /*
     *  Though we generally require that:
     *
     *      Require (text[0] == kEmbeddingSentinalChar);
     *
     *  we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
     *  in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
     *  when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
     *  character here. We know the right width here anyhow.
     *      See SPR#0821.
     */
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    distanceResults[0] = Led_GetDIBImageSize (GetDIBData ()).h + 2 * kDefaultEmbeddingMargin.h;
}

Led_Distance StandardDIBStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
    Assert (from + 1 == to);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    return (Led_GetDIBImageSize (GetDIBData ()).v + 2 * kDefaultEmbeddingMargin.v);
}

void StandardDIBStyleMarker::Write (SinkStream& sink)
{
    const Led_DIB* dib     = GetDIBData ();
    size_t         dibSize = Led_GetDIBImageByteCount (dib);
    sink.write (dib, dibSize);
}

void StandardDIBStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
    const Led_DIB* dib     = GetDIBData ();
    size_t         dibSize = Led_GetDIBImageByteCount (dib);
    flavorPackage.AddFlavorData (kClipFormat, dibSize, dib);
}

const char* StandardDIBStyleMarker::GetTag () const
{
    return kEmbeddingTag;
}

/*
 ********************************************************************************
 ****************************** StandardURLStyleMarker **************************
 ********************************************************************************
 */
#if qPlatform_MacOS
const Led_ClipFormat StandardURLStyleMarker::kURLDClipFormat = 'URLD';
#elif qPlatform_Windows
// Netscape USED to have some sort of predefined name like Netscsape Bookmark, or something like that.
// Apparently not any more. Will have to investigate further to see what todo for Netscape...
// LGP 960429
const Led_ClipFormat StandardURLStyleMarker::kURLDClipFormat     = ::RegisterClipboardFormat (_T ("Netscape Bookmark"));
const Led_ClipFormat StandardURLStyleMarker::kWin32URLClipFormat = ::RegisterClipboardFormat (_T ("UniformResourceLocator"));
#elif qXWindows
const Led_ClipFormat StandardURLStyleMarker::kURLDClipFormat = 'URLD'; //??? NOT SURE WHAT RIGHT ANSWER SB HERE!!!
#endif

#if !qURLStyleMarkerNewDisplayMode
#if qPlatform_MacOS
Led_Picture** StandardURLStyleMarker::sURLPict = nullptr;
#elif qPlatform_Windows
const Led_DIB* StandardURLStyleMarker::sURLPict = nullptr;
#endif

#endif
const Led_PrivateEmbeddingTag StandardURLStyleMarker::kEmbeddingTag = "URL";

StandardURLStyleMarker::StandardURLStyleMarker (const Led_URLD& urlData)
    : SimpleEmbeddedObjectStyleMarker ()
    , fURLData (urlData)
{
#if !qURLStyleMarkerNewDisplayMode
#if qPlatform_MacOS || qPlatform_Windows
    RequireNotNull (sURLPict); // If this is ever triggered, see class declaration where we delcare this field
#endif
#endif
}

StandardURLStyleMarker::~StandardURLStyleMarker ()
{
}

SimpleEmbeddedObjectStyleMarker* StandardURLStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
    Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
    Led_Arg_Unused (embeddingTag);
    return (new StandardURLStyleMarker (Led_URLD (data, len)));
}

SimpleEmbeddedObjectStyleMarker* StandardURLStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
    /*
     *  First try URLD format, and then Win32URL format.
     */
    if (flavorPackage.GetFlavorAvailable (kURLDClipFormat)) {
        size_t                              length = flavorPackage.GetFlavorSize (kURLDClipFormat);
        Memory::SmallStackBuffer<Led_tChar> buf (length);
        length = flavorPackage.ReadFlavorData (kURLDClipFormat, length, buf);
        return (mk (kEmbeddingTag, buf, length));
    }
#if qPlatform_Windows
    if (flavorPackage.GetFlavorAvailable (kWin32URLClipFormat)) {
        size_t                         length = flavorPackage.GetFlavorSize (kWin32URLClipFormat);
        Memory::SmallStackBuffer<char> buf (length);
        length = flavorPackage.ReadFlavorData (kWin32URLClipFormat, length, buf);
        // tmp/medium term hack.. Seems both formats look roughly the same. URL first. Then title. At least for now,
        // we can take advtangage of this and share code on read. If not, we can read/reformat to fit so this below works.
        // LGP 961012
        return (mk (kEmbeddingTag, buf, length));
    }
#endif
    Led_ThrowBadFormatDataException ();
    Assert (false);
    return nullptr;
}

void StandardURLStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text,
                                          const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    RequireNotNull (imager);

#if qURLStyleMarkerNewDisplayMode
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    Led_FontSpecification fsp         = GetDisplayFont (runElement);
    Led_tString           displayText = GetDisplayString ();
    imager->DrawSegment_ (tablet, fsp, from, from + displayText.length (), TextLayoutBlock_Basic (displayText.c_str (), displayText.c_str () + displayText.length ()),
                          drawInto, useBaseLine, pixelsDrawn);
#else
    Led_Distance width = 0;
    MeasureSegmentWidth (imager, runElement, from, to, text, &width);
    width -= 2 * kDefaultEmbeddingMargin.h;
    Led_Distance height = MeasureSegmentHeight (imager, runElement, from, to) - 2 * kDefaultEmbeddingMargin.v;

    Led_Rect ourBoundsRect     = drawInto - Led_Point (0, imager->GetHScrollPos ());
    ourBoundsRect.right        = ourBoundsRect.left + width + 2 * kDefaultEmbeddingMargin.h;
    Led_Coordinate embedBottom = useBaseLine;
    Led_Coordinate embedTop    = embedBottom - height;
    Assert (embedTop >= drawInto.top);
    Assert (embedBottom <= drawInto.bottom);
    Led_Rect innerBoundsRect = Led_Rect (Led_Point (embedTop, ourBoundsRects.left + kDefaultEmbeddingMargin.h), Led_Size (height, width));

    Led_Color foreColor = imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor);
    Led_Color backColor = imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor);

#if qPlatform_MacOS
    tablet->SetPort ();
#endif

#if qPlatform_MacOS
    GDI_RGBForeColor (foreColor.GetOSRep ());
    GDI_RGBBackColor (backColor.GetOSRep ());
#elif qPlatform_Windows
    tablet->SetTextColor (foreColor.GetOSRep ());
    tablet->SetBkColor (backColor.GetOSRep ());
#endif

    const char* url       = fURLData.PeekAtURLD ();
    const char* urlEnd    = ::strchr (url, '\r');
    size_t      urlStrLen = (urlEnd == nullptr) ? strlen (url) : (urlEnd - url);
    const char* name      = ::strchr (fURLData.PeekAtURLD (), '\r');
    if (name != nullptr) {
        name++; // skip CR
    }
    size_t nameStrLen = (name == nullptr) ? 0 : ::strlen (name);

/*
     *  Draw the marker on the baseline (to from the top of the drawing area).
     *  And be sure to erase everything in the draw rect!
     */
#if qPlatform_MacOS || qPlatform_Windows
    AssertNotNull (sURLPict);
#endif
#if qPlatform_MacOS
    Rect rr = AsQDRect (innerBoundsRect);
    PenNormal ();
    PenSize (2, 2);
    ::FrameRoundRect (&rr, 2, 2);
    Rect iconRect   = rr;
    iconRect.bottom = iconRect.top + Led_GetMacPictHeight (sURLPict);
    iconRect.right  = iconRect.left + Led_GetMacPictWidth (sURLPict);
    ::OffsetRect (&iconRect, 2, 2); // take into account border

    ::DrawPicture (sURLPict, &iconRect);

    ::MoveTo (iconRect.right + 3, iconRect.top + 16);
    ::TextFont (kFontIDTimes);
    ::TextSize (14);
    ::TextFace (0);
    ::DrawText (name, 0, nameStrLen);

    ::MoveTo (iconRect.right + 3, iconRect.top + 28);
    ::TextFont (kFontIDGeneva);
    ::TextSize (9);
    ::DrawText (url, 0, urlStrLen);
#elif qPlatform_Windows
    Led_Pen              pen (PS_SOLID, 2, RGB (0, 0, 0));
    Led_Win_Obj_Selector penWrapper (tablet, pen);
    Led_Win_Obj_Selector brush (tablet, ::GetStockObject (NULL_BRUSH));
    tablet->RoundRect (innerBoundsRect.left, innerBoundsRect.top, innerBoundsRect.right, innerBoundsRect.bottom, 2, 2);

    Led_Rect iconRect     = innerBoundsRect;
    Led_Size dibImageSize = Led_GetDIBImageSize (sURLPict);
    iconRect.bottom       = iconRect.top + dibImageSize.v;
    iconRect.right        = iconRect.left + dibImageSize.h;
    //const BITMAPINFOHEADER&   hdr         =   sURLPict->bmiHeader;
    const void* lpBits = Led_GetDIBBitsPointer (sURLPict);
    //const char*               lpBits      =   ((const char*)sURLPict) + Led_ByteSwapFromWindows (hdr.biSize) + Led_GetDIBPalletByteCount (sURLPict);
    ::StretchDIBits (tablet->m_hDC, iconRect.left, iconRect.top, iconRect.GetWidth (), iconRect.GetHeight (), 0, 0, dibImageSize.h, dibImageSize.v, lpBits, sURLPict, DIB_RGB_COLORS, SRCCOPY);

    Led_FontObject font1;
    {
        LOGFONT lf;
        memset (&lf, 0, sizeof lf);
        _tcscpy (lf.lfFaceName, _T ("System"));
        Verify (font1.CreateFontIndirect (&lf));
    }
    Led_Win_Obj_Selector font1Selector (tablet, font1);
    if (nameStrLen != 0) {
        ::TextOutA (*tablet, iconRect.right + 3, iconRect.top + 2, name, nameStrLen);
    }

    Led_FontObject font2;
    {
        LOGFONT lf;
        memset (&lf, 0, sizeof lf);
        _tcscpy (lf.lfFaceName, _T ("Arial"));
        Verify (font2.CreateFontIndirect (&lf));
        lf.lfHeight = -8;
    }
    Led_Win_Obj_Selector font2Selector (tablet, font2);
    if (urlStrLen != 0) {
        ::TextOutA (*tablet, iconRect.right + 3, iconRect.top + 16, url, urlStrLen);
    }
#endif
    if (pixelsDrawn != nullptr) {
        *pixelsDrawn = ourBoundsRect.GetWidth ();
    }
#endif
}

void StandardURLStyleMarker::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const
{
    Assert (from + 1 == to);
    RequireNotNull (text);
    /*
     *  Though we generally require that:
     *
     *      Require (text[0] == kEmbeddingSentinalChar);
     *
     *  we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
     *  in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
     *  when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
     *  character here. We know the right width here anyhow.
     *      See SPR#0821.
     */
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);

#if qURLStyleMarkerNewDisplayMode
    Led_FontSpecification fsp         = GetDisplayFont (runElement);
    Led_tString           displayText = GetDisplayString ();
    if (displayText.empty ()) {
        distanceResults[0] = 0;
    }
    else {
        Memory::SmallStackBuffer<Led_Distance> distRes (displayText.length ());
        imager->MeasureSegmentWidth_ (fsp, from, from + displayText.length (), displayText.c_str (), distRes);
        distanceResults[0] = distRes[displayText.length () - 1];
    }
#else
    const char* url       = fURLData.PeekAtURLD ();
    const char* urlEnd    = ::strchr (url, '\r');
    size_t      urlStrLen = (urlEnd == nullptr) ? strlen (url) : (urlEnd - url);
    const char* name      = ::strchr (fURLData.PeekAtURLD (), '\r');
    if (name != nullptr) {
        name++; // skip CR
    }
    size_t nameStrLen = (name == nullptr) ? 0 : ::strlen (name);

    TextInteractor::Tablet_Acquirer tablet_ (imager);
    Led_Tablet                      tablet = tablet_;

#if qPlatform_MacOS
    tablet->SetPort ();
#elif qPlatform_Windows
    Led_Tablet dc = tablet;
#endif

#if qPlatform_MacOS
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)sURLPict);
    // First add in width of picture
    distanceResults[0] = Led_GetMacPictWidth (sURLPict);

    distanceResults[0] += 2 * 3; // leave room for spacing around text

    // Leave room for text - largest of two strings
    ::TextFont (kFontIDTimes);
    ::TextSize (14);
    ::TextFace (0);
    Led_Distance string1Width = ::TextWidth (name, 0, nameStrLen);

    ::TextFont (kFontIDGeneva);
    ::TextSize (9);
    Led_Distance string2Width = ::TextWidth (url, 0, urlStrLen);

    distanceResults[0] += max (string1Width, string2Width) + 2 * kDefaultEmbeddingMargin.h;

#elif qPlatform_Windows
    // First add in width of picture
    distanceResults[0] = Led_GetDIBImageSize (sURLPict).h;

    distanceResults[0] += 2 * 3; // leave room for spacing around text

    // Leave room for text - largest of two strings
    Led_FontObject font1;
    {
        LOGFONT lf;
        memset (&lf, 0, sizeof lf);
        _tcscpy (lf.lfFaceName, _T ("System"));
        Verify (font1.CreateFontIndirect (&lf));
    }
    Led_Win_Obj_Selector font1Selector (tablet, font1);
    Led_Distance         string1Width = name == nullptr ? 0 : dc->GetTextExtent (Led_ANSI2SDKString (name).c_str (), nameStrLen).cx;

    Led_FontObject font2;
    {
        LOGFONT lf;
        memset (&lf, 0, sizeof lf);
        _tcscpy (lf.lfFaceName, _T ("Arial"));
        Verify (font2.CreateFontIndirect (&lf));
        lf.lfHeight = -8;
    }
    Led_Win_Obj_Selector font2Selector (tablet, font2);
    Led_Distance         string2Width = dc->GetTextExtent (Led_ANSI2SDKString (url).c_str (), urlStrLen).cx;

    distanceResults[0] += max (string1Width, string2Width) + 2 * kDefaultEmbeddingMargin.h;
#endif
#endif
}

Led_Distance StandardURLStyleMarker::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
{
    Assert (from + 1 == to);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);

#if qURLStyleMarkerNewDisplayMode
    Led_FontSpecification fsp         = GetDisplayFont (runElement);
    Led_tString           displayText = GetDisplayString ();
    return imager->MeasureSegmentHeight_ (fsp, from, from + displayText.length ());
#else
// this '36' is something of a hack. Really we should set the font into a grafport, and measure font metrics etc.
// but this is good enuf - I believe - LGP 960314
#if qPlatform_MacOS
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)sURLPict);
    return max (Led_GetMacPictHeight (sURLPict), 36) + 2 * kDefaultEmbeddingMargin.v;
#elif qPlatform_Windows
    return max (Led_GetDIBImageSize (sURLPict).v, 36) + 2 * kDefaultEmbeddingMargin.v;
#endif
#endif
}

void StandardURLStyleMarker::Write (SinkStream& sink)
{
    sink.write (fURLData.PeekAtURLD (), fURLData.GetURLDLength ());
}

void StandardURLStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
    flavorPackage.AddFlavorData (kURLDClipFormat, fURLData.GetURLDLength (), fURLData.PeekAtURLD ());
#if qPlatform_Windows
    size_t                         len = fURLData.GetURLLength () + 1;
    Memory::SmallStackBuffer<char> hackBuf (len);
    memcpy (hackBuf, fURLData.PeekAtURL (), len - 1);
    hackBuf[len] = '\0';
    flavorPackage.AddFlavorData (kWin32URLClipFormat, len, hackBuf);
#endif
}

const char* StandardURLStyleMarker::GetTag () const
{
    return kEmbeddingTag;
}

bool StandardURLStyleMarker::HandleOpen ()
{
    Led_URLManager::Get ().Open (fURLData.GetURL ());
    return false; // indicate double click 'eaten'
    //  return (HandleOpenURL (fURLData.PeekAtURLD ()));
}

vector<StandardURLStyleMarker::PrivateCmdNumber> StandardURLStyleMarker::GetCmdNumbers () const
{
    vector<PrivateCmdNumber> x;
    x.push_back (eOpenCmdNum);
    return x;
}

bool StandardURLStyleMarker::IsCmdEnabled (PrivateCmdNumber cmd) const
{
    switch (cmd) {
        case eOpenCmdNum:
            return true;
        default:
            return inherited::IsCmdEnabled (cmd);
    }
}

const Led_URLD& StandardURLStyleMarker::GetURLData () const
{
    return fURLData;
}

void StandardURLStyleMarker::SetURLData (const Led_URLD& urlData)
{
    fURLData = urlData;
}

Led_tString StandardURLStyleMarker::GetDisplayString () const
{
    string displayText = fURLData.GetTitle ();
    if (displayText.empty ()) {
        displayText = fURLData.GetURL ();
    }
    /*
     *  Replace tab characters with space characters, since tabs won't get aligned properly, and wouldn't make much
     *  sense anyhow (SPR#1131).
     */
    for (auto i = displayText.begin (); i != displayText.end (); ++i) {
        if (*i == '\t') {
            *i = ' ';
        }
    }
    return Led_ANSIString2tString (displayText);
}

Led_FontSpecification StandardURLStyleMarker::GetDisplayFont (const RunElement& runElement) const
{
    Led_FontSpecification fsp;
    if (dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (runElement.fMarker) != nullptr) {
        StandardStyledTextImager::StandardStyleMarker* sm = dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (runElement.fMarker);
        fsp                                               = sm->fFontSpecification;
    }
    else {
        for (auto i = runElement.fSupercededMarkers.begin (); i != runElement.fSupercededMarkers.end (); ++i) {
            StandardStyledTextImager::StandardStyleMarker* sm = dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (*i);
            if (sm != nullptr) {
                fsp = sm->fFontSpecification;
                break;
            }
        }
    }
    fsp.SetPointSize (static_cast<unsigned short> (fsp.GetPointSize () * 1.05));
    fsp.SetTextColor (Led_Color::kBlue);
    fsp.SetStyle_Underline (true);
    return fsp;
}

#if qPlatform_MacOS || qPlatform_Windows
/*
 ********************************************************************************
 ************************ StandardMacPictureWithURLStyleMarker ******************
 ********************************************************************************
 */
const Led_ClipFormat StandardMacPictureWithURLStyleMarker::kClipFormats[] = {
    kPICTClipFormat,
    StandardURLStyleMarker::kURLDClipFormat};
const size_t                  StandardMacPictureWithURLStyleMarker::kClipFormatCount  = sizeof (kClipFormats) / sizeof (kClipFormats[0]);
const Led_PrivateEmbeddingTag StandardMacPictureWithURLStyleMarker::kEmbeddingTag     = "Pict&URL";
const Led_PrivateEmbeddingTag StandardMacPictureWithURLStyleMarker::kOld1EmbeddingTag = "PictWEmbd"; // Only used in Led 2.0b7 - I believe - LGP 960427

StandardMacPictureWithURLStyleMarker::StandardMacPictureWithURLStyleMarker (const Led_Picture* pictData, size_t picSize, const Led_URLD& urlData)
    : SimpleEmbeddedObjectStyleMarker ()
    , fPictureHandle (nullptr)
    ,
#if qPlatform_Windows
    fPictureSize (0)
    ,
#endif
    fURLData (urlData)
{
    RequireNotNull (pictData);
#if qPlatform_MacOS
    fPictureHandle = (StandardMacPictureStyleMarker::PictureHandle)Led_DoNewHandle (picSize);
#elif qPlatform_Windows
    fPictureSize   = picSize;
    fPictureHandle = ::GlobalAlloc (GMEM_MOVEABLE, picSize);
    Led_ThrowIfNull (fPictureHandle);
#endif
    {
        Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
        memcpy (locker.GetPointer (), pictData, picSize);
    }
}

StandardMacPictureWithURLStyleMarker::~StandardMacPictureWithURLStyleMarker ()
{
    AssertNotNull (fPictureHandle);
#if qPlatform_MacOS
    ::DisposeHandle (Handle (fPictureHandle));
#elif qPlatform_Windows
    ::GlobalFree (fPictureHandle);
#endif
}

SimpleEmbeddedObjectStyleMarker* StandardMacPictureWithURLStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
    Require (memcmp (embeddingTag, kOld1EmbeddingTag, sizeof (kOld1EmbeddingTag)) == 0 or memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);

    if (memcmp (embeddingTag, kOld1EmbeddingTag, sizeof (kOld1EmbeddingTag)) == 0) {
        Led_Picture* picBuf  = (Led_Picture*)data;
        size_t       picSize = Led_ByteSwapFromMac (picBuf->picSize);

        if (picSize >= len) {
            Led_ThrowBadFormatDataException ();
        }
        const char* url     = ((char*)data) + picSize;
        size_t      urlSize = len - picSize;
        Assert (urlSize > 0); // cuz of above throw-test above...

        return new StandardMacPictureWithURLStyleMarker (picBuf, Led_ByteSwapFromMac (picBuf->picSize), Led_URLD (url, urlSize));
    }
    else {
        if (len < 4 + 1 + sizeof (Led_Picture)) {
            Led_ThrowBadFormatDataException ();
        }

        uint32_t picSize = *(uint32_t*)data;
        picSize          = Led_BufToULONG (&picSize);

        Led_Picture* picBuf = (Led_Picture*)((char*)data + 4);

        if (picSize + 4 >= len) { // must leave room for ULRD.
            Led_ThrowBadFormatDataException ();
        }
        const char* url     = ((char*)picBuf) + picSize;
        size_t      urlSize = len - 4 - picSize;
        Assert (urlSize > 0); // cuz of above throw-test above...

        return new StandardMacPictureWithURLStyleMarker (picBuf, picSize, Led_URLD (url, urlSize));
    }
}

SimpleEmbeddedObjectStyleMarker* StandardMacPictureWithURLStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
    size_t                         pictLength = flavorPackage.GetFlavorSize (kPICTClipFormat);
    Memory::SmallStackBuffer<char> buf1 (pictLength);
    pictLength          = flavorPackage.ReadFlavorData (kPICTClipFormat, pictLength, buf1);
    Led_Picture* picBuf = (Led_Picture*)(char*)buf1;

    size_t                              urlSize = flavorPackage.GetFlavorSize (StandardURLStyleMarker::kURLDClipFormat);
    Memory::SmallStackBuffer<Led_tChar> buf2 (urlSize);
    urlSize = flavorPackage.ReadFlavorData (StandardURLStyleMarker::kURLDClipFormat, urlSize, buf2);
    return new StandardMacPictureWithURLStyleMarker (picBuf, pictLength, Led_URLD (buf2, urlSize));
}

void StandardMacPictureWithURLStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
                                                        size_t from, size_t to, const TextLayoutBlock& text,
                                                        const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    Assert (from + 1 == to);
    Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    MacPictureDrawSegment (GetPictureHandle (), tablet,
                           imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
                           drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetMacPictSize ((Led_Picture*)locker.GetPointer ()));
}

void StandardMacPictureWithURLStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
                                                                const Led_tChar* text,
                                                                Led_Distance*    distanceResults) const
{
    Assert (from + 1 == to);
    RequireNotNull (text);
    /*
     *  Though we generally require that:
     *
     *      Require (text[0] == kEmbeddingSentinalChar);
     *
     *  we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
     *  in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
     *  when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
     *  character here. We know the right width here anyhow.
     *      See SPR#0821.
     */
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    distanceResults[0] = Led_GetMacPictWidth ((Led_Picture*)locker.GetPointer ()) + 2 * kDefaultEmbeddingMargin.h;
}

Led_Distance StandardMacPictureWithURLStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
    Assert (from + 1 == to);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    return (Led_GetMacPictHeight ((Led_Picture*)locker.GetPointer ()) + 2 * kDefaultEmbeddingMargin.v);
}

void StandardMacPictureWithURLStyleMarker::Write (SinkStream& sink)
{
    {
        uint32_t picSize = GetPictureByteSize ();
        Led_ULONGToBuf (picSize, &picSize);
        Assert (sizeof (picSize) == 4);
        sink.write (&picSize, sizeof (picSize));
    }
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    sink.write ((Led_Picture*)locker.GetPointer (), GetPictureByteSize ());
    sink.write (fURLData.PeekAtURLD (), fURLData.GetURLDLength ());
}

void StandardMacPictureWithURLStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)GetPictureHandle ());
    flavorPackage.AddFlavorData (StandardMacPictureStyleMarker::kClipFormat, GetPictureByteSize (), (Led_Picture*)locker.GetPointer ());
    flavorPackage.AddFlavorData (StandardURLStyleMarker::kURLDClipFormat, fURLData.GetURLDLength (), fURLData.PeekAtURLD ());
}

bool StandardMacPictureWithURLStyleMarker::HandleOpen ()
{
    Led_URLManager::Get ().Open (fURLData.GetURL ());
    return false; // indicate double click 'eaten'
}

vector<StandardMacPictureWithURLStyleMarker::PrivateCmdNumber> StandardMacPictureWithURLStyleMarker::GetCmdNumbers () const
{
    vector<PrivateCmdNumber> x;
    x.push_back (eOpenCmdNum);
    return x;
}

bool StandardMacPictureWithURLStyleMarker::IsCmdEnabled (PrivateCmdNumber cmd) const
{
    switch (cmd) {
        case eOpenCmdNum:
            return true;
        default:
            return inherited::IsCmdEnabled (cmd);
    }
}

const Led_URLD& StandardMacPictureWithURLStyleMarker::GetURLData () const
{
    return fURLData;
}

void StandardMacPictureWithURLStyleMarker::SetURLData (const Led_URLD& urlData)
{
    fURLData = urlData;
}

const char* StandardMacPictureWithURLStyleMarker::GetTag () const
{
    return kEmbeddingTag;
}
#endif

/*
 ********************************************************************************
 **************************** StandardDIBWithURLStyleMarker *********************
 ********************************************************************************
 */
const Led_ClipFormat StandardDIBWithURLStyleMarker::kClipFormats[] = {
    StandardDIBStyleMarker::kClipFormat,
    StandardURLStyleMarker::kURLDClipFormat};
const size_t                  StandardDIBWithURLStyleMarker::kClipFormatCount = sizeof (kClipFormats) / sizeof (kClipFormats[0]);
const Led_PrivateEmbeddingTag StandardDIBWithURLStyleMarker::kEmbeddingTag    = "DIB&URL";

StandardDIBWithURLStyleMarker::StandardDIBWithURLStyleMarker (const Led_DIB* dibData, const Led_URLD& urlData)
    : SimpleEmbeddedObjectStyleMarker ()
    , fDIBData (nullptr)
    , fURLData (urlData)
{
#if qPlatform_MacOS
    RequireNotNull (StandardDIBStyleMarker::sUnsupportedFormatPict); // see class declaration for descriptio
#endif
    RequireNotNull (dibData);
    fDIBData = Led_CloneDIB (dibData);
}

StandardDIBWithURLStyleMarker::~StandardDIBWithURLStyleMarker ()
{
    AssertNotNull (fDIBData);
    delete fDIBData;
}

SimpleEmbeddedObjectStyleMarker* StandardDIBWithURLStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
    Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
    Led_Arg_Unused (embeddingTag);

    if (len < 4 + 40) {
        // This is less than we need to peek and see size of DIB...
        Led_ThrowBadFormatDataException ();
    }

    uint32_t picSize = *(uint32_t*)data;
    picSize          = Led_BufToULONG (&picSize);

    Led_DIB* picBuf = (Led_DIB*)((char*)data + 4);

    if (len < picSize) {
        Led_ThrowBadFormatDataException ();
    }

    if (picSize + 4 >= len) { // must leave room for ULRD.
        Led_ThrowBadFormatDataException ();
    }
    const char* url     = ((char*)picBuf) + picSize;
    size_t      urlSize = len - 4 - picSize;
    Assert (urlSize > 0); // cuz of above throw-test above...

    return new StandardDIBWithURLStyleMarker (picBuf, Led_URLD (url, urlSize));
}

SimpleEmbeddedObjectStyleMarker* StandardDIBWithURLStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
    size_t                         length = flavorPackage.GetFlavorSize (StandardDIBStyleMarker::kClipFormat);
    Memory::SmallStackBuffer<char> buf (length);
    length = flavorPackage.ReadFlavorData (StandardDIBStyleMarker::kClipFormat, length, buf);
    if (length < 40) {
        // This is less than we need to peek and see size of DIB...
        Led_ThrowBadFormatDataException ();
    }

    size_t picSize = Led_GetDIBImageByteCount ((Led_DIB*)(char*)buf);
    // It appears that sometimes (at least on NT 4.0 beta) that the length of the flavor package is longer
    // than we expect. I'm guessing this is some optional additional information MS now passes along.
    // The picts APPEAR to come out looking fine if we ignore this stuff of the end. So continue doing so...
    if (length != picSize) {
        // Set a breakpoint here if this worries you...
        if (length < picSize) {
            // This is definitely bad!!!
            Led_ThrowBadFormatDataException ();
        }
        else {
            // we'll just ignore the stuff off the end... Hope thats OK - LGP 960429
        }
    }

    size_t                         urlSize = flavorPackage.GetFlavorSize (StandardURLStyleMarker::kURLDClipFormat);
    Memory::SmallStackBuffer<char> buf2 (urlSize);
    urlSize = flavorPackage.ReadFlavorData (StandardURLStyleMarker::kURLDClipFormat, urlSize, buf2);

    return new StandardDIBWithURLStyleMarker ((const Led_DIB*)(char*)buf, Led_URLD (buf2, urlSize));
}

void StandardDIBWithURLStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
                                                 size_t from, size_t to, const TextLayoutBlock& text,
                                                 const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    Assert (from + 1 == to);
    Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    DIBDrawSegment (fDIBData, tablet,
                    imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
                    drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetDIBImageSize (fDIBData));
}

void StandardDIBWithURLStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
                                                         const Led_tChar* text,
                                                         Led_Distance*    distanceResults) const
{
    Assert (from + 1 == to);
    RequireNotNull (text);
    /*
     *  Though we generally require that:
     *
     *      Require (text[0] == kEmbeddingSentinalChar);
     *
     *  we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
     *  in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
     *  when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
     *  character here. We know the right width here anyhow.
     *      See SPR#0821.
     */
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    distanceResults[0] = Led_GetDIBImageSize (GetDIBData ()).h + 2 * kDefaultEmbeddingMargin.h;
}

Led_Distance StandardDIBWithURLStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
    Assert (from + 1 == to);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    return (Led_GetDIBImageSize (GetDIBData ()).v + 2 * kDefaultEmbeddingMargin.v);
}

void StandardDIBWithURLStyleMarker::Write (SinkStream& sink)
{
    const Led_DIB* dib = GetDIBData ();
    {
        uint32_t dibSize = Led_GetDIBImageByteCount (dib);
        Led_ULONGToBuf (dibSize, &dibSize);
        Assert (sizeof (dibSize) == 4);
        sink.write (&dibSize, sizeof (dibSize));
    }
    sink.write (dib, Led_GetDIBImageByteCount (dib));
    sink.write (fURLData.PeekAtURLD (), fURLData.GetURLDLength ());
}

void StandardDIBWithURLStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
    const Led_DIB* dib     = GetDIBData ();
    size_t         dibSize = Led_GetDIBImageByteCount (dib);
    flavorPackage.AddFlavorData (StandardDIBStyleMarker::kClipFormat, dibSize, dib);
    flavorPackage.AddFlavorData (StandardURLStyleMarker::kURLDClipFormat, fURLData.GetURLDLength (), fURLData.PeekAtURLD ());
}

bool StandardDIBWithURLStyleMarker::HandleOpen ()
{
    Led_URLManager::Get ().Open (fURLData.GetURL ());
    return false; // indicate double click 'eaten'
}

vector<StandardDIBWithURLStyleMarker::PrivateCmdNumber> StandardDIBWithURLStyleMarker::GetCmdNumbers () const
{
    vector<PrivateCmdNumber> x;
    x.push_back (eOpenCmdNum);
    return x;
}

bool StandardDIBWithURLStyleMarker::IsCmdEnabled (PrivateCmdNumber cmd) const
{
    switch (cmd) {
        case eOpenCmdNum:
            return true;
        default:
            return inherited::IsCmdEnabled (cmd);
    }
}

const Led_URLD& StandardDIBWithURLStyleMarker::GetURLData () const
{
    return fURLData;
}

void StandardDIBWithURLStyleMarker::SetURLData (const Led_URLD& urlData)
{
    fURLData = urlData;
}

const char* StandardDIBWithURLStyleMarker::GetTag () const
{
    return kEmbeddingTag;
}

/*
 ********************************************************************************
 ************************* StandardUnknownTypeStyleMarker ***********************
 ********************************************************************************
 */
#if qPlatform_MacOS
Led_Picture** StandardUnknownTypeStyleMarker::sUnknownPict = nullptr;
#elif qPlatform_Windows
const Led_DIB* StandardUnknownTypeStyleMarker::sUnknownPict = nullptr;
#endif
const Led_PrivateEmbeddingTag StandardUnknownTypeStyleMarker::kDefaultEmbeddingTag = "UnknwnDlf";

StandardUnknownTypeStyleMarker::StandardUnknownTypeStyleMarker (Led_ClipFormat format, const char* embeddingTag, const void* unknownTypeData, size_t nBytes, const Led_DIB* dib)
    : SimpleEmbeddedObjectStyleMarker ()
    ,
    //  fShownSize (Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0))),
    fShownSize ()
    , fData (nullptr)
    , fLength (nBytes)
    , fFormat (format)
    ,
    //fEmbeddingTag (),
    fDisplayDIB ()
{
    memcpy (fEmbeddingTag, embeddingTag, sizeof (fEmbeddingTag));
#if qPlatform_MacOS || qPlatform_Windows
    RequireNotNull (sUnknownPict); // If this is ever triggered, see class declaration where we delcare this field
#endif
    fData = new char[nBytes];
    memcpy (fData, unknownTypeData, nBytes);

    if (dib != nullptr) {
#if qCannotAssignRValueAutoPtrToExistingOneInOneStepBug || qTroubleOverloadingXofXRefCTORWithTemplatedMemberCTOR
        unique_ptr<Led_DIB> x = unique_ptr<Led_DIB> (Led_CloneDIB (dib));
        fDisplayDIB           = x;
#else
        fDisplayDIB = unique_ptr<Led_DIB> (Led_CloneDIB (dib));
#endif
    }
    fShownSize = CalcDefaultShownSize ();
}

StandardUnknownTypeStyleMarker::~StandardUnknownTypeStyleMarker ()
{
    delete[](char*) fData;
}

/*
@METHOD:        StandardUnknownTypeStyleMarker::SetShownSize
@DESCRIPTION:   <p>See @'StandardUnknownTypeStyleMarker::GetShownSize', or
            @'StandardUnknownTypeStyleMarker::CalcDefaultShownSize'.</p>
                <p>NB: it is the CALLERs responsability to assure the appropriate
            TextInteractors/TextImagers are notified to adjust any caching of size information they may have. This can be avoided
            by setting this value BEFORE adding the embedding to the TextStore.</p>
*/
void StandardUnknownTypeStyleMarker::SetShownSize (Led_TWIPS_Point size)
{
    fShownSize = size;
}

/*
@METHOD:        StandardUnknownTypeStyleMarker::CalcDefaultShownSize
@DESCRIPTION:   <p>See @'StandardUnknownTypeStyleMarker::GetShownSize'.</p>
                <p>Generates a reasonable default size (based on sUnknownPict size) for the embedding. Used unless
            overridden by calls to @'StandardUnknownTypeStyleMarker::SetShownSize'.</p>
*/
Led_TWIPS_Point StandardUnknownTypeStyleMarker::CalcDefaultShownSize ()
{
    if (fDisplayDIB.get () != nullptr) {
        Led_Size pixelSize = Led_GetDIBImageSize (fDisplayDIB.get ());
        return Led_TWIPS_Point (Led_CvtScreenPixelsToTWIPSV (pixelSize.v), Led_CvtScreenPixelsToTWIPSH (pixelSize.h));
    }
    return CalcStaticDefaultShownSize ();
}

Led_TWIPS_Point StandardUnknownTypeStyleMarker::CalcStaticDefaultShownSize ()
{
#if qPlatform_MacOS
    RequireNotNull (sUnknownPict);
    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)sUnknownPict);
    Led_Size                   pixelSize = Led_GetMacPictSize (sUnknownPict);
#elif qPlatform_Windows
    RequireNotNull (sUnknownPict);
    Led_Size pixelSize = Led_GetDIBImageSize (sUnknownPict);
#elif qXWindows
    Led_Size pixelSize = Led_Size (10, 10);                            //  X-TMP-HACK-LGP2000-06-13
#endif

    return Led_TWIPS_Point (Led_CvtScreenPixelsToTWIPSV (pixelSize.v), Led_CvtScreenPixelsToTWIPSH (pixelSize.h));
}

void StandardUnknownTypeStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
                                                  size_t from, size_t to, const TextLayoutBlock& text,
                                                  const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    Assert (from + 1 == to);
    Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    Led_Size shownPixelSize = Led_Size (tablet->CvtFromTWIPSV (fShownSize.v), tablet->CvtFromTWIPSH (fShownSize.h));
    ;
    if (fDisplayDIB.get () != nullptr) {
        DIBDrawSegment (fDisplayDIB.get (), tablet,
                        imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
                        drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, shownPixelSize);
        return;
    }
#if qPlatform_MacOS
    MacPictureDrawSegment (sUnknownPict, tablet,
                           imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
                           drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, shownPixelSize);
#elif qPlatform_Windows
    DIBDrawSegment (sUnknownPict, tablet,
                    imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
                    drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, shownPixelSize);
#endif
}

void StandardUnknownTypeStyleMarker::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to,
                                                          const Led_tChar* text,
                                                          Led_Distance*    distanceResults) const
{
    Assert (from + 1 == to);
    RequireNotNull (text);
    /*
     *  Though we generally require that:
     *
     *      Require (text[0] == kEmbeddingSentinalChar);
     *
     *  we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
     *  in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
     *  when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
     *  character here. We know the right width here anyhow.
     *      See SPR#0821.
     */
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);
    Led_Arg_Unused (text);
    if (fDisplayDIB.get () != nullptr) {
        distanceResults[0] = Led_GetDIBImageSize (fDisplayDIB.get ()).h + 2 * kDefaultEmbeddingMargin.h;
        return;
    }
    TextInteractor::Tablet_Acquirer tablet_ (imager);
    Led_Tablet                      tablet = tablet_;
    distanceResults[0]                     = tablet->CvtFromTWIPSH (fShownSize.h) + 2 * kDefaultEmbeddingMargin.h;
}

Led_Distance StandardUnknownTypeStyleMarker::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to) const
{
    Assert (from + 1 == to);
    Led_Arg_Unused (from);
    Led_Arg_Unused (to);

    if (fDisplayDIB.get () != nullptr) {
        return (Led_GetDIBImageSize (fDisplayDIB.get ()).v + 2 * kDefaultEmbeddingMargin.v);
    }
    TextInteractor::Tablet_Acquirer tablet_ (imager);
    Led_Tablet                      tablet = tablet_;
    return tablet->CvtFromTWIPSV (fShownSize.v) + 2 * kDefaultEmbeddingMargin.v;
}

void StandardUnknownTypeStyleMarker::Write (SinkStream& sink)
{
    sink.write (fData, fLength);
}

void StandardUnknownTypeStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
    flavorPackage.AddFlavorData (fFormat, fLength, fData);
}

const char* StandardUnknownTypeStyleMarker::GetTag () const
{
    return fEmbeddingTag;
}

/*
 ********************************************************************************
 ************************ InsertEmbeddingForExistingSentinal ********************
 ********************************************************************************
 */
void Led::InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding)
{
    RequireNotNull (embedding);
    RequireNotNull (ownerForEmbedding);
    TextStore::SimpleUpdater updater (textStore, insertAt, insertAt + 1);
    textStore.AddMarker (embedding, insertAt, 1, ownerForEmbedding);
}

/*
 ********************************************************************************
 ********************************* AddEmbedding *********************************
 ********************************************************************************
 */
void Led::AddEmbedding (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding)
{
    RequireNotNull (embedding);
    RequireNotNull (ownerForEmbedding);
    textStore.Replace (insertAt, insertAt, &kEmbeddingSentinalChar, 1);
    InsertEmbeddingForExistingSentinal (embedding, textStore, insertAt, ownerForEmbedding);
}

#if qPlatform_MacOS || qPlatform_Windows
/*
 ********************************************************************************
 ************************** MacPictureDrawSegment *******************************
 ********************************************************************************
 */
static void MacPictureDrawSegment (StandardMacPictureStyleMarker::PictureHandle pictureHandle,
                                   Led_Tablet tablet, Led_Color foreColor, Led_Color backColor, const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
                                   const Led_Size& imageSize,
                                   const Led_Size& margin) noexcept
{
    RequireNotNull (pictureHandle);

    Led_StackBasedHandleLocker locker ((Led_StackBasedHandleLocker::GenericHandle)pictureHandle);

#if qPlatform_MacOS
    tablet->SetPort ();
#elif qPlatform_Windows
    Led_Tablet dc = tablet;
#endif

    Led_Size pictSize = imageSize;

    Led_Rect ourBoundsRect = drawInto;
    ourBoundsRect.SetRight (ourBoundsRect.GetLeft () + pictSize.h + 2 * margin.h);
    Led_Coordinate embedBottom = useBaseLine;
    Led_Coordinate embedTop    = embedBottom - pictSize.v;
    Assert (embedTop >= drawInto.top);
    Assert (embedBottom <= drawInto.bottom);
    Led_Rect innerBoundsRect = Led_Rect (Led_Point (embedTop, drawInto.GetLeft () + margin.h), pictSize);

#if qPlatform_MacOS
    GDI_RGBForeColor (foreColor.GetOSRep ());
    GDI_RGBBackColor (backColor.GetOSRep ());
#elif qPlatform_Windows
    dc->SetTextColor (foreColor.GetOSRep ());
    dc->SetBkColor (backColor.GetOSRep ());
#endif

#if qPlatform_MacOS
    // Must erase above the picture, and below it. And
    Rect rr = AsQDRect (innerBoundsRect);

    // Now draw the actual picture
    ::DrawPicture (pictureHandle, &rr);
#elif qPlatform_Windows && qUseQuicktimeForWindows
    // I tried doing a LoadLibrary/GetProcAddress ("DrawPicture") so that I wouldn't need
    // to link directly against the QT library, and so I wouldn't get errors at start time
    // if the lib wasn't present. But apparantly Apple has done all that for me in the
    // staticly linked library they provide. I get no errors when I run on a system that
    // doesn't have QT installed til the call to QTInitialize(). And staticly linking
    // only adds 3K - presumably just the code which maps the APIs to the DLLs, and does
    // the checking to find the DLL.
    //
    // So we simply staticly link, and only call QTInitialize() as needed. And then call
    // QTTerminate at exit (via static DTOR hack), as needed.
    struct QTIniter {
        QTIniter ()
        {
            fGood = (::QTInitialize (nullptr) == 0);
        }
        ~QTIniter ()
        {
            if (fGood) {
                ::QTTerminate ();
            }
        }
        bool fGood;
    };
    static QTIniter      sIniter;
    RECT                 rr = AsRECT (innerBoundsRect);
    Led_Brush            eraseBrush (backColor.GetOSRep ());
    Led_Win_Obj_Selector brush (dc, eraseBrush);
    bool                 displaySuccessful = false;
    if (sIniter.fGood) {
        displaySuccessful = (::DrawPicture (dc->m_hDC, (PicHandle)pictureHandle, &rr, nullptr) == noErr);
    }
    if (not displaySuccessful) {
        // fill in with some other picture...
        // treat all excpetions the same. In principle, could draw different picst for memory and
        // unsupported format exceptions...
        const Led_DIB* dib = StandardMacPictureStyleMarker::sUnsupportedFormatPict;
        AssertNotNull (dib);
        Led_Size dibImageSize = Led_GetDIBImageSize (dib);
        //      const BITMAPINFOHEADER& hdr         =   dib->bmiHeader;
        const void* lpBits = Led_GetDIBBitsPointer (dib);
        //      const char*             lpBits      =   ((const char*)dib) + Led_ByteSwapFromWindows (hdr.biSize) + Led_GetDIBPalletByteCount (dib);
        ::StretchDIBits (dc->m_hDC, innerBoundsRect.left, innerBoundsRect.top, innerBoundsRect.GetWidth (), innerBoundsRect.GetHeight (), 0, 0, dibImageSize.h, dibImageSize.v, lpBits, dib, DIB_RGB_COLORS, SRCCOPY);
    }
#endif
    if (pixelsDrawn != nullptr) {
        *pixelsDrawn = ourBoundsRect.GetWidth ();
    }
}
#endif

static void DIBDrawSegment (const Led_DIB* dib,
                            Led_Tablet     tablet,
                            Led_Color foreColor, Led_Color backColor,
                            const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
                            const Led_Size& imageSize,
                            const Led_Size& margin) noexcept
{
    RequireNotNull (dib);
    RequireNotNull (tablet);
    Led_Arg_Unused (foreColor);
    Led_Arg_Unused (backColor);

#if qPlatform_MacOS
    tablet->SetPort ();
#elif qPlatform_Windows
    Led_Tablet dc = tablet;
#endif

    Led_Size dibImageSize = imageSize;

    Led_Rect ourBoundsRect     = drawInto;
    ourBoundsRect.right        = ourBoundsRect.left + dibImageSize.h + 2 * margin.h;
    Led_Coordinate embedBottom = useBaseLine;
    Led_Coordinate embedTop    = embedBottom - dibImageSize.v;
    Assert (embedTop >= drawInto.GetTop ());
    Assert (embedBottom <= drawInto.GetBottom ());
    Led_Rect innerBoundsRect = Led_Rect (Led_Point (embedTop, drawInto.GetLeft () + margin.h), dibImageSize);

    if (pixelsDrawn != nullptr) {
        *pixelsDrawn = ourBoundsRect.GetWidth ();
    }

#if qPlatform_MacOS
#if 1
    GDI_RGBForeColor (Led_Color::kBlack.GetOSRep ());
    GDI_RGBBackColor (Led_Color::kWhite.GetOSRep ());
#else
    GDI_RGBForeColor (foreColor.GetOSRep ());
    GDI_RGBBackColor (backColor.GetOSRep ());
#endif
#elif qPlatform_Windows
    dc->SetTextColor (foreColor.GetOSRep ());
    dc->SetBkColor (backColor.GetOSRep ());
#endif

#if qPlatform_MacOS
    // Must erase above the picture, and below it. And
    Rect rr = AsQDRect (innerBoundsRect);

    // Turn the DIB into a pixmap, and then image it, and free it again...
    try {
        PixMap** pm       = MakePixMapFromDIB (dib);
        PixMap*  pmPtr    = *pm;
        GrafPtr  destPort = *tablet;
#if TARGET_CARBON
        ::CopyBits (reinterpret_cast<BitMap*> (pmPtr), GetPortBitMapForCopyBits (destPort), &pmPtr->bounds, &rr, srcCopy, nullptr);
#else
        ::CopyBits (reinterpret_cast<BitMap*> (pmPtr), &destPort->portBits, &pmPtr->bounds, &rr, srcCopy, nullptr);
#endif
        delete[](char*) pmPtr->baseAddr;
        ::DisposePixMap (pm);
    }
    catch (...) {
        // treat all excpetions the same. In principle, could draw different picst for memory and
        // unsupported format exceptions...
        AssertNotNull (StandardDIBStyleMarker::sUnsupportedFormatPict);
        ::DrawPicture (StandardDIBStyleMarker::sUnsupportedFormatPict, &rr);
    }
#elif qPlatform_Windows
    //const BITMAPINFOHEADER&   hdr         =   dib->bmiHeader;
    const void* lpBits = Led_GetDIBBitsPointer (dib);
    //const char*               lpBits      =   ((const char*)dib) + Led_ByteSwapFromWindows (hdr.biSize) + Led_GetDIBPalletByteCount (dib);
    ::StretchDIBits (dc->m_hDC, innerBoundsRect.left, innerBoundsRect.top, innerBoundsRect.GetWidth (), innerBoundsRect.GetHeight (), 0, 0, dibImageSize.h, dibImageSize.v, lpBits, dib, DIB_RGB_COLORS, SRCCOPY);
#endif
}

#if qPlatform_MacOS
static PixMap** MakePixMapFromDIB (const Led_DIB* dib)
{
    RequireNotNull (dib);

    Led_Size dibImageSize = Led_GetDIBImageSize (dib);

    const BITMAPINFOHEADER& hdr         = dib->bmiHeader;
    const RGBQUAD*          srcCLUT     = (const RGBQUAD*)(((const unsigned char*)dib) + Led_ByteSwapFromWindows (hdr.biSize));
    const unsigned char*    srcBits     = ((const unsigned char*)srcCLUT) + Led_GetDIBPalletByteCount (dib);
    unsigned short          bitCount    = Led_ByteSwapFromWindows (hdr.biBitCount);
    size_t                  srcRowBytes = (((dibImageSize.h * bitCount + 31) & ~31) >> 3);

    if (bitCount != 8 and bitCount != 24) { // only supported sizes, for now...
        // LGP 960430
        throw UnsupportedFormat ();
    }

    if (hdr.biCompression != 0) { // unsupported for now, thought here is DECODE code on the 'Encyclopedia of Graphix Formats' CD
        // LGP 960430
        throw UnsupportedFormat ();
    }

    if (bitCount > 8) {
        srcCLUT = 0;
    }

    // make sure CLUT looks good...
    if (srcCLUT != nullptr) {
        size_t nColors = Led_ByteSwapFromWindows (hdr.biClrUsed);
        if (nColors > (1 << bitCount)) {
            throw UnsupportedFormat (); // really bad format, probably...
        }
    }

    size_t dstBitCount = bitCount;
    if (bitCount == 24) {
        dstBitCount = 32;
    }
    size_t         dstRowBytes  = ((((dstBitCount * dibImageSize.h) + 15) >> 4) << 1);
    unsigned char* newImageData = new unsigned char[dstRowBytes * dibImageSize.v];
    AssertNotNull (newImageData);

    PixMap** result = ::NewPixMap ();
    if (result == nullptr) {
        delete[](char*) newImageData;
        Led_ThrowOutOfMemoryException ();
    }
    (*result)->bounds.top    = 0;
    (*result)->bounds.left   = 0;
    (*result)->bounds.bottom = dibImageSize.v;
    (*result)->bounds.right  = dibImageSize.h;

    (*result)->baseAddr = Ptr (newImageData);

    switch (bitCount) {
        case 8: {
            (*result)->rowBytes  = 0x8000 | dstRowBytes;
            (*result)->cmpCount  = 1;
            (*result)->cmpSize   = 8;
            (*result)->pixelType = chunky;
            (*result)->pixelSize = 8;
        } break;

        case 24: {
            (*result)->rowBytes  = 0x8000 | dstRowBytes;
            (*result)->cmpCount  = 3;
            (*result)->cmpSize   = 8;
            (*result)->pixelType = RGBDirect;
            (*result)->pixelSize = 32;
        } break;

        default: {
            Assert (false); // not supported - should have punted above!
        } break;
    }

    /*
     *  Copy the CLUT data.
     */
    if (srcCLUT != nullptr) { // 'if' so support 24-bit and no CLUT!!!
        size_t nColors = Led_ByteSwapFromWindows (hdr.biClrUsed);
        if (nColors == 0) {
            nColors = 1 << bitCount;
        }
        Assert (nColors <= (1 << bitCount));

        CTabHandle newCLUT = (CTabHandle)::NewHandle (sizeof (ColorTable) + (nColors - 1) * sizeof (ColorSpec));
        if (newCLUT == nullptr) {
            delete[](char*) newImageData;
            ::DisposePixMap (result);
            Led_ThrowOutOfMemoryException ();
        }
        (*newCLUT)->ctSeed  = ::GetCTSeed ();
        (*newCLUT)->ctFlags = 0;
        (*newCLUT)->ctSize  = nColors - 1;
        for (size_t i = 0; i < nColors; i++) {
            (*newCLUT)->ctTable[i].value     = i;
            (*newCLUT)->ctTable[i].rgb.red   = srcCLUT[i].rgbRed << 8;
            (*newCLUT)->ctTable[i].rgb.green = srcCLUT[i].rgbGreen << 8;
            (*newCLUT)->ctTable[i].rgb.blue  = srcCLUT[i].rgbBlue << 8;
        }
        if ((*result)->pmTable != nullptr) {
            ::DisposeCTable ((*result)->pmTable);
        }
        (*result)->pmTable = newCLUT;
        ::CTabChanged (newCLUT);
    }

    /*
     *  Copy the PixMap data.
     */
    bool rowsReversed = (Led_ByteSwapFromWindows (hdr.biHeight) > 0);
    for (size_t row = 0; row < dibImageSize.v; row++) {
        const unsigned char* srcRow = srcBits + (rowsReversed ? (dibImageSize.v - row - 1) : row) * srcRowBytes;
        unsigned char*       dstRow = newImageData + row * dstRowBytes;

        switch (bitCount) {
            case 8: {
                // we use the same CLUT, so this should be OK
                memcpy (dstRow, srcRow, min (srcRowBytes, dstRowBytes));
            } break;

            case 24: {
                for (size_t col = 0; col < dibImageSize.h; col++) {
                    const unsigned char* srcCell   = srcRow + 3 * col;
                    unsigned char*       dstCell   = dstRow + 4 * col;
                    unsigned char        blueComp  = *srcCell++;
                    unsigned char        greenComp = *srcCell++;
                    unsigned char        redComp   = *srcCell++;
                    *dstCell++                     = 0;
                    *dstCell++                     = redComp;
                    *dstCell++                     = greenComp;
                    *dstCell++                     = blueComp;
                }
            } break;

            default: {
                // too bad, we don't support that size - just zero out the memory...
                memset (dstRow, 0, dstRowBytes);
            } break;
        }
    }

    return result;
}
#endif
