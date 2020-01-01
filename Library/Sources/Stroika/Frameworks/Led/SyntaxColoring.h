/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SyntaxColoring_h_
#define _Stroika_Frameworks_Led_SyntaxColoring_h_ 1

/*
@MODULE:    SyntaxColoring
@DESCRIPTION:
        <p>The basic idea is that we will be - to some extent - mimicking the implementation of
    @'StandardStyledTextImager'.
    Recall that the class @'StyledTextImager' is what provides the basic infrastructure
    for displaying styled text.
    It introduces the special marker class @'StyledTextImager::StyleMarker' - which we can subclass todo
    our own kind of special display of text.
    And @'StandardStyledTextImager' simply leverages off this generic implementation, and maintains a database
    of non-overlapping StyleMarkers according to the usual editing, and style application conventions most
    standard text editing packages support (eg. apply style to region of text etc).
    </p>

        <p>Where we will diverge, is that we won't generate our style markers from external function calls or UI commands.
    Instead, we will programmaticly generate the style markers ourselves based on a simple
    lexical analysis of the text (@'SyntaxAnalyzer').
    </p>
 */

#include "../StroikaPreComp.h"

#include "StyledTextImager.h"
#include "Support.h"
#include "TextInteractor.h"

namespace Stroika::Frameworks::Led {
    /*
    @CLASS:         SyntaxAnalyzer
    @DESCRIPTION:   <p>This abstract class is the basic for defining the rules for hooking in arbitrary syntactical analyses into the
        syntax coloring code.</p>
            <p>See the @'TrivialRGBSyntaxAnalyzer' class as a trivial example, and the @'TableDrivenKeywordSyntaxAnalyzer' as a more
        useful starting point for various syntax coloring strategies.</p>
    */
    class SyntaxAnalyzer {
    public:
        virtual void AdjustLookBackRange (TextStore* ts, size_t* lookBackStart, size_t* lookBackTo) const                                                                               = 0;
        virtual void AddMarkers (TextStore* ts, TextInteractor* interactor, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const = 0;
    };

    /*
    @CLASS:         TrivialRGBSyntaxAnalyzer
    @BASES:         @'SyntaxAnalyzer'
    @DESCRIPTION:   <p>A simple example @'SyntaxAnalyzer', which demonstrates the little you need todo to hook in your own
                syntax analysis rules.</p>
    */
    class TrivialRGBSyntaxAnalyzer : public SyntaxAnalyzer {
    public:
        virtual void AdjustLookBackRange (TextStore* ts, size_t* lookBackStart, size_t* lookBackTo) const override;
        virtual void AddMarkers (TextStore* ts, TextInteractor* interactor, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const override;
    };

    /*
    @CLASS:         TableDrivenKeywordSyntaxAnalyzer
    @BASES:         @'SyntaxAnalyzer'
    @DESCRIPTION:   <p>A simple table-driven @'SyntaxAnalyzer', which looks up keywords from (constructor argument) tables.
                The elements of the argument table can be in any order - but no initial substring of a later string can come before
                an earlier one. You can specify an arbitrary compare function for matching keywords - but the two most obtious are
                @'Led_tStrnCmp' and @'Led_tStrniCmp'.</p>
                    <p>This class also has two pre-built static tables for two common syntax coloring cases you may want to use,
                or start from: kCPlusPlusKeywords and kVisualBasicKeywords.</p>
    */
    class TableDrivenKeywordSyntaxAnalyzer : public SyntaxAnalyzer {
    private:
        using inherited = SyntaxAnalyzer;

    public:
        class KeywordTable {
        public:
            KeywordTable (const Led_tChar* keyWords[], size_t nKeywords, int (*cmpFunction) (const Led_tChar*, const Led_tChar*, size_t) = Led_tStrnCmp);

        public:
            nonvirtual size_t MaxKeywordLength () const;
            nonvirtual size_t KeywordLength (const Led_tChar* t, size_t nTChars) const;

        private:
            const Led_tChar** fKeywords;
            size_t            fNKeywords;
            size_t            fMaxKeywordLength;
            int (*fCmpFunction) (const Led_tChar*, const Led_tChar*, size_t);
        };

    public:
        static KeywordTable kCPlusPlusKeywords;
        static KeywordTable kVisualBasicKeywords;

    public:
        TableDrivenKeywordSyntaxAnalyzer (const KeywordTable& keyTable);

    public:
        virtual void AdjustLookBackRange (TextStore* ts, size_t* lookBackStart, size_t* lookBackTo) const override;
        virtual void AddMarkers (TextStore* ts, TextInteractor* interactor, MarkerOwner* owner, size_t lookBackStart, size_t lookBackTo, vector<Marker*>* appendNewMarkersToList) const override;

    private:
        KeywordTable fKeywordTable;
    };

    /*
    @CLASS:         SyntaxColoringMarkerOwner
    @DESCRIPTION:   <p>An abstract class for controling the basic syntax coloring functionality. Try one of the subclasses,
            like @'SimpleSyntaxColoringMarkerOwner or @'WindowedSyntaxColoringMarkerOwner'.</p>
                <p>This class is usually used by instantiating a concrete subclass in your @'TextImager::HookGainedNewTextStore' override,
            and then destroyed in your  @'TextImager::HookLosingTextStore' override.
            You must also override @'TextImager::TabletChangedMetrics' to @'SyntaxColoringMarkerOwner::RecheckAll'.</p>
    */
    class SyntaxColoringMarkerOwner : public MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        SyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer);
        virtual ~SyntaxColoringMarkerOwner ();

