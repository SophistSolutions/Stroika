/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Throw_inl_
#define _Stroia_Foundation_Execution_Throw_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <typeinfo> // needed for use of typeid()

namespace Stroika::Foundation::Execution {

    namespace Private_ {
// avoid header dependencies on Debug/BackTrace and no Characters/String
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
        string  GetBT_s ();
        wstring GetBT_ws ();
#endif
        string ToString_ (const type_info&);
        template <typename T>
        inline string ToString_ (const T& t)
        {
            if constexpr (is_convertible_v<T, const exception&>) {
                return t.what ();
            }
            else {
                return ToString_ (typeid (T)); // exact match preferered over template
            }
        }
    }

    /*
     ********************************************************************************
     ***************************** Execution::Throw *********************************
     ********************************************************************************
     */
    template <typename T>
    [[noreturn]] inline void Throw (const T& e2Throw)
    {
        static_assert (is_convertible_v<T*, exception*>);
        if constexpr (qStroika_Foundation_Exection_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
            DbgTrace ("Throwing exception: %s from %s", Private_::ToString_ (e2Throw).c_str (), Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("Throwing exception: %s", Private_::ToString_ (e2Throw).c_str ());
#endif
        }
        throw e2Throw;
    }
    template <typename T>
    [[noreturn]] inline void Throw (const T& e2Throw, const char* traceMsg)
    {
        static_assert (is_convertible_v<T*, exception*>);
        DbgTrace ("%s", traceMsg);
        Throw (e2Throw); // important todo this way to get its template specialization (even though the cost is an extra trace message)
    }
    template <typename T>
    [[noreturn]] inline void Throw (const T& e2Throw, const wchar_t* traceMsg)
    {
        static_assert (is_convertible_v<T*, exception*>);
        DbgTrace (L"%s", traceMsg);
        Throw (e2Throw); // important todo this way to get its template specialization (even though the cost is an extra trace message)
    }

    /*
     ********************************************************************************
     ***************************** Execution::ReThrow *******************************
     ********************************************************************************
     */
    [[noreturn]] inline void ReThrow ()
    {
        if constexpr (qStroika_Foundation_Exection_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
            DbgTrace ("ReThrow from %s", Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("ReThrow");
#endif
        }
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e)
    {
        if constexpr (qStroika_Foundation_Exection_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
            DbgTrace ("ReThrow from %s", Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("ReThrow");
#endif
        }
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const char* traceMsg)
    {
        if constexpr (qStroika_Foundation_Exection_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
            DbgTrace ("ReThrow %s from %s", traceMsg, Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("ReThrow: %s", traceMsg);
#endif
        }
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const char* traceMsg)
    {
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
        DbgTrace ("ReThrow: %s from %s", traceMsg, Private_::GetBT_s ().c_str ());
#else
        DbgTrace ("ReThrow: %s", traceMsg);
#endif
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const wchar_t* traceMsg)
    {
        if constexpr (qStroika_Foundation_Exection_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
            DbgTrace (L"ReThrow: %s from %s", traceMsg, Private_::GetBT_ws ().c_str ());
#else
            DbgTrace (L"ReThrow: %s", traceMsg);
#endif
        }
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const wchar_t* traceMsg)
    {
        if constexpr (qStroika_Foundation_Exection_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
            DbgTrace (L"ReThrow: %s from %s", traceMsg, Private_::GetBT_ws ().c_str ());
#else
            DbgTrace (L"ReThrow: %s", traceMsg);
#endif
        }
        rethrow_exception (e);
    }

    /*
     ********************************************************************************
     *************************** Execution::ThrowIfNull *****************************
     ********************************************************************************
     */
    inline void ThrowIfNull (const void* p)
    {
        if (p == nullptr) [[unlikely]] {
            Throw (bad_alloc{}, "ThrowIfNull (nullptr) - throwing bad_alloc");
        }
    }
    template <typename E>
    inline void ThrowIfNull (const void* p, const E& e)
    {
        static_assert (is_convertible_v<E*, exception*>);
        if (p == nullptr) [[unlikely]] {
            Throw (e, "ThrowIfNull (nullptr,X) - throwing X");
        }
    }
    template <typename T>
    inline void ThrowIfNull (const unique_ptr<T>& p)
    {
        if (p == nullptr) [[unlikely]] {
            Throw (bad_alloc{}, "ThrowIfNull (unique_ptr<> (nullptr)) - throwing bad_alloc");
        }
    }
    template <typename T>
    inline void ThrowIfNull (const shared_ptr<T>& p)
    {
        if (p == nullptr) [[unlikely]] {
            Throw (bad_alloc{}, "ThrowIfNull (shared_ptr<> (nullptr)) - throwing bad_alloc");
        }
    }

}

#endif /*_Stroia_Foundation_Execution_Throw_inl_*/
