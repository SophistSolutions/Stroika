/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_stdwstring_h_
#define _Stroika_Foundation_Characters_String_stdwstring_h_ 1

#include "../../StroikaPreComp.h"

#include "../String.h"

/**
 *  \file
 *
 *
 * TODO:
 *      @todo   Do String_stdwstring() � as impl optimized to return std::wstring() a lot � saving that impl internally.
 *              Do make this efficient, must have pur virtual method of String:::Rep which fills in a wstring* arg
 *              (what about �into no-malloc semantics � I guess taken care of perhaps by this? Maybe not� THINKOUT �
 *              but pretty sure we want some sort of String_stdwstring().
 *
 *      @todo   Be sure can move-semantics into and out of String_stdwstring() - so can go back and forth between
 *              reps efficiently. This COULD use used to avoid any performance overhead with Stroika strings.
 *
 */

namespace Stroika::Foundation::Characters {

#if 0
    /**
     *  NOT YET IMPLEMETNED
     *
     *  String_stdwstring is completely compatible with any other String implementation, except that it represents things
     *  internally using the stdC++ wstring class. The principle advantage of this is that converting TO wstrings
     *  is much more efficient.
     *
     *      (AS OF YET UNCLEAR IF/HOW WE CAN SUPPORT MANIPULATIONS OF A wstring* or wstring& alias to the String rep's owned copy.
     *      probably won't be allowed, but it would be helpful to some applicaitons if we could)
     */
    class   String_stdwstring : public String {
    public:
        explicit String_stdwstring (const String& from);
    };
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String_stdwstring.inl"

#endif /*_Stroika_Foundation_Characters_String_stdwstring_h_*/
