/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Execution/OperationNotSupportedException.h"

#include    "TextInputStreamFromIStreamAdapter.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;



class   TextInputStreamFromIStreamAdapter::IRep_ : public TextInputStream::_IRep, public Seekable {
public:
    IRep_ (wistream& originalStream)
        : fOriginalStream_ (originalStream) {
    }

protected:
    virtual size_t    _Read (Character* intoStart, Character* intoEnd) override {
        RequireNotNull (intoStart);
        RequireNotNull (intoEnd);
        Require (intoStart < intoEnd);

        if (fOriginalStream_.eof ()) {
            return 0;
        }
        size_t  maxToRead   =   intoEnd - intoStart;
        fOriginalStream_.read (reinterpret_cast<wchar_t*> (intoStart), maxToRead);
        size_t  n   =    static_cast<size_t> (fOriginalStream_.gcount ());      // cast safe cuz amount asked to read was also size_t

        // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
        if (not fOriginalStream_.eof () and fOriginalStream_.fail ()) {
            Execution::DoThrow (Execution::StringException (L"Failed to read from wistream"));
        }
        return n;
    }

    virtual  void _PutBack (Character c) const override {
        AssertNotReached ();
    }

    virtual SeekOffsetType  _GetOffset () const override {
        // instead of tellg () - avoids issue with EOF where fail bit set???
        return fOriginalStream_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
    }

    virtual SeekOffsetType  _Seek (Whence whence, SignedSeekOffsetType offset) override {
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
    wistream&    fOriginalStream_;
};






/*
 ********************************************************************************
 *********** Streams::iostream::TextInputStreamFromIStreamAdapter ***************
 ********************************************************************************
 */
TextInputStreamFromIStreamAdapter::TextInputStreamFromIStreamAdapter (std::wistream& originalStream)
    : TextInputStream (shared_ptr<_IRep> (new IRep_ (originalStream)))
{
}
