/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryStream_inl_
#define _Stroika_Foundation_Streams_BinaryStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            //  class   BinaryStream::_IRep
            inline  BinaryStream::_IRep::_IRep ()
            {
            }
            inline  BinaryStream::_IRep::~_IRep ()
            {
            }


            //  class   BinaryStream
            inline  BinaryStream::BinaryStream (const _SharedIRep& rep)
                : fRep_ (rep)
                , fSeekable_ (dynamic_cast<Seekable*> (rep.get ()))
            {
            }
            inline  BinaryStream::_SharedIRep  BinaryStream::_GetRep () const
            {
                return fRep_;
            }
            inline  bool    BinaryStream::empty () const
            {
                return fRep_.get () == nullptr;
            }
            inline  void    BinaryStream::clear ()
            {
                fRep_.reset ();
                fSeekable_ = nullptr;
            }
            inline  bool    BinaryStream::IsSeekable () const
            {
                return fSeekable_ != nullptr;
            }
            inline  SeekOffsetType  BinaryStream::GetOffset () const
            {
                Require (IsSeekable ());
                AssertNotNull (fSeekable_);
                return fSeekable_->GetOffset ();
            }
            inline  void        BinaryStream::Seek (SeekOffsetType offset)
            {
                Require (IsSeekable ());
                AssertNotNull (fSeekable_);
                fSeekable_->Seek (offset);
            }
            inline  void        BinaryStream::Seek (Whence whence, SeekOffsetType offset)
            {
                Require (IsSeekable ());
                AssertNotNull (fSeekable_);
                fSeekable_->Seek (whence, offset);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryStream_inl_*/
