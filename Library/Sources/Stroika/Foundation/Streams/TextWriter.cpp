/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/CodeCvt.h"
#include "../Characters/TextConvert.h"
#include "../Containers/Support/ReserveTweaks.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Execution/Common.h"
#include "../Execution/OperationNotSupportedException.h"
#include "../Memory/InlineBuffer.h"
#include "../Memory/StackBuffer.h"

#include "TextWriter.h"

namesapce
{

    /*
     ********************************************************************************
     ********************** TextWriter::UnSeekable_CodeCvt_Rep_ *********************
     ********************************************************************************
     */
    class UnSeekable_CodeCvt_Rep_ : public OutputStream::IRep<Character> {
    public:
        UnSeekable_CodeCvt_Rep_ (const OutputStream<byte>::Ptr& src, Characters::CodeCvt<Character>&& converter)
            : _fSource{src}
            , _fConverter{move (converter)}
        {
        }

    protected:
        virtual bool IsSeekable () const override
        {
            return false;
        }
        virtual void CloseWrite () override
        {
            Require (IsOpenWrite ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            _fSource.Close ();
            Assert (_fSource == nullptr);
            Ensure (not IsOpenWrite ());
        }
        virtual bool IsOpenWrite () const override
        {
            return _fSource != nullptr;
        }
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
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenWrite ());
            Memory::StackBuffer<byte> cvtBuf{size_t (end - start) * 5}; // excessive but start with that
            auto                      srcSpan = span<const Character>{start, end};
            auto                      trgSpan = span<byte>{cvtBuf.data (), cvtBuf.size ()};
            trgSpan                           = _fConverter.Characters2Bytes (srcSpan, trgSpan);
            _fSource.Write (trgSpan.data (), trgSpan.data () + trgSpan.size ());
        }
        virtual void Flush () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenWrite ());
            _fSource.Flush ();
        }

    protected:
        OutputStream<byte>::Ptr                                        _fSource;
        Characters::CodeCvt<Character>                                 _fConverter;
        std::mbstate_t                                                 _fMBState_{};
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };

    /*
     ********************************************************************************
     ***************** TextWriter::UnSeekable_UTFConverter_Rep_ *********************
     ********************************************************************************
     */
    template <Characters ::IUNICODECanUnambiguouslyConvertFrom OUTPUT_CHAR_T>
    class UnSeekable_UTFConverter_Rep_ : public OutputStream<Character>::_IRep {
    public:
        template <typename CONVERTER>
        UnSeekable_UTFConverter_Rep_ (const OutputStream<byte>::Ptr& src, CONVERTER&& converter)
            : _fSource{src}
            , _fConverter{forward<CONVERTER> (converter)}
        {
        }
        UnSeekable_UTFConverter_Rep_ (const OutputStream<byte>::Ptr& src)
            : _fSource{src}
            , _fConverter{Characters::UTFConvert::kThe}
        {
        }

    protected:
        virtual bool IsSeekable () const override
        {
            return false;
        }
        virtual void CloseWrite () override
        {
            Require (IsOpenWrite ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            _fSource.Close ();
            Assert (_fSource == nullptr);
            Ensure (not IsOpenWrite ());
        }
        virtual bool IsOpenWrite () const override
        {
            return _fSource != nullptr;
        }
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
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenWrite ());
            auto                               srcSpan = span<const Character>{start, end};
            Memory::StackBuffer<OUTPUT_CHAR_T> cvtBuf{_fConverter.ComputeTargetBufferSize<OUTPUT_CHAR_T> (srcSpan)};
            auto                               trgSpan  = span<OUTPUT_CHAR_T>{cvtBuf.data (), cvtBuf.size ()};
            auto                               r        = _fConverter.ConvertSpan (srcSpan, trgSpan);
            auto                               trgBytes = as_bytes (r);
            _fSource.Write (trgBytes.data (), trgBytes.data () + trgBytes.size ());
        }
        virtual void Flush () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
            Require (IsOpenWrite ());
            _fSource.Flush ();
        }

    protected:
        OutputStream<byte>::Ptr                                        _fSource;
        Characters::UTFConvert                                         _fConverter;
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
    };
}
/*
     ********************************************************************************
     *********************************** TextWriter *********************************
     ********************************************************************************
     */
inline TextWriter::Ptr TextWriter::New (const OutputStream<byte>::Ptr& src, Characters::CodeCvt<>&& char2OutputConverter)
{
    return TextWriter::Ptr{make_shared<UnSeekable_CodeCvt_Rep_> (src, move (char2OutputConverter))};
}
inline TextWriter::Ptr TextWriter::New (const OutputStream<byte>::Ptr& src, Characters::UnicodeExternalEncodings e, Characters::ByteOrderMark bom)
{
    if (bom == Characters::ByteOrderMark::eInclude) {
        src.Write (Characters::GetByteOrderMark (e));
    }
    // handle a few common cases more efficiently, without vectoring through CodeCvt<> (which has an extra level of indirection)
    switch (e) {
        case Characters::UnicodeExternalEncodings::eUTF8:
            return TextWriter::Ptr{make_shared<UnSeekable_UTFConverter_Rep_<char8_t>> (src)};
        case Characters::UnicodeExternalEncodings::eUTF16:
            return TextWriter::Ptr{make_shared<UnSeekable_UTFConverter_Rep_<char16_t>> (src)};
        case Characters::UnicodeExternalEncodings::eUTF32:
            return TextWriter::Ptr{make_shared<UnSeekable_UTFConverter_Rep_<char32_t>> (src)};
        default:
            // but default to using the CodeCvt writer
            return New (src, Characters::CodeCvt<Character> (e));
    }
}
