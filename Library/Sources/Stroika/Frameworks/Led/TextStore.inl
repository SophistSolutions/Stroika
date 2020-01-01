/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_TextStore_inl_
#define _Stroika_Framework_Led_TextStore_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    //  class   TextStore::SimpleUpdater
    inline TextStore::SimpleUpdater::SimpleUpdater (TextStore& ts, const UpdateInfo& updateInfo)
        : fTextStore (ts)
        , fMarkerSink ()
        , fUpdateInfo (updateInfo)
        , fCanceled (false)
    {
        // Note that we EXPAND the list of markers we will notify to be sure markers just next to a change
        // are given a crack at it (CollectAllMarkersInRange_OrSurroundings)
        ts.CollectAllMarkersInRangeInto_OrSurroundings (updateInfo.fReplaceFrom, updateInfo.fReplaceTo, kAnyMarkerOwner, fMarkerSink);
        ts.DoAboutToUpdateCalls (fUpdateInfo, fMarkerSink.fMarkers.begin (), fMarkerSink.fMarkers.end ());
    }
    inline TextStore::SimpleUpdater::SimpleUpdater (TextStore& ts, size_t from, size_t to, bool realContentUpdate)
        : fTextStore (ts)
        , fMarkerSink ()
        , fUpdateInfo (from, to, LED_TCHAR_OF (""), 0, false, realContentUpdate)
        , fCanceled (false)
    {
        // Note that we EXPAND the list of markers we will notify to be sure markers just next to a change
        // are given a crack at it (CollectAllMarkersInRange_OrSurroundings)
        ts.CollectAllMarkersInRangeInto_OrSurroundings (from, to, kAnyMarkerOwner, fMarkerSink);
        ts.DoAboutToUpdateCalls (fUpdateInfo, fMarkerSink.fMarkers.begin (), fMarkerSink.fMarkers.end ());
    }
    inline TextStore::SimpleUpdater::~SimpleUpdater ()
    {
        if (not fCanceled) {
            fTextStore.DoDidUpdateCalls (fUpdateInfo, fMarkerSink.fMarkers.begin (), fMarkerSink.fMarkers.end ());
        }
    }
    inline void TextStore::SimpleUpdater::Cancel ()
    {
        fCanceled = true;
    }

    //  class   TextStore::SearchParameters
    inline TextStore::SearchParameters::SearchParameters (const Led_tString& searchString, bool wrap, bool wholeWord, bool caseSensative)
        : fMatchString (searchString)
        , fWrapSearch (wrap)
        , fWholeWordSearch (wholeWord)
        , fCaseSensativeSearch (caseSensative)
    {
    }

    //  class   TextStore::VectorMarkerSink
    inline TextStore::VectorMarkerSink::VectorMarkerSink (vector<Marker*>* markers)
        : fMarkers (markers)
    {
        RequireNotNull (fMarkers);
    }

    //  class   TextStore::SmallStackBufferMarkerSink
    inline TextStore::SmallStackBufferMarkerSink::SmallStackBufferMarkerSink ()
        : fMarkers (0)
    {
    }

    //  class   TextStore
    inline TextStore::TextStore ()
        : fMarkerOwners ()
        , fTextBreaker ()
    {
        fMarkerOwners.push_back (this);
    }
    /*
    @METHOD:        TextStore::AddMarkerOwner
    @DESCRIPTION:   <p>Register the given MarkerOwner with the TextStore for later notification of updates to the text.
        Remove via RemoveMarkerOwner ().</p>
            <p>NB: It is illegal to add a @'MarkerOwner' to more than one @'TextStore' at a time, or to the same one
        multiple times.</p>
    */
    inline void TextStore::AddMarkerOwner (MarkerOwner* owner)
    {
        RequireNotNull (owner);
#if !qVirtualBaseMixinCallDuringCTORBug
        Require (owner->PeekAtTextStore () == this);
#endif
        Require (find (fMarkerOwners.begin (), fMarkerOwners.end (), owner) == fMarkerOwners.end ());
        //          fMarkerOwners.push_back (owner);
        PUSH_BACK (fMarkerOwners, owner);
    }
    /*
    @METHOD:        TextStore::RemoveMarkerOwner
    @DESCRIPTION:
        <p>Unregister the given MarkerOwner which was previously registed with AddMarkerOwner ().</p>
    */
    inline void TextStore::RemoveMarkerOwner (MarkerOwner* owner)
    {
        RequireNotNull (owner);
#if !qVirtualBaseMixinCallDuringCTORBug
        Require (owner->PeekAtTextStore () == this);
#endif
        vector<MarkerOwner*>::iterator i = find (fMarkerOwners.begin (), fMarkerOwners.end (), owner);
        Assert (i != fMarkerOwners.end ());
        fMarkerOwners.erase (i);
    }
    /*
    @METHOD:            TextStore::GetMarkerOwners
    @DESCRIPTION:
                <p>Returns the list of all MarkerOwners registered for notification of changes to the text.</p>
    */
    inline const vector<MarkerOwner*>& TextStore::GetMarkerOwners () const noexcept
    {
        return fMarkerOwners;
    }
    /*
    @METHOD:        TextStore::RemoveMarker
    @DESCRIPTION:   <p>Remove the given marker from the text.</p>
    */
    inline void TextStore::RemoveMarker (Marker* marker)
    {
        RemoveMarkers (&marker, 1);
    }
    /*
    @METHOD:        TextStore::GetStart
    @DESCRIPTION:   <p>Returns the marker position of the beginning of the
                text buffer (always 0).</p>
    */
    inline size_t TextStore::GetStart ()
    {
        return (0);
    }
    /*
    @METHOD:        TextStore::GetEnd
    @DESCRIPTION:   <p>Returns the marker position of the end of the text buffer.</p>
    */
    inline size_t TextStore::GetEnd () const
    {
        return (GetLength ());
    }
    /*
    @METHOD:        TextStore::CollectAllMarkersInRange
    @DESCRIPTION:   <p>CollectAllMarkersInRange () is part of a family of routines to retreive markers
                of interest in a particular range of the text.</p>
                    <p>The @'TextStore::Overlap' method is what is used to see if a marker is considered to be in the
                given from/to range for the purpose of collection (does the obvious intersection test
                with the added caveat of not including markers which only overlap at one edge or the other
                - and not including any common characters - except special case of zero-sized marker).</p>
                    <p>You can either specify a callback function/object to be called with each found marker.
                If you only need the first such, you can throw to terminate the search. There is a help class
                and helper fuctions to allow you to fill an array with the all the matching Markers.</p>
                    <p>NB: this has changed somewhat since Led22 - see SPR#0489.</p>
    */
    inline vector<Marker*> TextStore::CollectAllMarkersInRange (size_t from, size_t to, const MarkerOwner* owner) const
    {
        Require (from <= to);
        Require (to <= GetEnd () + 1);
        vector<Marker*>  list;
        VectorMarkerSink vml (&list);
        CollectAllMarkersInRangeInto (from, to, owner, vml);
        return (list);
    }
    inline void TextStore::CollectAllMarkersInRangeInto_OrSurroundings (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const
    {
        Require (from <= to);
        Require (to <= GetEnd () + 1);
        CollectAllMarkersInRangeInto ((from > 0) ? (from - 1) : from, min (to + 1, GetEnd () + 1), owner, output);
    }
    inline void TextStore::CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, vector<Marker*>* markerList) const
    {
        RequireNotNull (markerList);
        markerList->clear ();
        VectorMarkerSink vml (markerList);
        CollectAllMarkersInRangeInto (from, to, owner, vml);
    }
    inline vector<Marker*> TextStore::CollectAllMarkersInRange_OrSurroundings (size_t from, size_t to, const MarkerOwner* owner) const
    {
        Require (from <= to);
        Require (to <= GetEnd () + 1);
        return CollectAllMarkersInRange ((from > 0) ? (from - 1) : from, min (to + 1, GetEnd () + 1), owner);
    }
    inline void TextStore::CollectAllMarkersInRangeInto_OrSurroundings (size_t from, size_t to, const MarkerOwner* owner, vector<Marker*>* markerList) const
    {
        Require (from <= to);
        Require (to <= GetEnd () + 1);
        RequireNotNull (markerList);
        VectorMarkerSink vml (markerList);
        CollectAllMarkersInRangeInto ((from > 0) ? (from - 1) : from, min (to + 1, GetEnd () + 1), owner, vml);
    }
