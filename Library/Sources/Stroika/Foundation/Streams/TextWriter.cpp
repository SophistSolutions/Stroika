/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <codecvt>
#include <cstdarg>

#include "../Characters/CodePage.h"
#include "../Characters/Format.h"
#include "../Containers/Common.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Execution/Common.h"
#include "../Execution/Exceptions.h"
#include "../Execution/OperationNotSupportedException.h"

#include "TextWriter.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Streams;

using Debug::AssertExternallySynchronizedMutex;

namespace {
    const codecvt_utf8<wchar_t> kConverter_; // safe to keep static because only read-only const methods used
}

class TextWriter::UnSeekable_UTF8_Rep_ : public OutputStream<Character>::_IRep {
public:
    UnSeekable_UTF8_Rep_ (const OutputStream<byte>::Ptr& src, bool useBOM)
        : _fSource{src}
    {
        using namespace Memory;
        constexpr byte kBOM_[]{0xEF_b, 0xBB_b, 0xBF_b}; //  see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            _fSource.Write (std::begin (kBOM_), std::end (kBOM_));
        }
    }

protected:
    virtual bool IsSeekable () const override { return false; }
    virtual void CloseWrite () override
    {
        Require (IsOpenWrite ());
        AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        _fSource.Close ();
        Assert (_fSource == nullptr);
        Ensure (not IsOpenWrite ());
    }
    virtual bool           IsOpenWrite () const override { return _fSource != nullptr; }
    virtual SeekOffsetType GetWriteOffset () const override
    {
        AssertNotImplemented ();
        Require (IsOpenWrite ());
        return 0;
    }
    virtual SeekOffsetType SeekWrite (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        AssertNotImplemented (); // not seekable
        Require (IsOpenWrite ());
        return 0;
    }
    virtual void Write (const Character* start, const Character* end) override
    {
        Require (IsOpenWrite ());
        // for now - temporarily - we use codecvt_utf8, so must convert to wchar_t
        //        [[maybe_unused]] conditional_t<sizeof (wchar_t) == sizeof (char32_t), char, Memory::StackBuffer<wchar_t>, char> ignored1;
        [[maybe_unused]] Memory::StackBuffer<wchar_t> ignored1;
        const wchar_t*                                sc = nullptr;
        const wchar_t*                                ec = nullptr;
        if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
            sc = reinterpret_cast<const wchar_t*> (start);
            ec = reinterpret_cast<const wchar_t*> (end);
        }
        else {
            ignored1.GrowToSize_uninitialized (UTFConverter::ComputeTargetBufferSize<wchar_t> (span{start, end}));
            auto sz = UTFConverter::kThe.Convert (span{start, end}, span{ignored1.begin (), ignored1.size ()}).fTargetProduced;
            sc      = ignored1.data ();
            ec      = sc + sz;
        }
        const wchar_t* pc = sc;

        char outBuf[10 * 1024];
        //char    outBuf[10]; // to test
        AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
    Again:
        char*                         p = std::begin (outBuf);
        codecvt_utf8<wchar_t>::result r = kConverter_.out (fMBState_, sc, ec, pc, std::begin (outBuf), std::end (outBuf), p);
        Assert (std::begin (outBuf) <= p and p <= std::end (outBuf));
        _fSource.Write (reinterpret_cast<const byte*> (std::begin (outBuf)), reinterpret_cast<const byte*> (p));
        if (r == codecvt_utf8<wchar_t>::partial or pc < ec) {
            sc = pc;
            goto Again;
        }
        if (r != codecvt_utf8<wchar_t>::ok) [[unlikely]] {
            // not sure waht to throw!
            Execution::Throw (Execution::RuntimeErrorException{"Error converting characters codepage"sv});
        }
    }
    virtual void Flush () override
    {
        Require (IsOpenWrite ());
        // NYI
    }

protected:
    mbstate_t                                                      fMBState_{};
    OutputStream<byte>::Ptr                                        _fSource;
    [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
};

