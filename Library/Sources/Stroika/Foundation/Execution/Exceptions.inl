/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_inl_
#define _Stroia_Foundation_Execution_Exceptions_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <typeinfo>      // needed for use of typeid()



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
            namespace   Private_ {
                // avoid header dependencies on Debug/BackTrace and no Characters/String
                string  GetBT_s ();
                wstring GetBT_ws ();
            }
#endif


            /*
             ********************************************************************************
             ***************************** Execution::Throw *********************************
             ********************************************************************************
             */
            template    <typename T>
            [[noreturn]]    void        Throw (const T& e2Throw)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace ("Throwing exception: %s from %s", typeid (T).name (), Private_::GetBT_s ().c_str ());
#else
                DbgTrace ("Throwing exception: %s", typeid (T).name ());
#endif
                throw e2Throw;
            }
            template    <typename T>
            [[noreturn]]    void        Throw (const T& e2Throw, const char* traceMsg)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace ("%s from %s", traceMsg, Private_::GetBT_s ().c_str ());
#else
                DbgTrace ("%s", traceMsg);
#endif
                Throw (e2Throw);      // important todo this way to get its template specialization (even though the cost is an extra trace message)
            }
            template    <typename T>
            [[noreturn]]    void        Throw (const T& e2Throw, const wchar_t* traceMsg)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace (L"%s from %s", traceMsg, Private_::GetBT_ws ().c_str ());
#else
                DbgTrace (L"%s", traceMsg);
#endif
                Throw (e2Throw);      // important todo this way to get its template specialization (even though the cost is an extra trace message)
            }


            /*
             ********************************************************************************
             ***************************** Execution::ReThrow *******************************
             ********************************************************************************
             */
            [[noreturn]]    inline  void    ReThrow ()
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace ("ReThrow from %s", Private_::GetBT_s ().c_str ());
#else
                DbgTrace ("ReThrow");
#endif
                throw;
            }
            [[noreturn]]    inline  void    ReThrow (const exception_ptr& e)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace ("ReThrow from %s", Private_::GetBT_s ().c_str ());
#else
                DbgTrace ("ReThrow");
#endif
                rethrow_exception (e);
            }
            [[noreturn]]    inline  void    ReThrow (const char* traceMsg)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace ("ReThrow %s from %s", traceMsg, Private_::GetBT_s ().c_str ());
#else
                DbgTrace ("ReThrow: %s", traceMsg);
#endif
                throw;
            }
            [[noreturn]]    inline  void    ReThrow (const exception_ptr& e, const char* traceMsg)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace ("ReThrow: %s from %s", traceMsg, Private_::GetBT_s ().c_str ());
#else
                DbgTrace ("ReThrow: %s", traceMsg);
#endif
                rethrow_exception (e);
            }
            [[noreturn]]    inline  void    ReThrow (const wchar_t* traceMsg)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace (L"ReThrow: %s from %s", traceMsg, Private_::GetBT_ws ().c_str ());
#else
                DbgTrace (L"ReThrow: %s", traceMsg);
#endif
                throw;
            }
            [[noreturn]]    inline  void    ReThrow (const exception_ptr& e, const wchar_t* traceMsg)
            {
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
                DbgTrace (L"ReThrow: %s from %s", traceMsg, Private_::GetBT_ws ().c_str ());
#else
                DbgTrace (L"ReThrow: %s", traceMsg);
#endif
                std::rethrow_exception (e);
            }



            /*
             ********************************************************************************
             ************************* Execution::ThrowIfNull *******************************
             ********************************************************************************
             */
            inline  void    ThrowIfNull (const void* p)
            {
                if (p == nullptr) {
                    Throw (bad_alloc (), "ThrowIfNull (nullptr) - throwing bad_alloc");
                }
            }
            template    <typename E>
            inline  void    ThrowIfNull (const void* p, const E& e)
            {
                if (p == nullptr) {
                    Throw (e, "ThrowIfNull (nullptr,X) - throwing X");
                }
            }
            template    <typename   T>
            inline  void    ThrowIfNull (const shared_ptr<T>& p)
            {
                if (p.get () == nullptr) {
                    Throw (bad_alloc (), "ThrowIfNull (shared_ptr<> (nullptr)) - throwing bad_alloc");
                }
            }


        }
    }
}

#endif  /*_Stroia_Foundation_Execution_Exceptions_inl_*/
