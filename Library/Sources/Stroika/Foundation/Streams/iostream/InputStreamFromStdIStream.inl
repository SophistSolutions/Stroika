/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"

namespace Stroika::Foundation::Streams::iostream::InputStreamFromStdIStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
        class Rep_ : public InputStream::IRep<ELEMENT_TYPE> {
        private:
            bool fOpen_{true};

        public:
            Rep_ (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream)
                : Rep_{originalStream, eSeekable}
            {
            }
            Rep_ (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable)
                : fOriginalStreamRef_{originalStream}
                , fSeekable_{seekable}
            {
            }

        protected:
            virtual bool IsSeekable () const override
            {
                return fSeekable_ == eSeekable;
            }
            virtual void CloseRead () override
            {
                Require (IsOpenRead ());
                fOpen_ = false;
                Ensure (not IsOpenRead ());
            }
            virtual bool IsOpenRead () const override
            {
                return fOpen_;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                streamsize                                             sz = fOriginalStreamRef_.rdbuf ()->in_avail ();
                // http://en.cppreference.com/w/cpp/io/basic_streambuf/in_avail
                if (sz == 0) {
                    return nullopt;
                }
                else if (sz == -1) {
                    return 0;
                }
                return static_cast<size_t> (sz);
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                AssertNotImplemented ();
                return nullopt; // pretty easy, but @todo
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (fOriginalStreamRef_.eof ()) {
                    return span<ELEMENT_TYPE>{};
                }
                if (blockFlag == NoDataAvailableHandling::eDontBlock and AvailableToRead () == nullopt) {
                    return nullopt;
                }
                size_t maxToRead = intoBuffer.size ();
                fOriginalStreamRef_.clear (); // clear any failures before read - it will report if a failure found
                fOriginalStreamRef_.read (reinterpret_cast<BASIC_ISTREAM_ELEMENT_TYPE*> (intoBuffer.data ()), maxToRead);
                size_t n = static_cast<size_t> (fOriginalStreamRef_.gcount ()); // cast safe cuz amount asked to read was also size_t
                Assert (n <= maxToRead);
                // apparently based on http://www.cplusplus.com/reference/iostream/istream/read/ EOF sets the EOF bit AND the fail bit
                if (not fOriginalStreamRef_.eof () and fOriginalStreamRef_.fail ()) [[unlikely]] {
                    static const Execution::RuntimeErrorException kException_{"Failed to read from istream"sv};
                    Execution::Throw (kException_);
                }
                return intoBuffer.subspan (0, n);
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                // instead of tellg () - avoids issue with EOF where fail bit set???
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                return fOriginalStreamRef_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                fOriginalStreamRef_.clear (); // in case we hit eof (causing fail) - eof gets cleared by seeking, but not failbit - it appears...--LGP 2024-02-11
                switch (whence) {
                    case Whence::eFromStart:
                        fOriginalStreamRef_.seekg (offset, ios::beg);
                        break;
                    case Whence::eFromCurrent:
                        fOriginalStreamRef_.seekg (offset, ios::cur);
                        break;
                    case Whence::eFromEnd:
                        fOriginalStreamRef_.seekg (offset, ios::end);
                        break;
                }
                // instead of tellg () - avoids issue with EOF where fail bit set???
                return fOriginalStreamRef_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::in);
            }

        private:
            basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& fOriginalStreamRef_;
            SeekableFlag                                                          fSeekable_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex        fThisAssertExternallySynchronized_;
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
        return Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep_<ELEMENT_TYPE, BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>> (originalStream)};
    }
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    inline auto New (basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>))
    {
        return Ptr<ELEMENT_TYPE>{
            make_shared<Private_::Rep_<ELEMENT_TYPE, BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>> (originalStream, seekable)};
    }
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    inline auto New (Execution::InternallySynchronized                                     internallySynchronized,
                     basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>))
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedInputStream::New<Private_::Rep_<ELEMENT_TYPE, BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>> (
                    {}, originalStream);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (originalStream);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }
    template <typename ELEMENT_TYPE, typename BASIC_ISTREAM_ELEMENT_TYPE, typename BASIC_ISTREAM_TRAITS_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized,
                     basic_istream<BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>& originalStream, SeekableFlag seekable) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_ISTREAM_ELEMENT_TYPE, wchar_t>))
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedInputStream::New<Private_::Rep_<ELEMENT_TYPE, BASIC_ISTREAM_ELEMENT_TYPE, BASIC_ISTREAM_TRAITS_TYPE>> (
                    {}, originalStream);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (originalStream, seekable);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}
