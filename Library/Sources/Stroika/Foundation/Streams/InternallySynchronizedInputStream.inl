/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedInputStream_inl_
#define _Stroika_Foundation_Streams_InternallySynchronizedInputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Streams {

    /*
     ********************************************************************************
     * InternallySynchronizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename BASE_CLASS, typename BASE_REP_TYPE>
    class InternallySynchronizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_ final : public BASE_REP_TYPE {
    public:
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
        virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
        {
            [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
            Require (IsOpenRead ());
            return BASE_REP_TYPE::Read (intoStart, intoEnd);
        }
        virtual optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
        {
            [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
            Require (IsOpenRead ());
            return BASE_REP_TYPE::ReadNonBlocking (intoStart, intoEnd);
        }

    private:
        mutable mutex fCriticalSection_;
    };

    /*
     ********************************************************************************
     ** InternallySynchronizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE> **
     ********************************************************************************
     */
    template <typename ELEMENT_TYPE, typename BASE_CLASS, typename BASE_REP_TYPE>
    template <typename... ARGS>
    inline auto InternallySynchronizedInputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::New (ARGS&&... args) -> Ptr
    {
        return inherited::_mkPtr (make_shared<Rep_> (forward<ARGS> (args)...));
    }

}

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedInputStream_inl_*/
