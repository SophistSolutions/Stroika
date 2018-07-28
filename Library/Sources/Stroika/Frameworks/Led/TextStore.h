/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_TextStore_h_
#define _Stroika_Frameworks_Led_TextStore_h_ 1

#include "../../Foundation/StroikaPreComp.h"

#include <algorithm>
#include <cstddef>
#include <list>
#include <vector>

#include "../../Foundation/Memory/BlockAllocated.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

/*
@MODULE:    TextStore
@DESCRIPTION:
        <p>This module introduces one of Led's key abstractions - the @'TextStore'. A @'TextStore' - as the name suggests -
    is where Led keeps track of text. It is abstract enuf to keep track in many different representations of the text.</p>
        <p>A @'TextStore' also keeps track of @'Marker's associated with the text.</p>
    <h4>Important Design Notes:</h4>
    <ul>
        <li>    Note that Led uses @'Led_tChar's as the meaning for the indexes. This means for
            wide character sets it means characters, and for mbyte or single character set
            builds it means bytes. In either case you can use CharacterToTCharIndex/
            TCharToCharacterIndex () to get whichever you want.
        </li>
    </ul>
 */

#include "Marker.h"
#include "Support.h"
#include "TextBreaks.h"

namespace Stroika::Frameworks::Led {

/*
    @CONFIGVAR:     qUseWin32CompareStringCallForCaseInsensitiveSearch
    @DESCRIPTION:   <p>Produces better internationalized results - but - of course - is Win32 specific - and a bit slower.
                Based on SPR#0864</p>
        */
#ifndef qUseWin32CompareStringCallForCaseInsensitiveSearch
#define qUseWin32CompareStringCallForCaseInsensitiveSearch qPlatform_Windows
#endif

    /*
    @CLASS:         TextStore
    @BASES:         @'MarkerOwner'
    @DESCRIPTION:   <p>An abstraction of something which contains text, and special objects
        called markers, which can be used to represent embedded data, or
        hypertext links, or font style runs. Markers have a left and right edge
        (named by indexes into the text buffer), and conceptually stick to the
        text they wrap as the text is modified (edited). </p>

            <p>There are two concrete TextStore implementations you can use (with Led 3.0).
                <ul style="margin-left: +6em">
                    <li>@'ChunkedArrayTextStore'</li>
                    <li>@'SimpleTextStore'</li>
                </ul>
            </p>

            <p>Future versions of Led may include TextStore implementations which:
                <ul style="margin-left: +6em">
                    <li>directly encode the markers into the stream of text</li><br>
                    Something along these lines is a common editor represention for styled text.
                    But in my case, Markers are arbitrary objects, stored by pointer.
                    So it's less clear how to accomplish that inline storage technique here.

                    <li>Store text on the disk, and then read in only the little bits it needed.</li><br>
                    This would be a sensible TextStore to use when implementing a 'programming'
                    style text editor, where you might reasonably want to edit many megabyte files,
                    and not really need to store many - if any - markers. By accessing the file
                    data on a demand-basis (memory mapped file?)- you could save a significant amount
                    of startup time, and time overall if not the entire file is viewed.
                </ul>
    */
    class TextStore : public virtual MarkerOwner {
    protected:
        TextStore ();

    public:
        virtual ~TextStore ();

        /*
        @METHOD:        TextStore::ConstructNewTextStore
        @DESCRIPTION:   <p>Constructs a new instance of an object of THIS type (whatever dynamic type the object has). The
                    newly constructed object bears no (other) assosiation with the original (THIS) object. This function
                    is pure-virtual, and must be overriden in all subclasses.</p>
        */
    public:
        virtual TextStore* ConstructNewTextStore () const = 0;

        /*
            *  Register a markerOwner here, and it will be notified of text changes before
            *  all markers, and AFTER all markers. Never register more than once, and
            *  always delete iff registered.
            */
    public:
        virtual void     AddMarkerOwner (MarkerOwner* owner);
        virtual void     RemoveMarkerOwner (MarkerOwner* owner);
        nonvirtual const vector<MarkerOwner*>& GetMarkerOwners () const noexcept;

    private:
        vector<MarkerOwner*> fMarkerOwners;

        // Retrieve the text
    public:
        /*
        @METHOD:        TextStore::GetLength
        @DESCRIPTION:   <p>Returns the number of @'Led_tChar's in this @'TextStore'.</p>
        */
        virtual size_t GetLength () const noexcept = 0;
        /*
        @METHOD:        TextStore::CopyOut
        @DESCRIPTION:   <p>CopyOut does NOT null terminate. It is an error to call with 'count' causing
                    access past end of TextStore buffer.</p>
                        <p>Note that it IS NOT an error to call CopyOut for multibyte characters and split them.
                    This is one of the few API routines where that is so.</p>
        */
        virtual void CopyOut (size_t from, size_t count, Led_tChar* buffer) const noexcept = 0;

