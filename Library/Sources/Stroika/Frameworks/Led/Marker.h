/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Marker_h_
#define _Stroika_Frameworks_Led_Marker_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    Marker
@DESCRIPTION:
        <p>Marker positions are measured in units of @'Led_tChar's - which don't
    necessarily correspond exactly to bytes or characters.</p>
        <p>A TextStore contains an array of character (Led_tChar) cells numbered from 0..n-1,
    where n is the length of the buffer. But we will rarely refer to these,
    and more often refer to Marker positions.</p>
        <p>A Marker is an abstract entity which is used to keep track of portions
    of the text. It maintains "pointers" just before, and just after character (Led_tChar)
    cells.</p>
        <p>It is IMPORTANT not to confuse Marker positions with character (Led_tChar) positions.
    They are closely related, but not identical. Marker positions are
    numbered from 0..n, where n is the length of the buffer. The marker position ALPHA,
    comes just before the character (Led_tChar) position ALPHA. In other words, the character ALPHA
    lies between marker positions ALPHA, and ALPHA+1.
<pre><code>
          (Marker m)
         /      |
        0 1 2 3 4 5 6 7....         (marker positions)
        |H|E|L|L|O| |W|O|R|L|D|
         0 1 2 3 4 5 6 7....        (character positions)
</code></pre>
    </p>
        <p>In this example, Marker m extends from marker positions 0..4, and has a
    length of 4-0=4, and bounds the characters "HELL" (which were at character (Led_tChar)
    positions 0..3).</p>
        <p>Since there is the potential for confusion between charater positions and
    marker positions, we will opt to ALWAYS refer to Marker positions - and NEVER
    refer to character positions. This will make things much clearer (I hope).</p>
        <p>So our insert routines will, for example insert not AT a particular location, but
    just AFTER a particular marker. Similarly for deletes, etc. When you ask for
    text to be copied out of the editor into a 'C' array of characters, you will specify
    the marker position just before the first character (Led_tChar) you want copied out - and so on.</p>
        <p>The main point of markers is to keep track of particular bits of text, and to
    attatch logical attributes to them. Some of these attributes might be visible (e.g.
    Bolding) and some may not (say a hyperlink, or dictionary source marker). But these
    markers are intended to be ATTACHED to particular words (or sequences of bytes) in
    the text, and NOT to positions in the text buffer. Therefore, it is one of the principle
    functions of a marker to adjust its position values so that it can keep track of a
    particular bit of text.</p>
        <p>Here is how we define the behavior of markers and their tracking of text in the
    presence of edit operations. If text is simply changed within a marker, the marker
    doesn't move (though notification methods are called). If text is inserted to
    the left (before) a given marker, its left and right sides are adjusted by the same
    amount to the right. If text is removed from the left (before) the marker, then
    the left and right sides are adjusted the same amount to the left. If the modification -
    addition or removal - takes place to the right (after) the right side of the marker, it
    has no effect. If an insertion or removal takes place INSIDE the bounds of the left
    and right sides of the marker, the left side remains unchanged, and the right side
    is incremented by the size of the insertion or decremented by the size of the removal.
    Note: this specification counts heavily on our definition that the text changes must
    happen before or after marker positions - and cannot happen AT those positions.
    A markers size can never collapse below zero.</p>
        <p>One implication of this definition of marker updating is that once a
    markers size becomes zero, it will never be automatically increased.
    Once they contain no text, the marker can only increase in size through programatic
    intervention. For this reason (at least partly) it is likely that many classes of
    markers will destroy themselves when their size decreses to zero.</p>
        <p>Markers can also be used to do things other than mark particular bits of text. They
    can be used to insert other sorts of graphix into the flow of the text. One particular
    marker subclass might display a picture. Since this marker really has nothing todo with
    any text, it might be a zero-length marker. Even zero length markers CAN end
    up being displayed. They participate in the TextImager layout, and display.
    But, more typically, it would be a marker of length 1 (with a sentinal character in the
    character position marked by that marker), so that cursoring in the editor the image
    is treated as a single item (like a character of text).</p>
        <p>It is for this reason (and others) that we care about the ordering of markers with
    zero length.</p>
        <p>Markers have a natural ordering. They are ordered by their left hand sides start
    positions. This ordering could be further refined to take into account the
    markers right-hand sides - but that ordering would still not be a well-ordering (we
    can easily built two markers with the same left and right sides).</p>
        <p>Since we care for display purposes about the relative ordering of markers - even
    those with zero length - we require a further constraint on the ordering of markers to
    make them well-ordered. We call this the "marker sub-position order". Each marker within
    the buffer which starts at a particular location has a sub-position index from 0..m-1,
    where m is the number of other markers which start at the same position. This sub-position
    order defines precedence at display time when markers overlap, and compete in the
    TextLayout and imaging process described in the TextImager header file.</p>
 */

