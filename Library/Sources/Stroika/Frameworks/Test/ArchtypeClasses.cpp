/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Debugger.h"

#include "TestHarness.h"

#include "ArchtypeClasses.h"

using namespace Stroika::Frameworks::Test::ArchtypeClasses;

/*
 ********************************************************************************
 ******************** OnlyCopyableMoveableAndTotallyOrdered *********************
 ********************************************************************************
 */
namespace {
    constexpr int kKnownGoodBitPatternValue_ = 0x0BADBEEF;
}

OnlyCopyableMoveableAndTotallyOrdered::OnlyCopyableMoveableAndTotallyOrdered (size_t v)
    : fValue_{v}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
}

OnlyCopyableMoveableAndTotallyOrdered::OnlyCopyableMoveableAndTotallyOrdered (OnlyCopyableMoveableAndTotallyOrdered&& src) noexcept
    : fValue_{src.fValue_}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

OnlyCopyableMoveableAndTotallyOrdered::OnlyCopyableMoveableAndTotallyOrdered (const OnlyCopyableMoveableAndTotallyOrdered& src) noexcept
    : fValue_{src.fValue_}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

OnlyCopyableMoveableAndTotallyOrdered::~OnlyCopyableMoveableAndTotallyOrdered ()
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (sTotalLiveObjects_ != 0);
    --sTotalLiveObjects_;
    fConstructed_ = ~kKnownGoodBitPatternValue_;
    VerifyTestResult (fConstructed_ != kKnownGoodBitPatternValue_);
}

size_t OnlyCopyableMoveableAndTotallyOrdered::GetTotalLiveCount ()
{
    return sTotalLiveObjects_;
}

OnlyCopyableMoveableAndTotallyOrdered OnlyCopyableMoveableAndTotallyOrdered::operator+ (const OnlyCopyableMoveableAndTotallyOrdered& rhs) const
{
    return OnlyCopyableMoveableAndTotallyOrdered (fValue_ + rhs.fValue_);
}

OnlyCopyableMoveableAndTotallyOrdered::operator size_t () const
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    return fValue_;
}

bool OnlyCopyableMoveableAndTotallyOrdered::operator== (const OnlyCopyableMoveableAndTotallyOrdered& rhs) const
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (rhs.fConstructed_ == kKnownGoodBitPatternValue_);
    return fValue_ == rhs.fValue_;
}

auto OnlyCopyableMoveableAndTotallyOrdered::operator<=> (const OnlyCopyableMoveableAndTotallyOrdered& rhs) const -> strong_ordering
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (rhs.fConstructed_ == kKnownGoodBitPatternValue_);
    return fValue_ <=> rhs.fValue_;
}

/*
 ********************************************************************************
 ************************* OnlyCopyableMoveable *********************************
 ********************************************************************************
 */
OnlyCopyableMoveable::OnlyCopyableMoveable (size_t v)
    : fValue_{v}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
}

OnlyCopyableMoveable::OnlyCopyableMoveable (OnlyCopyableMoveable&& src) noexcept
    : fValue_{src.fValue_}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

OnlyCopyableMoveable::OnlyCopyableMoveable (const OnlyCopyableMoveable& src) noexcept
    : fValue_{src.fValue_}
    , fConstructed_{kKnownGoodBitPatternValue_}
{
    ++sTotalLiveObjects_;
    VerifyTestResult (src.fConstructed_ == kKnownGoodBitPatternValue_);
}

OnlyCopyableMoveable::~OnlyCopyableMoveable ()
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    VerifyTestResult (sTotalLiveObjects_ != 0);
    --sTotalLiveObjects_;
    fConstructed_ = ~kKnownGoodBitPatternValue_;
    VerifyTestResult (fConstructed_ != kKnownGoodBitPatternValue_);
}

OnlyCopyableMoveable::operator size_t () const
{
    VerifyTestResult (fConstructed_ == kKnownGoodBitPatternValue_);
    return fValue_;
}

size_t OnlyCopyableMoveable::GetTotalLiveCount ()
{
    return sTotalLiveObjects_;
}

OnlyCopyableMoveable OnlyCopyableMoveable::operator+ (const OnlyCopyableMoveable& rhs) const
{
    return OnlyCopyableMoveable{fValue_ + rhs.fValue_};
}
