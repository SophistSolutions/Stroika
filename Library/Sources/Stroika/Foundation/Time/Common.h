/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Common_h_
#define _Stroika_Foundation_Time_Common_h_ 1

#include "../StroikaPreComp.h"

#include <chrono>
#include <ctime>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md">Alpha-Late</a>

 */

namespace Stroika::Foundation::Time {

    /**
     *    \note   Design Note
     *            About time_t
     *
     *            This is generally a 64-bit (but sometimes, like for 32-bit AIX, its 32-bit) signed (but not guaranteed signed) quantity. It generally
     *            represents a number of seconds since midnight Jan 1, 1970 in UTC.
     *
     *            From http://en.cppreference.com/w/cpp/chrono/c/time_t
     *                Arithmetic type capable of representing times.
     *
     *            Although not defined, this is almost always an integral value holding the number of seconds 
     *            (not counting leap seconds) since 00:00, Jan 1 1970 UTC, corresponding to POSIX time.
     *
     *            Because of this, basically ALL the Stroika APIs (unless an obvious or stated exception) treat time_t as
     *            in UTC.
     *
     *            The Stroika APIs do NOT assume 64-bit, nor signedness, but when we require signedness, we use make_signed_t<time_t>
     */

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Time_Common_h_*/
