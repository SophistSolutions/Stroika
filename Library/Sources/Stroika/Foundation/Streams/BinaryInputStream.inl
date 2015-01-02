/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputStream_inl_
#define _Stroika_Foundation_Streams_BinaryInputStream_inl_  1


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
             *************************** BinaryInputStream::_IRep ***************************
             ********************************************************************************
             */
            inline  BinaryInputStream::_IRep::_IRep ()
            {
            }



            /*
             ********************************************************************************
             ******************************* BinaryInputStream ******************************
             ********************************************************************************
             */
            inline  BinaryInputStream::BinaryInputStream ()
                : BinaryStream ()
            {
            }
            inline  BinaryInputStream::BinaryInputStream (nullptr_t)
                : BinaryStream ()
            {
            }
            inline  BinaryInputStream::BinaryInputStream (const _SharedIRep& rep)
                : BinaryStream (rep)
            {
            }
            inline  BinaryInputStream::BinaryInputStream (const _SharedIRep& rep, Seekable::_IRep* seekable)
                : BinaryStream (rep, seekable)
            {
            }
            inline  BinaryInputStream::_SharedIRep  BinaryInputStream::_GetRep () const
            {
                return dynamic_pointer_cast<_IRep> (BinaryStream::_GetRep ());
            }
            inline  Memory::Optional<Byte>  BinaryInputStream::Read () const
            {
                Byte b = 0;
                RequireNotNull (_GetRep ().get ());
                if (_GetRep ()->Read (&b, &b + 1) == 0) {
                    return Memory::Optional<Byte> ();
                }
                else {
                    return b;
                }
            }
            inline  size_t  BinaryInputStream::Read (Byte* intoStart, Byte* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->Read (intoStart, intoEnd);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_inl_*/
