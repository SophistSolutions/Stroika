/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Stream_inl_
#define _Stroika_Foundation_Streams_Stream_inl_  1


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
             **************************** Stream<ELEMENT_TYPE> ******************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            inline  Stream<ELEMENT_TYPE>::Stream (const _SharedIRep& rep)
                : fRep_ (rep)
                , fSeekable_ (rep == nullptr ? false : rep->IsSeekable ())
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  Stream<ELEMENT_TYPE>::Stream (nullptr_t)
                : fRep_ ()
                , fSeekable_ (false)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  auto    Stream<ELEMENT_TYPE>::_GetRep () const -> _SharedIRep
            {
                return fRep_;
            }
            template    <typename ELEMENT_TYPE>
            inline  bool    Stream<ELEMENT_TYPE>::IsSeekable () const
            {
                return fSeekable_;
            }
            template    <typename ELEMENT_TYPE>
            inline  bool    Stream<ELEMENT_TYPE>::empty () const
            {
                return fRep_.get () == nullptr;
            }
            template    <typename ELEMENT_TYPE>
            inline  void    Stream<ELEMENT_TYPE>::clear ()
            {
                fRep_.reset ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_Stream_inl_*/
