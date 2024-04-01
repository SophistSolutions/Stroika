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
                return ToString_ (typeid (T)); // exact match preferred over template
            }
        }

#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        void JustDbgTrace_ (const string& msg);
        void JustDbgTrace_ (const wstring& msg);
        void ThrowingExceptionDbgTrace_ (const string& msg);
        void ThrowingExceptionDbgTrace_ (const wstring& msg);
        void ReThrowingExceptionDbgTrace_ (const string& msg);
        void ReThrowingExceptionDbgTrace_ (const wstring& msg = {});
#endif

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
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::ThrowingExceptionDbgTrace_ (Private_::ToString_ (forward<T> (e2Throw)));
#endif
        throw e2Throw;
    }
    template <typename T>
    [[noreturn]] inline void Throw (T&& e2Throw, [[maybe_unused]] const char* traceMsg)
    {
        static_assert (is_convertible_v<remove_cvref_t<T>*, exception*>);
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::JustDbgTrace_ (traceMsg);
#endif
        Throw (forward<T> (e2Throw)); // important todo this way to get its template specialization (even though the cost is an extra trace message)
    }
    template <typename T>
    [[noreturn]] inline void Throw (T&& e2Throw, [[maybe_unused]] const wchar_t* traceMsg)
    {
        static_assert (is_convertible_v<remove_cvref_t<T>*, exception*>);
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::JustDbgTrace_ (traceMsg);
#endif
        Throw (forward<T> (e2Throw)); // important todo this way to get its template specialization (even though the cost is an extra trace message)
    }

    /*
     ********************************************************************************
     ***************************** Execution::ReThrow *******************************
     ********************************************************************************
     */
    [[noreturn]] inline void ReThrow ()
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::ReThrowingExceptionDbgTrace_ ();
#endif
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e)
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::ReThrowingExceptionDbgTrace_ ();
#endif
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const char* traceMsg)
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::ReThrowingExceptionDbgTrace_ (traceMsg);
#endif
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const char* traceMsg)
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::ReThrowingExceptionDbgTrace_ (traceMsg);
#endif
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const wchar_t* traceMsg)
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::ReThrowingExceptionDbgTrace_ (traceMsg);
#endif
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const wchar_t* traceMsg)
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        Private_::ReThrowingExceptionDbgTrace_ (traceMsg);
#endif
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
            Throw (e, "ThrowIfNull (nullptr) - throwing bad_alloc"); // todo fix trace message to depend on 'E'
        }
    }
    template <equality_comparable_with<nullptr_t> T>
    inline void ThrowIfNull (T p)
    {
        static const bad_alloc kException_;
        ThrowIfNull (p, kException_);
    }
}

#endif /*_Stroia_Foundation_Execution_Throw_inl_*/
