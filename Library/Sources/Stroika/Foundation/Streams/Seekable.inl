/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Seekable_inl_
#define _Stroika_Foundation_Streams_Seekable_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            //  class   Seekable
            inline  Seekable::Seekable (_IRep* rep)
                : fSeekableRep_ (rep)
            {
            }
            inline  bool    Seekable::IsSeekable () const
            {
                return fSeekableRep_ != nullptr;
            }
            inline  void    Seekable::_Clear ()
            {
                fSeekableRep_ = nullptr;
            }
            inline  SeekOffsetType  Seekable::GetOffset () const
            {
                RequireNotNull (fSeekableRep_);
                return fSeekableRep_->_GetOffset ();
            }
            inline  SeekOffsetType    Seekable::Seek (SignedSeekOffsetType offset) const
            {
                RequireNotNull (fSeekableRep_);
                return fSeekableRep_->_Seek (Whence::eFromStart, offset);
            }
            inline  SeekOffsetType    Seekable::Seek (Whence whence, SignedSeekOffsetType offset) const
            {
                RequireNotNull (fSeekableRep_);
                return fSeekableRep_->_Seek (whence, offset);
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_Seekable_inl_*/
