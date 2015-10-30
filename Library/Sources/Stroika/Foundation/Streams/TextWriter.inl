/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_inl_
#define _Stroika_Foundation_Streams_TextWriter_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Characters/Format.h"
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             *********************************** TextWriter *********************************
             ********************************************************************************
             */
            inline    TextWriter::TextWriter (const OutputStream<Characters::Character>& src)
                : inherited (src)
            {
            }
            inline  void    TextWriter::PrintF (const wchar_t* format, ...)
            {
                va_list     argsList;
                va_start (argsList, format);
                Write (Characters::FormatV (format, argsList));
                va_end (argsList);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextWriter_inl_*/
