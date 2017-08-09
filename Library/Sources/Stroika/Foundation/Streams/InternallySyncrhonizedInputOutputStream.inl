/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_inl_
#define _Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             * InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_ ***
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            class InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Rep_ : public BASE_REP_TYPE {
            public:
                template <typename... ARGS>
                Rep_ (ARGS&&... args)
                    : BASE_REP_TYPE (forward<ARGS> (args)...)
                {
                }
                Rep_ (const Rep_&) = delete;
                virtual bool IsSeekable () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return BASE_REP_TYPE::IsSeekable ();
                }
                virtual void CloseRead () override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    BASE_REP_TYPE::CloseRead ();
                }
                virtual bool IsOpenRead () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    return BASE_REP_TYPE::IsOpenRead ();
                }
                virtual SeekOffsetType GetReadOffset () const override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    Require (IsOpenRead ());
                    return BASE_REP_TYPE::GetReadOffset ();
                }
                virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    Require (IsOpenRead ());
                    return BASE_REP_TYPE::SeekRead (whence, offset);
                }
                virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    Require (IsOpenRead ());
                    return BASE_REP_TYPE::Read (intoStart, intoEnd);
                }
                virtual Memory::Optional<size_t> ReadNonBlocking (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                {
                    lock_guard<mutex> critSec{fCriticalSection_};
                    Require (IsOpenRead ());
                    return BASE_REP_TYPE::ReadNonBlocking (intoStart, intoEnd);
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
                virtual void Write (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end) override
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

            /*
             ********************************************************************************
             * InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            template <typename... ARGS>
            inline auto InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::New (ARGS&&... args) -> Ptr
            {
                return Ptr{make_shared<Rep_> (forward<ARGS> (args)...)};
            }

            /*
             ********************************************************************************
             * InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            inline InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr::Ptr (const shared_ptr<Rep_>& from)
                : inherited (from)
            {
            }
            template <typename ELEMENT_TYPE, template <typename> class BASE_CLASS, typename BASE_REP_TYPE>
            inline typename InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr& InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>::Ptr::operator= (const InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, BASE_CLASS, BASE_REP_TYPE>& rhs)
            {
                inherited::Ptr::operator= (rhs);
                return *this;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InternallySyncrhonizedInputOutputStream_inl_*/