        // utilities to simplify refering to beginning/end of buffer.
    public:
        static size_t GetStart ();
        nonvirtual size_t GetEnd () const;

        // Type of indexing conversion
        //
        // In Led - by default - we index in terms of Led_tChars. These are typically chars(bytes)
        // but for wide-charactersets such as UNICODE, then can be larger. It is possibly useful to
        // sometimes index in-terms of CHARACTERS instead of BYTES - even when using a multibyte
        // character set. These routines allow you to always freely go back and forth between
        // CHARACTER INDEXES and Led_tChar indexes (used throughout Led).
        //
        // NB: For MBYTE character sets - these CAN BE QUITE SLOW!
        //
    public:
        nonvirtual size_t CharacterToTCharIndex (size_t i);
        nonvirtual size_t TCharToCharacterIndex (size_t i);

        // Update the text
    public:
        nonvirtual void Replace (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount);

        /*
        @METHOD:        TextStore::ReplaceWithoutUpdate
        @DESCRIPTION:   <p>Similar to @'TextStore::Replace' except that this routine doesn't notify markers and markerowners about
                    the update. This should rarely be called directly - except in conjunction with @'TextStore::SimpleUpdater'</p>
                        <p>This method is pure-virtual, and implemented by a concrete subsclass. This method is new to Led 3.1</p>
        */
        virtual void ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount) = 0;

