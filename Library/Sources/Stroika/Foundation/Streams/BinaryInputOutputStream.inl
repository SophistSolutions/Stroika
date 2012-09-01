/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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


            //  class   BinaryInputOutputStream
            inline  BinaryInputOutputStream::_SharedInputIRep BinaryInputOutputStream::_GetInputRep () const
            {
                _SharedInputIRep   result  =   dynamic_pointer_cast<_SharedInputIRep::element_type> (_GetRep ());
                RequireNotNull (result.get ());
                return result;
            }
            inline  BinaryInputOutputStream::_SharedOutputIRep BinaryInputOutputStream::_GetOutputRep () const
            {
                _SharedOutputIRep   result  =   dynamic_pointer_cast<_SharedOutputIRep::element_type> (_GetRep ());
                RequireNotNull (result.get ());
                return result;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputOutputStream_inl_*/