#include "Support.h"

namespace Stroika::Frameworks::Led {

    class Marker;
    class TextStore;

    /*
    @CLASS:         MarkerOwner
    @DESCRIPTION:
            <p>Object which owns Markers. You register this with a @'TextStore'. And then when those markers are changed
            (contents within the marker), you are updated via the  @'MarkerOwner::AboutToUpdateText' and @'MarkerOwner::DidUpdateText'.
            methods.</p>
            <p>Note that these objects should not be copied with X(X&) CTORs etc and are generally handled by pointer - because pointers
        to particular instances are stored in TextStore objects</p>
    */
    class MarkerOwner {
    protected:
        MarkerOwner () = default;

    public:
        virtual ~MarkerOwner () = default;

        // Methods private and not actually implemented. Just declared to prevent users from accidentally copying instances of this class. Not intended to be
        // used that way.
    private:
        MarkerOwner (const MarkerOwner&);
        const MarkerOwner& operator= (const MarkerOwner&);

    public:
        class UpdateInfo;

    public:
        virtual void AboutToUpdateText (const UpdateInfo& updateInfo);
        virtual void EarlyDidUpdateText (const UpdateInfo& updateInfo) noexcept;
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept;

    public:
        /*
        @METHOD:        MarkerOwner::PeekAtTextStore
        @DESCRIPTION:   <p>Returns the currently associated TextStore for this @'MarkerOwner'.
            This method can return nullptr only if owns no markers!</p>
        */
        virtual TextStore* PeekAtTextStore () const = 0;

    public:
        nonvirtual TextStore& GetTextStore () const;

        /*
            *  Trivial wrappers on owned TextStore
            */
    public:
        nonvirtual size_t FindNextCharacter (size_t afterPos) const;
        nonvirtual size_t FindPreviousCharacter (size_t beforePos) const;
        nonvirtual size_t GetLength () const;
        nonvirtual size_t GetEnd () const;
        nonvirtual void   CopyOut (size_t from, size_t count, Led_tChar* buffer) const;
#if qMultiByteCharacters
    public:
        nonvirtual void Assert_CharPosDoesNotSplitCharacter (size_t charPos) const;
#endif

        // this field is managed by the TextStore subclass which
        // keeps track of these markers. NO-ONE ELSE SHOULD TOUCH!!!
        // The only reason this is public is cuz it can be used by
        // ANY subclass of TextStore which implements the marker store.
    public:
        class HookData;
        HookData* fTextStoreHook = nullptr;
    };

    /*
    @CLASS:         MarkerOwner::HookData
    @DESCRIPTION:   <p>An implementation detail of implementing a @'TextStore'. This class is used
        as an abstract interface for the data that must be kept track of by a @'TextStore' about
        a @'Marker' when it is added to that @'TextStore'.</p>
            <p>This class should only be of interest to those implementing a new @'TextStore' subclass.</p>
    */
    class MarkerOwner::HookData {
    protected:
        HookData () = default;

    public:
        virtual ~HookData () = default;
    };