#if qSingleByteCharacters || qWideCharacters
    // qMultiByteCharacters Code in C file - COMPLEX/SLOW
    inline size_t TextStore::CharacterToTCharIndex (size_t i)
    {
        return (i);
    }
    inline size_t TextStore::TCharToCharacterIndex (size_t i)
    {
        return (i);
    }
#endif
    /*
    @METHOD:        TextStore::SetMarkerStart
    @DESCRIPTION:   <p>Similar to @'TextStore::SetMarkerRange', except that the end-point doesn't change.
                Vectors to @'TextStore::SetMarkerRange'. See @'TextStore::SetMarkerEnd'.</p>
    */
    inline void TextStore::SetMarkerStart (Marker* marker, size_t start) noexcept
    {
        SetMarkerRange (marker, start, marker->GetEnd ());
    }
    /*
    @METHOD:        TextStore::SetMarkerEnd
    @DESCRIPTION:   <p>Similar to @'TextStore::SetMarkerRange', except that the start-point doesn't change.
                Vectors to @'TextStore::SetMarkerRange'. See @'TextStore::SetMarkerStart'.</p>
    */
    inline void TextStore::SetMarkerEnd (Marker* marker, size_t end) noexcept
    {
        SetMarkerRange (marker, marker->GetStart (), end);
    }
    /*
    @METHOD:        TextStore::SetMarkerLength
    @DESCRIPTION:   <p>Similar to @'TextStore::SetMarkerRange', except that the start-point doesn't change.
                Similar to @'TextStore::SetMarkerEnd' except that it takes a length, not an end-point.
                Vectors to @'TextStore::SetMarkerRange'. See @'TextStore::SetMarkerStart'.</p>
    */
    inline void TextStore::SetMarkerLength (Marker* marker, size_t length) noexcept
    {
        size_t start = marker->GetStart ();
        SetMarkerRange (marker, start, start + length);
    }
    inline size_t TextStore::GetLineLength (size_t lineNumber) const
    {
        return (GetStartOfLine (lineNumber) - GetEndOfLine (lineNumber));
    }
    inline size_t TextStore::FindNextCharacter (size_t afterPos) const
    {
        if (afterPos >= GetEnd ()) {
            return (GetEnd ());
        }
#if qSingleByteCharacters || qWideCharacters
        size_t result = afterPos + 1;
#elif qMultiByteCharacters
        Led_tChar thisChar;
        CopyOut (afterPos, 1, &thisChar);
        size_t result = Led_IsLeadByte (thisChar) ? (afterPos + 2) : (afterPos + 1);
#endif
        Ensure (result <= GetEnd ());
        return (result);
    }
    /*
    @METHOD:        TextStore::GetTextBreaker
    @DESCRIPTION:   <p>Returns a @'shared_ptr<T>' wrapper on the @'TextBreaks' subclass associated
                with this TextStore. This
                procedure can be changed at any time (though if any information in other parts of Led is cached and dependent on this procedures
                results - you may wish to invalidate those caches).</p>
                    <p>If none is associated with the TextStore right now - and default one is built and returned.</p>
                    <p>See also See @'TextStore::SetTextBreaker'.</p>
    */
    inline shared_ptr<TextBreaks> TextStore::GetTextBreaker () const
    {
        if (fTextBreaker == nullptr) {
            fTextBreaker = make_shared<TextBreaks_DefaultImpl> ();
        }
        return fTextBreaker;
    }
    /*
    @METHOD:        TextStore::SetTextBreaker
    @DESCRIPTION:   <p>See @'TextStore::GetTextBreaker'.</p>
    */
    inline void TextStore::SetTextBreaker (const shared_ptr<TextBreaks>& textBreaker)
    {
        fTextBreaker = textBreaker;
    }
    inline void TextStore::Invariant () const
    {
#if qDebug && qHeavyDebugging
        Invariant_ ();
#endif
    }
