/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Execution/OperationNotSupportedException.h"

#include    "BinaryInputStreamFromIStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;





class   BinaryInputStreamFromIStreamAdapter::IRep_ : public BinaryInputStream::_IRep {
public:
    IRep_ (istream& originalStream)
        : fOriginalStream_ (originalStream) {
    }

protected:
    virtual size_t          Read (Byte* intoStart, Byte* intoEnd) override {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);

        if (fOriginalStream_.eof ()) {
            return 0;
        }
        size_t  maxToRead   =   intoEnd - intoStart;
        fOriginalStream_.read (reinterpret_cast<char*> (intoStart), maxToRead);
        size_t  n   =    static_cast<size_t> (fOriginalStream_.gcount ());      // cast safe cuz amount asked to read was also size_t

        // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
        if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (L"Failed to read from istream"));
        }
        return n;
    }

    virtual SeekOffsetType  _GetOffset () const override {
        // instead of tellg () - avoids issue with EOF where fail bit set???
        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
    }

    virtual void            _Seek (Whence whence, SeekOffsetType offset) override {
        switch (whence) {
            case    FromStart_W:
                fOriginalStream_.seekg (offset, ios::beg);
                break;
            case    FromCurrent_W:
                fOriginalStream_.seekg (offset, ios::cur);
                break;
            case    FromEnd_W:
                fOriginalStream_.seekg (offset, ios::end);
                break;
        }
    }

private:
    istream&    fOriginalStream_;
};




/*
 ********************************************************************************
 ********* Streams::iostream::BinaryInputStreamFromIStreamAdapter ***************
 ********************************************************************************
 */
BinaryInputStreamFromIStreamAdapter::BinaryInputStreamFromIStreamAdapter (std::istream& originalStream)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (originalStream)))
{
}