    /*
    @CLASS:         MarkerOwner::UpdateInfo
    @DESCRIPTION:   <p>A packaging up of information about an update, for @'MarkerOwner::AboutToUpdateText' or
        @'MarkerOwner::DidUpdateText' methods.</p>
    */
    class MarkerOwner::UpdateInfo {
    public:
        UpdateInfo (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, bool textModified, bool realContentUpdate);

        size_t           fReplaceFrom;
        size_t           fReplaceTo;
        const Led_tChar* fTextInserted; // text that was/is to be inserted
        size_t           fTextLength;
        bool             fTextModified; // if true, fTextInserted contains any inserted text.
        // if false, may have just been a font/style or some such change
        bool fRealContentUpdate; // true if change is an action which needs to be 'saved'
        //      and should be interpretted as 'dirtying' the document.

    public:
        nonvirtual size_t GetResultingRHS () const;
    };

    /*
    @CLASS:         Marker
    @DESCRIPTION:   <p>A basic building-block of Led. This class marks a region of text, and sticks to that text, even as
                text before or after is updated. Also, Markers can be notified when any attempted update happens
                within their bounds.</p>
                    <p>Note that these objects should not be copied with X(X&) CTORs etc and are generally handled by pointer - because pointers
                to particular instances are stored in TextStore objects</p>
    */
    class Marker {
    public:
        Marker ();
        virtual ~Marker ();

        // Methods private and not actually implemented. Just declared to prevent users from accidentally copying instances of this class. Not intended to be
        // used that way.
    private:
        Marker (const Marker&);
        const Marker& operator= (const Marker&);

        // These methods are only legal to call when the marker has been added to a marker owner...
    public:
        nonvirtual size_t GetStart () const;
        nonvirtual size_t GetEnd () const;
        nonvirtual size_t GetLength () const;
        nonvirtual MarkerOwner* GetOwner () const;
        nonvirtual void         GetRange (size_t* start, size_t* end) const;

        /*
         *  Considered having one notification method since this would be more efficient - but
         *  using TWO like this is necessary to allow for cooperative transation processing.
         *  Need to be able to say NO to this transation - and then need to be notified and
         *  update our internals if it goes through.
         *
         *  So AboutToUpdateText () may or may not be balanced with a call to DidUpdateText (),
         *  but DidUpdateText () is ALWAYS proceeded by a call to AboutToUpdateText () (for this
         *  update).
         *
         *  Another interesting note on updates. The question is - what sorts of modifications
         *  produce update calls. I had considered having markers notified when text was changed
         *  INSIDE of them. This is clearly needed. But it is also - very often - quite convenient
         *  to be notified when text is changed one character to the left or right of our boundaries.
         *  For example - when we use markers to define boundaries of words - and someone types
         *  a non-space character just before or just after the word - we might want to re-consider
         *  whether we still have a valid word marker.
         */
    public:
        using UpdateInfo = MarkerOwner::UpdateInfo;
        virtual void AboutToUpdateText (const UpdateInfo& updateInfo); // throw to avoid actual update
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept;

        // this field is managed by the TextStore subclass which
        // keeps track of these markers. NO-ONE ELSE SHOULD TOUCH!!!
        // The only reason this is public is cuz it can be used by
        // ANY subclass of TextStore which implements the marker store.
    public:
        class HookData;
        HookData* fTextStoreHook;
    };

    /*
    @CLASS:         Marker::HookData
    @DESCRIPTION:   <p>An implementation detail of implementing a @'TextStore'. This class is used
        as an abstract interface for the data that must be kept track of by a @'TextStore' about
        a @'Marker' when it is added to that @'TextStore'.</p>
            <p>This class should only be of interest to those implementing a new @'TextStore' subclass.</p>
    */
    class Marker::HookData {
    protected:
        HookData () = default;

    public:
        virtual ~HookData () = default;

    public:
        virtual MarkerOwner* GetOwner () const                              = 0;
        virtual size_t       GetStart () const                              = 0;
        virtual size_t       GetEnd () const                                = 0;
        virtual size_t       GetLength () const                             = 0;
        virtual void         GetStartEnd (size_t* start, size_t* end) const = 0;
    };

