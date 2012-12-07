/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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

            //  class   TextOutputStream::_IRep
            inline  TextOutputStream::_IRep::_IRep ()
            {
            }


            //  class   TextOutputStream
            inline  TextOutputStream::TextOutputStream (const _SharedIRep& rep)
                : TextStream (rep)
            {
            }
            inline  TextOutputStream::_SharedIRep  TextOutputStream::_GetRep () const
            {
                return dynamic_pointer_cast<_IRep> (TextStream::_GetRep ());
            }
            void    TextOutputStream::Write (const Character* start, const Character* end) const
            {
                RequireNotNull (start);
                Require (end - start >= 0);
                _GetRep ()->Write (start, end);
            }
            void    TextOutputStream::Write (const wchar_t* start, const wchar_t* end) const
            {
                RequireNotNull (start);
                Require (end - start >= 0);
                static_assert (sizeof (wchar_t) == sizeof (Character), "This cast assumes the types are the same");
                _GetRep ()->Write (reinterpret_cast<const Character*> (start), reinterpret_cast<const Character*> (end));
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextOutputStream_inl_*/
