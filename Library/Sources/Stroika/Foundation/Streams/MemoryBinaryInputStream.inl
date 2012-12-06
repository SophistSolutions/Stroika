/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryBinaryInputStream_inl_
#define _Stroika_Foundation_Streams_MemoryBinaryInputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

#if     qCompilerAndStdLib_Supports_ConstructorDelegation
            template    <typename IteratorT>
            inline  Stroika::Foundation::Streams::MemoryBinaryInputStream::MemoryBinaryInputStream (IteratorT start, IteratorT end)
                : MemoryBinaryInputStream (&*start, &*end)
            {
            }
            template    <typename ContainerOfT>
            inline  Stroika::Foundation::Streams::MemoryBinaryInputStream::MemoryBinaryInputStream (const ContainerOfT& c)
                : MemoryBinaryInputStream (c.begin (), c.end ())
            {
            }
#endif

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_MemoryBinaryInputStream_inl_*/