        /*
            *  Access the markers.
            */
    public:
        /*
        @METHOD:        TextStore::AddMarker
        @DESCRIPTION:   <p>Add the given marker to this TextStore, starting at position lhs, and give the marker
            length 'length'. Record that its owner is 'owner'.</p>
                <p>NB: the @'MarkerOwner' - must be a valid @'MarkerOwner'. Either this TestStore,
            or a class which has been added to this @'TextStore' by @'TextStore::AddMarkerOwner'. This requirement is
            new to Led 2.3 (but unlikely ever violated before).</p>
                <p>It is an error to add a marker which is already added to some TextStore (even this TextStore).</p>
                <p>It is an error to destroy a TextStore without first removing all markers you had added.
            It is in order to make that more efficient that we provide the RemoveMarkers () method.</p>
                <p>We are strict about markers extenting outside valid index ranges - this is an prohibited.</p>
        */
        virtual void AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner) = 0;

        /*
        @METHOD:        TextStore::RemoveMarker
        @DESCRIPTION:   <p>Remove the given marker from the TextStore. It is an error if the given marker is not already
                    in this TextStore. See @'TextStore::AddMarker' for more details.</p>
        */

        nonvirtual void RemoveMarker (Marker* marker);
        /*
        @METHOD:        TextStore::RemoveMarkers
        @DESCRIPTION:   <p>Remove the given markers from the TextStore. This is essentially the same as doing multiple
                    @'TextStore::RemoveMarker' calls, except that it may be more efficient for removing large numbers of markers.</p>
                        <p>See also @'TextStore::PreRemoveMarker'.</p>
                        <p>See also @'TextStore::RemoveTypedMarkers'.</p>
        */
        virtual void RemoveMarkers (Marker* const markerArray[], size_t markerCount) = 0;

        template <typename T>
        /*
        @METHOD:        TextStore::RemoveTypedMarkers
        @DESCRIPTION:   <p>A variant of @'TextStore::RemoveMarkers' which can be called with an array of any time 'T' that publicly
                        subclasses from @'Marker'.</p>
        */
        nonvirtual void
        RemoveTypedMarkers (T* const ma[], size_t mc)
        {
            vector<Marker*> v;
            for (size_t i = 0; i < mc; ++i) {
                v.push_back (ma[i]);
            }
            RemoveMarkers (Containers::Start (v), mc);
        }

        template <typename T>
        /*
        @METHOD:        TextStore::RemoveAndDeleteMarkers
        @DESCRIPTION:   <p>Calls @'TextStore::RemoveTypedMarkers' and then deletes each marker.</p>
        */
        nonvirtual void
        RemoveAndDeleteMarkers (T* const ma[], size_t mc)
        {
            RemoveTypedMarkers (ma, mc);
            for (size_t i = 0; i < mc; ++i) {
                delete (ma[i]);
            }
        }

        /*
        @METHOD:        TextStore::PreRemoveMarker
        @DESCRIPTION:   <p>Don't entirely remove the marker (so it can still be queried for
            size etc). But mark it so it will not appear in future CollectAllXXX methods.</p>
                <p>This is <em>NOT</em> required before calling @'TextStore::RemoveMarker',
            but can be handy from classes like @'MarkerMortuary<MARKER>'.</p>
                <p>This method was added to fix SPR#0822 - see for details.</p>
        */
        virtual void PreRemoveMarker (Marker* marker) = 0;

    public:
        /*
        @METHOD:        TextStore::SetMarkerRange
        @DESCRIPTION:   <p>Set the bounds of the given marker. The given marker must already
                    be in this TextStore (see @'TextStore::AddMarker').
                    And it is required that the start/end values be within the valid marker
                    range for this buffer.</p>
        */
        virtual void    SetMarkerRange (Marker* marker, size_t start, size_t end) noexcept = 0;
        nonvirtual void SetMarkerStart (Marker* marker, size_t start) noexcept;
        nonvirtual void SetMarkerEnd (Marker* marker, size_t end) noexcept;
        nonvirtual void SetMarkerLength (Marker* marker, size_t length) noexcept;

        /*
            *  Family of routines to retreive markers of interest in a particular range of the text.
            *
            *  The 'Overlap' method is what is used to see if a marker is considered to be in the
            *  given from/to range for the purpose of collection (does the obvious intersection test
            *  with the added caveat of not including markers which only overlap at one edge or the other
            *  - and not including any common characters).
            *
            *  You can either specify a callback function/object to be called with each found marker.
            *  If you only need the first such, you can throw to terminate the search. There is a help class
            *  and helper fuctions to allow you to fill an array with the all the matching Markers.
            *
            *  NB: this has changed somewhat since Led21 - see qSupportLed21CompatAPI, and SPRs#420,421,422
            */
    public:
        /*
        @CLASS:         TextStore::MarkerSink
        @DESCRIPTION:
                <p>An abstract "callback class", used to be notified in calls to CollectAllMarkersInRangeInto.
                To use this class, subclass, and OVERRIDE the Append () method. Pass an instance of your
                subclass to TextStore::CollectAllMarkersInRangeInto (or some variant). Your classes Append
                method will be called for each marker in the given range.</p>
        */
        class MarkerSink {
        public:
            /*
            @METHOD:            TextStore::MarkerSink::Append
            @DESCRIPTION:
                    <p>Don't call directly. Called by TextStore::CollectAllMarkersInRangeInto (). Override this method
                and pass an instance of your subclass to TextStore::CollectAllMarkersInRangeInto ().</p>
            */
            virtual void Append (Marker* m) = 0;
        };
        /*
        @CLASS:         TextStore::VectorMarkerSink
        @BASES:         @'TextStore::MarkerSink'
        @DESCRIPTION:
                <p>A utility class which gathers all the markers passed to it into an array (vector).</p>
        */
        class VectorMarkerSink : public MarkerSink {
        public:
            VectorMarkerSink (vector<Marker*>* markers);

            virtual void Append (Marker* m) override;

        private:
            vector<Marker*>* fMarkers;
        };

        /*
        @CLASS:         TextStore::SmallStackBufferMarkerSink
        @BASES:         @'TextStore::MarkerSink'
        @DESCRIPTION:
                <p>A utility class which gathers all the markers passed to it into an array (vector).</p>
        */
        class SmallStackBufferMarkerSink : public MarkerSink {
        public:
            SmallStackBufferMarkerSink ();

            virtual void Append (Marker* m) override;

        public:
            Foundation::Memory::SmallStackBuffer<Marker*> fMarkers;
        };

    public:
        /*
        @METHOD:        TextStore::CollectAllMarkersInRangeInto
        @DESCRIPTION:   <p>Note - owner can be any valid MarkerOwner, or @'TextStore::kAnyMarkerOwner'.</p>
        */
        static const MarkerOwner* kAnyMarkerOwner;
        virtual void              CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const = 0;

        // Related helpers
        //  _OrSurroundings () versions include markers which overlapped just barely on an edge
    public:
        static bool Overlap (size_t mStart, size_t mEnd, size_t from, size_t to);
        static bool Overlap (const Marker& m, size_t from, size_t to);

        nonvirtual void CollectAllMarkersInRangeInto_OrSurroundings (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const;

        nonvirtual void CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, vector<Marker*>* markerList) const;
        nonvirtual void CollectAllMarkersInRangeInto_OrSurroundings (size_t from, size_t to, const MarkerOwner* owner, vector<Marker*>* markerList) const;

        nonvirtual vector<Marker*> CollectAllMarkersInRange (size_t from, size_t to, const MarkerOwner* owner = kAnyMarkerOwner) const;
        nonvirtual vector<Marker*> CollectAllMarkersInRange_OrSurroundings (size_t from, size_t to, const MarkerOwner* owner = kAnyMarkerOwner) const;

        /*
            *  NB: We consider that if there is NO text, there is still 1 line. We could the zero-length line
            *  at the end of the buffer as one line.
            *
            *  GetLineContainingPosition () returns the lineNumber that fully contains the
            *  character at position charPosition.  It is an error to call this if charPosition
            *  is not a valid position.
            *
            *  GetEndOfLine () returns the position BEFORE the NL (or after the last character)
            *  if there is no NL. This corresponds to being AFTER the last displayed character.
            */
    public:
        virtual size_t GetStartOfLine (size_t lineNumber) const;
        virtual size_t GetStartOfLineContainingPosition (size_t charPosition) const;
        virtual size_t GetEndOfLine (size_t lineNumber) const;
        virtual size_t GetEndOfLineContainingPosition (size_t afterPos) const;
        virtual size_t GetLineContainingPosition (size_t charPosition) const;
        virtual size_t GetLineCount () const;
        nonvirtual size_t GetLineLength (size_t lineNumber) const; // end-start

        /*
            *  Char/Line navigating commands.
            *
            *  These always return a valid character position. If they are pinned up against
            *  the beggining or end of the buffer, they just return that end position.
            */
    public:
        nonvirtual size_t FindNextCharacter (size_t afterPos) const;      // error to call in mid character - at end of buffer - we just return position after last character
        nonvirtual size_t FindPreviousCharacter (size_t beforePos) const; // error to call in mid character - at start of buffer, we just return 1

    public:
        nonvirtual shared_ptr<TextBreaks> GetTextBreaker () const;
        nonvirtual void                   SetTextBreaker (const shared_ptr<TextBreaks>& textBreaker);

    private:
        mutable shared_ptr<TextBreaks> fTextBreaker;

    public:
        nonvirtual void FindWordBreaks (size_t afterPosition, size_t* wordStartResult, size_t* wordEndResult, bool* wordReal, TextBreaks* useTextBreaker = nullptr);
        nonvirtual void FindLineBreaks (size_t afterPosition, size_t* wordEndResult, bool* wordReal, TextBreaks* useTextBreaker = nullptr);

        /*
            *  Some helpful word-break utility routines based on FindWordBreaks().
            */
    public:
        nonvirtual size_t FindFirstWordStartBeforePosition (size_t position, bool wordMustBeReal = true);
        // use for find-prev-word (can return position if position==1
        // and if no previous word - return 1.
        nonvirtual size_t FindFirstWordStartStrictlyBeforePosition (size_t position, bool wordMustBeReal = true);
        // use for find-prev-word (can return position if position==1
        // and if no previous word - return 1.
        nonvirtual size_t FindFirstWordEndAfterPosition (size_t position, bool wordMustBeReal = true);
        // Can return EndOfBuffer if no word-end after position.
        // Also - might not be called in the context of a word!
        // Can return position - if it is the end of a word.
        nonvirtual size_t FindFirstWordStartAfterPosition (size_t position); // use for find-next-word.
        // returns end-of-buffer if no following word-start
        // Can return position - note we didn't say STRICTLY after

        /*
            *  Search/Find/Replace support.
            */
    public:
        struct SearchParameters {
            SearchParameters (const Led_tString& searchString = LED_TCHAR_OF (""), bool wrap = true, bool wholeWord = false, bool caseSensative = false);

            Led_tString fMatchString;
            bool        fWrapSearch;
            bool        fWholeWordSearch;
            bool        fCaseSensativeSearch;
        };

        enum { eUseSearchParameters = kBadIndex };
        // return kBadIndex if no match found - otherwise, return index of first char in match.
        // Start searching after markerPos searchFrom. if searchTo==eUseSearchParameters, then
        // either search to end of buffer (fWrapSearch==false), or wrap search (fWrapSearch==true).
        // If searchTo!=eUseSearchParameters, then consider IT to specify the end of the search (ignore
        // fWrapSearch).
        virtual size_t Find (const SearchParameters& params, size_t searchFrom, size_t searchTo = eUseSearchParameters);

        // Helper functions, mainly for subclasses, but imagers may use too
    public:
        nonvirtual void DoAboutToUpdateCalls (const UpdateInfo& updateInfo, Marker* const* markersBegin, Marker* const* markersEnd);
        nonvirtual void DoDidUpdateCalls (const UpdateInfo& updateInfo, Marker* const* markersBegin, Marker* const* markersEnd) noexcept;

    public:
        class SimpleUpdater;

    public:
        virtual TextStore* PeekAtTextStore () const override;

        /*
            *      Debugging support.
            *      Note that all these calls (either fail to compile with debug off,
            *  or produce NO-CODE when debug is off. So they can be called freely (though
            *  they might be QUITE expensive when debug on - so SOME conservatism may
            *  be in order).
            */
    public:
        nonvirtual void Invariant () const;

