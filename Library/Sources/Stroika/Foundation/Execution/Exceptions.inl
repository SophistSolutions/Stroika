/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_inl_
#define _Stroia_Foundation_Execution_Exceptions_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <typeinfo> // needed for use of typeid()

namespace Stroika::Foundation::Execution {

    namespace Private_ {
// avoid header dependencies on Debug/BackTrace and no Characters/String
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        string  GetBT_s ();
        wstring GetBT_ws ();
#endif
        string ToString_s (const type_info&);
        template <typename T>
        inline string Except2String_ (const T& t, typename enable_if<is_convertible<T, const std::exception&>::value>::type* = 0)
        {
            return t.what ();
        }
        template <typename T>
        inline string Except2String_ (const T& /*t*/, typename enable_if<not is_convertible<T, const std::exception&>::value>::type* = 0)
        {
            return ToString_s (typeid (T));
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
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        DbgTrace ("Throwing exception: %s from %s", Private_::Except2String_ (e2Throw).c_str (), Private_::GetBT_s ().c_str ());
#else
        DbgTrace ("Throwing exception: %s", Private_::Except2String_ (e2Throw).c_str ());
#endif
#endif
        throw e2Throw;
    }
    template <typename T>
    [[noreturn]] inline void Throw (const T& e2Throw, const char* traceMsg)
    {
        DbgTrace ("%s", traceMsg);
        Throw (e2Throw); // important todo this way to get its template specialization (even though the cost is an extra trace message)
    }
    template <typename T>
    [[noreturn]] inline void Throw (const T& e2Throw, const wchar_t* traceMsg)
    {
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
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        DbgTrace ("ReThrow from %s", Private_::GetBT_s ().c_str ());
#else
        DbgTrace ("ReThrow");
#endif
#endif
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e)
    {
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        DbgTrace ("ReThrow from %s", Private_::GetBT_s ().c_str ());
#else
        DbgTrace ("ReThrow");
#endif
#endif
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const char* traceMsg)
    {
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        DbgTrace ("ReThrow %s from %s", traceMsg, Private_::GetBT_s ().c_str ());
#else
        DbgTrace ("ReThrow: %s", traceMsg);
#endif
#endif
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const char* traceMsg)
    {
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        DbgTrace ("ReThrow: %s from %s", traceMsg, Private_::GetBT_s ().c_str ());
#else
        DbgTrace ("ReThrow: %s", traceMsg);
#endif
        rethrow_exception (e);
    }
    [[noreturn]] inline void ReThrow ([[maybe_unused]] const wchar_t* traceMsg)
    {
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        DbgTrace (L"ReThrow: %s from %s", traceMsg, Private_::GetBT_ws ().c_str ());
#else
        DbgTrace (L"ReThrow: %s", traceMsg);
#endif
#endif
        throw;
    }
    [[noreturn]] inline void ReThrow (const exception_ptr& e, [[maybe_unused]] const wchar_t* traceMsg)
    {
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpoint
#if qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
        DbgTrace (L"ReThrow: %s from %s", traceMsg, Private_::GetBT_ws ().c_str ());
#else
        DbgTrace (L"ReThrow: %s", traceMsg);
#endif
#endif
        std::rethrow_exception (e);
    }

    /*
     ********************************************************************************
     *************************** Execution::ThrowIfNull *****************************
     ********************************************************************************
     */
    inline void ThrowIfNull (const void* p)
    {
        if (p == nullptr) {
            Throw (bad_alloc (), "ThrowIfNull (nullptr) - throwing bad_alloc");
        }
    }
    template <typename E>
    inline void ThrowIfNull (const void* p, const E& e)
    {
        if (p == nullptr) {
            Throw (e, "ThrowIfNull (nullptr,X) - throwing X");
        }
    }
    template <typename T>
    inline void ThrowIfNull (const shared_ptr<T>& p)
    {
        if (p.get () == nullptr) {
            Throw (bad_alloc (), "ThrowIfNull (shared_ptr<> (nullptr)) - throwing bad_alloc");
        }
    }

}

#endif /*_Stroia_Foundation_Execution_Exceptions_inl_*/
