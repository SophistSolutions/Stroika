/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Debugger.h"

#include "SimpleClass.h"
#include "TestHarness.h"

using namespace Stroika;

/*
 ********************************************************************************
 ********************************** SimpleClass *********************************
 ********************************************************************************
 */
namespace {
    constexpr int kKnownGoodBitPatternValue_ = 0x0BADBEEF;
}

size_t SimpleClass::sTotalLiveObjects_ = 0;

SimpleClass::SimpleClass (size_t v)
    : fValue_ (v)
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
}

SimpleClass::SimpleClass (SimpleClass&& src) noexcept
    : fValue_ (src.fValue_)
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

SimpleClass::SimpleClass (const SimpleClass& src) noexcept
    : fValue_ (src.fValue_)
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

SimpleClass::~SimpleClass ()
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (sTotalLiveObjects_ != 0);
    --sTotalLiveObjects_;
    fConstructed_ = ~kKnownGoodBitPatternValue_;
    VerifyTestResult (fConstructed_ != kKnownGoodBitPatternValue_);
}

size_t SimpleClass::GetValue () const
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    return (fValue_);
}

size_t SimpleClass::GetTotalLiveCount ()
{
    return sTotalLiveObjects_;
}

SimpleClass SimpleClass::operator+ (const SimpleClass& rhs) const
{
    return SimpleClass (fValue_ + rhs.fValue_);
}

SimpleClass::operator size_t () const
{
    return fValue_;
}

bool SimpleClass::operator== (const SimpleClass& rhs) const
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (rhs.fConstructed_ == kKnownGoodBitPatternValue_);
    return fValue_ == rhs.fValue_;
}

bool SimpleClass::operator< (const SimpleClass& rhs) const
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (rhs.fConstructed_ == kKnownGoodBitPatternValue_);
    return fValue_ < rhs.fValue_;
}

/*
 ********************************************************************************
 ******************* SimpleClassWithoutComparisonOperators **********************
 ********************************************************************************
 */
size_t SimpleClassWithoutComparisonOperators::sTotalLiveObjects_ = 0;

SimpleClassWithoutComparisonOperators::SimpleClassWithoutComparisonOperators (size_t v)
    : fValue_{v}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
}

SimpleClassWithoutComparisonOperators::SimpleClassWithoutComparisonOperators (SimpleClassWithoutComparisonOperators&& src) noexcept
    : fValue_{src.fValue_}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

SimpleClassWithoutComparisonOperators::SimpleClassWithoutComparisonOperators (const SimpleClassWithoutComparisonOperators& src) noexcept
    : fValue_{src.fValue_}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

SimpleClassWithoutComparisonOperators::~SimpleClassWithoutComparisonOperators ()
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (sTotalLiveObjects_ != 0);
    --sTotalLiveObjects_;
    fConstructed_ = ~kKnownGoodBitPatternValue_;
    VerifyTestResult (fConstructed_ != kKnownGoodBitPatternValue_);
}

size_t SimpleClassWithoutComparisonOperators::GetValue () const
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    return fValue_;
}

size_t SimpleClassWithoutComparisonOperators::GetTotalLiveCount ()
{
    return sTotalLiveObjects_;
}

SimpleClassWithoutComparisonOperators SimpleClassWithoutComparisonOperators::operator+ (const SimpleClassWithoutComparisonOperators& rhs) const
{
    return SimpleClassWithoutComparisonOperators{fValue_ + rhs.fValue_};
}
