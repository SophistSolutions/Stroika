/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <string.h>

#include "SimpleTextStore.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

class SimpleTextStoreMarkerHook : public Marker::HookData, public Memory::UseBlockAllocationIfAppropriate<SimpleTextStoreMarkerHook> {
public:
    SimpleTextStoreMarkerHook ()
        : Marker::HookData ()
        , fStart (0)
        , fLength (0)
        , fOwner (nullptr)
        , fIsPreRemoved (false)
    {
    }

public:
    virtual MarkerOwner* GetOwner () const override;
    virtual size_t       GetStart () const override;
    virtual size_t       GetEnd () const override;
    virtual size_t       GetLength () const override;
    virtual void         GetStartEnd (size_t* start, size_t* end) const override;

    size_t       fStart;
    size_t       fLength;
    MarkerOwner* fOwner;
    bool         fIsPreRemoved;
};
MarkerOwner* SimpleTextStoreMarkerHook::GetOwner () const
{
    return fOwner;
}

size_t SimpleTextStoreMarkerHook::GetStart () const
{
    Assert (fStart < 0x8000000); // a really big number - we don't have enough memory to trigger
    // this - only point is to test of accidental cast of negnum to
    // size_t.
    return fStart;
}

size_t SimpleTextStoreMarkerHook::GetEnd () const
{
    Assert (fStart < 0x8000000);  // See GetStart/GetEnd
    Assert (fLength < 0x8000000); // See GetStart/GetEnd
    return fStart + fLength;
}

size_t SimpleTextStoreMarkerHook::GetLength () const
{
    Assert (fLength < 0x8000000); // See GetStart
    return fLength;
}

void SimpleTextStoreMarkerHook::GetStartEnd (size_t* start, size_t* end) const
{
    Assert (fStart < 0x8000000);  // See GetStart
    Assert (fLength < 0x8000000); //     ''
    RequireNotNull (start);
    RequireNotNull (end);
    *start = fStart;
    *end   = fStart + fLength;
}

#if qStaticInlineFunctionsInDebugModeNoInliningArentTreatedAsStatic
#define OurStuff SimpleTextStore_OurStuff
#endif
static inline SimpleTextStoreMarkerHook* OurStuff (const Marker* marker)
{
    AssertNotNull (marker);
    AssertNotNull ((SimpleTextStoreMarkerHook*)marker->fTextStoreHook);
    AssertMember ((SimpleTextStoreMarkerHook*)marker->fTextStoreHook, SimpleTextStoreMarkerHook);
    return (SimpleTextStoreMarkerHook*)marker->fTextStoreHook;
}

/*
 ********************************************************************************
 ******************************** SimpleTextStore *******************************
 ********************************************************************************
 */
SimpleTextStore::SimpleTextStore ()
    : TextStore ()
    , fLength (0)
    , fBuffer (nullptr)
    , fMarkers ()
{
    fBuffer = new Led_tChar[0];
    AssertNotNull (fBuffer);
}

SimpleTextStore::~SimpleTextStore ()
{
    Require (GetMarkerOwners ().size () == 1); // Really this should properly be checked in the TextStore::DTOR - and it is.
    // But if this test fails, other tests within THIS DTOR will likely also fail. And
    // those can be confusing. This diagnostic should clearly indicate to users that they've
    // forgotten to remove some MarkerOwners - like Views or MarkerCovers, or ParagraphDatabases,
    // etc.

    Require (fMarkers.size () == 0); // All must have been removed by caller, otherwise its a user bug.
    delete[] fBuffer;
}

/*
@METHOD:        SimpleTextStore::ConstructNewTextStore
@DESCRIPTION:   <p>See @'TextStore::ConstructNewTextStore' ().</p>
*/
TextStore* SimpleTextStore::ConstructNewTextStore () const
{
    return new SimpleTextStore ();
}

void SimpleTextStore::CopyOut (size_t from, size_t count, Led_tChar* buffer) const noexcept
{
    // Note that it IS NOT an error to call CopyOut for multibyte characters and split them. This is one of the few
    // API routines where that is so...
    RequireNotNull (buffer);
    Require (from >= 0);
    Require (from + count <= GetEnd ()); // Be sure all Led_tChars requested fall in range
    (void)::memcpy (buffer, &fBuffer[from], count * sizeof (Led_tChar));
}

