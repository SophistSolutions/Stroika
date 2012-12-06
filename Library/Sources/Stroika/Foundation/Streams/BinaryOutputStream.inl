/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_inl_
#define _Stroika_Foundation_Streams_BinaryOutputStream_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            //  class   BinaryOutputStream::_IRep
            inline  BinaryOutputStream::_IRep::_IRep ()
            {
            }



            //  class   BinaryOutputStream
            inline  BinaryOutputStream::BinaryOutputStream (const _SharedIRep& rep)
                : BinaryStream (rep)
            {
            }
            inline  BinaryOutputStream::_SharedIRep   BinaryOutputStream::_GetRep () const
            {
                return dynamic_pointer_cast<_IRep> (BinaryStream::_GetRep ());
            }
            inline  void    BinaryOutputStream::Write (const Byte* start, const Byte* end) const
            {
                RequireNotNull (start);
                Require ((end - start) >= 1);
                RequireNotNull (_GetRep ().get ());
                _GetRep ()->Write (start, end);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_inl_*/
