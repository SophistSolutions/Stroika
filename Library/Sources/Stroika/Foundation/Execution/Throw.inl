/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
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
    [[noreturn]] inline void Throw (T&& e2Throw)
    {
        static_assert (is_convertible_v<remove_cvref_t<T>*, exception*>);
        if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
            DbgTrace ("Throwing exception: %s from %s", Private_::ToString_ (forward<T> (e2Throw)).c_str (), Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("Throwing exception: %s", Private_::ToString_ (e2Throw).c_str ());
#endif
        }
        throw e2Throw;
    }
    template <typename T>
    [[noreturn]] inline void Throw (T&& e2Throw, [[maybe_unused]] const char* traceMsg)
    {
        static_assert (is_convertible_v<remove_cvref_t<T>*, exception*>);
        DbgTrace ("%s", traceMsg);
        Throw (forward<T> (e2Throw)); // important todo this way to get its template specialization (even though the cost is an extra trace message)
    }
    template <typename T>
    [[noreturn]] inline void Throw (T&& e2Throw, [[maybe_unused]] const wchar_t* traceMsg)
    {
        static_assert (is_convertible_v<remove_cvref_t<T>*, exception*>);
        DbgTrace (L"%s", traceMsg);
        Throw (forward<T> (e2Throw)); // important todo this way to get its template specialization (even though the cost is an extra trace message)
    }

    /*
     ********************************************************************************
     ***************************** Execution::ReThrow *******************************
     ********************************************************************************
     */
    [[noreturn]] inline void ReThrow ()
    {
        if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
            DbgTrace ("ReThrow from %s", Execution::Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("ReThrow");
#endif
        }
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e)
    {
        if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
            DbgTrace ("ReThrow from %s", Execution::Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("ReThrow");
#endif
        }
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const char* traceMsg)
    {
        if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
            DbgTrace ("ReThrow %s from %s", traceMsg, Execution::Private_::GetBT_s ().c_str ());
#else
            DbgTrace ("ReThrow: %s", traceMsg);
#endif
        }
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const char* traceMsg)
    {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
        DbgTrace ("ReThrow: %s from %s", traceMsg, Execution::Private_::GetBT_s ().c_str ());
#else
        DbgTrace ("ReThrow: %s", traceMsg);
#endif
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const wchar_t* traceMsg)
    {
        if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
            DbgTrace (L"ReThrow: %s from %s", traceMsg, Execution::Private_::GetBT_ws ().c_str ());
#else
            DbgTrace (L"ReThrow: %s", traceMsg);
#endif
        }
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const wchar_t* traceMsg)
    {
        if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
            DbgTrace (L"ReThrow: %s from %s", traceMsg, Execution::Private_::GetBT_ws ().c_str ());
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
    template <equality_comparable_with<nullptr_t> T, typename E>
    inline void ThrowIfNull (T p, const E& e)
    {
        if (p == nullptr) [[unlikely]] {
            Throw (e, "ThrowIfNull (nullptr) - throwing bad_alloc");
        }
    }

}

#endif /*_Stroia_Foundation_Execution_Throw_inl_*/
