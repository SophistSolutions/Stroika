/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_inl_
#define _Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Characters/String_Constant.h"
#include    "../../Debug/AssertExternallySynchronizedLock.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/StringException.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                /*
                 ********************************************************************************
                 **************** InputStreamFromStdIStream<ELEMENT_TYPE>::Rep_ *****************
                 ********************************************************************************
                 */
                template    <typename   ELEMENT_TYPE, typename TRAITS>
                class   InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
                private:
                    using   IStreamType = typename TRAITS::IStreamType;

                private:
                    SeekableFlag    DefaultSeekable_ (IStreamType& originalStream)
                    {
                        // SB something like lseek(fd, CURRENT, 0) not an error, but that doesnt work wtih seekg() on
                        // MSVC2k13. Not sure of a good portable, and yet non-destructive way...
                        return eSeekable;
                    }
                public:
                    Rep_ (IStreamType& originalStream)
                        : Rep_ (originalStream, DefaultSeekable_ (originalStream))
                    {
                    }
                    Rep_ (IStreamType& originalStream, SeekableFlag seekable)
                        : fOriginalStream_ (originalStream)
                        , fSeekable_ (seekable)
                    {
                    }

                protected:
                    virtual bool    IsSeekable () const override
                    {
                        return fSeekable_ == eSeekable;
                    }
                    virtual size_t  Read (SeekOffsetType* offset, ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                    {
                        // @todo implement 'offset' support
                        RequireNotNull (intoStart);
                        RequireNotNull (intoEnd);
                        Require (intoStart < intoEnd);

                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                        if (fOriginalStream_.eof ()) {
                            return 0;
                        }
                        size_t  maxToRead   =   intoEnd - intoStart;
                        using StreamElementType = typename IStreamType::char_type;
                        fOriginalStream_.read (reinterpret_cast<StreamElementType*> (intoStart), maxToRead);
                        size_t  n   =    static_cast<size_t> (fOriginalStream_.gcount ());      // cast safe cuz amount asked to read was also size_t

                        // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
                        if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
                            Execution::DoThrow (Execution::StringException (Characters::String_Constant { L"Failed to read from istream" }));
                        }
                        return n;
                    }
                    virtual SeekOffsetType  GetReadOffset () const override
                    {
                        // instead of tellg () - avoids issue with EOF where fail bit set???
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
                    }
                    virtual SeekOffsetType  SeekRead (Whence whence, SignedSeekOffsetType offset) override
                    {
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                        switch (whence) {
                            case    Whence::eFromStart:
                                fOriginalStream_.seekg (offset, ios::beg);
                                break;
                            case    Whence::eFromCurrent:
                                fOriginalStream_.seekg (offset, ios::cur);
                                break;
                            case    Whence::eFromEnd:
                                fOriginalStream_.seekg (offset, ios::end);
                                break;
                        }
                        // instead of tellg () - avoids issue with EOF where fail bit set???
                        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
                    }

                private:
                    IStreamType&    fOriginalStream_;
                    SeekableFlag    fSeekable_;
                };


                /*
                 ********************************************************************************
                 ********************* InputStreamFromStdIStream<ELEMENT_TYPE> ******************
                 ********************************************************************************
                 */
                template    <typename   ELEMENT_TYPE, typename TRAITS>
                InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::InputStreamFromStdIStream (IStreamType& originalStream)
                    : InputStream<ELEMENT_TYPE> (make_shared<Rep_> (originalStream))
                {
                }
                template    <typename   ELEMENT_TYPE, typename TRAITS>
                InputStreamFromStdIStream<ELEMENT_TYPE, TRAITS>::InputStreamFromStdIStream (IStreamType& originalStream, SeekableFlag seekable)
                    : InputStream<ELEMENT_TYPE> (make_shared<Rep_> (originalStream, seekable))
                {
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Streams_iostream_InputStreamFromStdIStream_inl_*/
