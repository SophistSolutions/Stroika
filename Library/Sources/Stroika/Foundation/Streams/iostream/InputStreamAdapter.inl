/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_InputStreamAdapter_inl_
#define _Stroika_Foundation_Streams_iostream_InputStreamAdapter_inl_   1


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
                 ******************** InputStreamAdapter<ELEMENT_TYPE>::Rep_ ********************
                 ********************************************************************************
                 */
                template    <typename   ELEMENT_TYPE>
                class   InputStreamAdapter<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
                public:
                    Rep_ (istream& originalStream)
                        : fOriginalStream_ (originalStream)
                    {
                    }

                protected:
                    virtual bool    IsSeekable () const override
                    {
                        return true;
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
                        fOriginalStream_.read (reinterpret_cast<char*> (intoStart), maxToRead);
                        size_t  n   =    static_cast<size_t> (fOriginalStream_.gcount ());      // cast safe cuz amount asked to read was also size_t

                        // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
                        if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
                            Execution::DoThrow (Execution::StringException (Characters::String_Constant (L"Failed to read from istream")));
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
                        return fOriginalStream_.tellg ();
                    }

                private:
                    istream&        fOriginalStream_;
                };


                /*
                 ********************************************************************************
                 *********************** InputStreamAdapter<ELEMENT_TYPE> ***********************
                 ********************************************************************************
                 */
                template    <typename   ELEMENT_TYPE>
                InputStreamAdapter<ELEMENT_TYPE>::InputStreamAdapter (std::istream& originalStream)
                    : InputStream<ELEMENT_TYPE> (make_shared<Rep_> (originalStream))
                {
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Streams_iostream_InputStreamAdapter_inl_*/
