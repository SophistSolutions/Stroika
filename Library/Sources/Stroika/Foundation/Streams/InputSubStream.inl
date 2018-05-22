/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputSubStream_inl_
#define _Stroika_Foundation_Streams_InputSubStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Memory/SmallStackBuffer.h"

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             ************************* Streams::InputSubStream::Rep_ ************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            class InputSubStream<ELEMENT_TYPE>::Rep_ : public InputStream<ELEMENT_TYPE>::_IRep, private Debug::AssertExternallySynchronizedLock {
            public:
                Rep_ (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn, const Optional<SeekOffsetType>& start, const Optional<SeekOffsetType>& end)
                    : InputStream<ELEMENT_TYPE>::_IRep ()
                    , fRealIn_ (realIn)
                    , fOffsetMine2Real_ (start.Value (realIn.GetSeekOffset ()))
                    , fForcedEndInReal_ (end)
                {
                    if (start) {
                        // seek up to zero point, to begin with, avoiding ambiguity about when this gets done (could be done lazily as well, but I think behavior less clear then)
                        if (fRealIn_.IsSeekable ()) {
                            fRealIn_.SeekRead (Whence::eFromStart, start);
                        }
                        else {
                            // else read to advance, and silently ignore if already past start. That might make sense? and at any rate is well defined
                            SeekOffsetType realSeekOffset = realIn.GetSeekOffset ();
                            if (realSeekOffset < start) {
                                Memory::SmallStackBuffer<ELEMENT_TYPE> buf{start - realSeekOffset};
                                (void)realIn.ReadAll (buf.begin (), buf.end ()); // read exactly that many elements, and drop them on the floor
                            }
                        }
                    }
                }
                virtual bool IsSeekable () const override
                {
                    return fRealIn_.IsSeekable ();
                }
                virtual void CloseRead () override
                {
                    Require (IsOpenRead ());
                    fRealIn_.Close ();
                    Assert (fRealIn_ == nullptr);
                }
                virtual bool IsOpenRead () const override
                {
                    return fRealIn_ != nullptr;
                }
                virtual SeekOffsetType GetReadOffset () const override
                {
                    RequireNotReached ();
                    Require (IsOpenRead ());
                    SeekOffsetType realOffset = fRealIn_.GetReadOffset ();
                    ValidateRealOffset_ (realOffset);
                    return realOffset - fOffsetMine2Real_;
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    RequireNotReached ();
                    Require (IsOpenRead ());
                    if (fForcedEndInReal_) {
                        SignedSeekOffsetType effectiveRealTarget;
                        switch (whence) {
                            case Whence::eFromCurrent:
                                effectiveRealTarget = fRealIn_.GetReadOffset () + offset;
                                break;
                            case Whence::eFromStart:
                                effectiveRealTarget = fOffsetMine2Real_ + offset;
                                break;
                            case Whence::eFromEnd:
                                if (fForcedEndInReal_) {
                                    // Don't go to REAL end - that's likely out of range, and not what caller meant
                                    effectiveRealTarget = *fForcedEndInReal_;
                                }
                                else {
                                    effectiveRealTarget = fRealIn_.GetReadOffset () + fRealIn_.GetOffsetToEndOfStream () + offset;
                                }
                                break;
                        }
                        ValidateRealOffset_ (effectiveRealTarget);
                        SignedSeekOffsetType result = fRealIn_.SeekRead (Whence::eFromStart, effectiveRealTarget);
                        ValidateRealOffset_ (result);
                        return result - fOffsetMine2Real_;
                    }
                    else {
                        return fRealIn_.SeekRead (whence, offset + fOffsetMine2Real_) - fOffsetMine2Real_;
                    }
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    Require (intoEnd - intoStart >= 1); // rule for InputStream<>::_IRep
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    Require (IsOpenRead ());
                    SignedSeekOffsetType realOffset = fRealIn_.GetReadOffset ();
                    if (fForcedEndInReal_) {
                        // adjust intoEnd to accomodate shortened stream
                        SeekOffsetType curReal    = fRealIn_.GetReadOffset ();
                        SeekOffsetType maxNewReal = curReal + (intoEnd - intoStart);
                        if (maxNewReal > *fForcedEndInReal_) {
                            if (curReal == *fForcedEndInReal_) {
                                return 0; // EOF
                            }
                            else {
                                ELEMENT_TYPE* newIntoEnd{intoStart + *fForcedEndInReal_};
                                Assert (newIntoEnd < intoEnd);
                                return fRealIn_.Read (intoStart, newIntoEnd);
                            }
                        }
                    }
                    return fRealIn_.Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    Require (IsOpenRead ());
                    if (fForcedEndInReal_) {
                        // adjust intoEnd to accomodate shortened stream
                        SeekOffsetType curReal    = fRealIn_.GetReadOffset ();
                        SeekOffsetType maxNewReal = curReal + (intoEnd - intoStart);
                        if (maxNewReal > *fForcedEndInReal_) {
                            if (curReal == *fForcedEndInReal_) {
                                return 0; // EOF
                            }
                            else {
                                ELEMENT_TYPE* newIntoEnd{intoStart + *fForcedEndInReal_};
                                Assert (newIntoEnd < intoEnd);
                                return fRealIn_.ReadNonBlocking (intoStart, newIntoEnd);
                            }
                        }
                    }
                    return fRealIn_.ReadNonBlocking (intoStart, intoEnd);
                }

            private:
                nonvirtual void ValidateRealOffset_ (SignedSeekOffsetType offset)
                {
                    if (offset < fOffsetMine2Real_) {
                        Execution::Throw (std::range_error ("offset before beginning"));
                    }
                    if (fForcedEndInReal_) {
                        if (offset > *fForcedEndInReal_) {
                            Execution::Throw (EOFException::kThe);
                        }
                    }
                }

            private:
                typename InputStream<ELEMENT_TYPE>::Ptr fRealIn_;
                SeekOffsetType                          fOffsetMine2Real_; // subtract from real offset to get our offset
                Optional<SeekOffsetType>                fForcedEndInReal_;
            };

            /*
             ********************************************************************************
             ************************** Streams::InputSubStream *****************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline auto InputSubStream<ELEMENT_TYPE>::New (const typename InputStream<ELEMENT_TYPE>::Ptr& realIn, const Optional<SeekOffsetType>& start, const Optional<SeekOffsetType>& end) -> Ptr
            {
                return make_shared<Rep_> (realIn, start, end);
            }

            /*
             ********************************************************************************
             ********************** InputSubStream<ELEMENT_TYPE>::Ptr ***********************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline InputSubStream<ELEMENT_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InputSubStream_inl_*/
