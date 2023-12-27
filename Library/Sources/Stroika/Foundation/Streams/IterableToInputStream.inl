/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_IterableToInputStream_inl_
#define _Stroika_Foundation_Streams_IterableToInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedMutex.h"

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
            virtual size_t Read (span<ELEMENT_TYPE> intoBuffer) override
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
                return outI - intoBuffer.data ();
            }
            virtual optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
            {
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                Require (IsOpenRead ());
                if (intoStart == nullptr) {
                    // Don't read (so don't update fOffset_) - just see how much available
                    Iterator<ELEMENT_TYPE> srcIt = fSrcIter_;
                    size_t                 cnt{};
                    if (fPutBack_) {
                        ++cnt;
                    }
                    for (; srcIt != fSource_.end (); ++srcIt, ++cnt)
                        ;
                    return cnt;
                }
                else {
                    return Read (span{intoStart, intoEnd}); // safe because implementation of Read () in this type of stream doesn't block
                }
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
                AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                size_t                                          sourceLen = fSource_.size ();
                SeekOffsetType                                  newOffset{};
                switch (whence) {
                    case Whence::eFromStart: {
                        if (offset < 0) [[unlikely]] {
                            Execution::Throw (range_error{"seek"});
                        }
                        if (static_cast<SeekOffsetType> (offset) > sourceLen) [[unlikely]] {
                            Execution::Throw (range_error{"seek"});
                        }
                        newOffset = static_cast<SeekOffsetType> (offset);
                    } break;
                    case Whence::eFromCurrent: {
                        Streams::SignedSeekOffsetType tmpOffset = fOffset_ + offset;
                        if (tmpOffset < 0) [[unlikely]] {
                            Execution::Throw (range_error{"seek"});
                        }
                        if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen) [[unlikely]] {
                            Execution::Throw (range_error{"seek"});
                        }
                        newOffset = static_cast<SeekOffsetType> (tmpOffset);
                    } break;
                    case Whence::eFromEnd: {
                        Streams::SignedSeekOffsetType tmpOffset = fSource_.size () + offset;
                        if (tmpOffset < 0) [[unlikely]] {
                            Execution::Throw (range_error{"seek"});
                        }
                        if (static_cast<SeekOffsetType> (tmpOffset) > sourceLen) [[unlikely]] {
                            Execution::Throw (range_error{"seek"});
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
#endif /*_Stroika_Foundation_Streams_IterableToInputStream_inl_*/
