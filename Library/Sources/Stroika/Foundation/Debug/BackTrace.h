/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Backtrace_h_
#define _Stroika_Foundation_Debug_Backtrace_h_ 1

#include "../StroikaPreComp.h"

#include <limits>

#include "../Characters/String.h"

/**
 *  \file
 *
 * TODO
 *      @todo   https://stroika.atlassian.net/browse/STK-462 - do windows version (sb pretty easy)
 *
 */

namespace Stroika::Foundation::Debug {

    namespace BackTrace {
        struct Options {
            [[maybe_unused]] unsigned int fSkipFrames = 0;

            /**
             * Max frames to return (doesn't count skipped frames)
             */
            unsigned int fMaxFrames = numeric_limits<unsigned int>::max ();
        };

        /**
         *  Return a string/printable version of the current stack backtrace (deepest part of the stack first). This is handy in debugging.
         *
         *  This function will fail gracefully and return an empty string if needed.
         *
         *  There are cases where one would want to limit the number of 'stack frames' returned - since this is just used
         *  for debugging...
         *
         *  The frames are EOL (line) delimited.
         *
         *  \note - if you've distributed a copy of the program without symbols, you can use gdb to read back symbol names with:
         *      > gdb-multiarch Output/arm-linux-gnueabi/Debug/BLKQCL-Controller
         *        info symbol 0x770368
         *        info symbol 0x1b974
         *      ... etc for each symbol returned in []
         *
         *  \note   to get symbols working on Linux (GNU linker), it may be necessary to link with -rdynamic
         *          This can be done with the Stroika configure flags:
         *              --extra-linker-args -rdynamic
         *          OR
         *              --apply-default-debug-flags
         *
         *  \note   BackTrace () Uses no Stroika classes internally (like String, SmallStackBuffer) etc, since
         *          doing so could create deadlocks in the likely use cases where one would want to call this, from
         *          a low level place where you might have locks.
         *
         *          This DOES - however - however, call STL routines and C-library routines, like string::CTOR {}
         *
         *  \note in C++20 we will transition to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0881r3.html
         *        but for now, typically delegate to https://www.boost.org/doc/libs/1_65_1/doc/html/stacktrace.html
         *
         *  \note The first few frames are internal to the implementation of BackTrace() so not interesting
         */
        wstring Capture (const Options& options = {});
    }

}
#endif /*_Stroika_Foundation_Debug_Backtrace_h_*/
