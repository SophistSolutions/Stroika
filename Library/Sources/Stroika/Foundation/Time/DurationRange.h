/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DurationRange_h_
#define _Stroika_Foundation_Time_DurationRange_h_ 1

#include "../StroikaPreComp.h"

#include "../Traversal/Range.h"

#include "Duration.h"

/**
 *  \file
 *
 *  *** FILE DEPRECATED ***
 */

namespace Stroika::Foundation::Time {

    using DurationRange [[deprecated ("Since Stroika v2.1b8, just use Range<Duration>")]] = Traversal::Range<Duration>;

}

#endif /*_Stroika_Foundation_Time_DurationRange_h_*/
