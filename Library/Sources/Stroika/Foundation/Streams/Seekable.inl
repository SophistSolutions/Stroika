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
namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            //  class   Seekable
            inline  Seekable::~Seekable ()
            {
            }
            inline  SeekOffsetType  Seekable::GetOffset () const
            {
                return _GetOffset ();
            }
            inline  void    Seekable::Seek (SeekOffsetType offset)
            {
                _Seek (FromStart_W, offset);
            }
            inline  void    Seekable::Seek (Whence whence, SeekOffsetType offset)
            {
                _Seek (whence, offset);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_Seekable_inl_*/
