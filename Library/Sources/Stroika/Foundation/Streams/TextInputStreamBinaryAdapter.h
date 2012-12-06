/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "../Memory/BLOB.h"

#include    "BinaryInputStream.h"
#include    "Seekable.h"
#include    "TextInputStream.h"



/**
 *  \file
 *
 *      @todo   Must use either stdc++ multibyte code or Stroika CodePage code - currnetly a hack assuming input is ascii.
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            using   Characters::Character;

            /**
             *
             */
            class   TextInputStreamBinaryAdapter : public virtual TextInputStream, public virtual Seekable {
            public:
                TextInputStreamBinaryAdapter (BinaryInputStream src);

            protected:
                virtual size_t          _Read (Character* intoStart, Character* intoEnd) override;
                virtual SeekOffsetType  _GetOffset () const override;
                virtual SeekOffsetType  _Seek (Whence whence, SignedSeekOffsetType offset) override;

            private:
                BinaryInputStream   fSource_;
                String              fTmpHackTextRemaining_;
                size_t              fOffset_;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_*/
