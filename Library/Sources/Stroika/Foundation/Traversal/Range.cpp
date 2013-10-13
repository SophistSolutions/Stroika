/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Range.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Traversal;


#if     !qSupportTemplateParamterOfNumericLimitsMinMax
#if     !qCompilerAndStdLib_Supports_constexpr_StaticDataMember
template    <>
const int DefaultRangeTraits_Template_numericLimitsBWA<int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, INT_MIN, INT_MAX, int, unsigned int>::kMin   =   INT_MIN;
template    <>
const int DefaultRangeTraits_Template_numericLimitsBWA<int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, INT_MIN, INT_MAX, int, unsigned int>::kMax   =   INT_MAX;
template    <>
const int DefaultRangeTraits_Template_numericLimitsBWA<int, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, INT_MIN, INT_MAX, int, unsigned int>::kMin   =   INT_MIN;
template    <>
const int DefaultRangeTraits_Template_numericLimitsBWA<int, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, INT_MIN, INT_MAX, int, unsigned int>::kMax   =   INT_MAX;

template    <>
const unsigned int DefaultRangeTraits_Template_numericLimitsBWA<unsigned int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, 0, UINT_MAX, int, unsigned int>::kMin   =   0;
template    <>
const unsigned int DefaultRangeTraits_Template_numericLimitsBWA<unsigned int, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, 0, UINT_MAX, int, unsigned int>::kMax   =   UINT_MAX;
template    <>
const unsigned int DefaultRangeTraits_Template_numericLimitsBWA<unsigned int, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, 0, UINT_MAX, int, unsigned int>::kMin   =   0;
template    <>
const unsigned int DefaultRangeTraits_Template_numericLimitsBWA<unsigned int, RangeBase::Openness::eClosed, RangeBase::Openness::eClosed, 0, UINT_MAX, int, unsigned int>::kMax   =   UINT_MAX;
#endif
#endif
