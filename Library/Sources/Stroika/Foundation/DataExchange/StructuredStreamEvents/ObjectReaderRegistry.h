/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_    1

#include    "../../StroikaPreComp.h"


#include    "ObjectReader.h"



/**
 *
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


                using   Characters::String;
                using   Containers::Mapping;
                using   Containers::Sequence;
                using   Containers::Set;
                using   Memory::Byte;
#pragma message ("Warning: THIS FILE IS DEPRECATED - use ObjectReader.h - since v2.0a189")

                //tmphack while tarnsitioning
                struct   ObjectReaderRegistry : ObjectReader::Registry {

                    using Context = ObjectReader::Context;
                    using IElementConsumer = ObjectReader::IElementConsumer;
                    using IConsumerDelegateToContext = ObjectReader::IConsumerDelegateToContext;
                    using StructFieldInfo = ObjectReader::StructFieldInfo;
                    using ReadDownToReader = ObjectReader::ReadDownToReader;
                    using ReaderFromVoidStarFactory = ObjectReader::ReaderFromVoidStarFactory;
                    template    <typename   T>
                    using   MixinReader = ObjectReader::MixinReader<T>;
                    template    <typename   T>
                    using   RangeReader = ObjectReader::RangeReader<T>;
                    template    <typename   T>
                    using   ReaderFromTStarFactory = ObjectReader::ReaderFromTStarFactory<T>;

                    template    <typename   T, typename TRAITS = ObjectReader::RepeatedElementReader_DefaultTraits<T>>
                    using   RepeatedElementReader = ObjectReader::RepeatedElementReader<T, TRAITS>;

                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "ObjectReaderRegistry.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_*/
