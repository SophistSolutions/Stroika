/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputSubStream_inl_
#define _Stroika_Foundation_Streams_InputSubStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Memory/StackBuffer.h"
#include "InternallySynchronizedInputStream.h"

namespace Stroika::Foundation::Streams::InputSubStream {

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_ : public InputStream::IRep<ELEMENT_TYPE> {
        public:
            Rep_ (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn, const optional<SeekOffsetType>& start, const optional<SeekOffsetType>& end)
                : fRealIn_{realIn}
                , fOffsetMine2Real_{start.value_or (realIn.GetOffset ())}
                , fForcedEndInReal_{end}
            {
                if (start) {
                    // seek up to zero point, to begin with, avoiding ambiguity about when this gets done (could be done lazily as well, but I think behavior less clear then)
                    if (fRealIn_.IsSeekable ()) {
                        fRealIn_.Seek (Whence::eFromStart, *start);
                    }
                    else {
                        // else read to advance, and silently ignore if already past start. That might make sense? and at any rate is well defined
                        SeekOffsetType realSeekOffset = realIn.GetOffset ();
                        if (realSeekOffset < *start) {
                            Assert (*start - realSeekOffset < sizeof (size_t)); // NYI crazy corner case
                            Memory::StackBuffer<ELEMENT_TYPE> buf;
                            if constexpr (is_trivially_copyable_v<ELEMENT_TYPE>) {
                                buf.resize_uninitialized (static_cast<size_t> (*start - realSeekOffset));
                            }
                            else {
                                buf.resize (static_cast<size_t> (*start - realSeekOffset));
                            }
                            (void)realIn.ReadAll (span{buf}); // read exactly that many elements, and drop them on the floor
                        }
                    }
                }
            }
            Rep_ (const Rep_&) = delete;
            Rep_ ()            = delete;
            virtual bool IsSeekable () const override
            {
                return fRealIn_.IsSeekable ();
            }
            virtual void CloseRead () override
            {
                Require (IsOpenRead ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
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
                SeekOffsetType realOffset = fRealIn_.GetOffset ();
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
                            effectiveRealTarget = fRealIn_.GetOffset () + offset;
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
                                auto getOffsetToEndOfStream = [] (typename InputStream::Ptr<ELEMENT_TYPE> in) {
                                    SeekOffsetType savedReadFrom = in.GetOffset ();
                                    SeekOffsetType size          = in.Seek (Whence::eFromEnd, 0);
                                    in.Seek (Whence::eFromStart, savedReadFrom);
                                    Assert (size >= savedReadFrom);
                                    size -= savedReadFrom;
                                    return size;
                                };
                                effectiveRealTarget = fRealIn_.GetOffset () + getOffsetToEndOfStream (fRealIn_) + offset;
                            }
                            break;
                        default:
                            effectiveRealTarget = 0; // silence warning
                            RequireNotReached ();
                    }
                    ValidateRealOffset_ (effectiveRealTarget);
                    SignedSeekOffsetType result = fRealIn_.Seek (Whence::eFromStart, effectiveRealTarget);
                    ValidateRealOffset_ (result);
                    return result - fOffsetMine2Real_;
                }
                else {
                    return fRealIn_.Seek (whence, offset + fOffsetMine2Real_) - fOffsetMine2Real_;
                }
            }
            virtual optional<size_t> AvailableToRead () override
            {
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                SeekOffsetType myOffset = fRealIn_.GetOffset ();
                if (fForcedEndInReal_ and myOffset >= *fForcedEndInReal_) { // could be past end if through another non-substream Ptr we read past
                    return 0;
                }
                // otherwise, our answer is same as answer from underlying stream (since we do no buffering)
                return fRealIn_.AvailableToRead (); // @todo nechnically maybe wrong, in may suggest we can read more than we have but not worth fix cuz can cause no problems I'm aware of
            }
            virtual optional<span<ELEMENT_TYPE>> Read (span<ELEMENT_TYPE> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                Require (not intoBuffer.empty ());
                Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
                Require (IsOpenRead ());
                if (fForcedEndInReal_) {
                    // @todo clean this code up for switch to spans - simplify!
                    // adjust intoEnd to accomodate shortened stream
                    SeekOffsetType curReal    = fRealIn_.GetOffset ();
                    SeekOffsetType maxNewReal = curReal + intoBuffer.size ();
                    if (maxNewReal > *fForcedEndInReal_) {
                        if (curReal == *fForcedEndInReal_) {
                            return span<ELEMENT_TYPE>{}; // EOF
                        }
                        else {
                            ELEMENT_TYPE* newIntoEnd{intoBuffer.data () + *fForcedEndInReal_ - curReal};
                            Assert (newIntoEnd < intoBuffer.data () + intoBuffer.size ());
                            return fRealIn_.Read (span{intoBuffer.data (), newIntoEnd}, blockFlag);
                        }
                    }
                }
                return fRealIn_.Read (intoBuffer, blockFlag);
            }

        private:
            nonvirtual void ValidateRealOffset_ (SignedSeekOffsetType offset) const
            {
                if (offset < static_cast<SignedSeekOffsetType> (fOffsetMine2Real_)) [[unlikely]] {
                    Execution::Throw (range_error ("offset before beginning"));
                }
                if (fForcedEndInReal_) {
                    if (offset > static_cast<SignedSeekOffsetType> (*fForcedEndInReal_)) [[unlikely]] {
                        Execution::Throw (EOFException::kThe);
                    }
                }
            }
            typename InputStream::Ptr<ELEMENT_TYPE>                        fRealIn_;
            SeekOffsetType                                                 fOffsetMine2Real_; // subtract from real offset to get our offset
            optional<SeekOffsetType>                                       fForcedEndInReal_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        };
    }

    /*
     ********************************************************************************
     ************************** Streams::InputSubStream *****************************
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE>
    inline auto New (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn, const optional<SeekOffsetType>& start,
                     const optional<SeekOffsetType>& end) -> Ptr<ELEMENT_TYPE>
    {
        return Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep_<ELEMENT_TYPE>> (realIn, start, end)};
    }
    template <typename ELEMENT_TYPE>
    inline auto New (Execution::InternallySynchronized internallySynchronized, const typename InputStream::Ptr<ELEMENT_TYPE>& realIn,
                     const optional<SeekOffsetType>& start, const optional<SeekOffsetType>& end) -> Ptr<ELEMENT_TYPE>
    {
        switch (internallySynchronized) {
            case Execution::eInternallySynchronized:
                return InternallySynchronizedInputStream::New<Private_::Rep_<ELEMENT_TYPE>> ({}, realIn, start, end);
            case Execution::eNotKnownInternallySynchronized:
                return New<ELEMENT_TYPE> (start, end);
        }
    }

}

#endif /*_Stroika_Foundation_Streams_InputSubStream_inl_*/
