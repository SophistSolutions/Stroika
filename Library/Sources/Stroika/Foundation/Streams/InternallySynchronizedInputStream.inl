/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputStream_inl_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Memory/BlockAllocated.h"

namespace Stroika::Foundation::Streams::InternallySynchronizedInputStream {

    namespace Private_ {
        template <typename BASE_REP_TYPE>
        struct Rep_ final : Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE>, BASE_REP_TYPE> {
        public:
            using ElementType = typename BASE_REP_TYPE::ElementType;
            template <typename... ARGS>
            Rep_ (ARGS&&... args)
                : Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE>, BASE_REP_TYPE>{forward<ARGS> (args)...}
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
                Require (IsOpenRead ());
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

        private:
            mutable mutex fCriticalSection_;
        };
    }

    /*
     ********************************************************************************
     **************** InternallySynchronizedInputStream<BASE_REP_TYPE> **************
     ********************************************************************************
     */
    template <typename BASE_REP_TYPE, typename... ARGS>
    inline typename InputStream::Ptr<typename BASE_REP_TYPE::ElementType> New (ARGS&&... args)
    {
        return typename InputStream::Ptr<typename BASE_REP_TYPE::ElementType>{make_shared<Private_::Rep_<BASE_REP_TYPE>> (forward<ARGS> (args)...)};
    }
}

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputStream_inl_*/
