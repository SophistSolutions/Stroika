/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BasicBinaryInputStream_inl_
#define _Stroika_Foundation_Streams_BasicBinaryInputStream_inl_  1


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
             ***************************** BasicBinaryInputStream ***************************
             ********************************************************************************
             */
#if 0
            template    <typename ITERATOR_TYPE>
            BasicBinaryInputStream::BasicBinaryInputStream (const ITERATOR_TYPE& start, const ITERATOR_TYPE& end)
                : BasicBinaryInputStream (
                    [start, end] ()
            {
                size_t n = 0;
                for (auto i = start; i != end; ++i) {
                    n++;
                }
                Memory::SmallStackBuffer<Byte>    bytesArray (n);
                for (auto i = start; i != end; ++i) {
                    bytesArray.push_back (*i);
                }
                return bytesArray;
            } ()
            )
            {
            }
#endif
#if     0
            template    <typename ContainerOfT>
            inline  Stroika::Foundation::Streams::BasicBinaryInputStream::BasicBinaryInputStream (const ContainerOfT& c)
                : BasicBinaryInputStream (c.begin (), c.end ())
            {
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BasicBinaryInputStream_inl_*/
