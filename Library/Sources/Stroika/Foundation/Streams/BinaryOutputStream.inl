/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_inl_
#define _Stroika_Foundation_Streams_BinaryOutputStream_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Memory/BLOB.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ***************************** BinaryOutputStream *******************************
             ********************************************************************************
             */
            inline  BinaryOutputStream::BinaryOutputStream (const OutputStream<Byte>& from)
                : inherited (from._GetRep ())
            {
            }
            inline  BinaryOutputStream::BinaryOutputStream (nullptr_t)
                : inherited (nullptr)
            {
            }
            inline  BinaryOutputStream::BinaryOutputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            inline  void    BinaryOutputStream::Write (const Memory::BLOB& blob) const
            {
                Write (blob.begin (), blob.end ());
            }
            inline  void    BinaryOutputStream::Write (const Byte* start, const Byte* end) const
            {
                inherited::Write (start, end);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_inl_*/
