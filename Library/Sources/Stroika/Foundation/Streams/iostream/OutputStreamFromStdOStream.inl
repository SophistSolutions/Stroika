/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/UTFConvert.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedOutputStream.h"

namespace Stroika::Foundation::Streams::iostream::OutputStreamFromStdOStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE, typename BASIC_OSTREAM_ELEMENT_TYPE, typename BASIC_OSTREAM_TRAITS_TYPE>
        class Rep_ : public OutputStream::IRep<ELEMENT_TYPE> {
        public:
            /**
             *  Characters to a 16-bit wchar_t - UTF-16, where a Character is one element or two: so this converts, and cannot
             *  seek, its offsets counting Characters but the ostream's counting wchar_ts
             */
            static constexpr bool kTranscodes = sizeof (ELEMENT_TYPE) != sizeof (BASIC_OSTREAM_ELEMENT_TYPE);

        private:
            bool fOpen_{true};

        public:
            Rep_ (basic_ostream<BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>& originalStream)
                : fOriginalStreamRef_{originalStream}
            {
            }

        protected:
            virtual bool IsSeekable () const override
            {
                Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return not kTranscodes;
            }
            virtual void CloseWrite () override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fOpen_ = false;
                Ensure (not IsOpenWrite ());
            }
            virtual bool IsOpenWrite () const override
            {
                Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fOpen_;
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                if constexpr (kTranscodes) {
                    return fOffset_;
                }
                else {
                    // instead of tellg () - avoids issue with EOF where fail bit set???
                    return fOriginalStreamRef_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::out);
                }
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                Require (IsSeekable ());
                switch (whence) {
                    case eFromStart:
                        fOriginalStreamRef_.seekp (offset, ios::beg);
                        break;
                    case eFromCurrent:
                        fOriginalStreamRef_.seekp (offset, ios::cur);
                        break;
                    case eFromEnd:
                        fOriginalStreamRef_.seekp (offset, ios::end);
                        break;
                }
                return fOriginalStreamRef_.tellp ();
            }
            virtual void Write (span<const ELEMENT_TYPE> elts) override
            {
                Require (not elts.empty ());
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};

                if constexpr (kTranscodes) {
                    Memory::StackBuffer<BASIC_OSTREAM_ELEMENT_TYPE> units{
                        Memory::eUninitialized, Characters::UTFConvert::ComputeTargetBufferSize<BASIC_OSTREAM_ELEMENT_TYPE> (elts)};
                    span<BASIC_OSTREAM_ELEMENT_TYPE> converted = Characters::UTFConvert::kThe.ConvertSpan (elts, span{units});
                    fOriginalStreamRef_.write (converted.data (), converted.size ());
                    fOffset_ += elts.size ();
                }
                else {
                    fOriginalStreamRef_.write (reinterpret_cast<const BASIC_OSTREAM_ELEMENT_TYPE*> (elts.data ()), elts.size ());
                }
                if (fOriginalStreamRef_.fail ()) [[unlikely]] {
                    static const Execution::Exception<runtime_error> kException_{"Failed to write from ostream"sv};
                    Execution::Throw (kException_);
                }
            }
            virtual void Flush () override
            {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                fOriginalStreamRef_.flush ();
                if (fOriginalStreamRef_.fail ()) [[unlikely]] {
                    static const Execution::Exception<runtime_error> kException_{"Failed to flush ostream"sv};
                    Execution::Throw (kException_);
                }
            }

        private:
            basic_ostream<BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>& fOriginalStreamRef_;
            SeekOffsetType fOffset_{0}; // Characters written - kTranscodes only, where the ostream's offsets count its elements
            Stroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     **************** OutputStreamFromStdOStream<ELEMENT_TYPE>::New *****************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename BASIC_OSTREAM_ELEMENT_TYPE, typename BASIC_OSTREAM_TRAITS_TYPE>
    inline auto New (basic_ostream<BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>& originalStream) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, wchar_t>))
    {
        return Ptr<ELEMENT_TYPE>{Memory::MakeSharedPtr<Private_::Rep_<ELEMENT_TYPE, BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>> (originalStream)};
    }
    template <typename ELEMENT_TYPE, typename BASIC_OSTREAM_ELEMENT_TYPE, typename BASIC_OSTREAM_TRAITS_TYPE>
    inline auto New (Execution::InternallySynchronized                                     internallySynchronized,
                     basic_ostream<BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>& originalStream) -> Ptr<ELEMENT_TYPE>
        requires ((same_as<ELEMENT_TYPE, byte> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, char>) or
                  (same_as<ELEMENT_TYPE, Characters::Character> and same_as<BASIC_OSTREAM_ELEMENT_TYPE, wchar_t>))
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedOutputStream::New<Private_::Rep_<ELEMENT_TYPE, BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>> (
                    {}, originalStream);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (originalStream);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}
