/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_
#define _Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_ 1

#include    "../../StroikaPreComp.h"

#include    <istream>

#include    "../../Configuration/Common.h"
#include    "../../Execution/CriticalSection.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "../TextInputStream.h"
#include    "../Seekable.h"



/*
 * TODO:
 *          o   Consider mixing in Seekable here. Would be easy
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {

                /**
                 *  @todo must redo this like BinaryInputStreamFromIStreamAdapter when I redo TextInputStream to act like
                 *  BinaryInputSteam - with smartpointer stuff
                 */
                class   TextInputStreamFromIStreamAdapter : public TextInputStream {
                public:
                    TextInputStreamFromIStreamAdapter (wistream& originalStream);

                protected:
                    // Pointer must refer to valid memory at least bufSize long, and cannot be nullptr. bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of characters read
                    // BLOCKING until data is available, but can return with fewer bytes than bufSize without prejudice about how much more is available.
                    virtual size_t  _Read (Character* intoStart, Character* intoEnd) override;

                private:
                    wistream&   fOriginalStream_;
                };

            }
        }
    }
}

#endif  /*_Stroika_Foundation_Streams_iostream_TextInputStreamFromIStreamAdapter_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
