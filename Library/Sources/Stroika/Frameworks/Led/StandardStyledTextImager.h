/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StandardStyledTextImager_h_
#define _Stroika_Frameworks_Led_StandardStyledTextImager_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    StandardStyledTextImager
@DESCRIPTION:   <p>@'StandardStyledTextImager' is not intended to be a general and flexible
    style run interface. Rather, it is intended to mimic the usual style-run
    support found in other text editors, like MS-Word, or Apples TextEdit.</p>
        <p>It keeps style runs in markers which are a subclass of StyledTextImager::StyleMarker,
    called @'StandardStyledTextImager::StandardStyleMarker'.</p>

 */

#include "StyledTextImager.h"

namespace Stroika::Frameworks::Led {

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
        StandardStyledTextImager ();
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
        /*
        @CLASS:         StandardStyledTextImager::InfoSummaryRecord
        @BASES:         @'Led_FontSpecification'
        @DESCRIPTION:   <p>Add a length attribute to @'Led_FontSpecification'. Used in specifying style runs by
            @'StandardStyledTextImager'.</p>
        */
        struct InfoSummaryRecord : public Led_FontSpecification {
        public:
            InfoSummaryRecord (const Led_FontSpecification& fontSpec, size_t length);

        public:
            size_t fLength;
        };

        nonvirtual Led_FontSpecification GetStyleInfo (size_t charAfterPos) const;
        nonvirtual                       vector<InfoSummaryRecord>
        GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const;
        nonvirtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo);
        nonvirtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos);
        nonvirtual void SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos);

    public:
        virtual Led_FontMetrics GetFontMetricsAt (size_t charAfterPos) const override;

    public:
        virtual Led_FontSpecification GetDefaultSelectionFont () const override;

        // Do macstyle lookalike routines (DoSetStyle, DoContinuous etc)
    public:
        virtual Led_IncrementalFontSpecification GetContinuousStyleInfo (size_t from, size_t nTChars) const; // was DoContinuousStyle()
    protected:
        nonvirtual Led_IncrementalFontSpecification GetContinuousStyleInfo_ (const vector<InfoSummaryRecord>& summaryInfo) const;

#if qPlatform_MacOS
    public:
        nonvirtual bool DoContinuousStyle_Mac (size_t from, size_t nTChars, short* mode, TextStyle* theStyle);
#endif

#if qPlatform_MacOS
        // macstyle routines to get/set 'styl' resources for range of text...
    public:
        static vector<InfoSummaryRecord> Convert (const ScrpSTElement* teScrapFmt, size_t nElts);
        static void                      Convert (const vector<InfoSummaryRecord>& fromLedStyleRuns, ScrpSTElement* teScrapFmt); // Assumed pre-alloced and same legnth as fromLedStyleRuns
#endif

    public:
        class StandardStyleMarker;

    public:
        class AbstractStyleDatabaseRep;
        class StyleDatabaseRep;

    public:
        /*
        @CLASS:         StandardStyledTextImager::StyleDatabasePtr
        @BASES:         @'shared_ptr<T>', (T=@'StandardStyledTextImager::StyleDatabaseRep')
        @DESCRIPTION:
        */
        using StyleDatabasePtr = shared_ptr<AbstractStyleDatabaseRep>;

    public:
        nonvirtual StyleDatabasePtr GetStyleDatabase () const;
        nonvirtual void             SetStyleDatabase (const StyleDatabasePtr& styleDatabase);

    protected:
        virtual void HookStyleDatabaseChanged ();

    private:
        StyleDatabasePtr fStyleDatabase;
        bool             fICreatedDatabase;

// Debug support
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
    class StandardStyledTextImager::StandardStyleMarker : public StyledTextImager::StyleMarker {
    private:
        using inherited = StyledTextImager::StyleMarker;

    public:
        StandardStyleMarker (const Led_FontSpecification& styleInfo = TextImager::GetStaticDefaultFont ());

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;
        virtual Led_Distance MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

    public:
        nonvirtual Led_FontSpecification GetInfo () const;
        nonvirtual void                  SetInfo (const Led_FontSpecification& fsp);

    public:
        Led_FontSpecification fFontSpecification;
    };

    /*
    @CLASS:         StandardStyledTextImager::AbstractStyleDatabaseRep
    @BASES:         @'MarkerOwner'
    @DESCRIPTION:   <p>xxx.</p>
    */
    class StandardStyledTextImager::AbstractStyleDatabaseRep : public virtual MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        virtual vector<InfoSummaryRecord> GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const                                              = 0;
        virtual void                      SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo) = 0;
        nonvirtual void                   SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos);
        virtual void                      SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos) = 0;

        // Debug support
    public:
        nonvirtual void Invariant () const;
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         StandardStyledTextImager::StyleDatabaseRep
    @BASES:         @'MarkerCover'<@'StandardStyleMarker',@'Led_FontSpecification',@'Led_IncrementalFontSpecification'>
    @DESCRIPTION:   <p>Reference counted object which stores all the style runs objects for a
        @'StandardStyledTextImager'. An explicit storage object like this is used, instead of using
        the @'StandardStyledTextImager' itself, so as too allow you to have either multiple views onto the
        same text which use the SAME database of style runs, or to allow using different style info databases,
        all live on the same text.</p>
    */
    class StandardStyledTextImager::StyleDatabaseRep : public StandardStyledTextImager::AbstractStyleDatabaseRep, private MarkerCover<StandardStyledTextImager::StandardStyleMarker, Led_FontSpecification, Led_IncrementalFontSpecification> {
    private:
        using inheritedMC = MarkerCover<StandardStyledTextImager::StandardStyleMarker, Led_FontSpecification, Led_IncrementalFontSpecification>;

    public:
        StyleDatabaseRep (TextStore& textStore);

    public:
        virtual vector<InfoSummaryRecord> GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const override;
        virtual void                      SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo) override;
        virtual void                      SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos) override;

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
                embedded @'StandardStyledTextImager::StandardStyleMarker' will have its associated @'Led_FontSpecification' copied out and used
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

    public:
        using RunElement = StyledTextImager::RunElement;

    protected:
        virtual RunElement MungeRunElement (const RunElement& inRunElt) const override;

    protected:
        virtual Led_FontSpecification MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const override;

    private:
        mutable Led_FontSpecification fFSP;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StandardStyledTextImager.inl"

#endif /*_Stroika_Frameworks_Led_StandardStyledTextImager_h_*/
