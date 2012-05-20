/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_
#define _Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_ 1

#include    "../StroikaPreComp.h"

#include    <string>

#include    "BinaryInputStream.h"
#include    "Seekable.h"
#include    "TextInputStream.h"



/*
 * TODO:
 *          o   Must use either stdc++ multibyte code or Stroika CodePage code - currnetly a hack assuming input is ascii.
 *
 *          o   CONSIDER REDOING USE shared_ptr<> - to make lifetime stuff workout better...
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            using   Characters::Character;

            /*
             */
            class   TextInputStreamBinaryAdapter : public virtual TextInputStream, public virtual Seekable {
            public:
                TextInputStreamBinaryAdapter (BinaryInputStream& src);

            protected:
                virtual size_t          _Read (Character* intoStart, Character* intoEnd) override;
                virtual SeekOffsetType  _GetOffset () const override;
                virtual void            _Seek (Whence whence, SeekOffsetType offset) override;

            private:
                BinaryInputStream&  fSource_;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_TextInputStreamBinaryAdapter_h_*/
