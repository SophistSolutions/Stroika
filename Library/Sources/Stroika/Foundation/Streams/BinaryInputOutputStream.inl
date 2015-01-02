/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputOutputStream_inl_
#define _Stroika_Foundation_Streams_BinaryInputOutputStream_inl_  1


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
             ************************ BinaryInputOutputStream::_IRep ************************
             ********************************************************************************
             */
            inline  BinaryInputOutputStream::_IRep::_IRep ()
            {
            }


            /*
             ********************************************************************************
             ******************************* BinaryInputOutputStream ************************
             ********************************************************************************
             */
            inline  BinaryInputOutputStream::_SharedIRep BinaryInputOutputStream::_GetRep () const
            {
                _SharedIRep   result  =   dynamic_pointer_cast<_SharedIRep::element_type> (BinaryStream::_GetRep ());
                RequireNotNull (result.get ());
                return result;
            }
            inline  size_t  BinaryInputOutputStream::Read (Byte* intoStart, Byte* intoEnd)
            {
                return _GetRep ()->Read (intoStart, intoEnd);
            }
            inline  void    BinaryInputOutputStream::Write (const Byte* start, const Byte* end)
            {
                _GetRep ()->Write (start, end);
            }
            inline void    BinaryInputOutputStream::Flush ()
            {
                _GetRep ()->Flush ();
            }
            inline SeekOffsetType      BinaryInputOutputStream::ReadGetOffset () const
            {
                return _GetRep ()->ReadGetOffset ();
            }
            inline   SeekOffsetType      BinaryInputOutputStream::ReadSeek (Whence whence, SignedSeekOffsetType offset)
            {
                return _GetRep ()->ReadSeek (whence, offset);
            }
            inline   SeekOffsetType      BinaryInputOutputStream::WriteGetOffset () const
            {
                return _GetRep ()->WriteGetOffset ();
            }
            inline   SeekOffsetType      BinaryInputOutputStream::WriteSeek (Whence whence, SignedSeekOffsetType offset)
            {
                return _GetRep ()->WriteSeek (whence, offset);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputOutputStream_inl_*/