void SimpleTextStore::ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount)
{
    Assert (from <= to);
#if qMultiByteCharacters
    Assert (Led_IsValidMultiByteString (withWhat, withWhatCount));

    Assert_CharPosDoesNotSplitCharacter (from);
    Assert_CharPosDoesNotSplitCharacter (to);
#endif

    Invariant ();

    // THIS ISN't QUITE RIGHT - A GOOD APPROX HOWEVER...
    // cuz we don't update markers properly yet... Close - but not quite, a replace
    // is treated as a delete/insert - which isn't quite what we want...
    /*
     *  Though the implication for updating markers is slightly different, for updating just
     *  the text, we can treat this as a delete, followed by an insert.
     */
    DeleteAfter_ (to - from, from);
    InsertAfter_ (withWhat, withWhatCount, from);
    Invariant ();
}

void SimpleTextStore::InsertAfter_ (const Led_tChar* what, size_t howMany, size_t after)
{
    Invariant ();
    size_t     newBufSize = howMany + fLength;
    Led_tChar* newBuf     = new Led_tChar[newBufSize];
    if (fLength != 0) {
        (void)::memcpy (newBuf, fBuffer, after * sizeof (Led_tChar));
    }
    if (howMany != 0) {
        (void)::memcpy (newBuf + after, what, howMany * sizeof (Led_tChar));
    }
    (void)::memcpy (newBuf + after + howMany, &fBuffer[after], (fLength - after) * sizeof (Led_tChar));
    delete[] fBuffer;
    fBuffer = newBuf;
    fLength = newBufSize;

    // update markers
    // since we did an insert - all this does is increment the start point for all markers that were
    // already past here and the lengths of any where the start is to the left and the right is past here.
    for (size_t i = 0; i < fMarkers.size (); i++) {
        Marker* mi    = fMarkers[i];
        size_t  start = mi->GetStart ();
        size_t  len   = mi->GetLength ();
        size_t  end   = start + len;
        if (after < start) {
            OurStuff (mi)->fStart = start + howMany;
        }
        else if (after >= start and after < end) {
            OurStuff (mi)->fLength = len + howMany;
        }
    }
    Invariant ();
}

void SimpleTextStore::DeleteAfter_ (size_t howMany, size_t after)
{
    Assert (after >= 0);
    Assert (howMany + after <= fLength);
    Invariant ();

    Assert (fLength >= howMany);
    size_t howManyToMove = fLength - (after + howMany);
    Assert (howManyToMove <= fLength);
    Assert (howManyToMove + after <= fLength);
    (void)::memmove (&fBuffer[after], &fBuffer[after + howMany], howManyToMove * sizeof (Led_tChar));
    fLength -= howMany;

    // update markers
    for (size_t i = 0; i < fMarkers.size (); i++) {
        Marker* mi    = fMarkers[i];
        size_t  start = mi->GetStart ();
        size_t  len   = mi->GetLength ();
        size_t  end   = start + len;
        if (after < start) {
            if (howMany + after <= start) {
                Assert (start >= howMany);
                OurStuff (mi)->fStart = start - howMany;
            }
            else {
                Assert (howMany > (start - after));
                size_t deleteNCharsOffFront = howMany - (start - after);
                size_t moveFront            = howMany - deleteNCharsOffFront;
                OurStuff (mi)->fStart       = start - moveFront;
                /*
                 * Note when the whole extent is deleted - we simply pin the size to zero.
                 */
                OurStuff (mi)->fLength = (len > deleteNCharsOffFront) ? (len - deleteNCharsOffFront) : 0;
            }
        }
        else if (after >= start and after < end) {
            size_t newEnd = end;
            if (end - after < howMany) {
                newEnd = after;
            }
            else {
                newEnd -= howMany;
            }
            Assert (newEnd >= start);
            size_t newLen          = newEnd - start;
            OurStuff (mi)->fLength = newLen;
        }
    }
    Invariant ();
}

