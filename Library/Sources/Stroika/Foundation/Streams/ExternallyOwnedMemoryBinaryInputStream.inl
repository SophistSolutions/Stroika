/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_inl_
#define _Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_inl_ 1


#include    "../Traversal/Iterator.h"


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
             ****************** ExternallyOwnedMemoryBinaryInputStream **********************
             ********************************************************************************
             */
            template    <typename   BYTE_RANDOM_ACCESS_ITERATOR>
            inline  ExternallyOwnedMemoryBinaryInputStream::ExternallyOwnedMemoryBinaryInputStream (BYTE_RANDOM_ACCESS_ITERATOR start, BYTE_RANDOM_ACCESS_ITERATOR end)
                : ExternallyOwnedMemoryBinaryInputStream (Traversal::Iterator2Address (start), Traversal::Iterator2Address (start) + (end - start))
            {
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_ExternallyOwnedMemoryBinaryInputStream_inl_*/
