/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <codecvt>
#include <cstdarg>

#include "../Characters/CodePage.h"
#include "../Characters/Format.h"
#include "../Characters/String_Constant.h"
#include "../Containers/Common.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Execution/StringException.h"
#include "../Memory/SmallStackBuffer.h"

#include "TextWriter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Streams;

using Execution::make_unique_lock;

namespace {
    const codecvt_utf8<wchar_t> kConverter_; // safe to keep static because only read-only const methods used
}

class TextWriter::UnSeekable_UTF8_Rep_ : public OutputStream<Character>::_IRep, private Debug::AssertExternallySynchronizedLock {
public:
    UnSeekable_UTF8_Rep_ (const OutputStream<Byte>::Ptr& src, bool useBOM)
        : _fSource (src)
    {
        constexpr Byte kBOM[] = {0xEF, 0xBB, 0xBF}; //  see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            _fSource.Write (std::begin (kBOM), std::end (kBOM));
        }
    }

protected:
    virtual bool IsSeekable () const override
    {
        return false;
    }
    virtual SeekOffsetType GetWriteOffset () const override
    {
        AssertNotImplemented ();
        return 0;
    }
    virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
    {
        AssertNotImplemented ();
        return 0;
    }
    virtual void Write (const Character* start, const Character* end) override
    {
        const wchar_t* sc = CVT_CHARACTER_2_wchar_t (start);
        const wchar_t* ec = CVT_CHARACTER_2_wchar_t (end);
        const wchar_t* pc = sc;

        char outBuf[10 * 1024];
        //char    outBuf[10]; // to test
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Again:
        char*                         p = std::begin (outBuf);
        codecvt_utf8<wchar_t>::result r = kConverter_.out (fMBState_, sc, ec, pc, std::begin (outBuf), std::end (outBuf), p);
        Assert (std::begin (outBuf) <= p and p <= std::end (outBuf));
        _fSource.Write (reinterpret_cast<const Byte*> (std::begin (outBuf)), reinterpret_cast<const Byte*> (p));
        if (r == codecvt_utf8<wchar_t>::partial or pc < ec) {
            sc = pc;
            goto Again;
        }
        if (r != codecvt_utf8<wchar_t>::ok) {
            // not sure waht to throw!
            Execution::Throw (Execution::StringException (String_Constant (L"Error converting characters codepage")));
        }
    }
    virtual void Flush () override
    {
        // NYI
    }

protected:
    mbstate_t               fMBState_{};
    OutputStream<Byte>::Ptr _fSource;
};

class TextWriter::UnSeekable_WCharT_Rep_ : public OutputStream<Character>::_IRep, private Debug::AssertExternallySynchronizedLock {
public:
    UnSeekable_WCharT_Rep_ (const OutputStream<Byte>::Ptr& src, bool useBOM)
        : _fSource (src)
    {
        constexpr Character kBOM = L'\xFEFF'; //  same whether 16 or 32 bit encoding -- see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            Write (&kBOM, &kBOM + 1);
        }
    }

protected:
    virtual bool IsSeekable () const override
    {
        return false;
    }
    virtual SeekOffsetType GetWriteOffset () const override
    {
        AssertNotImplemented ();
        return 0;
    }
    virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
    {
        AssertNotImplemented ();
        return 0;
    }
    virtual void Write (const Character* start, const Character* end) override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        _fSource.Write (reinterpret_cast<const Byte*> (start), reinterpret_cast<const Byte*> (end));
    }
    virtual void Flush () override
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        _fSource.Flush ();
    }

protected:
    OutputStream<Byte>::Ptr _fSource;
};

class TextWriter::Seekable_UTF8_Rep_ : public UnSeekable_UTF8_Rep_ {
public:
    Seekable_UTF8_Rep_ (const OutputStream<Byte>::Ptr& src, bool useBOM)
        : UnSeekable_UTF8_Rep_ (src, useBOM)
        , fOffset_ (0)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
#if 0
    virtual void    Write (const Character* start, const Character* end)  override
    {
        SeekOffsetType  offset  =   fOffset_;
        // Take into account seek offset...
    }
#endif

    virtual SeekOffsetType GetWriteOffset () const override
    {
        AssertNotImplemented ();
        return fOffset_;
    }

    virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
    {
        AssertNotImplemented ();
        return 0;
    }

private:
    size_t fOffset_;
};

class TextWriter::Seekable_WCharT_Rep_ : public UnSeekable_WCharT_Rep_ {
public:
    Seekable_WCharT_Rep_ (const OutputStream<Byte>::Ptr& src, bool useBOM)
        : UnSeekable_WCharT_Rep_ (src, useBOM)
        , fOffset_ (0)
    {
    }

protected:
    virtual bool IsSeekable () const override
    {
        return true;
    }
#if 0
    virtual void    Write (const Character* start, const Character* end)  override
    {
        SeekOffsetType  offset  =   fOffset_;
        // Take into account seek offset...
    }
#endif

    virtual SeekOffsetType GetWriteOffset () const override
    {
        AssertNotImplemented ();
        return fOffset_;
    }

    virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
    {
        AssertNotImplemented ();
        return 0;
    }

private:
    size_t fOffset_;
};

/*
 ********************************************************************************
 ****************************** Streams::TextWriter *****************************
 ********************************************************************************
 */
auto TextWriter::New (const OutputStream<Byte>::Ptr& src, Format format) -> Ptr
{
    return Ptr{mk_ (src, format)};
}

auto TextWriter::New (Execution::InternallySyncrhonized internallySyncrhonized, const OutputStream<Characters::Character>::Ptr& src) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return src;
        case Execution::eNotKnwonInternallySynchronized:
            return src;
        default:
            RequireNotReached ();
    }
}
auto TextWriter::New (Execution::InternallySyncrhonized internallySyncrhonized, const OutputStream<Byte>::Ptr& src, Format format) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return Ptr{mk_ (src, format)};
        case Execution::eNotKnwonInternallySynchronized:
            return Ptr{mk_ (src, format)};
        default:
            RequireNotReached ();
    }
}

shared_ptr<OutputStream<Characters::Character>::_IRep> TextWriter::mk_ (const OutputStream<Byte>::Ptr& src, Format format)
{
    bool newOneSeekable = src.IsSeekable ();
    bool withBOM        = (format == Format::eUTF8WithBOM or format == Format::eWCharTWithBOM);
    switch (format) {
        case Format::eUTF8WithBOM:
        case Format::eUTF8WithoutBOM:
            return newOneSeekable ? make_shared<Seekable_UTF8_Rep_> (src, withBOM) : make_shared<UnSeekable_UTF8_Rep_> (src, withBOM);
        case Format::eWCharTWithBOM:
        case Format::eWCharTWithoutBOM:
            return newOneSeekable ? make_shared<Seekable_WCharT_Rep_> (src, withBOM) : make_shared<UnSeekable_WCharT_Rep_> (src, withBOM);
        default:
            RequireNotReached ();
            return nullptr;
    }
}
