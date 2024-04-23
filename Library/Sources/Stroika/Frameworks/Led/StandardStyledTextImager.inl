/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     *************************** StyledInfoSummaryRecord ****************************
     ********************************************************************************
     */
    inline StyledInfoSummaryRecord::StyledInfoSummaryRecord (const FontSpecification& fontSpec, size_t length)
        : FontSpecification{fontSpec}
        , fLength{length}
    {
    }

    /*
     ********************************************************************************
     **************************** StyledInfoSummaryRecord ***************************
     ********************************************************************************
     */
    inline void AbstractStyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<StyledInfoSummaryRecord>& styleInfos)
    {
        SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos.size (), &styleInfos.front ());
    }
    inline void AbstractStyleDatabaseRep::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }

    /*
     ********************************************************************************
     ****************************** StandardStyleMarker *****************************
     ********************************************************************************
     */
    inline StandardStyleMarker::StandardStyleMarker (const FontSpecification& styleInfo)
        : fFontSpecification{styleInfo}
    {
    }
    inline FontSpecification StandardStyleMarker::GetInfo () const
    {
        return fFontSpecification;
    }
    inline void StandardStyleMarker::SetInfo (const FontSpecification& fsp)
    {
        fFontSpecification = fsp;
    }

    /*
     ********************************************************************************
     ** SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS> **
     ********************************************************************************
     */
    template <class BASECLASS>
    StyleRunElement SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::MungeRunElement (const StyleRunElement& inRunElt) const
    {
        using SSM = StandardStyleMarker;

        fFSP                   = FontSpecification{};
        StyleRunElement result = inherited::MungeRunElement (inRunElt);
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
#if qStroika_Frameworks_Led_SupportGDI
    template <class BASECLASS>
    FontSpecification SimpleStyleMarkerByIncrementalFontSpecStandardStyleMarkerHelper<BASECLASS>::MakeFontSpec (const StyledTextImager* /*imager*/,
                                                                                                                const StyleRunElement& /*runElement*/) const
    {
        return fFSP;
    }
#endif

#if qStroika_Frameworks_Led_SupportGDI
    /*
     ********************************************************************************
     *************************** StandardStyledTextImager ***************************
     ********************************************************************************
     */
    inline shared_ptr<AbstractStyleDatabaseRep> StandardStyledTextImager::GetStyleDatabase () const
    {
        return fStyleDatabase;
    }
    inline FontSpecification StandardStyledTextImager::GetStyleInfo (size_t charAfterPos) const
    {
        vector<StyledInfoSummaryRecord> result = fStyleDatabase->GetStyleInfo (charAfterPos, 1);
        Assert (result.size () == 1);
        return result[0];
    }
    inline vector<StyledInfoSummaryRecord> StandardStyledTextImager::GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const
    {
        return (fStyleDatabase->GetStyleInfo (charAfterPos, nTCharsFollowing));
    }
    inline void StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalFontSpecification& styleInfo)
    {
        fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfo);
    }
    inline void StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<StyledInfoSummaryRecord>& styleInfos)
    {
        fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos);
    }
    inline void StandardStyledTextImager::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos,
                                                        const StyledInfoSummaryRecord* styleInfos)
    {
        fStyleDatabase->SetStyleInfo (charAfterPos, nTCharsFollowing, nStyleInfos, styleInfos);
    }
#endif

}
