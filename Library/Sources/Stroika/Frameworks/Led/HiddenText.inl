/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_HiddenText_inl_
#define _Stroika_Framework_Led_HiddenText_inl_ 1

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

#endif /*_Stroika_Framework_Led_HiddenText_inl_*/
