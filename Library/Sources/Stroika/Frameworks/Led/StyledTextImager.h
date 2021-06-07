/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextImager_h_
#define _Stroika_Frameworks_Led_StyledTextImager_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    StyledTextImager
@DESCRIPTION:
        <p>Basic foundations of display of 'styled', or marked up, text. The central
    class of this module is @'StyledTextImager'</p>

 */

#include "MarkerCover.h"
#include "Support.h"
#include "TextImager.h"

#if qPlatform_MacOS
struct TextStyle;
struct ScrpSTElement;
#endif

namespace Stroika::Frameworks::Led {

/*
    @CONFIGVAR:     qAssertWarningForEqualPriorityMarkers
    @DESCRIPTION:   <p>Debugging hack for @'StyledTextImager::StyleMarkerSummarySink::CombineElements'.
                It CAN cause problems having multiple markers of the same priority overlapping, so this
                code will assert-out / warn when that happens. It COULD be OK - so thats why this is optional
                checking.</p>
        */
#ifndef qAssertWarningForEqualPriorityMarkers
#define qAssertWarningForEqualPriorityMarkers qDebug
#endif

    /*
    @CLASS:         StyledTextImager
    @BASES:         virtual @'TextImager'
    @DESCRIPTION:   <p>The class StyledTextImager is a @'TextImager' which knows about special markers,
                either owned by itself, or the TextStore, which it uses to render and
                measure the text. It is intended that these special markers it uses to
                render the text (@'StyledTextImager::StyleMarker''s) be general enough to support both standard
                style runs, as well as other fancier text adornments, like pictures, opendoc
                embeddings, etc.</p>
                    <p>You can add arbitrary, and overlapping StyleMarkers to this class, and it will
                simply render them. Since it must pick ONE StyleMarker to ask todo the drawing,
                it asks the one with the highest priority (@'StyledTextImager::StyleMarker::GetPriority' ()). If you
                have some style marker which is smart enuf to pay attention to the
                overlapping of markers (RARE - IF EVER) it is up to your marker to find which
                other markers it overlaps with, and handle this combination itself.</p>
                    <p>This class is intended to make easy things like wrapping keywords with little
                markers which affect how they are displayed. It is ideal for something like
                a programming text editor which colors keywords, or a typical web browser
                that has to keep associated links with parts of the text anyhow (keep it in a
                marker that subclasses from SytleMarker, and then change the color, or font of
                your display).</p>
                    <p>For the more conventional Style-Run type API, see the class @'StandardStyledTextImager'.</p>
    */
    class StyledTextImager : public virtual TextImager {
    private:
        using inherited = TextImager;

    protected:
        StyledTextImager () = default;

    public:
        class StyleMarker;
        struct RunElement;
        class StyleMarkerSummarySink;
        class StyleMarkerSummarySinkForSingleOwner;

    protected:
        virtual vector<RunElement> SummarizeStyleMarkers (size_t from, size_t to) const;
        virtual vector<RunElement> SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const;

    protected:
        // Must OVERRIDE Draw/Measure text routines so style runs get hooked in and have some effect
        // when this class is mixed in.
        virtual void         DrawSegment (Tablet* tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
                                          CoordinateType useBaseLine, DistanceType* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
                                                  DistanceType* distanceResults) const override;
        virtual DistanceType MeasureSegmentHeight (size_t from, size_t to) const override;
        virtual DistanceType MeasureSegmentBaseLine (size_t from, size_t to) const override;

        // Debug support
    public:
        nonvirtual void Invariant () const;
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

    /*
    @CLASS:         StyledTextImager::StyleMarker
    @BASES:         @'Marker'
    @DESCRIPTION:   <p>Abstract class - subclass this to provide custom style drawing. Managed by
                class @'StyledTextImager'.</p>
    */
    class StyledTextImager::StyleMarker : public Marker {
    protected:
        StyleMarker ();

    public:
        enum { eBaselinePriority = 0 };

    public:
        virtual int GetPriority () const;