class TextWriter::UnSeekable_WCharT_Rep_ : public OutputStream<Character>::_IRep {
public:
    UnSeekable_WCharT_Rep_ (const OutputStream<byte>::Ptr& src, bool useBOM)
        : _fSource{src}
    {
        constexpr wchar_t kBOM = L'\xFEFF'; //  same whether 16 or 32 bit encoding -- see http://en.wikipedia.org/wiki/Byte_order_mark
        if (useBOM) {
            src.Write (reinterpret_cast<const byte*> (&kBOM), reinterpret_cast<const byte*> (&kBOM + 1));
        }
    }

protected:
    virtual bool IsSeekable () const override { return false; }
    virtual void CloseWrite () override
    {
        Require (IsOpenWrite ());
        AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        _fSource.Close ();
        Assert (_fSource == nullptr);
        Ensure (not IsOpenWrite ());
    }
    virtual bool           IsOpenWrite () const override { return _fSource != nullptr; }
    virtual SeekOffsetType GetWriteOffset () const override
    {
        AssertNotImplemented ();
        Require (IsOpenWrite ());
        return 0;
    }
    virtual SeekOffsetType SeekWrite (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        AssertNotImplemented (); // not seekable
        Require (IsOpenWrite ());
        return 0;
    }
    virtual void Write (const Character* start, const Character* end) override
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        Require (IsOpenWrite ());
        _fSource.Write (reinterpret_cast<const byte*> (start), reinterpret_cast<const byte*> (end));
    }
    virtual void Flush () override
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        Require (IsOpenWrite ());
        _fSource.Flush ();
    }

protected:
    OutputStream<byte>::Ptr                                        _fSource;
    [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
};

class TextWriter::Seekable_UTF8_Rep_ final : public UnSeekable_UTF8_Rep_ {
public:
    Seekable_UTF8_Rep_ (const OutputStream<byte>::Ptr& src, bool useBOM)
        : UnSeekable_UTF8_Rep_{src, useBOM}
        , fOffset_{0}
    {
    }

protected:
    virtual bool IsSeekable () const override { return true; }
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
        Require (IsOpenWrite ());
        return fOffset_;
    }

    virtual SeekOffsetType SeekWrite (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        AssertNotImplemented (); // @todo - FIX - BAD - this must be seekable, but we've not yet implemented that feature yet!!!
        Require (IsOpenWrite ());
        return fOffset_;
    }

private:
    size_t fOffset_;
};

class TextWriter::Seekable_WCharT_Rep_ final : public UnSeekable_WCharT_Rep_ {
public:
    Seekable_WCharT_Rep_ (const OutputStream<byte>::Ptr& src, bool useBOM)
        : UnSeekable_WCharT_Rep_{src, useBOM}
        , fOffset_{0}
    {
    }

protected:
    virtual bool IsSeekable () const override { return true; }
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
        Require (IsOpenWrite ());
        return fOffset_;
    }

    virtual SeekOffsetType SeekWrite (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        AssertNotImplemented ();
        Require (IsOpenWrite ());
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
auto TextWriter::New (const OutputStream<byte>::Ptr& src, Format format) -> Ptr { return Ptr{mk_ (src, format)}; }

auto TextWriter::New (Execution::InternallySynchronized internallySynchronized, const OutputStream<Characters::Character>::Ptr& src) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return src;
        case Execution::eNotKnownInternallySynchronized:
            return src;
        default:
            RequireNotReached ();
            return src;
    }
}
auto TextWriter::New (Execution::InternallySynchronized internallySynchronized, const OutputStream<byte>::Ptr& src, Format format) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            AssertNotImplemented ();
            //return InternalSyncRep_::New ();
            return Ptr{mk_ (src, format)};
        case Execution::eNotKnownInternallySynchronized:
            return Ptr{mk_ (src, format)};
        default:
            RequireNotReached ();
            return Ptr{mk_ (src, format)};
    }
}

shared_ptr<OutputStream<Characters::Character>::_IRep> TextWriter::mk_ (const OutputStream<byte>::Ptr& src, Format format)
{
    Require (src.IsOpen ());
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
