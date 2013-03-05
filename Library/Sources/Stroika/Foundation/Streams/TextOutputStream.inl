/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextOutputStream_inl_
#define _Stroika_Foundation_Streams_TextOutputStream_inl_   1


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
             **************************** TextOutputStream::_IRep ***************************
             ********************************************************************************
             */
            inline  TextOutputStream::_IRep::_IRep ()
            {
            }


            /*
             ********************************************************************************
             *********************************** TextOutputStream ***************************
             ********************************************************************************
             */
            inline  TextOutputStream::TextOutputStream (const _SharedIRep& rep)
                : TextStream (rep)
            {
            }
            inline  TextOutputStream::_SharedIRep  TextOutputStream::_GetRep () const
            {
                return dynamic_pointer_cast<_IRep> (TextStream::_GetRep ());
            }
            inline  void    TextOutputStream::Write (const Character* start, const Character* end) const
            {
                Require (start <= end);
                Require (start != nullptr or start == end);
                Require (end != nullptr or start == end);
                if (start != end) {
                    _GetRep ()->Write (start, end);
                }
            }
            inline  void    TextOutputStream::Write (const wchar_t* start, const wchar_t* end) const
            {
                static_assert (sizeof (wchar_t) == sizeof (Character), "This cast assumes the types are the same");
                Write (reinterpret_cast<const Character*> (start), reinterpret_cast<const Character*> (end));
            }
            inline  void    TextOutputStream::Write (const wchar_t* cStr) const
            {
                return Write (cStr, cStr + ::wcslen (cStr));
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextOutputStream_inl_*/
