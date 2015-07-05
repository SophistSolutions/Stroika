/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStream_inl_
#define _Stroika_Foundation_Streams_TextInputStream_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ***************************** TextInputStream **********************************
             ********************************************************************************
             */
            inline  TextInputStream::TextInputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            inline  size_t  TextInputStream::Read (wchar_t* intoStart, wchar_t* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                static_assert (sizeof (wchar_t) == sizeof (Character), "This cast assumes the types are the same");
                return _GetRep ()->Read (nullptr, reinterpret_cast<Character*> (intoStart), reinterpret_cast<Character*> (intoEnd));
            }
            inline  size_t  TextInputStream::Read (Character* intoStart, Character* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->Read (nullptr, intoStart, intoEnd);
            }
            inline  Character   TextInputStream::Read () const
            {
                Character   c;
                if (Read (&c, &c + 1) == 1) {
                    return c;
                }
                return '\0';
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextInputStream_inl_*/
