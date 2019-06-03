/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddressRange_h_
#define _Stroika_Foundation_IO_Network_InternetAddressRange_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Traversal/DiscreteRange.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 */

namespace Stroika::Foundation::IO::Network {

    using Characters::String;

    namespace Private_ {
        struct InternetAddressRangeTraits_ : Traversal::RangeTraits::ExplicitRangeTraitsWithoutMinMax<InternetAddress, Traversal::Openness::eClosed, Traversal::Openness::eClosed, int, unsigned int> {
            static InternetAddress GetNext (InternetAddress n)
            {
                return n == kUpperBound ? n : n.Offset (1);
            }
            using RangeTraitsType                        = InternetAddressRangeTraits_;
            static constexpr InternetAddress kLowerBound = InternetAddress::min ();
            static constexpr InternetAddress kUpperBound = InternetAddress::max ();
        };
    }

    /**
     */
    using InternetAddressRange = Traversal::DiscreteRange<InternetAddress, Private_::InternetAddressRangeTraits_>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetAddressRange.inl"

#endif /*_Stroika_Foundation_IO_Network_InternetAddressRange_h_*/
