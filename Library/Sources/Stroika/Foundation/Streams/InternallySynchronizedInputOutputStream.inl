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
        template <typename BASE_REP_TYPE>
        class Rep_ final : public BASE_REP_TYPE, public Memory::UseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE>> {
        public:
            using ElementType = typename BASE_REP_TYPE::ElementType;
            template <typename... ARGS>
            Rep_ (ARGS&&... args)
                : BASE_REP_TYPE{forward<ARGS> (args)...}
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
            mutable mutex fCriticalSection_;
        };
    }

    /*
     ********************************************************************************
     ************* InternallySynchronizedInputOutputStream<BASE_REP_TYPE> ***********
     ********************************************************************************
     */
    template <typename BASE_REP_TYPE, typename... ARGS>
    inline auto New (ARGS&&... args) -> typename InputOutputStream<typename BASE_REP_TYPE::ElementType>::Ptr
    {
        return typename InputOutputStream<typename BASE_REP_TYPE::ElementType>::Ptr{make_shared<Private_::Rep_<BASE_REP_TYPE>> (forward<ARGS> (args)...)};
    }

}

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputOutputStream_inl_*/
