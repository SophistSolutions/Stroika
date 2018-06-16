/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddressRange_h_
#define _Stroika_Foundation_IO_Network_InternetAddressRange_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Traversal/DiscreteRange.h"

#include "InternetAddress.h"

/**
 *  \file
 *
 * TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                using Characters::String;

                namespace Private_ {
                    struct InternetAddressRangeTraits_ : Traversal::RangeTraits::ExplicitRangeTraitsWithoutMinMax<InternetAddress, Traversal::Openness::eClosed, Traversal::Openness::eClosed, int, unsigned int> {
                        static InternetAddress GetNext (InternetAddress n);
                        using RangeTraitsType = InternetAddressRangeTraits_;
                        static const InternetAddress kLowerBound;
                        static const InternetAddress kUpperBound;
                    };
                }

                /**
                 */
                using InternetAddressRange = Traversal::DiscreteRange<InternetAddress, Private_::InternetAddressRangeTraits_>;
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternetAddressRange.inl"

#endif /*_Stroika_Foundation_IO_Network_InternetAddressRange_h_*/
