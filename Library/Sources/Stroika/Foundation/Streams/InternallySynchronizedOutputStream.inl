/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../Memory/BlockAllocated.h"

namespace Stroika::Foundation::Streams::InternallySynchronizedOutputStream {

    namespace Private_ {
        template <typename BASE_REP_TYPE, typename OPTIONS>
        struct Rep_ final : Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE, OPTIONS>, BASE_REP_TYPE> {
            using ElementType = typename BASE_REP_TYPE::ElementType;
            using inherited   = Memory::InheritAndUseBlockAllocationIfAppropriate<Rep_<BASE_REP_TYPE, OPTIONS>, BASE_REP_TYPE>;
            template <typename... ARGS>
            Rep_ (OPTIONS o, ARGS&&... args)
                : inherited{forward<ARGS> (args)...}
                , fOptions_{o}
            {
            }
            Rep_ (const Rep_&) = delete;
            virtual bool IsSeekable () const override
            {
                lock_guard critSec{fCriticalSection_};
                return BASE_REP_TYPE::IsSeekable ();
            }
            virtual void CloseWrite () override
            {
                lock_guard critSec{fCriticalSection_};
                BASE_REP_TYPE::CloseWrite ();
            }
            virtual bool IsOpenWrite () const override
            {
                lock_guard critSec{fCriticalSection_};
                return BASE_REP_TYPE::IsOpenWrite ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                return BASE_REP_TYPE::GetWriteOffset ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                return BASE_REP_TYPE::SeekWrite (whence, offset);
            }
            virtual void Write (span<const ElementType> elts) override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                BASE_REP_TYPE::Write (elts);
            }
            virtual void Flush () override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                BASE_REP_TYPE::Flush ();
            }

        private:
            [[no_unique_address]] OPTIONS       fOptions_;
            mutable typename OPTIONS::MutexType fCriticalSection_;
        };
        template <typename ELEMENT_TYPE, typename OPTIONS>
        struct Rep2_ : OutputStream::IRep<ELEMENT_TYPE> {
            using ElementType = ELEMENT_TYPE;
            Rep2_ ([[maybe_unused]] const OPTIONS& o, const OutputStream::Ptr<ELEMENT_TYPE>& stream2Wrap)
                : fStream2Wrap{stream2Wrap}
            {
            }
            virtual bool IsSeekable () const override
            {
                lock_guard critSec{fCriticalSection_};
                return fStream2Wrap.IsSeekable ();
            }
            virtual void CloseWrite () override
            {
                lock_guard critSec{fCriticalSection_};
                fStream2Wrap.CloseWrite ();
            }
            virtual bool IsOpenWrite () const override
            {
                lock_guard critSec{fCriticalSection_};
                return fStream2Wrap.IsOpenWrite ();
            }
            virtual SeekOffsetType GetWriteOffset () const override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                return fStream2Wrap.GetWriteOffset ();
            }
            virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                return fStream2Wrap.SeekWrite (whence, offset);
            }
            virtual void Write (span<const ElementType> elts) override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                fStream2Wrap.Write (elts);
            }
            virtual void Flush () override
            {
                lock_guard critSec{fCriticalSection_};
                Require (IsOpenWrite ());
                fStream2Wrap.Flush ();
            }

        private:
            OutputStream::Ptr<ELEMENT_TYPE>     fStream2Wrap;
            mutable typename OPTIONS::MutexType fCriticalSection_;
        };
    }

    /*
     ********************************************************************************
     ***************** InternallySynchronizedOutputStream<>::New ********************
     ********************************************************************************
     */
    template <typename BASE_REP_TYPE, typename OPTIONS, typename... ARGS>
    inline Ptr<typename BASE_REP_TYPE::ElementType> New (const OPTIONS& o, ARGS&&... args)
    {
        return Ptr<typename BASE_REP_TYPE::ElementType>{Memory::MakeSharedPtr<Private_::Rep_<BASE_REP_TYPE, OPTIONS>> (o, forward<ARGS> (args)...)};
    }
    template <typename ELEMENT_TYPE, typename OPTIONS>
    inline Ptr<ELEMENT_TYPE> New (const OPTIONS& o, const OutputStream::Ptr<ELEMENT_TYPE>& stream2Wrap)
    {
        return Ptr<ELEMENT_TYPE>{make_shared<Private_::Rep2_<ELEMENT_TYPE, OPTIONS>> (o, stream2Wrap)};
    }

}
