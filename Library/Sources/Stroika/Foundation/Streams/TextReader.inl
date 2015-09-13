/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextReader_inl_
#define _Stroika_Foundation_Streams_TextReader_inl_   1


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
             *********************************** TextReader *********************************
             ********************************************************************************
             */
            inline  TextReader::TextReader (const InputStream<Character>& src)
                : inherited (src)
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextReader_inl_*/
