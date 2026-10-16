/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/UTFConvert.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/BufferedInputStream.h"

namespace Stroika::Foundation::Streams::iostream::InputStreamFromStdIStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
        class Rep_ : public InputStream::IRep<ELEMENT_TYPE> {
        public:
            /**
             *  Characters from a 16-bit wchar_t - UTF-16, where a Character is one element or two: so this converts, and
             *  cannot seek, its offsets counting Characters but the istream's counting wchar_ts (New () adds seekability)
             */
            static constexpr bool kTranscodes = sizeof (ELEMENT_TYPE) != sizeof (BASIC_ISTREAM_ELEMENT_TYPE);

        private:
            bool fOpen_{true};

        public:
            Rep_ (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream)
                : Rep_{originalStream, kTranscodes ? eNotSeekable : eSeekable}
            {
            }
            Rep_ (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable)
                : fOriginalStreamRef_{originalStream}
                , fSeekable_{seekable}
            {
                Require (not kTranscodes or seekable == eNotSeekable);
            }

        protected:
            virtual bool IsSeekable () const override
            {
                return fSeekable_ == eSeekable;
            }
            virtual void CloseRead () override
            {
                fOpen_ = false;
                Ensure (not IsOpenRead ());
            }
            virtual bool IsOpenRead () const override
            {
                return fOpen_;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                streamsize                                               sz = fOriginalStreamRef_.rdbuf ()->in_avail ();
                // http://en.cppreference.com/w/cpp/io/basic_streambuf/in_avail
                if (sz == 0) {
                    return nullopt;
                }
                else if (sz == -1) {
                    return 0;
                }
                if constexpr (kTranscodes) {
                    // sz counts wchar_ts: two make one Character in a surrogate pair, and a last one may be half a pair still to come
                    return sz == 1 ? nullopt : optional<size_t>{static_cast<size_t> (sz) / 2};
                }
                return static_cast<size_t> (sz);
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if constexpr (kTranscodes) {
                    return nullopt; // Characters left cannot be known without reading them
                }
                AssertNotImplemented ();
                return nullopt; // pretty easy, but @todo
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (fOriginalStreamRef_.eof ()) {
                    return span<ELEMENT_TYPE>{};
                }
                if (blockFlag == NoDataAvailableHandling::eDontBlock and AvailableToRead () == nullopt) {
                    return nullopt;
                }
                if constexpr (kTranscodes) {
                    // intoBuffer.size () wchar_ts make at most that many Characters - and so do they with one more, completing a
                    // surrogate pair they split
                    Memory::StackBuffer<BASIC_ISTREAM_ELEMENT_TYPE> units{Memory::eUninitialized, intoBuffer.size () + 1};
                    size_t                                          n = ReadElements_ (span{units}.subspan (0, intoBuffer.size ()));
                    if (n != 0 and Characters::Character::IsSurrogatePair_Hi (static_cast<char16_t> (units[n - 1]))) {
                        n += ReadElements_ (span{units}.subspan (n, 1));
                    }
                    Memory::StackBuffer<ELEMENT_TYPE> characters{
                        Memory::eUninitialized, Characters::UTFConvert::ComputeTargetBufferSize<ELEMENT_TYPE, BASIC_ISTREAM_ELEMENT_TYPE> (n)};
                    span<ELEMENT_TYPE> converted =
                        Characters::UTFConvert::kThe.ConvertSpan (span<const BASIC_ISTREAM_ELEMENT_TYPE>{units.data (), n}, span{characters});
                    Assert (converted.size () <= intoBuffer.size ());
                    std::copy (converted.begin (), converted.end (), intoBuffer.begin ());
                    fOffset_ += converted.size ();
                    return intoBuffer.subspan (0, converted.size ());
                }
                else {
                    return intoBuffer.subspan (
                        0, ReadElements_ (span{reinterpret_cast<BASIC_ISTREAM_ELEMENT_TYPE*> (intoBuffer.data ()), intoBuffer.size ()}));
                }
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if constexpr (kTranscodes) {
                    return fOffset_;
                }
                else {
                    // instead of tellg () - avoids issue with EOF where fail bit set???
                    return fOriginalStreamRef_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
                }
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                Require (IsSeekable ());
                fOriginalStreamRef_.clear (); // in case we hit eof (causing fail) - eof gets cleared by seeking, but not failbit - it appears...--LGP 2024-02-11
                switch (whence) {
                    case eFromStart:
                        fOriginalStreamRef_.seekg (offset, ios::beg);
                        break;
                    case eFromCurrent:
                        fOriginalStreamRef_.seekg (offset, ios::cur);
                        break;
                    case eFromEnd:
                        fOriginalStreamRef_.seekg (offset, ios::end);
                        break;
                }
                // instead of tellg () - avoids issue with EOF where fail bit set???
                return fOriginalStreamRef_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
            }

        private:
            // Up to into.size () of the istream's elements, blocking: how many were read
            size_t ReadElements_ (span<BASIC_ISTREAM_ELEMENT_TYPE> into)
            {
                fOriginalStreamRef_.clear (); // clear any failures before read - it will report if a failure found
                fOriginalStreamRef_.read (into.data (), into.size ());
                size_t n = static_cast<size_t> (fOriginalStreamRef_.gcount ()); // cast safe cuz amount asked to read was also size_t
                Assert (n <= into.size ());
                // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
                if (not fOriginalStreamRef_.eof () and fOriginalStreamRef_.fail ()) [[unlikely]] {
                    static const Execution::Exception<runtime_error> kException_{"Failed to read from istream"sv};
                    Execution::Throw (kException_);
                }
                return n;
            }

        private:
            basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& fOriginalStreamRef_;
            SeekableFlag                                                          fSeekable_;
            SeekOffsetType fOffset_{0}; // Characters read - kTranscodes only, where the istream's offsets count its elements
            Stroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ***************** InputStreamFromStdIStream<ELEMENT_TYPE>::New *****************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    inline auto New (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>))
    {
        return New<ELEMENT_TYPE> (originalStream, eSeekable);
    }
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    inline auto New (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>))
    {
        using Rep = Private_::Rep_<ELEMENT_TYPE, BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>;
        if constexpr (Rep::kTranscodes) {
            Ptr<ELEMENT_TYPE> unseekable{Memory::MakeSharedPtr<Rep> (originalStream, eNotSeekable)};
            if (seekable == eSeekable) {
                return BufferedInputStream::New<ELEMENT_TYPE> (unseekable, eSeekable);
            }
            return unseekable;
        }
        else {
            return Ptr<ELEMENT_TYPE>{Memory::MakeSharedPtr<Rep> (originalStream, seekable)};
        }
    }
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    inline auto New (Execution::InternallySynchronized                                     internallySynchronized,
                     basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>))
    {
        return New<ELEMENT_TYPE> (internallySynchronized, originalStream, eSeekable);
    }
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized,
                     basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>))
    {
        using Rep = Private_::Rep_<ELEMENT_TYPE, BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>;
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                if constexpr (Rep::kTranscodes) {
                    if (seekable == eSeekable) {
                        return BufferedInputStream::New<ELEMENT_TYPE> (Execution::eInternallySynchronized,
                                                                       New<ELEMENT_TYPE> (originalStream, eNotSeekable), eSeekable);
                    }
                }
                return InternallySynchronizedInputStream::New<Rep> ({}, originalStream, seekable);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (originalStream, seekable);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}
