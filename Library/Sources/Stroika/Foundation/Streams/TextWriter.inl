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
namespace Stroika::Foundation::Streams::TextWriter {

    namespace Private_ {
        class UnSeekable_CodeCvt_Rep_ : public OutputStream<Character>::_IRep {
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
     ****************************** TextWriter::New *********************************
     ********************************************************************************
     */
    inline auto New (const OutputStream<Character>::Ptr& src) -> Ptr
    {
        return src;
    }
    inline Ptr New (const OutputStream<byte>::Ptr& src, Characters::CodeCvt<>&& char2OutputConverter)
    {
        return Ptr{make_shared<Private_::UnSeekable_CodeCvt_Rep_> (src, move (char2OutputConverter))};
    }
    inline Ptr New (const OutputStream<byte>::Ptr& src, Characters::UnicodeExternalEncodings e, Characters::ByteOrderMark bom)
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

    /////////////// ***************** DEPRECATED BELOW /////////////////

    DISABLE_COMPILER_MSC_WARNING_START (4996); // DEPRECATED
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    enum class [[deprecated ("Since Stroka v3.0d1, use UnicodeExternalEncodings overload")]] Format : uint8_t{
        eUTF8WithBOM = 1, eUTF8WithoutBOM = 2, eUTF8 = eUTF8WithBOM, eWCharTWithBOM = 3, eWCharTWithoutBOM = 4, eWCharT = eWCharTWithBOM,
    };
    [[deprecated ("Since Stroka v3.0d1, use UnicodeExternalEncodings overload")]] static Ptr New (const OutputStream<byte>::Ptr& src,
                                                                                                  Format format); // to be deprecated soon
    [[deprecated ("Since Stroka v3.0d1, just wrap in InternallySynchronizedOutputStream direclty if needed")]] static Ptr
    New (Execution::InternallySynchronized internallySynchronized, const OutputStream<byte>::Ptr& src, Format format = Format::eUTF8);
    [[deprecated ("Since Stroka v3.0d1, just wrap in InternallySynchronizedOutputStream direclty if needed")]] static Ptr
    New (Execution::InternallySynchronized internallySynchronized, const OutputStream<Character>::Ptr& src);
    DISABLE_COMPILER_MSC_WARNING_END (4996); // DEPRECATED
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    DISABLE_COMPILER_MSC_WARNING_START (4996); // DEPRECATED
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    inline auto TextWriter::New (const OutputStream<byte>::Ptr& src, Format format) -> Ptr
    {
        using Characters::UnicodeExternalEncodings;
        switch (format) {
            case Format::eUTF8WithBOM:
            case Format::eUTF8WithoutBOM:
                return New (src, UnicodeExternalEncodings::eUTF8, format == Format::eUTF8WithBOM ? ByteOrderMark::eInclude : ByteOrderMark::eDontInclude);
            case Format::eWCharTWithBOM:
            case Format::eWCharTWithoutBOM:
                return New (src, sizeof (wchar_t) == 2 ? UnicodeExternalEncodings::eUTF16 : UnicodeExternalEncodings::eUTF32,
                            format == Format::eWCharTWithBOM ? ByteOrderMark::eInclude : ByteOrderMark::eDontInclude);
            default:
                RequireNotReached ();
                return Ptr{};
        }
    }
    inline auto TextWriter::New ([[maybe_unused]] Execution::InternallySynchronized internallySynchronized, const OutputStream<Character>::Ptr& src) -> Ptr
    {
        Assert (internallySynchronized == Execution::eNotKnownInternallySynchronized);
        return src;
    }
    inline auto TextWriter::New ([[maybe_unused]] Execution::InternallySynchronized internallySynchronized,
                                 const OutputStream<byte>::Ptr& src, Format format) -> Ptr
    {
        Assert (internallySynchronized == Execution::eNotKnownInternallySynchronized);
        return TextWriter::New (src, format);
    }
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_MSC_WARNING_END (4996); // DEPRECATED

}

#endif /*_Stroika_Foundation_Streams_TextWriter_inl_*/
