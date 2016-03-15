/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Backtrace_h_
#define _Stroika_Foundation_Debug_Backtrace_h_   1

#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Characters/String.h"



/**
 * TODO
 *
 *  \file
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-462 - do windows version (sb pretty easy)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /**
             *  Return a string/printable version of the current stack backtrace. This is handy in debugging.
             *
             *  This function will fail gracefully and return an empty string if needed.
             *
             *  There are cases where one would want to limit the number of 'stack frames' returned - since this is just used
             *  for debugging...
             *
             *  The frames are EOL (line) delimited.
             */
            Characters::String    BackTrace (unsigned int maxFrames = numeric_limits<unsigned int>::max ());

        }
    }
}
#endif  /*_Stroika_Foundation_Debug_Backtrace_h_*/
