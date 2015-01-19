/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     !qCompilerAndStdLib_string_conversions_Buggy
#include    <codecvt>
#endif

#include    "../Characters/CodePage.h"
#include    "../Characters/String_Constant.h"
#include    "../Containers/Common.h"
#include    "../Execution/Common.h"
#include    "../Execution/StringException.h"
#include    "../Execution/OperationNotSupportedException.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "TextOutputStreamBinaryAdapter.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Streams;

using   Execution::make_unique_lock;

#if     !qCompilerAndStdLib_string_conversions_Buggy
namespace {
    const   codecvt_utf8<wchar_t>   kConverter_;        // safe to keep static because only read-only const methods used
}
#endif

class   TextOutputStreamBinaryAdapter::UnSeekable_UTF8_IRep_ : public TextOutputStream::_IRep {
public:
    UnSeekable_UTF8_IRep_ (const BinaryOutputStream& src, bool useBOM)
        : _fCriticalSection ()
        , _fSource (src)
    {
        const   Byte    kBOM[]  =    { 0xEF, 0xBB, 0xBF};   //  see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            _fSource.Write (std::begin (kBOM), std::end (kBOM));
        }
    }

protected:
    virtual void    Write (const Character* start, const Character* end)  override
    {
#if     !qCompilerAndStdLib_string_conversions_Buggy
        const wchar_t*  sc  =   CVT_CHARACTER_2_wchar_t (start);
        const wchar_t*  ec  =   CVT_CHARACTER_2_wchar_t (end);
        const wchar_t*  pc  =   sc;

        mbstate_t mb {};

        char  outBuf[10 * 1024];
        //char    outBuf[10]; // to test
        auto    critSec { make_unique_lock (_fCriticalSection) };
Again:
        char*   p   =   std::begin (outBuf);
        codecvt_utf8<wchar_t>::result r = kConverter_.out (mb, sc, ec, pc, std::begin (outBuf), std::end (outBuf), p);
        Assert (std::begin (outBuf) <= p and p <= std::end (outBuf));
        _fSource.Write (reinterpret_cast<const Byte*> (std::begin (outBuf)), reinterpret_cast<const Byte*> (p));
        if (r == codecvt_utf8<wchar_t>::partial or pc < ec) {
            sc = pc;
            goto Again;
        }
        if (r != codecvt_utf8<wchar_t>::ok) {
            // not sure waht to throw!
            Execution::DoThrow (Execution::StringException (String_Constant (L"Error converting characters codepage")));
        }
#else
        const wchar_t*  sc  =   CVT_CHARACTER_2_wchar_t (start);
        const wchar_t*  ec  =   CVT_CHARACTER_2_wchar_t (end);
        string tmp = Characters::WideStringToUTF8 (wstring (sc, ec));
        _fSource.Write (reinterpret_cast<const Byte*> (Containers::Start (tmp)), reinterpret_cast<const Byte*> (Containers::Start (tmp) + tmp.length ()));
#endif
    }

protected:
    mutable recursive_mutex     _fCriticalSection;
    BinaryOutputStream          _fSource;
};


class   TextOutputStreamBinaryAdapter::UnSeekable_WCharT_IRep_ : public TextOutputStream::_IRep {
public:
    UnSeekable_WCharT_IRep_ (const BinaryOutputStream& src, bool useBOM)
        : _fSource (src)
    {
        const   Character   kBOM    =    L'\xFEFF'; //  same whether 16 or 32 bit encoding -- see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            Write (&kBOM, &kBOM + 1);
        }
    }

protected:
    virtual void    Write (const Character* start, const Character* end)  override
    {
        _fSource.Write (reinterpret_cast<const Byte*> (start), reinterpret_cast<const Byte*> (end));
    }

protected:
    BinaryOutputStream          _fSource;
};


class   TextOutputStreamBinaryAdapter::Seekable_UTF8_IRep_ : public UnSeekable_UTF8_IRep_, public Seekable::_IRep {
public:
    Seekable_UTF8_IRep_ (const BinaryOutputStream& src, bool useBOM)
        : UnSeekable_UTF8_IRep_ (src, useBOM)
        , fOffset_ (0)
    {
    }

protected:
#if 0
    virtual void    Write (const Character* start, const Character* end)  override
    {
        SeekOffsetType  offset  =   fOffset_;
        // Take into account seek offset...
    }
#endif

    virtual SeekOffsetType  GetOffset () const override
    {
        AssertNotImplemented ();
        return fOffset_;
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override
    {
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
        , fOffset_ (0)
    {
    }

protected:
#if 0
    virtual void    Write (const Character* start, const Character* end)  override
    {
        SeekOffsetType  offset  =   fOffset_;
        // Take into account seek offset...
    }
#endif

    virtual SeekOffsetType  GetOffset () const override
    {
        AssertNotImplemented ();
        return fOffset_;
    }

    virtual SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) override
    {
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

