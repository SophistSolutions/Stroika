/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_inl_
#define _Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_inl_   1


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
                 *************** OutputStreamFromStdOStream<ELEMENT_TYPE>::Rep_ *****************
                 ********************************************************************************
                 */
                template    <typename   ELEMENT_TYPE, typename TRAITS>
                class   OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::Rep_ : public OutputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
                private:
                    using   OStreamType = typename TRAITS::OStreamType;

                public:
                    Rep_ (OStreamType& originalStream)
                        : fOriginalStream_ (originalStream)
                    {
                    }

                protected:
                    virtual bool    IsSeekable () const override
                    {
                        return true;
                    }
                    virtual SeekOffsetType  GetWriteOffset () const override
                    {
                        // instead of tellg () - avoids issue with EOF where fail bit set???
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::out);
                    }
                    virtual SeekOffsetType  SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                    {
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                        switch (whence) {
                            case    Whence::eFromStart:
                                fOriginalStream_.seekp (offset, ios::beg);
                                break;
                            case    Whence::eFromCurrent:
                                fOriginalStream_.seekp (offset, ios::cur);
                                break;
                            case    Whence::eFromEnd:
                                fOriginalStream_.seekp (offset, ios::end);
                                break;
                        }
                        return fOriginalStream_.tellp ();
                    }
                    virtual void    Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                    {
                        Require (start != nullptr or start == end);
                        Require (end != nullptr or start == end);

                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };

                        using StreamElementType = typename OStreamType::char_type;
                        fOriginalStream_.write (reinterpret_cast<const StreamElementType*> (start), end - start);
                        if (fOriginalStream_.fail ()) {
                            Execution::DoThrow (Execution::StringException (Characters::String_Constant (L"Failed to write from ostream")));
                        }
                    }
                    virtual void    Flush () override
                    {
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                        fOriginalStream_.flush ();
                        if (fOriginalStream_.fail ()) {
                            Execution::DoThrow (Execution::StringException (Characters::String_Constant (L"Failed to flush ostream")));
                        }
                    }

                private:
                    OStreamType&    fOriginalStream_;
                };


                /*
                 ********************************************************************************
                 *********************** OutputStreamFromStdOStream<ELEMENT_TYPE> ***************
                 ********************************************************************************
                 */
                template    <typename   ELEMENT_TYPE, typename TRAITS>
                OutputStreamFromStdOStream<ELEMENT_TYPE, TRAITS>::OutputStreamFromStdOStream (OStreamType& originalStream)
                    : OutputStream<ELEMENT_TYPE> (make_shared<Rep_> (originalStream))
                {
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Streams_iostream_OutputStreamFromStdOStream_inl_*/
