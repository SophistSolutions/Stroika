/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "InternallySynchronizedInputOutputStream.h"

namespace Stroika::Foundation::Streams::LoggingInputOutputStream {

    namespace Private_ {

        template <typename ELEMENT_TYPE>
        class Rep_ : public InputOutputStream::IRep<ELEMENT_TYPE> {
        public:
            Rep_ (const typename InputOutputStream::Ptr<ELEMENT_TYPE>& realStream, const typename OutputStream::Ptr<ELEMENT_TYPE>& logInput,
                  const typename OutputStream::Ptr<ELEMENT_TYPE>& logOutput)
                : fRealStream_{realStream}
                , fLogInput_{logInput}
                , fLogOutput_{logOutput}
            {
                Require (not realStream.IsSeekable () or (logInput.IsSeekable () and logOutput.IsSeekable ())); // since may need to delegate seeks
            }

            // Streams::_IRep<ELEMENT_TYPE>
        public:
            virtual bool IsSeekable () const override
            {
                return fRealStream_.IsSeekable ();
            }

            // InputStream::IRep
        public:
            virtual void CloseRead () override
            {
                fRealStream_.CloseRead ();
                fLogInput_.Close ();
            }
            virtual bool IsOpenRead () const override
            {
                return fRealStream_.IsOpenRead ();
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                return fRealStream_.GetReadOffset ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                SeekOffsetType result = fRealStream_.SeekRead (whence, offset);
                // @todo - perhaps should seek the fLogInput_ stream? But not clear by how much
                // DEFINITELY should or read logging will produce weird ansers - and least write SOME SORT OF NOTE that a seek happened! (like seeked from pos X to Y in stream)
                return result;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                return fRealStream_.AvailableToRead ();
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                optional<span<ELEMENT_TYPE>> result = fRealStream_.Read (intoBuffer, blockFlag);
                if (result) {
                    fLogInput_.Write (*result);
                }
                return result;
            }

            // OutputStream::IRep
        public:
            virtual void CloseWrite () override
            {
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fRealStream_.CloseWrite ();
                Assert (fRealStream_ == nullptr);
            }
            virtual bool IsOpenWrite () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fRealStream_.IsOpenWrite ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                return fRealStream_.GetWriteOffset ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                SeekOffsetType                                         o1 = fRealStream_.SeekWrite (whence, offset);
                [[maybe_unused]] SeekOffsetType o2 = fLogOutput_.Seek (whence, offset); // @todo - not sure if/how mcuh to see - since not totally in sync
                return o1;
            }
            virtual void Flush () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenWrite ());
                fRealStream_.Flush ();
            }
            // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
            // Writes always succeed fully or throw.
            virtual void Write (span<const ELEMENT_TYPE> elts) override
            {
                Require (not elts.empty ()); // for OutputStream<byte> - this function requires non-empty write
                Require (IsOpenWrite ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                fRealStream_.Write (elts);
                fLogOutput_.Write (elts);
            }

        private:
            typename InputOutputStream::Ptr<ELEMENT_TYPE>                  fRealStream_;
            typename OutputStream::Ptr<ELEMENT_TYPE>                       fLogInput_;
            typename OutputStream::Ptr<ELEMENT_TYPE>                       fLogOutput_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     **************** Streams::LoggingInputOutputStream::Rep_ ***********************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE> New (const typename InputOutputStream::Ptr<ELEMENT_TYPE>& realStream,
                                  const typename OutputStream::Ptr<ELEMENT_TYPE>& logInput, const typename OutputStream::Ptr<ELEMENT_TYPE>& logOutput)
    {
        return _mkPtr (make_shared<Private_::Rep_> (realStream, logInput, logOutput));
    }
    template <typename ELEMENT_TYPE>
    inline Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename InputOutputStream::Ptr<ELEMENT_TYPE>& realStream,
                                  const typename OutputStream::Ptr<ELEMENT_TYPE>& logInput, const typename OutputStream::Ptr<ELEMENT_TYPE>& logOutput)
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedInputOutputStream::New<Private_::Rep_> ({}, realStream, logInput, logOutput);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (realStream, logInput, logOutput);
            default:
                RequireNotReached ();
                return nullptr;
        }
    }

}
