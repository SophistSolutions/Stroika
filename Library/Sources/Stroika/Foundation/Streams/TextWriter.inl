/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_TextWriter_inl_
#define _Stroika_Foundation_Streams_TextWriter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InternallySynchronizedOutputStream.h" // no need to include once we remove deprecated references to this

namespace Stroika::Foundation::Streams::TextWriter {

    namespace Private_ {
        class UnSeekable_CodeCvt_Rep_ : public OutputStream::IRep<Character> {
        public:
            UnSeekable_CodeCvt_Rep_ (const OutputStream::Ptr<byte>& src, Characters::CodeCvt<Character>&& converter)
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
            OutputStream::Ptr<byte>                                        _fSource;
            Characters::CodeCvt<Character>                                 _fConverter;
            std::mbstate_t                                                 _fMBState_{};
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };

        template <Characters ::IUNICODECanUnambiguouslyConvertFrom OUTPUT_CHAR_T>
        class UnSeekable_UTFConverter_Rep_ : public OutputStream::IRep<Character> {
        public:
            template <typename CONVERTER>
            UnSeekable_UTFConverter_Rep_ (const OutputStream::Ptr<byte>& src, CONVERTER&& converter)
                : _fSource{src}
                , _fConverter{forward<CONVERTER> (converter)}
            {
            }
            UnSeekable_UTFConverter_Rep_ (const OutputStream::Ptr<byte>& src)
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
            OutputStream::Ptr<byte>                                        _fSource;
            Characters::UTFConvert                                         _fConverter;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ****************************** TextWriter::New *********************************
     ********************************************************************************
     */
    inline auto New (const OutputStream::Ptr<Character>& src) -> Ptr
    {
        return src;
    }
    inline Ptr New (const OutputStream::Ptr<byte>& src, Characters::CodeCvt<>&& char2OutputConverter)
    {
        return Ptr{make_shared<Private_::UnSeekable_CodeCvt_Rep_> (src, move (char2OutputConverter))};
    }
    inline Ptr New (const OutputStream::Ptr<byte>& src, Characters::UnicodeExternalEncodings e, Characters::ByteOrderMark bom)
    {
        if (bom == Characters::ByteOrderMark::eInclude) {
            src.Write (Characters::GetByteOrderMark (e));
        }
        // handle a few common cases more efficiently, without vectoring through CodeCvt<> (which has an extra level of indirection)
        switch (e) {
            case Characters::UnicodeExternalEncodings::eUTF8:
                return Ptr{make_shared<Private_::UnSeekable_UTFConverter_Rep_<char8_t>> (src)};
            case Characters::UnicodeExternalEncodings::eUTF16:
                return Ptr{make_shared<Private_::UnSeekable_UTFConverter_Rep_<char16_t>> (src)};
            case Characters::UnicodeExternalEncodings::eUTF32:
                return Ptr{make_shared<Private_::UnSeekable_UTFConverter_Rep_<char32_t>> (src)};
            default:
                // but default to using the CodeCvt writer
                return New (src, Characters::CodeCvt<Character> (e));
        }
    }
    template <typename... ARGS>
    inline Ptr New (Execution::InternallySynchronized internallySynchronized, ARGS... args)
    {
        switch (internallySynchronized) {
            case Execution::InternallySynchronized::eNotKnownInternallySynchronized:
                return New (forward<ARGS> (args...));
            case Execution::InternallySynchronized::eInternallySynchronized:
                // @todo could explicitly specialize more cases and handle more efficiently, but using the REP overload of InternallySynchronizedInputStream
                return InternallySynchronizedOutputStream::New ({}, New (forward<ARGS> (args...)));
        }
    }

}

#endif /*_Stroika_Foundation_Streams_TextWriter_inl_*/