void SimpleTextStore::AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner)
{
    RequireNotNull (marker);
    RequireNotNull (owner);
#if !qVirtualBaseMixinCallDuringCTORBug
    Require (owner->PeekAtTextStore () == this);
#endif
    Require (owner == this or IndexOf (GetMarkerOwners (), owner) != kBadIndex); // new Led 2.3 requirement - not strictly required internally yet - but it will be - LGP 980416
    Require (IndexOf (fMarkers, marker) == kBadIndex);                           // better not be there!
    Require (lhs < 0x80000000);                                                  // not real test, just sanity check
    Require (length < 0x80000000);                                               // not real test, just sanity check
    Invariant ();
    Assert (marker->fTextStoreHook == nullptr);
    marker->fTextStoreHook = new SimpleTextStoreMarkerHook ();
    Assert (marker->GetOwner () == nullptr);
    OurStuff (marker)->fOwner  = owner;
    OurStuff (marker)->fStart  = lhs;
    OurStuff (marker)->fLength = length;

    /*
     *  Insert the marker in-order in the list.
     */
    // For now - assume we always append - for marker sub-order...
    for (size_t i = 0; i < fMarkers.size (); i++) {
        Marker* mi = fMarkers[i];
        if (mi->GetStart () > lhs) {
            // we've gone one too far!!!
            //fMarkers.InsertAt (marker, i);
            fMarkers.insert (fMarkers.begin () + i, marker);
            Invariant ();
            return;
        }
    }
    // if we never found a marker greater - we must be greatest of all - and so append
    //fMarkers.push_back (marker);
    PUSH_BACK (fMarkers, marker);
    Invariant ();
}

void SimpleTextStore::RemoveMarkers (Marker* const markerArray[], size_t markerCount)
{
    Assert (markerCount == 0 or markerArray != nullptr);
    for (size_t i = 0; i < markerCount; i++) {
        Marker* marker = markerArray[i];
        if (marker->fTextStoreHook != nullptr) {
            AssertNotNull (marker->GetOwner ());
            Invariant ();
            vector<Marker*>::iterator index = find (fMarkers.begin (), fMarkers.end (), marker);
            Assert (index != fMarkers.end ());
            fMarkers.erase (index);
            OurStuff (marker)->fOwner = nullptr;
            Invariant ();
            delete marker->fTextStoreHook;
            marker->fTextStoreHook = nullptr;
        }
    }
}

void SimpleTextStore::PreRemoveMarker (Marker* marker)
{
    RequireNotNull (marker);
    Require (not OurStuff (marker)->fIsPreRemoved);
    OurStuff (marker)->fIsPreRemoved = true;
}

void SimpleTextStore::SetMarkerRange (Marker* marker, size_t start, size_t end) noexcept
{
    Assert (start >= 0);
    Assert (end >= 0);
    Assert (start <= end);
    AssertNotNull (marker);

    // changing the start may force a re-ordering...
    if (marker->GetStart () == start) {
        OurStuff (marker)->fLength = end - start;
    }
    else {
        // Really - we should do better than this and NOT force a re-ordering if not needed...
        MarkerOwner* owner = marker->GetOwner ();
        RemoveMarker (marker);
        AddMarker (marker, start, end - start, owner);
    }
}

void SimpleTextStore::CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const
{
    RequireNotNull (owner); // though it can be TextStore::kAnyMarkerOwner.
    for (auto i = fMarkers.begin (); i != fMarkers.end (); i++) {
        Marker* m = *i;
        AssertNotNull (m);
        if (Overlap (*m, from, to)) {
            if (owner == kAnyMarkerOwner or owner == m->GetOwner ()) {
                if (not OurStuff (m)->fIsPreRemoved) {
                    output.Append (m);
                }
            }
        }
    }
}

#if qDebug
void SimpleTextStore::Invariant_ () const
{
    TextStore::Invariant_ ();
    for (size_t i = 0; i < fMarkers.size (); i++) {
        Marker* mi = fMarkers[i];
        AssertNotNull (mi);
        Assert (IndexOf (fMarkers, mi) == i); // be sure same marker doesn't appear multiply in the
        // list!!!
        size_t start = mi->GetStart ();
        size_t len   = mi->GetLength ();
        size_t end   = start + len;
        Assert (start >= 0);
        Assert (start <= end);
        Assert (end <= GetEnd () + 1); // allowed 1 past last valid markerpos
    }
}
#endif
