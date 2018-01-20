/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_inl_
#define _Stroika_Foundation_Streams_TextWriter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             *********************************** TextWriter::Ptr ****************************
             ********************************************************************************
             */
            inline TextWriter::Ptr::Ptr (const shared_ptr<OutputStream<Characters::Character>::_IRep>& from)
                : inherited (from)
            {
            }
            inline TextWriter::Ptr::Ptr (const OutputStream<Characters::Character>::Ptr& from)
                : inherited (from)
            {
            }

            /*
             ********************************************************************************
             *********************************** TextWriter *********************************
             ********************************************************************************
             */
            inline auto TextWriter::New (const OutputStream<Characters::Character>::Ptr& src) -> Ptr
            {
                return src;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_TextWriter_inl_*/
