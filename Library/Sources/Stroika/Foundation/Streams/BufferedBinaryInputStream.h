/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "BinaryInputStream.h"




/*
 *  \file
 *
 *  TODO:
 *
 *      @todo   Not actually implemented. Must think out if this should mixin Seekable or not. I THINK it must.
 *              I THINK if one SEEKS this, but the underlying class doesn't supprot seeking, we MAY need to either BUFFER MORE, or throw not supported.
 *              BufferedBinaryInputStream::IRep_ IMPLEMENTATION NEEDED
 *
 *      @todo   Examine other choices in stream class. Decide between using the shared_ptr<> pattern, or just using references (and count on coding styles
 *              to manage lifetime issues).
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *
             */
            class   BufferedBinaryInputStream : public BinaryInputStream {
            private:
                class   IRep_;
            public:
                BufferedBinaryInputStream (const BinaryInputStream::_SharedIRep& realIn);
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
