/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

#if qStroika_Frameworks_Led_SupportGDI

    /*
     ********************************************************************************
     ********************* SyntaxColoringMarkerOwner::ColoredStyleMarker ************
     ********************************************************************************
     */
    inline SyntaxColoringMarkerOwner::ColoredStyleMarker::ColoredStyleMarker (const Color& color)
        : fColor{color}
    {
    }

    /*
     ********************************************************************************
     ******************** TableDrivenKeywordSyntaxAnalyzer::KeywordTable ************
     ********************************************************************************
     */
    inline TableDrivenKeywordSyntaxAnalyzer::KeywordTable::KeywordTable (const Led_tChar* keyWords[], size_t nKeywords,
                                                                         int (*cmpFunction) (const Led_tChar*, const Led_tChar*, size_t))
        : fKeywords (keyWords)
        , fNKeywords (nKeywords)
        , fMaxKeywordLength ()
        , fCmpFunction (cmpFunction)
    {
        RequireNotNull (cmpFunction);
        unsigned u = 0;
        for (size_t i = 0; i < nKeywords; ++i) {
            u = max (u, unsigned (Led_tStrlen (keyWords[i])));
        }
        fMaxKeywordLength = u;
    }
    inline size_t TableDrivenKeywordSyntaxAnalyzer::KeywordTable::MaxKeywordLength () const
    {
        return fMaxKeywordLength;
    }
    inline size_t TableDrivenKeywordSyntaxAnalyzer::KeywordTable::KeywordLength (const Led_tChar* t, size_t nTChars) const
    {
        for (size_t i = 0; i < fNKeywords; ++i) {
            const size_t kKeywordLen = Led_tStrlen (fKeywords[i]);
            if (kKeywordLen <= nTChars and fCmpFunction (fKeywords[i], t, kKeywordLen) == 0) {
                return kKeywordLen;
            }
        }
        return 0;
    }
#endif

}
