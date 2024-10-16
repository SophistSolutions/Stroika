/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Streams::InternallySynchronizedInputStream {

    namespace Private_ {
        template <typename BASE_REP_TYPE, typename OPTIONS>
        struct Rep_ final : Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE, OPTIONS>, BASE_REP_TYPE> {
            using ElementType = typename BASE_REP_TYPE::ElementType;
            using inherited   = Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE, OPTIONS>, BASE_REP_TYPE>;
            template <typename... ARGS>
            Rep_ ([[maybe_unused]] const OPTIONS& o, ARGS&&... args)
                : inherited{forward<ARGS> (args)...}
            {
            }
            Rep_ (const Rep_&) = delete;
            virtual bool IsSeekable () const override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                return BASE_REP_TYPE::IsSeekable ();
            }
            virtual void CloseRead () override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                BASE_REP_TYPE::CloseRead ();
            }
            virtual bool IsOpenRead () const override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                return BASE_REP_TYPE::IsOpenRead ();
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::GetReadOffset ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::SeekRead (whence, offset);
            }
            virtual optional<size_t> AvailableToRead () override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::AvailableToRead ();
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::RemainingLength ();
            }
            virtual optional<span<ElementType>> Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::Read (intoBuffer, blockFlag);
            }

        private:
            mutable typename OPTIONS::MutexType fCriticalSection_;
        };
        template <typename ELEMENT_TYPE, typename OPTIONS>
        struct Rep2_ : InputStream::IRep<ELEMENT_TYPE> {
            using ElementType = ELEMENT_TYPE;
            Rep2_ ([[maybe_unused]] const OPTIONS& o, const InputStream::Ptr<ELEMENT_TYPE>& stream2Wrap)
                : fStream2Wrap{stream2Wrap}
            {
            }
            virtual bool IsSeekable () const override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                return fStream2Wrap.IsSeekable ();
            }
            virtual void CloseRead () override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                fStream2Wrap.CloseRead ();
            }
            virtual bool IsOpenRead () const override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                return fStream2Wrap.IsOpenRead ();
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.GetReadOffset ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.SeekRead (whence, offset);
            }
            virtual optional<SeekOffsetType> RemainingLength () override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.RemainingLength ();
            }
            virtual optional<size_t> AvailableToRead () override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.AvailableToRead ();
            }
            virtual span<ElementType> Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag) override
            {
                [[maybe_unused]] lock_guard critSec{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.Read (intoBuffer, blockFlag);
            }

        private:
            InputStream::Ptr<ELEMENT_TYPE>      fStream2Wrap;
            mutable typename OPTIONS::MutexType fCriticalSection_;
        };
    }

    /*
     ********************************************************************************
     ************* InternallySynchronizedInputStream<BASE_REP_TYPE>::New ************
     ********************************************************************************
     */
    template <typename BASE_REP_TYPE, typename OPTIONS, typename... ARGS>
    inline Ptr<typename BASE_REP_TYPE::ElementType> New (const OPTIONS& o, ARGS&&... args)
    {
        return Ptr<typename BASE_REP_TYPE::ElementType>{make_shared<Private_::Rep_<BASE_REP_TYPE, OPTIONS>> (o, forward<ARGS> (args)...)};
    }
    template <typename ELEMENT_TYPE, typename OPTIONS>
    inline Ptr<ELEMENT_TYPE> New (const OPTIONS& o, const InputStream::Ptr<ELEMENT_TYPE>& stream2Wrap)
    {
        return Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep2_<ELEMENT_TYPE, OPTIONS>> (o, stream2Wrap)};
    }

}