    /*
    @CLASS:         MarkerMortuary<MARKER>
    @BASES:
    @DESCRIPTION:   <p>MarkerMortuary is a template used to help delete markers from a TextStore, in a situation where
        the markers might still be refered to someplace.</p>
            <p>A typical use would be to accomulate them for deletion - marking them as uninteresting. Then calling
        FinalizeMarkerDeletions () when there are certain to be no more outstanding pointers.</p>
            <p>NB: We require that the markers added to a MarkerMortuary all share a common TextStore.</p>
    */
    template <typename MARKER>
    class MarkerMortuary {
    public:
        MarkerMortuary () = default;
        ~MarkerMortuary ();

    public:
        nonvirtual void AccumulateMarkerForDeletion (MARKER* m);
        nonvirtual void SafeAccumulateMarkerForDeletion (MARKER* m);
        nonvirtual void FinalizeMarkerDeletions () noexcept;
        nonvirtual bool IsEmpty () const noexcept;

    private:
        vector<MARKER*> fMarkersToBeDeleted;
    };

    bool Contains (const Marker& containedMarker, const Marker& containerMarker);
    bool Contains (size_t containedMarkerStart, size_t containedMarkerEnd, const Marker& containerMarker);
    bool Contains (const Marker& marker, size_t charPos);
    bool Contains (size_t containedMarkerStart, size_t containedMarkerEnd, size_t charPos);

    /*
    @CLASS:         LessThan<MARKER>
    @BASES:
    @DESCRIPTION:
        <p>Use this class when you have a vector of some marker subclass and want to sort it.</p>
        <p>As in:
        </p>
        <code><pre>
        vector&lt;MyMarker*&gt; markers = get_em ();
        sort (markers.begin (), markers.end (), LessThan&lt;MyMarker&gt; ())
        </pre></code>
            <p>Note that as of Led 3.0d6, this allows for where two markers have
        the same start, and then measures less
        according to where they end.</p>
    */
    template <typename MARKER>
    struct LessThan {
        bool operator() (const MARKER* lhs, const MARKER* rhs)
        {
            RequireNotNull (lhs);
            RequireNotNull (rhs);
            int diff = int (lhs->GetStart ()) - int (rhs->GetStart ());
            if (diff == 0) {
                return (lhs->GetEnd () < rhs->GetEnd ());
            }
            else {
                return (diff < 0);
            }
        }
    };

    /*
    @CLASS:         TempMarker
    @BASES:         @'MarkerOwner'
    @DESCRIPTION:   <p>A trivial helper class which can be used in a 'stack based' fashion to
                keep temporary track of a region of text, without all the bookkeeping of
                having to add/remove the marker and marker owner, etc.</p>
    */
    class TempMarker : public MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        TempMarker (TextStore& ts, size_t start, size_t end);
        ~TempMarker ();

    public:
        nonvirtual size_t GetStart () const;
        nonvirtual size_t GetEnd () const;
        nonvirtual size_t GetLength () const;
        nonvirtual void   GetLocation (size_t* from, size_t* to) const;

    public:
        virtual TextStore* PeekAtTextStore () const override;

    private:
        TextStore& fTextStore;
        Marker     fMarker;
    };

    /*
    @CLASS:         TemporaryMarkerSlideDown<MARKER>
    @DESCRIPTION:   <p>A simple helper class to take a vector of markers, and slide them one way or the other, and then restore them.
                This can occasionally be helpful when you are about to do some operation which you don't want to affect these markers - you can slide
                them out of the way - so they don't get updated - and then slide them back.</p>
    */
    template <typename MARKER>
    class TemporaryMarkerSlideDown {
    public:
        TemporaryMarkerSlideDown (TextStore& ts, const vector<MARKER*>& m, ptrdiff_t slideBy = 1);
        ~TemporaryMarkerSlideDown ();

    private:
        TextStore&      fTextStore;
        vector<MARKER*> fMarkers;
        ptrdiff_t       fSlideBy;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Marker.inl"

#endif /*_Stroika_Frameworks_Led_Marker_h_*/
