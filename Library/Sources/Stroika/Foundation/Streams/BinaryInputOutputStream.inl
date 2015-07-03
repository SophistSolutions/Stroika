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


#if 1
            /*
             ********************************************************************************
             ******************************* BinaryInputOutputStream ************************
             ********************************************************************************
             */
            inline  BinaryInputOutputStream::BinaryInputOutputStream (const _SharedIRep& rep)
                : BinaryInputStream (rep)
                , BinaryOutputStream (rep)
            {
                //Require (dynamic_cast<const Seekable*>(rep.get ()) == nullptr);
            }
            inline  BinaryInputOutputStream::_SharedIRep BinaryInputOutputStream::_GetRep () const
            {
                // NB: could have used base BinaryOutputStream - either way same - @todo add assert
                _SharedIRep   result  =   dynamic_pointer_cast<_SharedIRep::element_type> (BinaryInputStream::_GetRep ());
                RequireNotNull (result.get ());
                return result;
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputOutputStream_inl_*/
