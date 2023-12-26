/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_inl_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Memory/BlockAllocated.h"

namespace Stroika::Foundation::Streams::InternallySynchronizedInputOutputStream {

    namespace Private_ {
        template <typename BASE_REP_TYPE, typename OPTIONS>
        struct Rep_ final : Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE, OPTIONS>, BASE_REP_TYPE> {
            using inherited   = Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE, OPTIONS>, BASE_REP_TYPE>;
            using ElementType = typename BASE_REP_TYPE::ElementType;
            template <typename... ARGS>
            Rep_ (OPTIONS o, ARGS&&... args)
                : inherited{forward<ARGS> (args)...}
                , fOptions_{o}
            {
            }
            Rep_ (const Rep_&) = delete;
            virtual bool IsSeekable () const override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                return BASE_REP_TYPE::IsSeekable ();
            }
            virtual void CloseRead () override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                BASE_REP_TYPE::CloseRead ();
            }
            virtual bool IsOpenRead () const override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                return BASE_REP_TYPE::IsOpenRead ();
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::GetReadOffset ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::SeekRead (whence, offset);
            }
            virtual size_t Read (ElementType* intoStart, ElementType* intoEnd) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::Read (intoStart, intoEnd);
            }
            virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return BASE_REP_TYPE::ReadNonBlocking (intoStart, intoEnd);
            }
            virtual void CloseWrite () override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                BASE_REP_TYPE::CloseWrite ();
            }
            virtual bool IsOpenWrite () const override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                return BASE_REP_TYPE::IsOpenWrite ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenWrite ());
                return BASE_REP_TYPE::GetWriteOffset ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenWrite ());
                return BASE_REP_TYPE::SeekWrite (whence, offset);
            }
            virtual void Write (const ElementType* start, const ElementType* end) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenWrite ());
                BASE_REP_TYPE::Write (start, end);
            }
            virtual void Flush () override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenWrite ());
                BASE_REP_TYPE::Flush ();
            }

        private:
            [[no_unique_address]] OPTIONS       fOptions_;
            mutable typename OPTIONS::MutexType fCriticalSection_;
        };
        template <typename ELEMENT_TYPE, typename OPTIONS>
        struct Rep2_ : InputOutputStream::IRep<ELEMENT_TYPE> {
            using ElementType = ELEMENT_TYPE;
            Rep2_ ([[maybe_unused]] const OPTIONS& o, const InputOutputStream::Ptr<ELEMENT_TYPE>& stream2Wrap)
                : fStream2Wrap{stream2Wrap}
            {
            }
            virtual bool IsSeekable () const override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                return fStream2Wrap.IsSeekable ();
            }
            virtual void CloseRead () override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                fStream2Wrap.CloseRead ();
            }
            virtual bool IsOpenRead () const override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                return fStream2Wrap.IsOpenRead ();
            }
            virtual SeekOffsetType GetReadOffset () const override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.GetReadOffset ();
            }
            virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.SeekRead (whence, offset);
            }
            virtual size_t Read (ElementType* intoStart, ElementType* intoEnd) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.Read (intoStart, intoEnd);
            }
            virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
            {
                [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
                Require (IsOpenRead ());
                return fStream2Wrap.ReadNonBlocking (intoStart, intoEnd);
            }

        private:
            InputOutputStream::Ptr<ELEMENT_TYPE> fStream2Wrap;
            mutable typename OPTIONS::MutexType  fCriticalSection_;
        };
    }

    /*
     ********************************************************************************
     ************* InternallySynchronizedInputOutputStream<BASE_REP_TYPE> ***********
     ********************************************************************************
     */
    template <typename BASE_REP_TYPE, typename OPTIONS, typename... ARGS>
    inline auto New (const OPTIONS& o, ARGS&&... args) -> typename InputOutputStream::Ptr<typename BASE_REP_TYPE::ElementType>
    {
        return typename InputOutputStream::Ptr<typename BASE_REP_TYPE::ElementType>{
            make_shared<Private_::Rep_<BASE_REP_TYPE, OPTIONS>> (o, forward<ARGS> (args)...)};
    }
    template <typename ELEMENT_TYPE, typename OPTIONS>
    inline typename InputOutputStream::Ptr<ELEMENT_TYPE> New (const OPTIONS& o, const InputOutputStream::Ptr<ELEMENT_TYPE>& stream2Wrap)
    {
        return typename InputOutputStream::Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep2_<ELEMENT_TYPE, OPTIONS>> (o, stream2Wrap)};
    }

}

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_inl_*/
