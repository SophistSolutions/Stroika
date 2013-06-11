/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_stdwstring_h_
#define _Stroika_Foundation_Characters_String_stdwstring_h_    1

#include    "../../StroikaPreComp.h"

#include    "../String.h"


/**
 *  \file
 *
 *
 * TODO:
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


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
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "String_stdwstring.inl"

#endif  /*_Stroika_Foundation_Characters_String_stdwstring_h_*/
