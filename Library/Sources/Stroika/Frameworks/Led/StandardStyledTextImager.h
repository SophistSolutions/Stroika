/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StandardStyledTextImager_h_
#define _Stroika_Frameworks_Led_StandardStyledTextImager_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

/*
@MODULE:    StandardStyledTextImager
@DESCRIPTION:   <p>@'StandardStyledTextImager' is not intended to be a general and flexible
    style run interface. Rather, it is intended to mimic the usual style-run
    support found in other text editors, like MS-Word, or Apples TextEdit.</p>
        <p>It keeps style runs in markers which are a subclass of StyledTextImager::StyleMarker,
    called @'StandardStyledTextImager::StandardStyleMarker'.</p>

 */

#include "Stroika/Frameworks/Led/StyledTextImager.h"

namespace Stroika::Frameworks::Led {

    /*
        @CLASS:         StyledInfoSummaryRecord
        @BASES:         @'FontSpecification'
        @DESCRIPTION:   <p>Add a length attribute to @'FontSpecification'. Used in specifying style runs by
            @'StandardStyledTextImager'.</p>
        */
    struct StyledInfoSummaryRecord : public FontSpecification {
    public:
        StyledInfoSummaryRecord (const FontSpecification& fontSpec, size_t length);

    public:
        size_t fLength;
    };

    /*
    @CLASS:         StandardStyledTextImager::AbstractStyleDatabaseRep
    @BASES:         @'MarkerOwner'
    @DESCRIPTION:   <p>xxx.</p>
    */
    class AbstractStyleDatabaseRep : public virtual MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        virtual vector<StyledInfoSummaryRecord> GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const                  = 0;
        virtual void    SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalFontSpecification& styleInfo) = 0;
        nonvirtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<StyledInfoSummaryRecord>& styleInfos);
        virtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const StyledInfoSummaryRecord* styleInfos) = 0;

        // Debug support
    public:
        nonvirtual void Invariant () const;
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

    /*
    @CLASS:         StandardStyledTextImager::StandardStyleMarker
    @BASES:         @'StyledTextImager::StyleMarker'
    @DESCRIPTION:
            <p>Private, implementation detail class. Part of @'StandardStyledTextImager' implementation.</p>
            <p>Should NOT be subclassed. These participate in routines to grab
        runs of style info, etc, and aren't really designed to be subclassed. Also, all elements
        of this type in the text buffer are summarily deleted upon deletion of the owning StyleDatabase.</p>
    */
    class StandardStyleMarker : public StyleMarker {
    private:
        using inherited = StyleMarker;

    public:
        StandardStyleMarker (const FontSpecification& styleInfo = GetStaticDefaultFont ());

#if qStroika_Frameworks_Led_SupportGDI
    public:
        virtual void DrawSegment (const StyledTextImager* imager, const StyleRunElement& runElement, Tablet* tablet, size_t from, size_t to,
                                  const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                  CoordinateType useBaseLine, DistanceType* pixelsDrawn) override;
        virtual void MeasureSegmentWidth (const StyledTextImager* imager, const StyleRunElement& runElement, size_t from, size_t to,
                                          const Led_tChar* text, DistanceType* distanceResults) const override;
        virtual DistanceType MeasureSegmentHeight (const StyledTextImager* imager, const StyleRunElement& runElement, size_t from, size_t to) const override;
        virtual DistanceType MeasureSegmentBaseLine (const StyledTextImager* imager, const StyleRunElement& runElement, size_t from, size_t to) const override;
#endif

    public:
        nonvirtual FontSpecification GetInfo () const;
        nonvirtual void              SetInfo (const FontSpecification& fsp);

    public:
        FontSpecification fFontSpecification;
    };

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         StandardStyledTextImager::StyleDatabaseRep
    @BASES:         @'MarkerCover'<@'StandardStyleMarker',@'FontSpecification',@'IncrementalFontSpecification'>
    @DESCRIPTION:   <p>Reference counted object which stores all the style runs objects for a
        @'StandardStyledTextImager'. An explicit storage object like this is used, instead of using
        the @'StandardStyledTextImager' itself, so as too allow you to have either multiple views onto the
        same text which use the SAME database of style runs, or to allow using different style info databases,
        all live on the same text.</p>
    */
    class StyleDatabaseRep : public AbstractStyleDatabaseRep, private MarkerCover<StandardStyleMarker, FontSpecification, IncrementalFontSpecification> {
    private:
        using inheritedMC = MarkerCover<StandardStyleMarker, FontSpecification, IncrementalFontSpecification>;

    public:
        StyleDatabaseRep (TextStore& textStore);

    public:
        virtual vector<StyledInfoSummaryRecord> GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const override;
        virtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalFontSpecification& styleInfo) override;
        virtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const StyledInfoSummaryRecord* styleInfos) override;

#if qDebug
    protected:
        virtual void Invariant_ () const override;
