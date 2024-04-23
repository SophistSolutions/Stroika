/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

namespace Stroika::Foundation::Streams::IterableToInputStream {

    /*
     ********************************************************************************
     **************** Streams::IterableToInputStream::New ***************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const Traversal::Iterable<ELEMENT_TYPE>& it) -> Ptr<ELEMENT_TYPE>
    {
        using Debug::AssertExternallySynchronizedMutex;
        using Traversal::Iterable;
        using Traversal::Iterator;
        // Simply iterate over the 'iterable' of characacters, but allow seekability (by saving original iteration start)
        class IterableAdapterStreamRep_ final : public InputStream::IRep<ELEMENT_TYPE> {
        public:
            IterableAdapterStreamRep_ (const Traversal::Iterable<ELEMENT_TYPE>& src)
                : fSource_{src}
                , fSrcIter_{fSource_.begin ()}
            {
            }

        private:
            bool fIsOpen_{true};

        protected:
            virtual bool IsSeekable () const override
            {
                return true;
            }
            virtual void CloseRead () override
            {
                Require (IsOpenRead ());
                fIsOpen_ = false;
            }
            virtual bool IsOpenRead () const override
            {
                return fIsOpen_;
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Require (IsOpenRead ());
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                // usually just want to know 0 or >= 1, so don't bother computing full length
                return fSrcIter_.Done () ? 0 : 1;
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                AssertNotImplemented ();
                return nullopt; // pretty easy, but @todo
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, [[maybe_unused]] NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                ELEMENT_TYPE* outI = intoBuffer.data ();
                if (fPutBack_) {
                    *outI     = *fPutBack_;
                    fPutBack_ = nullopt;
                    ++outI;
                    // fOffset_ doesn't take into account putback
                }
                for (; fSrcIter_ != fSource_.end () and outI != intoBuffer.data () + intoBuffer.size (); ++fSrcIter_, ++outI) {
                    *outI = *fSrcIter_;
                    ++fOffset_;
                }
                if (outI > intoBuffer.data ()) {
                    fPrevCharCached_ = *(outI - 1);
                }
                else {
                    fPrevCharCached_ = nullopt;
                }
                return intoBuffer.subspan (0, outI - intoBuffer.data ());
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (fPutBack_) {
                    Assert (fOffset_ >= 1);
                    return fOffset_ - 1;
                }
                return fOffset_;
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                Require (IsOpenRead ());
                static const auto                               kException_ = range_error{"seek"};
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                size_t                                          sourceLen = fSource_.size ();
                SeekOffsetType                                  newOffset{};
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        if (static_cast<SeekOffsetType> (offset) > sourceLen) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        newOffset = static_cast<SeekOffsetType> (offset);
                    } break;
                    case Whence::eFromCurrent: {
                        Streams::SignedSeekOffsetType tmpOffset = fOffset_ + offset;
                        if (tmpOffset < 0) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        newOffset = static_cast<SeekOffsetType> (tmpOffset);
                    } break;
                    case Whence::eFromEnd: {
                        Streams::SignedSeekOffsetType tmpOffset = fSource_.size () + offset;
                        if (tmpOffset < 0) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen) [[unlikely]] {
                            Execution::Throw (kException_);
                        }
                        newOffset = static_cast<SeekOffsetType> (tmpOffset);
                    } break;
                }
                if (newOffset == fOffset_ - 1 and fPrevCharCached_) {
                    fPutBack_        = fPrevCharCached_;
                    fPrevCharCached_ = nullopt;
                    return GetReadOffset ();
                }
                else if (newOffset < fOffset_) {
                    fSrcIter_ = fSource_.begin ();
                    fOffset_  = 0;
                }
                while (fOffset_ < newOffset) {
                    if (fSrcIter_.Done ()) {
                        AssertNotReached (); // because we checked within maxlen above
                        //Execution::Throw (Execution::RuntimeErrorException {"Seek past end of input"sv}); // @todo clarify - docuemnt - not sure if/how to handle this
                    }
                    ++fSrcIter_;
                    ++fOffset_;
                }
                return fOffset_;
            }

        private:
            Iterable<ELEMENT_TYPE> fSource_;
            Iterator<ELEMENT_TYPE> fSrcIter_;
            size_t                 fOffset_{};
            optional<ELEMENT_TYPE> fPrevCharCached_{}; // fPrevCharCached_/fPutBack_ speed hack to support IsAtEOF (), and Peek () more efficiently, little cost, big cost avoidance for seek
            optional<ELEMENT_TYPE>                                         fPutBack_{};
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
        return Ptr<ELEMENT_TYPE>{make_shared<IterableAdapterStreamRep_> (it)};
    }

}
