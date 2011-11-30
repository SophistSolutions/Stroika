/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Exceptions_inl_
#define	_Stroia_Foundation_Execution_Exceptions_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {



			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw) 
					{
						DbgTrace ("Throwing exception: %s", typeid (T).name ());
						throw e2Throw;
					}
			template	<typename T>
				void		_NoReturn_	DoThrow (const T& e2Throw, const char* traceMsg)
					{
						DbgTrace ("%s", traceMsg);
						DoThrow (e2Throw);		// important todo this way to get its template specialization (even though the cost is an extra trace message)
					}
			template	<typename T>
				void		_NoReturn_	DoThrow (const T& e2Throw, const wchar_t* traceMsg)
					{
						DbgTrace (L"%s", traceMsg);
						DoThrow (e2Throw);		// important todo this way to get its template specialization (even though the cost is an extra trace message)
					}




			inline	void	_NoReturn_	DoReThrow ()
				{
					DbgTrace ("DoReThrow");
					throw;
				}
			inline	void	_NoReturn_	DoReThrow (const char* traceMsg)
				{
					DbgTrace ("DoReThrow: %s", traceMsg);
					throw;
				}
			inline	void	_NoReturn_	DoReThrow (const wchar_t* traceMsg)
				{
					DbgTrace (L"DoReThrow: %s", traceMsg);
					throw;
				}




			inline	void	ThrowIfNull (const void* p)
				{
					if (p == nullptr) {
						DoThrow (bad_alloc (), "ThrowIfNull (nullptr) - throwing bad_alloc");
					}
				}
			template	<typename E>
				inline	void	ThrowIfNull (const void* p, const E& e)
					{
						if (p == nullptr) {
							DoThrow (e, "ThrowIfNull (nullptr,X) - throwing X");
						}
					}
		}
	}
}

#endif	/*_Stroia_Foundation_Execution_Exceptions_inl_*/
