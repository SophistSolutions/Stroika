/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "InputStream.h"
#include    "BufferedInputStream.h"




#pragma message ("Warning: BufferedBinaryInputStream FILE DEPRECATED - use BufferedInputStream<Memory::Byte>")



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using BufferedBinaryInputStream = BufferedInputStream<Memory::Byte>;

#if 0
            /**
             *  @brief  BufferedBinaryInputStream is a BinaryInputStream which provides buffered access.
             *          This is useful if calls to the underling stream source can be expensive. This class
             *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
             */
            class   BufferedBinaryInputStream : public InputStream<Memory::Byte> {
            private:
                class   Rep_;
            public:
                BufferedBinaryInputStream (const InputStream<Memory::Byte>& realIn);
            };
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "BufferedBinaryInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_*/