    public:
        nonvirtual void RecheckAll ();

    protected:
        virtual void RecheckRange (size_t updateFrom, size_t updateTo) = 0;

    public:
        class ColoredStyleMarker;

    public:
        /*
        @CLASS:         SyntaxColoringMarkerOwner::FontChangeStyleMarker
        @BASES:         @'TrivialFontSpecStyleMarker'
        @DESCRIPTION:   <p>This is used internally by the syntax coloring code, and is exposed only in case you want to write your own
            Syntax Analyzer code. This simply takes a @'Led_FontSpecification' object and applies that to the given text.</p>
        */
        using FontChangeStyleMarker = TrivialFontSpecStyleMarker;

    public:
        virtual TextStore* PeekAtTextStore () const override;

    protected:
        TextInteractor&       fInteractor;
        TextStore&            fTextStore;
        const SyntaxAnalyzer& fSyntaxAnalyzer;
    };

    /*
    @CLASS:         SimpleSyntaxColoringMarkerOwner
    @BASES:         @'SyntaxColoringMarkerOwner'
    @DESCRIPTION:   <p>A simple but effective brute-force coloring algorithm. This simply colors the entire document.
                And when any part of the document changes - this simply recolors the document. This runs very quickly. But
                for large documents, where you may never scroll to view large subsets of the document (or where you need to be able
                to open quickly) - this may not be your best choice.</p>
                    <p>See also @'WindowedSyntaxColoringMarkerOwner'.</p>
    */
    class SimpleSyntaxColoringMarkerOwner : public SyntaxColoringMarkerOwner {
    private:
        using inherited = SyntaxColoringMarkerOwner;

    public:
        SimpleSyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer);
        virtual ~SimpleSyntaxColoringMarkerOwner ();

    protected:
        virtual void RecheckRange (size_t updateFrom, size_t updateTo) override;

    public:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    private:
        vector<Marker*> fMarkers;
    };

    /*
    @CLASS:         WindowedSyntaxColoringMarkerOwner
    @BASES:         @'SyntaxColoringMarkerOwner'
    @DESCRIPTION:   <p>This @'SyntaxColoringMarkerOwner' tries to be clever about what areas to syntax analyze. It only analyzes
                the current window. This makes for very fast opening of large documents (independent of actual file size). But it can make
                typing and scrolling somewhat slower. This really doesn't matter as long as its faster than some particular user-measurable
                threshold. I think on a 400Mz or faster Pentium machine - this will always be fast enuf to be a better choice than
                @'SimpleSyntaxColoringMarkerOwner'. But you can easily try both, and see for yourself.</p>
                    <p>In addition to settup according to the docs in @'SyntaxColoringMarkerOwner' - you must also override @'TextInteractor::UpdateScrollBars' to
                call @'WindowedSyntaxColoringMarkerOwner::RecheckScrolling'.</p>
                    <p>See also @'SimpleSyntaxColoringMarkerOwner'.</p>
    */
    class WindowedSyntaxColoringMarkerOwner : public SyntaxColoringMarkerOwner {
    private:
        using inherited = SyntaxColoringMarkerOwner;

    public:
        WindowedSyntaxColoringMarkerOwner (TextInteractor& interactor, TextStore& textStore, const SyntaxAnalyzer& syntaxAnalyzer);
        virtual ~WindowedSyntaxColoringMarkerOwner ();

    public:
        nonvirtual void RecheckScrolling ();

    protected:
        virtual void RecheckRange (size_t updateFrom, size_t updateTo) override;

    public:
        virtual void AboutToUpdateText (const UpdateInfo& updateInfo) override;
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    private:
        vector<Marker*> fMarkers;
        bool            fDeletedLines;
        // scrolling speed tweek - so we don't inval too much!
        size_t fCachedWindowStart;
        size_t fCachedWindowEnd;
    };

    /*
    @CLASS:         SyntaxColoringMarkerOwner::ColoredStyleMarker
    @BASES:         @'SimpleStyleMarkerByFontSpec'
    @DESCRIPTION:   <p>This is used internally by the syntax coloring code, and is exposed only in case you want to write your own
                Syntax Analyzer code. This simply takes a @'Led_Color' object and uses that to color the given text.</p>
    */
    class SyntaxColoringMarkerOwner::ColoredStyleMarker : public SimpleStyleMarkerByFontSpec<>, public Foundation::Memory::UseBlockAllocationIfAppropriate<ColoredStyleMarker> {
    private:
        using inherited = SimpleStyleMarkerByFontSpec<>;

    public:
        ColoredStyleMarker (const Led_Color& color);

    protected:
        virtual Led_FontSpecification MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const override;

    public:
        Led_Color fColor;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SyntaxColoring.inl"

#endif /*_Stroika_Frameworks_Led_SyntaxColoring_h_*/