#endif
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    /*
    @CLASS:         SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper
    @BASES:         BASECLASS
    @DESCRIPTION:   <p>Simple helper class so that @'SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>' will use the font specification of any
                embedded @'StandardStyledTextImager::StandardStyleMarker' will have its associated @'FontSpecification' copied out and used
                in the FontSpec.</p>
                    <p>As a kludge - this class depends on the user ALSO using the template @'SimpleStyleMarkerWithExtraDraw<BASECLASS>'. We override
                its @'SimpleStyleMarkerWithExtraDraw<BASECLASS>::MungeRunElement' () method. This isn't strictly necesary, but it saves us alot of code.
                That way - we can just change the RunElt to return nullptr for the marker - and then the caller will use its default algorithm, and call
                the MakeFontSpec () method.</p>
                    <p>The nature of class class could change in the future - so its not recomended that anyone directly use it.</p>
    */
    template <class BASECLASS>
    class SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper : public BASECLASS {
    private:
        using inherited = BASECLASS;

    protected:
        virtual StyleRunElement MungeRunElement (const StyleRunElement& inRunElt) const override;

#if qStroika_Frameworks_Led_SupportGDI
    protected:
        virtual FontSpecification MakeFontSpec (const StyledTextImager* imager, const StyleRunElement& runElement) const override;
#endif

    private:
        mutable FontSpecification fFSP;
    };

#if qStroika_Frameworks_Led_SupportGDI
    /*
    @CLASS:         StandardStyledTextImager
    @BASES:         virtual @'StyledTextImager'
    @DESCRIPTION:   <p>StandardStyledTextImager is not intended to be a general and flexible
                style run interface (like @'StyledTextImager'). Rather, it is intended to mimic
                traditional API for handling styled text found in other text editors, like MS-Word,
                or Apples TextEdit.
                </p>

                    <p>You can specify ranges of text, and apply font styles to them.
                And there are routines (like @'StandardStyledTextImager::GetContinuousStyleInfo' etc)
                to help find/idenitfiy font style runs, etc.</p>

                    <p>This class is built trivially ontop of the @'StyledTextImager'.
                The rendering of styles and layout issues are all taken care of.
                This class is merely responsable for preserving a disjoint cover (partition) of styles
                (@'StandardStyledTextImager::StandardStyleMarker'), and coalescing adjacent
                ones that have the same font info.</p>
    */
    class StandardStyledTextImager : public virtual StyledTextImager {
    private:
        using inherited = StyledTextImager;

    protected:
        StandardStyledTextImager ()          = default;
        virtual ~StandardStyledTextImager () = default;

    protected:
        virtual void    HookLosingTextStore () override;
        nonvirtual void HookLosingTextStore_ ();
        virtual void    HookGainedNewTextStore () override;
        nonvirtual void HookGainedNewTextStore_ ();

        /*
     *  Interface to getting and setting STANDARD style info. These routines ignore
     *  custom style markers.
     */
    public:
        nonvirtual FontSpecification GetStyleInfo (size_t charAfterPos) const;
        nonvirtual vector<StyledInfoSummaryRecord> GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const;
        nonvirtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalFontSpecification& styleInfo);
        nonvirtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<StyledInfoSummaryRecord>& styleInfos);
        nonvirtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const StyledInfoSummaryRecord* styleInfos);

    public:
        virtual FontMetrics GetFontMetricsAt (size_t charAfterPos) const override;

    public:
        virtual FontSpecification GetDefaultSelectionFont () const override;

        // Do macstyle lookalike routines (DoSetStyle, DoContinuous etc)
    public:
        virtual IncrementalFontSpecification GetContinuousStyleInfo (size_t from, size_t nTChars) const; // was DoContinuousStyle()
    protected:
        nonvirtual IncrementalFontSpecification GetContinuousStyleInfo_ (const vector<StyledInfoSummaryRecord>& summaryInfo) const;

#if qPlatform_MacOS
    public:
        nonvirtual bool DoContinuousStyle_Mac (size_t from, size_t nTChars, short* mode, TextStyle* theStyle);
#endif

#if qPlatform_MacOS
        // macstyle routines to get/set 'styl' resources for range of text...
    public:
        static vector<StyledInfoSummaryRecord> Convert (const ScrpSTElement* teScrapFmt, size_t nElts);
        static void Convert (const vector<StyledInfoSummaryRecord>& fromLedStyleRuns, ScrpSTElement* teScrapFmt); // Assumed pre-alloced and same legnth as fromLedStyleRuns
#endif

    public:
        nonvirtual shared_ptr<AbstractStyleDatabaseRep> GetStyleDatabase () const;
        nonvirtual void                                 SetStyleDatabase (const shared_ptr<AbstractStyleDatabaseRep>& styleDatabase);

    protected:
        virtual void HookStyleDatabaseChanged ();

    private:
        shared_ptr<AbstractStyleDatabaseRep> fStyleDatabase{nullptr};
        bool                                 fICreatedDatabase{false};

// Debug support
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StandardStyledTextImager.inl"

#endif /*_Stroika_Frameworks_Led_StandardStyledTextImager_h_*/
