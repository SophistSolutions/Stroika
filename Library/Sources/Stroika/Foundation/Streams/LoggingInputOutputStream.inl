/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_LoggingInputOutputStream_inl_
#define _Stroika_Foundation_Streams_LoggingInputOutputStream_inl_ 1

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             **************** Streams::LoggingInputOutputStream::Rep_ ***********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            class LoggingInputOutputStream<ELEMENT_TYPE>::Rep_ : public InputOutputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
            public:
                Rep_ (const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realStream, const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput)
                    : InputOutputStream<ELEMENT_TYPE>::_IRep ()
                    , fRealStream_ (realStream)
                    , fLogInput_ (logInput)
                    , fLogOutput_ (logOutput)
                {
                    Require (not realStream.IsSeekable () or (logInput.IsSeekable () and logOutput.IsSeekable ())); // since may need to delegate seeks
                }

                // Stream<ELEMENT_TYPE>::_IRep
            public:
                virtual bool IsSeekable () const override
                {
                    return fRealStream_.IsSeekable ();
                }

                // InputStream::_IRep
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
                    return result;
                }
                virtual size_t Read (ElementType* intoStart, ElementType* intoEnd) override
                {
                    size_t result = fRealStream_.Read (intoStart, intoEnd);
                    fLogInput_.Write (intoStart, intoStart + result);
                    return result;
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
                {
                    // note - in rep, intoStart==nullptr allowed, but not allowed in call to smart ptr public API
                    Require (((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1));
                    if (intoStart == nullptr) {
                        return fRealStream_.ReadNonBlocking ();
                    }
                    else {
                        if (Memory::Optional<size_t> result = fRealStream_.ReadNonBlocking (intoStart, intoEnd)) {
                            fLogInput_.Write (intoStart, intoStart + *result);
                            return result;
                        }
                        return {};
                    }
                }

                // OutputStream::_IRep
            public:
                virtual void CloseWrite () override
                {
                    Require (IsOpenWrite ());
                    fRealStream_.CloseWrite ();
                    Assert (fRealStream_ == nullptr);
                }
                virtual bool IsOpenWrite () const override
                {
                    return fRealStream_.IsOpenWrite ();
                }
                virtual SeekOffsetType GetWriteOffset () const override
                {
                    return fRealStream_.GetWriteOffset ();
                }
                virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                {
                    Require (IsOpenWrite ());
                    SeekOffsetType o1 = fRealStream_.SeekWrite (whence, offset);
                    SeekOffsetType o2 = fLogOutput_.Seek (whence, offset); // @todo - not sure if/how mcuh to see - since not totally in sync
                    return o1;
                }
                virtual void Flush () override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    Require (IsOpenWrite ());
                    fRealStream_.Flush ();
                }
                // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                // Writes always succeed fully or throw.
                virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
                {
                    Require (start < end); // for OutputStream<Byte> - this funciton requires non-empty write
                    Require (IsOpenWrite ());
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    fRealStream_.Write (start, end);
                    fLogOutput_.Write (start, end);
                }

            private:
                typename InputOutputStream<ELEMENT_TYPE>::Ptr fRealStream_;
                typename OutputStream<ELEMENT_TYPE>::Ptr      fLogInput_;
                typename OutputStream<ELEMENT_TYPE>::Ptr      fLogOutput_;
            };

            /*
             ********************************************************************************
             **************** Streams::LoggingInputOutputStream::Rep_ ***********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            typename LoggingInputOutputStream<ELEMENT_TYPE>::Ptr LoggingInputOutputStream<ELEMENT_TYPE>::New (const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realStream, const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput)
            {
                return _mkPtr (make_shared<Rep_> (realStream, logInput, logOutput));
            }
            template <typename ELEMENT_TYPE>
            typename LoggingInputOutputStream<ELEMENT_TYPE>::Ptr LoggingInputOutputStream<ELEMENT_TYPE>::New (Execution::InternallySyncrhonized internallySyncrhonized, const typename InputOutputStream<ELEMENT_TYPE>::Ptr& realStream, const typename OutputStream<ELEMENT_TYPE>::Ptr& logInput, const typename OutputStream<ELEMENT_TYPE>::Ptr& logOutput)
            {
                switch (internallySyncrhonized) {
                    case Execution::eInternallySynchronized:
                        return InternalSyncRep_::New (realStream, logInput, logOutput);
                    case Execution::eNotKnownInternallySynchronized:
                        return New (realStream, logInput, logOutput);
                    default:
                        RequireNotReached ();
                        return nullptr;
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_LoggingInputOutputStream_inl_*/
