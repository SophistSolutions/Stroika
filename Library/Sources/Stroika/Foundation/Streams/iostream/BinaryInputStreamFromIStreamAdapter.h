/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>

#include    "../../Configuration/Common.h"
#include    "../../Execution/CriticalSection.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../BinaryInputStream.h"
#include    "../Seekable.h"



/*
 * TODO:
 *          o   Consider mixing in Seekable here. Would be easy
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {

                /*
                 * @THREADSAFETY:   BinaryInputStreamFromIStreamAdapter is not necessarily thread safe. Its roughly as safe as the underlying istream implementation, except
                 *  that we call read, followed by gcount () - which could be a race.
                 */
                class   BinaryInputStreamFromIStreamAdapter : public BinaryInputStream {
                private:
                    class   IRep_;
                public:
                    BinaryInputStreamFromIStreamAdapter (istream& originalStream);
                };

            }
        }
    }
}

#endif  /*_Stroika_Foundation_Streams_iostream_BinaryInputStreamFromIStreamAdapter_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
