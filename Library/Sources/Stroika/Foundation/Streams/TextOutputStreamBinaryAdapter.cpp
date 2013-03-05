/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qCompilerAndStdLib_Supports_string_conversions
#include    <codecvt>
#endif

#include    "../Characters/CodePage.h"
#include    "../Containers/Common.h"
#include    "../Execution/StringException.h"
#include    "../Execution/OperationNotSupportedException.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "TextOutputStreamBinaryAdapter.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;



class   TextOutputStreamBinaryAdapter::UnSeekable_UTF8_IRep_ : public TextOutputStream::_IRep {
public:
    UnSeekable_UTF8_IRep_ (const BinaryOutputStream& src, bool useBOM)
        : _fCriticalSection ()
        , _fSource (src) {
        const   Byte    kBOM[]  =    { 0xEF, 0xBB, 0xBF};   //  see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            _fSource.Write (StartOfArray (kBOM), EndOfArray (kBOM));
        }
    }

protected:
    virtual void    Write (const Character* start, const Character* end)  override {
#if    qCompilerAndStdLib_Supports_string_conversions
        codecvt_utf8<wchar_t>   converter;
        const wchar_t*  sc  =   CVT_CHARACTER_2_wchar_t (start);
        const wchar_t*  ec  =   CVT_CHARACTER_2_wchar_t (end);
        const wchar_t*  pc  =   sc;

        // convert characters to bytes using codepage, and then
        mbstate_t mb = mbstate_t(); // docs say this is OK, but on windows maybe not???

        //char  outBuf[10*1024];
        char    outBuf[10]; // to test
        char*   p   =   StartOfArray (outBuf);
        lock_guard<recursive_mutex>  critSec (_fCriticalSection);
Again:
        codecvt_utf8<wchar_t>::result r = converter.out (mb, sc, ec, pc, StartOfArray (outBuf), EndOfArray (outBuf), p);
        Assert (StartOfArray (outBuf) <= p and p <= EndOfArray (outBuf));
        _fSource.Write (reinterpret_cast<const Byte*> (StartOfArray (outBuf)), reinterpret_cast<const Byte*> (p));
        if (r == codecvt_utf8<wchar_t>::partial) {
            goto Again;
        }
        if (r != codecvt_utf8<wchar_t>::ok) {
            // not sure waht to throw!
            Execution::DoThrow<Execution::StringException> (Execution::StringException (L"Error converting characters codepage"));
        }
#else
        const wchar_t*  sc  =   CVT_CHARACTER_2_wchar_t (start);
        const wchar_t*  ec  =   CVT_CHARACTER_2_wchar_t (end);
        string tmp = Characters::WideStringToUTF8 (wstring (sc, ec));
        _fSource.Write (reinterpret_cast<const Byte*> (Containers::Start (tmp)), reinterpret_cast<const Byte*> (Containers::End (tmp)));
#endif
    }

protected:
    mutable recursive_mutex     _fCriticalSection;
    BinaryOutputStream          _fSource;
};


class   TextOutputStreamBinaryAdapter::UnSeekable_WCharT_IRep_ : public TextOutputStream::_IRep {
public:
    UnSeekable_WCharT_IRep_ (const BinaryOutputStream& src, bool useBOM)
        : _fSource (src) {
        const   Character   kBOM    =    L'\xFEFF'; //  same whether 16 or 32 bit encoding -- see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            Write (&kBOM, &kBOM + 1);
        }
    }

protected:
    virtual void    Write (const Character* start, const Character* end)  override {
        _fSource.Write (reinterpret_cast<const Byte*> (start), reinterpret_cast<const Byte*> (end));
    }

protected:
    BinaryOutputStream          _fSource;
};


class   TextOutputStreamBinaryAdapter::Seekable_UTF8_IRep_ : public UnSeekable_UTF8_IRep_, public Seekable::_IRep {
public:
    Seekable_UTF8_IRep_ (const BinaryOutputStream& src, bool useBOM)
        : UnSeekable_UTF8_IRep_ (src, useBOM)
        , fOffset_ (0) {
    }

protected:
#if 0
    virtual void    Write (const Character* start, const Character* end)  override {
        SeekOffsetType  offset  =   fOffset_;
        // Take into account seek offset...
    }
#endif

    virtual SeekOffsetType  GetOffset () const override {
        AssertNotImplemented ();
        return fOffset_;
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override {
        AssertNotImplemented ();
        return 0;
    }

private:
    size_t              fOffset_;
};


class   TextOutputStreamBinaryAdapter::Seekable_WCharT_IRep_ : public UnSeekable_WCharT_IRep_, public Seekable::_IRep {
public:
    Seekable_WCharT_IRep_ (const BinaryOutputStream& src, bool useBOM)
        : UnSeekable_WCharT_IRep_ (src, useBOM)
        , fOffset_ (0) {
    }

protected:
#if 0
    virtual void    Write (const Character* start, const Character* end)  override {
        SeekOffsetType  offset  =   fOffset_;
        // Take into account seek offset...
    }
#endif

    virtual SeekOffsetType  GetOffset () const override {
        AssertNotImplemented ();
        return fOffset_;
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override {
        AssertNotImplemented ();
        return 0;
    }

private:
    size_t              fOffset_;
};



/*
 ********************************************************************************
 *************** Streams::TextOutputStreamBinaryAdapter *************************
 ********************************************************************************
 */
TextOutputStreamBinaryAdapter::TextOutputStreamBinaryAdapter (const BinaryOutputStream& src, Format format)
    : TextOutputStream (mk_ (src, format))
{
}

shared_ptr<TextOutputStreamBinaryAdapter::_IRep> TextOutputStreamBinaryAdapter::mk_ (const BinaryOutputStream& src, Format format)
{
    bool    newOneSeekable  =   src.IsSeekable ();
    bool    withBOM         =   (format == Format::eUTF8WithBOM or format == Format::eWCharTWithBOM);
    switch (format) {
        case Format::eUTF8WithBOM:
        case Format::eUTF8WithoutBOM:
            return shared_ptr<_IRep> (newOneSeekable ? new Seekable_UTF8_IRep_ (src, withBOM) : new UnSeekable_UTF8_IRep_ (src, withBOM));
        case Format::eWCharTWithBOM:
        case Format::eWCharTWithoutBOM:
            return shared_ptr<_IRep> (newOneSeekable ? new Seekable_WCharT_IRep_ (src, withBOM) : new UnSeekable_WCharT_IRep_ (src, withBOM));
        default:
            RequireNotReached();
            return shared_ptr<_IRep> ();
    }
}

