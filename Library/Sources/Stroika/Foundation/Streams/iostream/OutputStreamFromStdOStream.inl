/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedOutputStream.h"

namespace Stroika::Foundation::Streams::iostream::OutputStreamFromStdOStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE, typename BASIC_OSTREAM_ELEMENT_TYPE, typename BASIC_OSTREAM_TRAITS_TYPE>
        class Rep_ : public OutputStream::IRep<ELEMENT_TYPE> {
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
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return true;
            }
            virtual void CloseWrite () override
            {
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fOpen_ = false;
                Ensure (not IsOpenWrite ());
            }
            virtual bool IsOpenWrite () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fOpen_;
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                // instead of tellg () - avoids issue with EOF where fail bit set???
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                return fOriginalStreamRef_.rdbuf ()->pubseekoff (0, ios_base::cur, ios_base::out);
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                switch (whence) {
                    case Whence::eFromStart:
                        fOriginalStreamRef_.seekp (offset, ios::beg);
                        break;
                    case Whence::eFromCurrent:
                        fOriginalStreamRef_.seekp (offset, ios::cur);
                        break;
                    case Whence::eFromEnd:
                        fOriginalStreamRef_.seekp (offset, ios::end);
                        break;
                }
                return fOriginalStreamRef_.tellp ();
            }
            virtual void Write (span<const ELEMENT_TYPE> elts) override
            {
                Require (not elts.empty ());
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};

                using StreamElementType = BASIC_OSTREAM_ELEMENT_TYPE;
                fOriginalStreamRef_.write (reinterpret_cast<const StreamElementType*> (elts.data ()), elts.size ());
                if (fOriginalStreamRef_.fail ()) [[unlikely]] {
                    static const Execution::RuntimeErrorException kException_{"Failed to write from ostream"sv};
                    Execution::Throw (kException_);
                }
            }
            virtual void Flush () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                fOriginalStreamRef_.flush ();
                if (fOriginalStreamRef_.fail ()) [[unlikely]] {
                    static const Execution::RuntimeErrorException kException_{"Failed to flush ostream"sv};
                    Execution::Throw (kException_);
                }
            }

        private:
            basic_ostream<BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>& fOriginalStreamRef_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex        fThisAssertExternallySynchronized_;
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
        return Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep_<ELEMENT_TYPE, BASIC_OSTREAM_ELEMENT_TYPE, BASIC_OSTREAM_TRAITS_TYPE>> (originalStream)};
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
