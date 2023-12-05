/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Visualizations_h_
#define _Stroika_Foundation_Debug_Visualizations_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

/**
 *  \file
 *
 *  \note - This Header file must be included in at least one module your application links with to force/pull in the visualization code
 *          into the debugger.
 * 
 *  The purpose of the module is to provide single function entrypoints callable by the Microsoft debugger (visual studio and visual studio code on windows)
 *  to evaluate / example objects (see .natvis file).
 * 
 *  A mechanism akin to this might be used going forward for gdb/lldb?
 */

namespace Stroika::Foundation::Debug::Visualizations {

    namespace Private_ {
        extern bool gMagicSoIncluded;

#if qDebug
        /**
         *  Magic to force the linker to include the visualizations obj file in executables.
         */
        const inline bool gMag2 = gMagicSoIncluded;
#endif
    }


    template<typename REP_CHAR>
    struct StringRep /* : StringRepHelperAllFitInSize_::Rep<char> */ {
        virtual ~StringRep (){};
        span<const REP_CHAR> fData;
    };


    void ForceInclude ();

}
#endif /*_Stroika_Foundation_Debug_Visualizations_h_*/
