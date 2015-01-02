/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicTextOutputStream_inl_
#define _Stroika_Foundation_Streams_BasicTextOutputStream_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


#if 0


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
                Write (cStr, cStr + ::wcslen (cStr));
            }
            inline  void    TextOutputStream::Write (const String& s) const
            {
                Write (s.c_str ());
            }
            template    <>
            inline  const TextOutputStream& TextOutputStream::operator<< (const String& write2TextStream) const
            {
                Write (write2TextStream);
                return *this;
            }
            template    <>
            inline  const TextOutputStream& TextOutputStream::operator<< (const wchar_t* write2TextStream) const
            {
                Write (write2TextStream);
                return *this;
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BasicTextOutputStream_inl_*/
