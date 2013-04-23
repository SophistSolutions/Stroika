/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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


            /*
             ********************************************************************************
             ************************** BinaryOutputStream::_IRep ***************************
             ********************************************************************************
             */
            inline  BinaryOutputStream::_IRep::_IRep ()
            {
            }


            /*
             ********************************************************************************
             ***************************** BinaryOutputStream *******************************
             ********************************************************************************
             */
            inline  BinaryOutputStream::BinaryOutputStream ()
                : BinaryStream ()
            {
            }
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
                Require (start <= end);
                Require (start != nullptr or start == end);
                Require (end != nullptr or start == end);
                if (start != end) {
                    _GetRep ()->Write (start, end);
                }
            }
            inline  void    BinaryOutputStream::Flush () const
            {
                _GetRep ()->Flush ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_inl_*/
