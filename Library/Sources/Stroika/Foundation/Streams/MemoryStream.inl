/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryStream_inl_
#define _Stroika_Foundation_Streams_MemoryStream_inl_  1


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
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            template    <typename   T>
            T   MemoryStream::As () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_MemoryStream_inl_*/
