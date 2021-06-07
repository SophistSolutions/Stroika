/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_StyledTextImager_inl_
#define _Stroika_Framework_Led_StyledTextImager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ************************ StyledTextImager::StyleMarker *************************
     ********************************************************************************
     */
    inline StyledTextImager::StyleMarker::StyleMarker ()
        : Marker ()
    {
    }
    inline void StyledTextImager::Invariant () const
    {
#if qDebug && qHeavyDebugging
        Invariant_ ();
#endif
    }

    // class StyledTextImager::RunElement
    inline StyledTextImager::RunElement::RunElement (StyledTextImager::StyleMarker* marker, size_t length)
        : fMarker (marker)
        , fLength (length)
    {
    }

    // class SimpleStyleMarkerByFontSpec<BASECLASS>
    template <class BASECLASS>
    inline SimpleStyleMarkerByFontSpec<BASECLASS>::SimpleStyleMarkerByFontSpec ()
        : inherited ()
    {
    }
    template <class BASECLASS>
    FontSpecification SimpleStyleMarkerByFontSpec<BASECLASS>::MakeFontSpec (const StyledTextImager* imager, const RunElement& /*runElement*/) const
    {
        RequireNotNull (imager);
        return imager->GetDefaultFont ();
    }
    template <class BASECLASS>
    void SimpleStyleMarkerByFontSpec<BASECLASS>::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Tablet* tablet,
                                                              size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                                              CoordinateType useBaseLine, DistanceType* pixelsDrawn)
    {
        RequireNotNull (imager);
        imager->DrawSegment_ (tablet, MakeFontSpec (imager, runElement), from, to, text, drawInto, useBaseLine, pixelsDrawn);
    }
    template <class BASECLASS>
    void SimpleStyleMarkerByFontSpec<BASECLASS>::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                                      const Led_tChar* text,
                                                                      DistanceType*    distanceResults) const
    {
        RequireNotNull (imager);
        imager->MeasureSegmentWidth_ (this->MakeFontSpec (imager, runElement), from, to, text, distanceResults);
    }
    template <class BASECLASS>
    DistanceType SimpleStyleMarkerByFontSpec<BASECLASS>::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
    {
        RequireNotNull (imager);
        return (imager->MeasureSegmentHeight_ (this->MakeFontSpec (imager, runElement), from, to));
    }
    template <class BASECLASS>
    DistanceType SimpleStyleMarkerByFontSpec<BASECLASS>::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
    {
        RequireNotNull (imager);
        return (imager->MeasureSegmentBaseLine_ (this->MakeFontSpec (imager, runElement), from, to));
    }

    // class SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>
    template <class BASECLASS>
    inline SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>::SimpleStyleMarkerByIncrementalFontSpec (const IncrementalFontSpecification& styleInfo)
        : fFontSpecification (styleInfo)
    {
    }
    template <class BASECLASS>
    FontSpecification SimpleStyleMarkerByIncrementalFontSpec<BASECLASS>::MakeFontSpec (const StyledTextImager* imager, const RunElement& runElement) const
    {
        RequireNotNull (imager);
        FontSpecification fsp = inherited::MakeFontSpec (imager, runElement);
        fsp.MergeIn (fFontSpecification);
        return fsp;
    }

    // class TrivialFontSpecStyleMarker
    inline TrivialFontSpecStyleMarker::TrivialFontSpecStyleMarker (const IncrementalFontSpecification& styleInfo)
        : inherited (styleInfo)
    {
    }

    // class SimpleStyleMarkerWithExtraDraw<BASECLASS>
    template <class BASECLASS>
    typename SimpleStyleMarkerWithExtraDraw<BASECLASS>::RunElement SimpleStyleMarkerWithExtraDraw<BASECLASS>::MungeRunElement (const RunElement& inRunElt) const
    {
        using StyleMarker        = StyledTextImager::StyleMarker;
        RunElement newRunElement = inRunElt;
        Require (inRunElt.fMarker == (StyleMarker*)this);
        newRunElement.fMarker = nullptr;
        for (auto i = newRunElement.fSupercededMarkers.begin (); i != newRunElement.fSupercededMarkers.end ();) {
            if (newRunElement.fMarker == nullptr or newRunElement.fMarker->GetPriority () < (*i)->GetPriority ()) {
                newRunElement.fMarker = (*i);
                i                     = newRunElement.fSupercededMarkers.erase (i);
            }
            else {
                ++i;
            }
        }
        return newRunElement;
    }
    template <class BASECLASS>
    void SimpleStyleMarkerWithExtraDraw<BASECLASS>::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Tablet* tablet,
                                                                 size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
                                                                 CoordinateType useBaseLine, DistanceType* pixelsDrawn)
    {
        RequireNotNull (imager);
        AssertNotNull (pixelsDrawn); // if allowed to pass nullptr - I must pass my  own value so I can pass along to DrawExtra - see if anyone does this - LGP 2000-07-08

        RunElement re = MungeRunElement (runElement);
        if (re.fMarker == nullptr) {
            imager->DrawSegment_ (tablet, this->MakeFontSpec (imager, re), from, to, text, drawInto, useBaseLine, pixelsDrawn);
        }
        else {
            re.fMarker->DrawSegment (imager, re, tablet, from, to, text, drawInto, invalidRect, useBaseLine, pixelsDrawn);
        }
        DrawExtra (imager, re, tablet, from, to, text, drawInto, useBaseLine, *pixelsDrawn);
    }
    template <class BASECLASS>
    void SimpleStyleMarkerWithExtraDraw<BASECLASS>::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                                         const Led_tChar* text,
                                                                         DistanceType*    distanceResults) const
    {
        RequireNotNull (imager);
        RunElement re = MungeRunElement (runElement);
        if (re.fMarker == nullptr) {
            imager->MeasureSegmentWidth_ (this->MakeFontSpec (imager, re), from, to, text, distanceResults);
        }
        else {
            re.fMarker->MeasureSegmentWidth (imager, re, from, to, text, distanceResults);
        }
    }
    template <class BASECLASS>
    DistanceType SimpleStyleMarkerWithExtraDraw<BASECLASS>::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
    {
        RequireNotNull (imager);
        RunElement re = MungeRunElement (runElement);
        if (re.fMarker == nullptr) {
            return (imager->MeasureSegmentHeight_ (this->MakeFontSpec (imager, re), from, to));
        }
        else {
            return (re.fMarker->MeasureSegmentHeight (imager, re, from, to));
        }
    }
    template <class BASECLASS>
    DistanceType SimpleStyleMarkerWithExtraDraw<BASECLASS>::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
    {
        RequireNotNull (imager);
        RunElement re = MungeRunElement (runElement);
        if (re.fMarker == nullptr) {
            return (imager->MeasureSegmentBaseLine_ (this->MakeFontSpec (imager, re), from, to));
        }
        else {
            return (re.fMarker->MeasureSegmentBaseLine (imager, re, from, to));
        }
    }

    // class SimpleStyleMarkerWithLightUnderline<BASECLASS>
    template <typename BASECLASS>
    inline SimpleStyleMarkerWithLightUnderline<BASECLASS>::SimpleStyleMarkerWithLightUnderline ()
        : inherited ()
    {
    }
    template <typename BASECLASS>
    void SimpleStyleMarkerWithLightUnderline<BASECLASS>::DrawExtra (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, Tablet* tablet,
                                                                    size_t /*from*/, size_t /*to*/, const TextLayoutBlock& /*text*/, const Led_Rect& drawInto,
                                                                    CoordinateType useBaseLine, DistanceType pixelsDrawn)
    {
        Color lightColor = Color::kWhite / 2 + GetUnderlineBaseColor () / 2; // (white - baseColor)/2 + baseColor, but careful to avoid int overflow...
#if qPlatform_Windows
        Pen pen (PS_DOT, 1, lightColor.GetOSRep ());
#elif qPlatform_MacOS
        Pen pen (patCopy, &Pen::kGrayPattern, lightColor);
#elif qStroika_FeatureSupported_XWindows
        Pen pen;
#endif
        GDI_Obj_Selector penWrapper (tablet, pen);
        CoordinateType   underlineAt = useBaseLine;
        if (underlineAt + 1 < drawInto.bottom) {
            underlineAt += 1;
        }
        tablet->MoveTo (Led_Point (underlineAt, drawInto.left));
        tablet->LineTo (Led_Point (underlineAt, drawInto.left + pixelsDrawn));
    }
    template <typename BASECLASS>
    Color SimpleStyleMarkerWithLightUnderline<BASECLASS>::GetUnderlineBaseColor () const
    {
        return Color::kBlack;
    }

}

#endif /*_Stroika_Framework_Led_StyledTextImager_inl_*/
