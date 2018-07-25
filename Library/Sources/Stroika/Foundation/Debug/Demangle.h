/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Demangle_h_
#define _Stroika_Foundation_Debug_Demangle_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

namespace Stroika::Foundation::Debug {

    /**
     *  Attempt to take a C++ symbol name which is mangled (maybe not) and return its demangled form.
     *  This attempts to handle already mangled names, and may do nothing depending on availability of underlying
     *  symbol or other information.
     */
    Characters::String Demangle (const Characters::String& originalName);
}
#endif /*_Stroika_Foundation_Debug_Demangle_h_*/