    public:
        using RunElement = StyledTextImager::RunElement;

    public:
        /*
        @METHOD:        StyledTextImager::StyleMarker::DrawSegment
        @DESCRIPTION:   <p>This pure-virtual hook function is called when the given range of text needs to be
                    drawn. (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
                        <p>NB: an extra 'invalidRect' argument was added to this method in Led 3.1a6. Note that
                    this is incompatible change.</p>
        */
        virtual void DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Tablet* tablet,
                                  size_t from, size_t to, const TextLayoutBlock& text,
                                  const Led_Rect& drawInto, const Led_Rect& invalidRect, CoordinateType useBaseLine,
                                  DistanceType* pixelsDrawn) = 0;

        /*
        @METHOD:        StyledTextImager::StyleMarker::MeasureSegmentWidth
        @DESCRIPTION:   <p>This pure-virtual hook function is called when the given range of text needs to be
                    measured (character widths). (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
        */
        virtual void MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement,
                                          size_t from, size_t to,
                                          const Led_tChar* text,
                                          DistanceType*    distanceResults) const = 0;
        /*
        @METHOD:        StyledTextImager::StyleMarker::MeasureSegmentWidth
        @DESCRIPTION:   <p>This pure-virtual hook function is called when the given range of text needs to be
                    measured (hieght of text segment). (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
        */
        virtual DistanceType MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement,
                                                   size_t from, size_t to) const = 0;

        /*
        @METHOD:        StyledTextImager::StyleMarker::MeasureSegmentBaseLine
        @DESCRIPTION:   <p>This pure-virtual hook function is called when the given range of text needs to be
                    measured (baseline of text segment). (SHOULD EXPLAIN THIS ALOT MORE!!!)</p>
        */
        virtual DistanceType MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement,
                                                     size_t from, size_t to) const = 0;
    };

    /*
    @CLASS:         StyledTextImager::RunElement
    @DESCRIPTION:   <p>A simple summary structure typically used from
                @'StandardStyledTextImager::SummarizeStyleMarkers', or subclasses, to represent the net effect of overlapping
                style runs.</p>
                    <p><code><pre>
        struct  StyledTextImager::RunElement {
            ...
            StyleMarker*          fMarker;
            size_t                fLength;
            vector&lt;StyleMarker*&gt;  fSupercededMarkers;
            ...
        };
    </pre></code></p>
    */
    struct StyledTextImager::RunElement {
        RunElement (StyleMarker* marker, size_t length);

        StyleMarker*         fMarker{nullptr};
        size_t               fLength{0};
        vector<StyleMarker*> fSupercededMarkers;
    };

    /*
    @CLASS:         StyledTextImager::StyleMarkerSummarySink
    @BASES:         @'TextStore::MarkerSink'
    @DESCRIPTION:   <p>A marker sink used in calls to @'TextStore::CollectAllMarkersInRangeInto' to extract from the textstore
                the style marker information which applies to a given region of text. This is typically used from
                @'StandardStyledTextImager::SummarizeStyleMarkers', or subclasses. And you would override it (and that method)
                to provide an alternate mechanism for combining/interpretting style markers within a region (say when the overlap).</p>
    */
    class StyledTextImager::StyleMarkerSummarySink : public TextStore::MarkerSink {
    private:
        using inherited = TextStore::MarkerSink;

    public:
        StyleMarkerSummarySink (size_t from, size_t to);
        StyleMarkerSummarySink (size_t from, size_t to, const TextLayoutBlock& text);

    public:
        nonvirtual vector<RunElement> ProduceOutputSummary () const;

    public:
        virtual void Append (Marker* m) override;

    protected:
        virtual void CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker);

    private:
        nonvirtual void SplitIfNeededAt (size_t markerPos);

    private:
        vector<RunElement>     fBuckets;
        const TextLayoutBlock* fText; // make this a reference once I've gotten rid of CTOR that takes no TextLayoutBlock - LGP 2002-12-16
        size_t                 fFrom;
        size_t                 fTo;
    };

    /*
    @CLASS:         StyledTextImager::StyleMarkerSummarySinkForSingleOwner
    @BASES:         @'StyledTextImager::StyleMarkerSummarySink'
    @DESCRIPTION:   <p>Ignore style markers from an owner other than the one given as argument in the constructor.</p>
    */
    class StyledTextImager::StyleMarkerSummarySinkForSingleOwner : public StyledTextImager::StyleMarkerSummarySink {
    private:
        using inherited = StyleMarkerSummarySink;

    public:
        StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to);
        StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to, const TextLayoutBlock& text);

    protected:
        virtual void CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker) override;

    private:
        const MarkerOwner& fOwner;
    };

    /*
    @CLASS:         SimpleStyleMarkerByFontSpec<BASECLASS>
    @BASES:         BASECLASS = @'StyledTextImager::StyleMarker'
    @DESCRIPTION:   <p>Very frequently, you will want to implement a style-marker which just uses some @'FontSpecification'</p>
                and applies that to the given text, in a maner vaguely similar to what @'StandardStyledTextImager::StandardStyleMarker' does.
                    <p>This class is an abstract class, where you must specify the particular @'FontSpecification' by overriding
                the pure-virtual @'SimpleStyleMarkerByFontSpec::MakeFontSpec'. That font-spec is then used in the various
                DrawSegemnt () etc overrides.</p>
                    <p>This class is not intended to be an abstract interface one programs to, but rather a helper class for subclasses
                of the abstract @'StyledTextImager::StyleMarker' class. So you are not encouraged to declare variables of the type
                <code>SimpleStyleMarkerByFontSpec<>*</code>. Just use the class as a helper.</p>
                    <p>See @'TrivialFontSpecStyleMarker' for an even simpler class to use.</p>
    */
    template <class BASECLASS = StyledTextImager::StyleMarker>
    class SimpleStyleMarkerByFontSpec : public BASECLASS {
    private:
        using inherited = BASECLASS;

    public:
        using RunElement = StyledTextImager::RunElement;

    protected:
        SimpleStyleMarkerByFontSpec ();

    protected:
        /*
        @METHOD:        SimpleStyleMarkerByFontSpec<BASECLASS>::MakeFontSpec
        @DESCRIPTION:   <p>Virtual method which subclasses override to specify how <em>they</em> want to have the given text displayed.
                    Hopefully enough context is passed into this function to make this helper class widely applicable. All it must do is return
                    a simple @'FontSpecification' result, and that will be used for all measurements and
                    display of this marker.</p>
                        <p>By default, it just returns the default font associated with the imager.</p>
        */
        virtual FontSpecification MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const;

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Tablet* tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          CoordinateType useBaseLine, DistanceType* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  DistanceType*    distanceResults) const override;
        virtual DistanceType MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;
        virtual DistanceType MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;
    };

    /*
    @CLASS:         SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>
    @BASES:         BASECLASS (which should be a subclass of @'SimpleStyleMarkerByFontSpec<BASECLASS>')
    @DESCRIPTION:   <p>This helper can be used to avoid manually subclassing @'SimpleStyleMarkerByFontSpec<BASECLASS>'
                and instead, just takes an @'IncrementalFontSpecification' and does the right thing - mapping
                that into what is displayed.</p>
    */
    template <class BASECLASS>
    class SimpleStyleMarkerByIncrementalFontSpec : public BASECLASS {
    private:
        using inherited = BASECLASS;

    public:
        using RunElement = StyledTextImager::RunElement;

    public:
        SimpleStyleMarkerByIncrementalFontSpec (const IncrementalFontSpecification& styleInfo = IncrementalFontSpecification ());

    protected:
        virtual FontSpecification MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const override;

    public:
        IncrementalFontSpecification fFontSpecification;
    };

    /*
    @CLASS:         TrivialFontSpecStyleMarker
    @BASES:         @'SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>' with BASECLASS = @'SimpleStyleMarkerByFontSpec<BASECLASS>'.
    @DESCRIPTION:   <p>This class just adds to @'SimpleStyleMarkerByFontSpec' a field which is the @'IncrementalFontSpecification'.
                This is <em>not</em> intended to be subclassed. If you do subclass - beware the overload of operator new () and
                block-allocation usage. Or better yet, subclass @'SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>' instead.</p>
    */
    class TrivialFontSpecStyleMarker : public SimpleStyleMarkerByIncrementalFontSpec<SimpleStyleMarkerByFontSpec<>>, public Foundation::Memory::UseBlockAllocationIfAppropriate<TrivialFontSpecStyleMarker> {
    private:
        using inherited = SimpleStyleMarkerByIncrementalFontSpec<SimpleStyleMarkerByFontSpec<>>;

    public:
        TrivialFontSpecStyleMarker (const IncrementalFontSpecification& styleInfo);

    public:
        virtual int GetPriority () const override;
    };

    /*
    @CLASS:         SimpleStyleMarkerWithExtraDraw<BASECLASS>
    @BASES:         @'SimpleStyleMarkerByFontSpec<BASECLASS>' with BASECLASS defaulting to @'StyledTextImager::StyleMarker'
    @DESCRIPTION:   <p>Very frequently, you will want to implement a style-marker which just draws some
                extra stuff drawn at the end.</p>
                    <p>This class is an abstract class, where you must specify the particular extra drawing in the
                @'SimpleStyleMarkerWithExtraDraw<BASECLASS>::DrawExtra' method.</p>
                    <p>See also @'SimpleStyleMarkerWithLightUnderline<BASECLASS>'.</p>
    */
    template <typename BASECLASS = StyledTextImager::StyleMarker>
    class SimpleStyleMarkerWithExtraDraw : public SimpleStyleMarkerByFontSpec<BASECLASS> {
    private:
        using inherited = SimpleStyleMarkerByFontSpec<BASECLASS>;

    public:
        using RunElement = StyledTextImager::RunElement;

    protected:
        SimpleStyleMarkerWithExtraDraw () = default;

    protected:
        /*
        @METHOD:        SimpleStyleMarkerWithExtraDraw<BASECLASS>::DrawExtra
        @DESCRIPTION:   <p>Pure virtual method which subclasses override to specify how <em>they</em> want to
            draw (some additional markup - e.g. an underline).</p>
        */
        virtual void DrawExtra (const StyledTextImager* imager, const RunElement& runElement, Tablet* tablet,
                                size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
                                CoordinateType useBaseLine, DistanceType pixelsDrawn) = 0;

    protected:
        virtual RunElement MungeRunElement (const RunElement& inRunElt) const;

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Tablet* tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          CoordinateType useBaseLine, DistanceType* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  DistanceType*    distanceResults) const override;
        virtual DistanceType MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;
        virtual DistanceType MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;
    };

    /*
    @CLASS:         SimpleStyleMarkerWithLightUnderline<BASECLASS>
    @BASES:         BASECLASS
    @DESCRIPTION:   <p>Very frequently, you will want to implement a style-marker which just draws some
                extra stuff drawn at the end.</p>
                    <p>This template is typically used with the default BASECLASS of @'SimpleStyleMarkerWithExtraDraw<BASECLASS>'.</p>
    */
    template <class BASECLASS = SimpleStyleMarkerWithExtraDraw<StyledTextImager::StyleMarker>>
    class SimpleStyleMarkerWithLightUnderline : public BASECLASS {
    private:
        using inherited = BASECLASS;

    public:
        using RunElement = StyledTextImager::RunElement;

    public:
        SimpleStyleMarkerWithLightUnderline ();

    protected:
        virtual void DrawExtra (const StyledTextImager* imager, const RunElement& runElement, Tablet* tablet,
                                size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
                                CoordinateType useBaseLine, DistanceType pixelsDrawn) override;

    public:
        virtual Color GetUnderlineBaseColor () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StyledTextImager.inl"

#endif /*_Stroika_Frameworks_Led_StyledTextImager_h_*/
