/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryInputOutputStream_inl_
#define _Stroika_Foundation_Streams_BasicBinaryInputOutputStream_inl_  1


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
#if     !qCompilerAndStdLib_FailsStaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded
            template    <typename   T>
            T   BasicBinaryInputOutputStream::As () const
            {
                static_assert (false, "Only specifically specialized variants are supported");
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BasicBinaryInputOutputStream_inl_*/