#if qMultiByteCharacters
    inline void TextStore::Assert_CharPosDoesNotSplitCharacter (size_t charPos) const
    {
#if qDebug
        /*
            *  We know that line (not row) breaks are a good syncronization point to look back and scan to make
            *  sure all the double-byte characters are correct - because an NL is NOT a valid second byte.
            */
        Assert (not Led_IsValidSecondByte ('\n'));
        size_t startOfFromLine = GetStartOfLineContainingPosition (charPos);
        Assert (startOfFromLine <= charPos);
        size_t                              len = charPos - startOfFromLine;
        Memory::SmallStackBuffer<Led_tChar> buf (len);
        CopyOut (startOfFromLine, len, buf);
        Assert (Led_IsValidMultiByteString (buf, len)); // This check that the whole line from the beginning to the charPos point
                                                        // is valid makes sure that the from position doesn't split a double-byte
                                                        // character.
#endif
    }
#endif
    inline bool TextStore::Overlap (size_t mStart, size_t mEnd, size_t from, size_t to)
    {
        Require (mStart <= mEnd);
        Require (from <= to);

        if ((from <= mEnd) and (mStart <= to)) {
            // Maybe overlap - handle nuanced cases of zero-sized overlaps
            size_t overlapSize;
            if (to >= mEnd) {
                Assert (mEnd >= from);
                overlapSize = min (mEnd - from, mEnd - mStart);
            }
            else {
                Assert (to >= mStart);
                overlapSize = min (to - from, to - mStart);
            }
            Assert (overlapSize <= (to - from));
            Assert (overlapSize <= (mEnd - mStart));

            if (overlapSize == 0) {
                /*
                    *  The ONLY case where we want to allow for a zero-overlap to imply a legit overlap is when the marker itself
                    *  is zero-sized (cuz otherwise - it would never get found).
                    */
                return mEnd == mStart;
            }
            else {
                return true;
            }
        }
        else {
            return false;
        }
    }

    /*
    @METHOD:        TextStore::Overlap
    @DESCRIPTION:   <p>The idea here is to test if a marker overlaps with a given range of the document. But
                only in some <b>INTERESTING</b> way.</p>
                    <p>The one case one might plausibly consider overlap which this routine
                does NOT is the case where two markers touch only at the edges (lhs of one == rhs of the other).
                For the purposes for which markers are used - in my experience - this is NOT an interesting case
                and it makes code using @'TextStore::CollectAllMarkersInRange' nearly always simpler and more efficient to
                be able to avoid those cases.</p>
                    <p>There is one further refinement added here in Led 2.3 (970929, for spr#0489). When the marker is
                zero-length (not the from/to args, but the 'm' arg), then the overlap is allowed to be zero width
                and it is still considerd a valid overlap. This is true because we want CollectAllMarkersInRange ()
                to be usable to pick up zero-length markers. Essentially, this means it is "OverlapOrStrictlyContains".</p>
                    <p>NB: The details of this special 'zero-width' overlap case were further clarified, and improved, and
                revised in Led 3.0d6 (2000/04/26, SPR#0745).</p>
                    <p><em>Important:</em><br>
                        The definition of overlap is now that a marker overlaps with a given region if it overlaps by ONE FULL
                marker position, or one special case of ZERO overlap. The only ZERO-overlap case which is supported is
                simply if the marker-width is zero sized.</p>
                    <p>The part which is a change from Led 2.3 (and earlier) is the details of WHICH zero-width overlap cases
                are now considered overlap. I think the new rule is simpler, and more intuitive. See Led 2.3 for the old rule/code.</p>
                    <p>NB: This routine is mainly called by the @'TextStore::CollectAllMarkersInRange' () family of functions.</p>
                    <p>NB:  This routine is <b>NOT</b> symmetric. By this I mean that Overlap (A,B) is not always the same
                as Overlap (B,A). The reason for this is because we specially treat the case of a zero-width first arg to
                overlap. And we make no such special treatment of the second argument.</p>
                    <p>See SPR#0745 for more details. Also, SPR#0489, and SPR#420.</p>
    */
    inline bool TextStore::Overlap (const Marker& m, size_t from, size_t to)
    {
        Require (from <= to);

        size_t start;
        size_t end;
        m.GetRange (&start, &end);
        Assert (start <= end);

#if qDebug
        // Note - the old algorithm DOESNT give the same answers as the new one. Otherwise - we wouldn't bother with a new algorithm.
        // This assertion/testing code is just temporary - for me to get a sense how often we're producing different answers, and how
        // serious this will be (a testing issue) - LGP 2000/04/26
        // Well - its been almost a year - and we've not seen this yet... Hmmm - LGP 2001-03-05
        bool oldAlgorithmAnswer;
        {
            size_t Xend = end;
            if (start == end) {
                Xend++;
            }

            oldAlgorithmAnswer = (from < Xend) and (start < to);
        }
#endif

        if ((from <= end) and (start <= to)) {
            // Maybe overlap - handle nuanced cases of zero-sized overlaps
            size_t overlapSize;
            if (to >= end) {
                Assert (end >= from);
                overlapSize = min (end - from, end - start);
            }
            else {
                Assert (to >= start);
                overlapSize = min (to - from, to - start);
            }
            Assert (overlapSize <= (to - from));
            Assert (overlapSize <= (end - start));

            if (overlapSize == 0) {
                /*
                    *  The ONLY case where we want to allow for a zero-overlap to imply a legit overlap is when the marker itself
                    *  is zero-sized (cuz otherwise - it would never get found).
                    */
                //  Ensure (oldAlgorithmAnswer == (end == start));
                return end == start;
            }
            else {
                Ensure (oldAlgorithmAnswer == true);
                return true;
            }
        }
        else {
            Ensure (oldAlgorithmAnswer == false);
            return false;
        }
    }

    //  class   MarkerOfATypeMarkerSink
    template <typename T>
    inline MarkerOfATypeMarkerSink<T>::MarkerOfATypeMarkerSink ()
        : fResult (nullptr)
    {
    }
    template <typename T>
    void MarkerOfATypeMarkerSink<T>::Append (Marker* m)
    {
        RequireNotNull (m);
        T* tMarker = dynamic_cast<T*> (m);
        if (tMarker != nullptr) {
            Assert (fResult == nullptr); // we require at most one marker be added to us
            fResult = tMarker;
        }
    }

    //  class   MarkersOfATypeMarkerSink2Vector
    template <typename T>
    inline MarkersOfATypeMarkerSink2Vector<T>::MarkersOfATypeMarkerSink2Vector ()
        : fResult ()
    {
    }
    template <typename T>
    void MarkersOfATypeMarkerSink2Vector<T>::Append (Marker* m)
    {
        RequireNotNull (m);
        T* tMarker = dynamic_cast<T*> (m);
        if (tMarker != nullptr) {
            //fResult.push_back (tMarker);
            PUSH_BACK (fResult, tMarker);
        }
    }

    //  class   MarkersOfATypeMarkerSink2SmallStackBuffer
    template <typename T>
    inline MarkersOfATypeMarkerSink2SmallStackBuffer<T>::MarkersOfATypeMarkerSink2SmallStackBuffer ()
        : fResult ()
    {
    }
    template <typename T>
    void MarkersOfATypeMarkerSink2SmallStackBuffer<T>::Append (Marker* m)
    {
        RequireNotNull (m);
        T* tMarker = dynamic_cast<T*> (m);
        if (tMarker != nullptr) {
            fResult.push_back (tMarker);
        }
    }

}

#endif /*_Stroika_Framework_Led_TextStore_inl_*/
