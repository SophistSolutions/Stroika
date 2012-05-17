/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedPtr.h"
#include    "BinaryInputStream.h"




/*
 *  TODO:
 *
 *      o Not actually implemented. Must think out if this should mixin Seekable or not. I THINK it must.
 *          I THINK if one SEEKS this, but the underlying class doesn't supprot seeking, we MAY need to either BUFFER MORE, or throw not supported.
 *
 *      o   Examine other choices in stream class. Decide between using the Memory::SharedPtr<> pattern, or just using references (and count on coding styles
 *          to manage lifetime issues).
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             *
             */
            class   BufferedBinaryInputStream : public BinaryInputStream {
            public:
                BufferedBinaryInputStream (const Memory::SharedPtr<BinaryInputStream>& realIn);

            protected:
                virtual size_t          _Read (Byte* intoStart, Byte* intoEnd) override;

            private:
                Memory::SharedPtr<BinaryInputStream>    fRealIn_;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BufferedBinaryInputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BufferedBinaryInputStream.inl"
