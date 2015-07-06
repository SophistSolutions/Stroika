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
            template    <typename   BASESTREAM>
            inline  BinaryOutputStream<BASESTREAM>::BinaryOutputStream (const OutputStream<Byte>& from)
                : inherited (from._GetRep ())
            {
            }
            template    <typename   BASESTREAM>
            inline  BinaryOutputStream<BASESTREAM>::BinaryOutputStream (nullptr_t)
                : inherited (nullptr)
            {
            }
            template    <typename   BASESTREAM>
            inline  BinaryOutputStream<BASESTREAM>::BinaryOutputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            template    <typename   BASESTREAM>
            inline  void    BinaryOutputStream<BASESTREAM>::Write (const Memory::BLOB& blob) const
            {
                Write (blob.begin (), blob.end ());
            }
            template    <typename   BASESTREAM>
            inline  void    BinaryOutputStream<BASESTREAM>::Write (const Byte* start, const Byte* end) const
            {
                inherited::Write (start, end);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_inl_*/
