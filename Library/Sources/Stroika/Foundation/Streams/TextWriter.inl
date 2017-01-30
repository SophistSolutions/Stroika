/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_inl_
#define _Stroika_Foundation_Streams_TextWriter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             *********************************** TextWriter *********************************
             ********************************************************************************
             */
            inline TextWriter::TextWriter (const OutputStream<Characters::Character>& src)
                : inherited (src)
            {
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_TextWriter_inl_*/
