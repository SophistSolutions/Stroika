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

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {



            /*
             ********************************************************************************
             ******************************* BinaryInputStream ******************************
             ********************************************************************************
             */
            inline  BinaryInputStream::BinaryInputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            inline  BinaryInputStream::BinaryInputStream (const InputStream<Byte>& from)
                : inherited (from._GetRep ())
            {
            }
            inline   BinaryInputStream::BinaryInputStream (nullptr_t)
                : inherited (nullptr)
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_inl_*/
