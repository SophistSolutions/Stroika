/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputStream_inl_
#define _Stroika_Foundation_Streams_BinaryInputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Memory/BLOB.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

#if 0

            /*
             ********************************************************************************
             ******************************* BinaryInputStream ******************************
             ********************************************************************************
             */
            template    <typename   BASESTREAM>
            inline  BinaryInputStream<BASESTREAM>::BinaryInputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            template    <typename   BASESTREAM>
            inline  BinaryInputStream<BASESTREAM>::BinaryInputStream (const InputStream<Byte>& from)
                : inherited (from._GetRep ())
            {
            }
            template    <typename   BASESTREAM>
            inline   BinaryInputStream<BASESTREAM>::BinaryInputStream (nullptr_t)
                : inherited (nullptr)
            {
            }
#endif
#if 0
            template    <typename   BASESTREAM>
            Memory::BLOB BinaryInputStream<BASESTREAM>::ReadAll () const
            {
                if (this->IsSeekable ()) {
                    SeekOffsetType  size = this->GetOffsetToEndOfStream ();
                    if (size >= numeric_limits<size_t>::max ()) {
                        Execution::DoThrow<bad_alloc> (bad_alloc ());
                    }
                    size_t sb = static_cast<size_t> (size);
                    if (sb == 0) {
                        return Memory::BLOB ();
                    }
                    Byte* b = new Byte[sb];   // if this fails, we had no way to create the BLOB
                    size_t n = this->Read (b, b + sb);
                    Assert (n <= sb);
                    return Memory::BLOB::Attach (b, b + n);
                }
                else {
                    // Less efficient implementation
                    vector<Byte>    r;
                    size_t          n;
                    Byte            buf[32 * 1024];
                    while ( (n = this->Read (std::begin (buf), std::end (buf))) != 0) {
                        r.insert (r.end (), std::begin (buf), std::begin (buf) + n);
                    }
                    return Memory::BLOB (r);
                }
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_inl_*/
