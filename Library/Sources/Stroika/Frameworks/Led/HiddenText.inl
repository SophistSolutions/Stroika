/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ***************************** HidableTextMarkerOwner ***************************
     ********************************************************************************
     */
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

    /*
     ********************************************************************************
     ******************* HidableTextMarkerOwner::HidableTextMarker ******************
     ********************************************************************************
     */
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

    /*
     ********************************************************************************
     ********************* HidableTextMarkerHelper<BASECLASS> ***********************
     ********************************************************************************
     */
    template <typename BASECLASS>
    int HidableTextMarkerHelper<BASECLASS>::GetPriority () const
    {
        // simpleembedings are priorty 100 - maybe should use ENUM CONST? return 100;       // large enough to override most other markers (esp StyledTextImager::StyleMarker which we know how to aggregate).
        return 200; // large enough to override most other markers (esp StyledTextImager::StyleMarker which we know how to aggregate).
    }
#if qStroika_Frameworks_Led_SupportGDI
    template <typename BASECLASS>
    void HidableTextMarkerHelper<BASECLASS>::DrawSegment (const StyledTextImager* imager, const StyleRunElement& runElement, Tablet* tablet,
                                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
                                                          const Led_Rect& invalidRect, CoordinateType useBaseLine, DistanceType* pixelsDrawn)
    {
        if (this->IsShown ()) {
            inherited::DrawSegment (imager, runElement, tablet, from, to, text, drawInto, invalidRect, useBaseLine, pixelsDrawn);
        }
        else {
            *pixelsDrawn = 0;
        }
    }
    template <typename BASECLASS>
    void HidableTextMarkerHelper<BASECLASS>::MeasureSegmentWidth (const StyledTextImager* imager, const StyleRunElement& runElement, size_t from,
                                                                  size_t to, const Led_tChar* text, DistanceType* distanceResults) const
    {
        if (this->IsShown ()) {
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
    DistanceType HidableTextMarkerHelper<BASECLASS>::MeasureSegmentHeight (const StyledTextImager* imager,
                                                                           const StyleRunElement& runElement, size_t from, size_t to) const
    {
        if (this->IsShown ()) {
            return inherited::MeasureSegmentHeight (imager, runElement, from, to);
        }
        else {
            return 0;
        }
    }
    template <typename BASECLASS>
    DistanceType HidableTextMarkerHelper<BASECLASS>::MeasureSegmentBaseLine (const StyledTextImager* imager,
                                                                             const StyleRunElement& runElement, size_t from, size_t to) const
    {
        if (this->IsShown ()) {
            return inherited::MeasureSegmentBaseLine (imager, runElement, from, to);
        }
        else {
            return 0;
        }
    }
#endif

    /*
     ********************************************************************************
     *************** HidableTextMarkerOwner::FontSpecHidableTextMarker **************
     ********************************************************************************
     */
    inline HidableTextMarkerOwner::FontSpecHidableTextMarker::FontSpecHidableTextMarker (const IncrementalFontSpecification& styleInfo)
        : inherited{}
        , fFontSpecification{styleInfo}
    {
    }

    /*
     ********************************************************************************
     ************************ UniformHidableTextMarkerOwner *************************
     ********************************************************************************
     */
    inline bool UniformHidableTextMarkerOwner::IsHidden () const
    {
        return fHidden;
    }

    /*
     ********************************************************************************
     ************************ ColoredUniformHidableTextMarkerOwner ******************
     ********************************************************************************
     */
    inline ColoredUniformHidableTextMarkerOwner::ColoredUniformHidableTextMarkerOwner (TextStore& textStore)
        : inherited{textStore}
        , fColor{Color::kRed}
        , fColored{false}
    {
    }
    inline Color ColoredUniformHidableTextMarkerOwner::GetColor () const
    {
        return fColor;
    }
    inline void ColoredUniformHidableTextMarkerOwner::SetColor (const Color& color)
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
