/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_StandardStyledTextImager_inl_
#define _Stroika_Framework_Led_StandardStyledTextImager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     *************************** StandardStyledTextImager ***************************
     ********************************************************************************
     */
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

#endif /*_Stroika_Framework_Led_StandardStyledTextImager_inl_*/
