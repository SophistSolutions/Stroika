/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_inl_
#define _Stroika_Foundation_Streams_InternallySynchronizedOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Memory/BlockAllocated.h"

namespace Stroika::Foundation::Streams::InternallySynchronizedOutputStream {

    namespace Private_ {
        template <typename BASE_REP_TYPE>
        class Rep_ final : public BASE_REP_TYPE, public Memory::UseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE>> {
            using ElementType = typename BASE_REP_TYPE::ElementType;

        public:
            template <typename... ARGS>
            Rep_ (ARGS&&... args)
                : BASE_REP_TYPE{forward<ARGS> (args)...}
            {
            }
            Rep_ (const Rep_&) = delete;
            virtual bool IsSeekable () const override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                return BASE_REP_TYPE::IsSeekable ();
            }
            virtual void CloseWrite () override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                BASE_REP_TYPE::CloseWrite ();
            }
            virtual bool IsOpenWrite () const override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                return BASE_REP_TYPE::IsOpenWrite ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                return BASE_REP_TYPE::GetWriteOffset ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                return BASE_REP_TYPE::SeekWrite (whence, offset);
            }
            virtual void Write (const ElementType* start, const ElementType* end) override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                BASE_REP_TYPE::Write (start, end);
            }
            virtual void Flush () override
            {
                lock_guard<mutex> critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                BASE_REP_TYPE::Flush ();
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
    inline typename OutputStream::Ptr<typename BASE_REP_TYPE::ElementType> New (ARGS&&... args)
    {
        return typename OutputStream::Ptr<typename BASE_REP_TYPE::ElementType>{
            Memory::MakeSharedPtr<Private_::Rep_<BASE_REP_TYPE>> (forward<ARGS> (args)...)};
    }

}

#endif /*_Stroika_Foundation_Streams_InternallySynchronizedOutputStream_inl_*/
