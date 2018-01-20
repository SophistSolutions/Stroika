/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StandardStyledTextImager_h_
#define _Stroika_Frameworks_Led_StandardStyledTextImager_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    StandardStyledTextImager
@DESCRIPTION:   <p>@'StandardStyledTextImager' is not intended to be a general and flexible
    style run interface. Rather, it is intended to mimic the usual style-run
    support found in other text editors, like MS-Word, or Apples TextEdit.</p>
        <p>It keeps style runs in markers which are a subclass of StyledTextImager::StyleMarker,
    called @'StandardStyledTextImager::StandardStyleMarker'.</p>

 */

#include "StyledTextImager.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {

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
                virtual ~StandardStyledTextImager ();

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

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

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

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

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

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */

            // class StandardStyledTextImager::StandardStyleMarker
            inline StandardStyledTextImager::StandardStyleMarker::StandardStyleMarker (const Led_FontSpecification& styleInfo)
                : StyleMarker ()
                , fFontSpecification (styleInfo)
            {
            }
            inline Led_FontSpecification StandardStyledTextImager::StandardStyleMarker::GetInfo () const
            {
                return fFontSpecification;
            }
            inline void StandardStyledTextImager::StandardStyleMarker::SetInfo (const Led_FontSpecification& fsp)
            {
                fFontSpecification = fsp;
            }

            // class StandardStyledTextImager::InfoSummaryRecord
            inline StandardStyledTextImager::InfoSummaryRecord::InfoSummaryRecord (const Led_FontSpecification& fontSpec, size_t length)
                : Led_FontSpecification (fontSpec)
                , fLength (length)
            {
            }

            // class StandardStyledTextImager
            inline StandardStyledTextImager::StyleDatabasePtr StandardStyledTextImager::GetStyleDatabase () const
            {
                return fStyleDatabase;
            }
            inline Led_FontSpecification StandardStyledTextImager::GetStyleInfo (size_t charAfterPos) const
            {
                vector<StandardStyledTextImager::InfoSummaryRecord> result = fStyleDatabase->GetStyleInfo (charAfterPos, 1);
                Assert (result.size () == 1);
                return result[0];
            }
            inline vector<StandardStyledTextImager::InfoSummaryRecord> StandardStyledTextImager::GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const
            {
                return (fStyleDatabase->GetStyleInfo (charAfterPos, nTCharsFollowing));
            }
            inline void StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo)
            {
                fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfo);
            }
            inline void StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos)
            {
                fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos);
            }
            inline void StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos)
            {
                fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, nStyleInfos, styleInfos);
            }

            // class StandardStyledTextImager::AbstractStyleDatabaseRep
            inline void StandardStyledTextImager::AbstractStyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos)
            {
                SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos.size (), &styleInfos.front ());
            }
            inline void StandardStyledTextImager::AbstractStyleDatabaseRep::Invariant () const
            {
#if qDebug
                Invariant_ ();
#endif
            }

            // class SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>
            template <class BASECLASS>
            typename SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::RunElement SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::MungeRunElement (const RunElement& inRunElt) const
            {
                using StyleMarker = StyledTextImager::StyleMarker;
                using SSM         = StandardStyledTextImager::StandardStyleMarker;

                fFSP              = Led_FontSpecification ();
                RunElement result = inherited::MungeRunElement (inRunElt);
                for (auto i = result.fSupercededMarkers.begin (); i != result.fSupercededMarkers.end (); ++i) {
                    if (SSM* ssm = dynamic_cast<SSM*> (*i)) {
                        fFSP = ssm->fFontSpecification;
                    }
                }
                if (SSM* ssm = dynamic_cast<SSM*> (result.fMarker)) {
                    fFSP           = ssm->fFontSpecification;
                    result.fMarker = nullptr;
                }
                return result;
            }
            template <class BASECLASS>
            Led_FontSpecification SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::MakeFontSpec (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/) const
            {
                return fFSP;
            }
        }
    }
}

#endif /*_Stroika_Frameworks_Led_StandardStyledTextImager_h_*/
