/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_HiddenText_h_
#define _Stroika_Frameworks_Led_HiddenText_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    HiddenText
@DESCRIPTION:
        <p>This module provides generic support for keeping arbitrary ranges of text hidden within
    a Led buffer, but making it available for later display.</p>.
 */

#include "StandardStyledTextImager.h" //only for template used in light-underline-xxxx - maybe move to WP file... so dont need this include here!
#include "StyledTextImager.h"
#include "Support.h"
#include "TextInteractor.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4786)
#endif

            /*
            @CLASS:         HidableTextMarkerOwner
            @DESCRIPTION:   <p>HidableTextMarkerOwner is a @'MarkerOwner' class, collecting together a set of
                @'HidableTextMarkerOwner::HidableTextMarker' elements. Each of these hidden-text elements can contain
                either currently hidden text, or text which could potentially be hidden.</p>
                    <p>Note that not all @'HidableTextMarkerOwner::HidableTextMarker' elements need to share the same
                'hidden' / 'shown' state. Some can be hidden while others shown. And markers of different subclasses of
                @'HidableTextMarkerOwner::HidableTextMarker' can be collected together in a single, or in multiple
                @'HidableTextMarkerOwner's.</p>
                    <p>See @'HidableTextMarkerOwner::HidableTextMarker' for more details, and subclasses.</p>
                    <p><em>Implemenation Note:</em><br>
                    I considered implementing this using a map<> for its red-black tree implementation. That might turn out to
                be faster if you have lots of hidden text. But the APIs STL provides you when you try todo this are not very friendly.
                I battled them for a while (you must make a map with a second bogus field, and then everywhere try to say 'first' after
                you access the interator - and thats not so bad in MY code - but when I use the builtin STL helpers - its more of a pain, and
                then the lookup - find routine was a real pain - cuz I don't want to lookup by Marker* - but by POSITION).</p>
            */
            class HidableTextMarkerOwner : public MarkerOwner {
            private:
                using inherited = MarkerOwner;

            public:
                HidableTextMarkerOwner (TextStore& textStore);
                virtual ~HidableTextMarkerOwner ();

            public:
                virtual void HideAll ();
                virtual void HideAll (size_t from, size_t to);
                virtual void ShowAll ();
                virtual void ShowAll (size_t from, size_t to);

            public:
                virtual void MakeRegionHidable (size_t from, size_t to);
                virtual void MakeRegionUnHidable (size_t from, size_t to);

            public:
                nonvirtual DiscontiguousRun<bool> GetHidableRegions () const;
                nonvirtual DiscontiguousRun<bool> GetHidableRegions (size_t from, size_t to) const;

            public:
                nonvirtual bool GetHidableRegionsContiguous (size_t from, size_t to, bool hidden) const;

            public:
                nonvirtual shared_ptr<FlavorPackageInternalizer> GetInternalizer () const;
                nonvirtual void                                  SetInternalizer (const shared_ptr<FlavorPackageInternalizer>& i);

            private:
                shared_ptr<FlavorPackageInternalizer> fInternalizer;

            public:
                nonvirtual shared_ptr<FlavorPackageExternalizer> GetExternalizer () const;
                nonvirtual void                                  SetExternalizer (const shared_ptr<FlavorPackageExternalizer>& e);

            private:
                shared_ptr<FlavorPackageExternalizer> fExternalizer;

            protected:
                class HidableTextMarker;
                class FontSpecHidableTextMarker;
                class LightUnderlineHidableTextMarker;

                // called by 'HidableTextMarker's to compute their 'saved data'
            protected:
                virtual void CollapseMarker (HidableTextMarker* m);
                virtual void ReifyMarker (HidableTextMarker* m);

            protected:
                virtual HidableTextMarker* MakeHidableTextMarker ();

            public:
                virtual TextStore* PeekAtTextStore () const override;

            public:
                virtual void AboutToUpdateText (const UpdateInfo& updateInfo) override;
                virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

            private:
                TextStore&                                fTextStore;
                mutable MarkerMortuary<HidableTextMarker> fMarkersToBeDeleted;

            protected:
                using MarkerList = vector<HidableTextMarker*>;

            protected:
                nonvirtual MarkerList CollectAllInRange (size_t from, size_t to) const;
                nonvirtual MarkerList CollectAllInRange_OrSurroundings (size_t from, size_t to) const;

            public:
                nonvirtual void Invariant () const;

#if qDebug
            protected:
                virtual void Invariant_ () const;
#endif

            public:
                friend class HidableTextMarker;
            };

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

            /*
            @CLASS:         UniformHidableTextMarkerOwner
            @DESCRIPTION:   <p>UniformHidableTextMarkerOwner is a @'HidableTextMarkerOwner' class, which has a notion of
                whether or not <em>all</em> the hidden text markers are hidden or not. You can manually change the hidden
                state of particular markers - if you wish (via @'HidableTextMarkerOwner::HideAll ()' commands with a from/to range).</p>
                    <p>But the default behavior is that all the markers share this hidden or not hidden state, and NEWLY created
                'hiding' markers inherit this state.</p>
            */
            class UniformHidableTextMarkerOwner : public HidableTextMarkerOwner {
            private:
                using inherited = HidableTextMarkerOwner;

            public:
                UniformHidableTextMarkerOwner (TextStore& textStore);

            public:
                virtual void HideAll () override;
                virtual void ShowAll () override;

            public:
                nonvirtual bool IsHidden () const;

            public:
                virtual void MakeRegionHidable (size_t from, size_t to) override;

            private:
                bool fHidden;
            };

            /*
            @CLASS:         HidableTextMarkerOwner::HidableTextMarker
            @BASES:         @'StyledTextImager::StyleMarker'
            @DESCRIPTION:
            */
            class HidableTextMarkerOwner::HidableTextMarker : public StyledTextImager::StyleMarker {
            private:
                using inherited = StyledTextImager::StyleMarker;

            public:
                HidableTextMarker ();

            public:
                nonvirtual void Hide ();
                nonvirtual void Show ();
                nonvirtual bool IsShown () const;

            protected:
                nonvirtual HidableTextMarkerOwner& GetOwner () const;

            private:
                bool fShown;

            public:
                friend class HidableTextMarkerOwner;
            };

            /*
            @CLASS:         HidableTextMarkerHelper<BASECLASS>
            @BASES:         BASECLASS
            @DESCRIPTION:   <p>This class can be used with any 'BASECLASS' of subtype @'StyledTextImager::StyleMarker' and which also has
                        an IsShown () method. Really it should most probably just be used with subclasses of @'HidableTextMarkerOwner::HidableTextMarker'.
                        Note that the 'BASECLASS' should also be a CONCRETE base class - or at least have already defined its DrawSegment, etc methods. Basically
                        all this helper does is choose whether or not to invoke those inherited DrawSegment etc methods or to return fake zero-size versions.
                        </p>
            */
            template <typename BASECLASS>
            class HidableTextMarkerHelper : public BASECLASS {
            private:
                using inherited = BASECLASS;

            public:
                using RunElement = StyledTextImager::RunElement;

            public:
                HidableTextMarkerHelper ();

            public:
                virtual int GetPriority () const override;

            protected:
                virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                                  size_t from, size_t to, const TextLayoutBlock& text,
                                                  const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine,
                                                  Led_Distance* pixelsDrawn) override;
                virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement,
                                                          size_t from, size_t to,
                                                          const Led_tChar* text,
                                                          Led_Distance*    distanceResults) const override;
                virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement,
                                                           size_t from, size_t to) const override;

                virtual Led_Distance MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement,
                                                             size_t from, size_t to) const override;
            };

            /*
            @CLASS:         HidableTextMarkerOwner::FontSpecHidableTextMarker
            @BASES:         @'SimpleStyleMarkerByFontSpec<BASECLASS>, where BASECLASS = @'HidableTextMarkerHelper<BASECLASS>' where BASECLASS = @'HidableTextMarkerOwner::HidableTextMarker'
            @DESCRIPTION:   <p>Adds the argument style specification to the selected text. Smart enough to handle style overlaps with some kinds of
                style markers (most importantly - @'StandardStyledTextImager::StandardStyleMarker').</p>
                    <p>May have to also add embedding support to this. And may want to move this functionality out of here, and into
                some other helper file, and just USE the functionality here.</p>
                    <p>See also @'HidableTextMarkerOwner::LightUnderlineHidableTextMarker'.</p>
            */
            class HidableTextMarkerOwner::FontSpecHidableTextMarker : public HidableTextMarkerHelper<SimpleStyleMarkerByFontSpec<HidableTextMarkerOwner::HidableTextMarker>> {
            private:
                using inherited = HidableTextMarkerHelper<SimpleStyleMarkerByFontSpec<HidableTextMarkerOwner::HidableTextMarker>>;

            public:
                FontSpecHidableTextMarker (const Led_IncrementalFontSpecification& styleInfo);

            public:
                DECLARE_USE_BLOCK_ALLOCATION (FontSpecHidableTextMarker);

            protected:
                virtual Led_FontSpecification MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const override;

            public:
                Led_IncrementalFontSpecification fFontSpecification;
            };

            /*
            @CLASS:         HidableTextMarkerOwner::LightUnderlineHidableTextMarker
            @BASES:         @'HidableTextMarkerHelper<BASECLASS>', where BASECLASS = @'SimpleStyleMarkerWithLightUnderline<BASECLASS>, where BASECLASS = @'HidableTextMarkerOwner::HidableTextMarker'
            @DESCRIPTION:   <p>Adds a light dashed underline to the given region. One advantage of
                        this over @'HidableTextMarkerOwner::FontSpecHidableTextMarker' is that
                        it works well with other embeddings and display markers, cuz it lets them
                        do their drawing, and simply adds the underline.</p>
            */
            class HidableTextMarkerOwner::LightUnderlineHidableTextMarker : public HidableTextMarkerHelper<SimpleStyleMarkerWithLightUnderline<SimpleStyleMarkerByIncrementalFontSpec<SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<SimpleStyleMarkerWithExtraDraw<HidableTextMarkerOwner::HidableTextMarker>>>>> {
            private:
                using inherited = HidableTextMarkerHelper<SimpleStyleMarkerWithLightUnderline<SimpleStyleMarkerByIncrementalFontSpec<SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<SimpleStyleMarkerWithExtraDraw<HidableTextMarkerOwner::HidableTextMarker>>>>>;

            public:
                LightUnderlineHidableTextMarker (const Led_IncrementalFontSpecification& fsp = Led_IncrementalFontSpecification ());

                virtual Led_Color GetUnderlineBaseColor () const override;

            public:
                DECLARE_USE_BLOCK_ALLOCATION (LightUnderlineHidableTextMarker);
            };

            /*
            @CLASS:         ColoredUniformHidableTextMarkerOwner
            @BASES:         @'UniformHidableTextMarkerOwner'
            @DESCRIPTION:   <p>A @'UniformHidableTextMarkerOwner' where you can specify (simply) a color for the
                        hidable text markers (when they are shown).</p>
            */
            class ColoredUniformHidableTextMarkerOwner : public UniformHidableTextMarkerOwner {
            private:
                using inherited = UniformHidableTextMarkerOwner;

            public:
                ColoredUniformHidableTextMarkerOwner (TextStore& textStore);

            public:
                nonvirtual Led_Color GetColor () const;
                nonvirtual void      SetColor (const Led_Color& color);
                nonvirtual bool      GetColored () const;
                nonvirtual void      SetColored (bool colored);

            protected:
                nonvirtual void FixupSubMarkers ();

            protected:
                virtual HidableTextMarker* MakeHidableTextMarker () override;

            private:
                Led_Color fColor;
                bool      fColored;
            };

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            //  class   HidableTextMarkerOwner
            inline shared_ptr<FlavorPackageInternalizer> HidableTextMarkerOwner::GetInternalizer () const
            {
                return fInternalizer;
            }
            inline shared_ptr<FlavorPackageExternalizer> HidableTextMarkerOwner::GetExternalizer () const
            {
                return fExternalizer;
            }
            inline void HidableTextMarkerOwner::Invariant () const
            {
#if qDebug
                Invariant_ ();
#endif
            }

            //  class   HidableTextMarkerOwner::HidableTextMarker
            inline HidableTextMarkerOwner::HidableTextMarker::HidableTextMarker ()
                : inherited ()
                , fShown (true)
            {
            }
            inline HidableTextMarkerOwner& HidableTextMarkerOwner::HidableTextMarker::GetOwner () const
            {
                MarkerOwner*            mo  = inherited::GetOwner ();
                HidableTextMarkerOwner* hmo = dynamic_cast<HidableTextMarkerOwner*> (mo);
                EnsureNotNull (hmo);
                return *hmo;
            }
            inline void HidableTextMarkerOwner::HidableTextMarker::Hide ()
            {
                if (fShown) {
                    GetOwner ().CollapseMarker (this);
                }
                Ensure (not fShown);
            }
            inline void HidableTextMarkerOwner::HidableTextMarker::Show ()
            {
                if (not fShown) {
                    GetOwner ().ReifyMarker (this);
                }
                Ensure (fShown);
            }
            inline bool HidableTextMarkerOwner::HidableTextMarker::IsShown () const
            {
                return fShown;
            }

            //  class   HidableTextMarkerHelper<BASECLASS>
            template <typename BASECLASS>
            inline HidableTextMarkerHelper<BASECLASS>::HidableTextMarkerHelper ()
                : inherited ()
            {
            }
            template <typename BASECLASS>
            int HidableTextMarkerHelper<BASECLASS>::GetPriority () const
            {
                // simpleembedings are priorty 100 - maybe should use ENUM CONST? return 100;       // large enough to override most other markers (esp StyledTextImager::StyleMarker which we know how to aggregate).
                return 200; // large enough to override most other markers (esp StyledTextImager::StyleMarker which we know how to aggregate).
            }
            template <typename BASECLASS>
            void HidableTextMarkerHelper<BASECLASS>::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                                                  size_t from, size_t to, const TextLayoutBlock& text,
                                                                  const Led_Rect& drawInto, const Led_Rect& invalidRect, Led_Coordinate useBaseLine,
                                                                  Led_Distance* pixelsDrawn)
            {
                if (IsShown ()) {
                    inherited::DrawSegment (imager, runElement, tablet, from, to, text, drawInto, invalidRect, useBaseLine, pixelsDrawn);
                }
                else {
                    *pixelsDrawn = 0;
                }
            }
            template <typename BASECLASS>
            void HidableTextMarkerHelper<BASECLASS>::MeasureSegmentWidth (const StyledTextImager* imager, const StyledTextImager::RunElement& runElement, size_t from, size_t to, const Led_tChar* text,
                                                                          Led_Distance* distanceResults) const
            {
                if (IsShown ()) {
                    inherited::MeasureSegmentWidth (imager, runElement, from, to, text, distanceResults);
                }
                else {
                    size_t len = to - from;
                    for (size_t i = 0; i < len; ++i) {
                        distanceResults[i] = 0;
                    }
                }
            }
            template <typename BASECLASS>
            Led_Distance HidableTextMarkerHelper<BASECLASS>::MeasureSegmentHeight (const StyledTextImager* imager, const StyledTextImager::RunElement& runElement, size_t from, size_t to) const
            {
                if (IsShown ()) {
                    return inherited::MeasureSegmentHeight (imager, runElement, from, to);
                }
                else {
                    return 0;
                }
            }
            template <typename BASECLASS>
            Led_Distance HidableTextMarkerHelper<BASECLASS>::MeasureSegmentBaseLine (const StyledTextImager* imager, const StyledTextImager::RunElement& runElement, size_t from, size_t to) const
            {
                if (IsShown ()) {
                    return inherited::MeasureSegmentBaseLine (imager, runElement, from, to);
                }
                else {
                    return 0;
                }
            }

            //  class   FontSpecHidableTextMarkerOwner::FontSpecHidableTextMarker
            inline HidableTextMarkerOwner::FontSpecHidableTextMarker::FontSpecHidableTextMarker (const Led_IncrementalFontSpecification& styleInfo)
                : inherited ()
                , fFontSpecification (styleInfo)
            {
            }

            //  class   UniformHidableTextMarkerOwner
            inline bool UniformHidableTextMarkerOwner::IsHidden () const
            {
                return fHidden;
            }

            //  class   ColoredUniformHidableTextMarkerOwner
            inline ColoredUniformHidableTextMarkerOwner::ColoredUniformHidableTextMarkerOwner (TextStore& textStore)
                : inherited (textStore)
                , fColor (Led_Color::kRed)
                , fColored (false)
            {
            }
            inline Led_Color ColoredUniformHidableTextMarkerOwner::GetColor () const
            {
                return fColor;
            }
            inline void ColoredUniformHidableTextMarkerOwner::SetColor (const Led_Color& color)
            {
                fColor = color;
                FixupSubMarkers ();
            }
            inline bool ColoredUniformHidableTextMarkerOwner::GetColored () const
            {
                return fColored;
            }
            inline void ColoredUniformHidableTextMarkerOwner::SetColored (bool colored)
            {
                fColored = colored;
                FixupSubMarkers ();
            }
        }
    }
}

#endif /*_Stroika_Frameworks_Led_HiddenText_h_*/