#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif

#if qMultiByteCharacters
    public:
        nonvirtual void Assert_CharPosDoesNotSplitCharacter (size_t charPos) const;
#endif
    };

    /*
    @CLASS:         TextStore::SimpleUpdater
    @DESCRIPTION:   <p>Simple utility to make it a bit easier to make @'TextStore::DoAboutToUpdateCalls' /
                @'TextStore::DoDidUpdateCalls'.</p>
                    <p>Constructing the object does the about-to-update call (collecting all the markers in
                the range given by from/to or the explic updateInfo object), and then the destructor calls the
                didUpdates. So you would write code like:
                    <code><pre>
                        TextStore::SimpleUpdater updater (*ts, from, to);
                        try {
                            Do_Some_Code ();
                        }
                        catch (...) {
                            updater.Cancel ();
                            throw;
                        }
                    </pre></code>
                    </p>
                    <p>The did-updates are done when the updater object goes out of scope; even if it goes out
                of scope because of an exception - whcih MAY NOT be desired. Because of this, you should
                call the cancel method to prevent the didUpdates from happening on destruction.
                </p>
    */
    class TextStore::SimpleUpdater : public Foundation::Memory::UseBlockAllocationIfAppropriate<SimpleUpdater> {
    public:
        SimpleUpdater (TextStore& ts, const UpdateInfo& updateInfo);
        SimpleUpdater (TextStore& ts, size_t from, size_t to, bool realContentUpdate = true);
        ~SimpleUpdater ();

    public:
        nonvirtual void Cancel ();

    private:
        TextStore&                 fTextStore;
        SmallStackBufferMarkerSink fMarkerSink;
        UpdateInfo                 fUpdateInfo;
        bool                       fCanceled;
    };

    // Helpers for MarkerSink classes

    /*
    @CLASS:         MarkerOfATypeMarkerSink<T>
    @BASES:         @'TextStore::MarkerSink'
    @DESCRIPTION:   <p>A MarkerSink template which grabs only Markers of subtype 'T' (using dynamic_cast<>). Similar to
                @'MarkersOfATypeMarkerSink2Vector<T>' except that this class only grabs a SINGLE marker in that range
                (which is stored in fResult). It is an error (detected via an assertion) if this class is ever used
                to sink more than one marker (though its OK if it gets none - fResult will be nullptr).</p>
    */
    template <typename T>
    class MarkerOfATypeMarkerSink : public TextStore::MarkerSink {
    public:
        MarkerOfATypeMarkerSink ();

        virtual void Append (Marker* m) override;

        T* fResult;
    };

    /*
    @CLASS:         MarkersOfATypeMarkerSink2Vector<T>
    @BASES:         @'TextStore::MarkerSink'
    @DESCRIPTION:   <p>A MarkerSink template which grabs only Markers of subtype 'T' (using dynamic_cast<>).
                Dumps results into a vector named 'fResult'.</p>
    */
    template <typename T>
    class MarkersOfATypeMarkerSink2Vector : public TextStore::MarkerSink {
    public:
        MarkersOfATypeMarkerSink2Vector ();

        virtual void Append (Marker* m) override;

        vector<T*> fResult;
    };

    /*
    @CLASS:         MarkersOfATypeMarkerSink2SmallStackBuffer<T>
    @BASES:         @'TextStore::MarkerSink'
    @DESCRIPTION:   <p>A MarkerSink template which grabs only Markers of subtype 'T' (using dynamic_cast<>).
                Dumps results into a @'Memory::SmallStackBuffer<T>' named 'fResult'.</p>
    */
    template <typename T>
    class MarkersOfATypeMarkerSink2SmallStackBuffer : public TextStore::MarkerSink {
    public:
        MarkersOfATypeMarkerSink2SmallStackBuffer ();

        virtual void Append (Marker* m) override;

        Foundation::Memory::SmallStackBuffer<T*> fResult;
    };

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

#endif /*_Stroika_Frameworks_Led_TextStore_h_*/
