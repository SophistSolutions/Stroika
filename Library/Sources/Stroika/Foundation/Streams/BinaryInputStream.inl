/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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


            //  class   BinaryInputStream
            inline  BinaryInputStream::BinaryInputStream (const _SharedIRep& rep)
                : BinaryStream (rep)
            {
            }
            inline  BinaryInputStream::_SharedIRep  BinaryInputStream::GetRep () const
            {
                return dynamic_pointer_cast<_IRep> (BinaryStream::GetRep ());
            }
            inline  size_t  BinaryInputStream::Read (Byte* intoStart, Byte* intoEnd)
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (GetRep ().get ());
                return GetRep ()->Read (intoStart, intoEnd);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_inl_*/
