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

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ******************************* BinaryInputOutputStream ************************
             ********************************************************************************
             */
            inline  BinaryInputOutputStream::BinaryInputOutputStream (const _SharedIRep& rep)
                : InputOutputStream<Byte> (rep)
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputOutputStream_inl_*/
