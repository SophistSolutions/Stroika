/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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
                : fRep_ (rep)
                , fSeekable_ (dynamic_cast<Seekable*> (rep.get ()))
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
                fSeekable_ = nullptr;
            }
            inline  bool    TextStream::IsSeekable () const
            {
                return fSeekable_ != nullptr;
            }
            inline  SeekOffsetType  TextStream::GetOffset () const
            {
                Require (IsSeekable ());
                AssertNotNull (fSeekable_);
                return fSeekable_->GetOffset ();
            }
            inline  SeekOffsetType        TextStream::Seek (SignedSeekOffsetType offset) const
            {
                Require (IsSeekable ());
                AssertNotNull (fSeekable_);
                return fSeekable_->Seek (offset);
            }
            inline  SeekOffsetType        TextStream::Seek (Whence whence, SignedSeekOffsetType offset) const
            {
                Require (IsSeekable ());
                AssertNotNull (fSeekable_);
                return fSeekable_->Seek (whence, offset);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextStream_inl_*/
