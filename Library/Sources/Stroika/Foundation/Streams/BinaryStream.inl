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
                , fIsSeekable_ (dynamic_cast<const Seekable*> (rep.get ()) != nullptr)
            {
            }
            inline  BinaryStream::_SharedIRep  BinaryStream::GetRep () const
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
                fIsSeekable_ = false;
            }
            inline  bool    BinaryStream::IsSeekable () const
            {
                return fIsSeekable_;
            }
            inline  SeekOffsetType  BinaryStream::GetOffset () const
            {
                Require (IsSeekable ());
                return dynamic_cast<const Seekable*> (fRep_.get ())->GetOffset ();
            }
            inline  void        BinaryStream::Seek (SeekOffsetType offset)
            {
                Require (IsSeekable ());
                dynamic_cast<Seekable*> (fRep_.get ())->Seek (offset);
            }
            inline  void        BinaryStream::Seek (Whence whence, SeekOffsetType offset)
            {
                Require (IsSeekable ());
                dynamic_cast<Seekable*> (fRep_.get ())->Seek (whence, offset);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryStream_inl_*/
