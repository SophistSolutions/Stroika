/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextStream_inl_
#define _Stroika_Foundation_Streams_TextStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            //  class   TextStream::_IRep
            inline  TextStream::_IRep::_IRep ()
            {
            }
            inline  TextStream::_IRep::~_IRep ()
            {
            }


            //  class   TextStream
            inline  TextStream::TextStream (const _SharedIRep& rep)
                : Seekable (dynamic_cast<Seekable::_IRep*> (rep.get ()))
                , fRep_ (rep)
            {
            }
            inline  TextStream::_SharedIRep  TextStream::_GetRep () const
            {
                return fRep_;
            }
            inline  bool    TextStream::empty () const
            {
                return fRep_.get () == nullptr;
            }
            inline  void    TextStream::clear ()
            {
                fRep_.reset ();
                Seekable::_Clear ();
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextStream_inl_*/
