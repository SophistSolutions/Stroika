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
#include	<typeinfo>

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {




	
		//	class	errno_ErrorException
			inline	errno_ErrorException::operator errno_t () const
				{
					return fError;
				}
			inline	TString	errno_ErrorException::LookupMessage () const
				{
					return LookupMessage (fError);
				}



			template	<typename T>
				inline		void	 _NoReturn_	DoThrow (const T& e2Throw) 
					{
						DbgTrace ("Throwing exception: %s", typeid (T).name ());
						throw e2Throw;
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


			template	<>
				inline	void	_NoReturn_	DoThrow (const StringException& e2Throw)
					{
						DbgTrace (L"Throwing StringException: '%s'", static_cast<wstring> (e2Throw).substr (0, 20).c_str ());
						throw e2Throw;
					}
		#if		qPlatform_Windows
			template	<>
				inline	void	_NoReturn_	DoThrow (const Platform::Windows::StructuredException& e2Throw)
					{
						DbgTrace ("Throwing Win32StructuredException: fSECode = 0x%x", static_cast<int> (e2Throw));
						throw e2Throw;
					}
		#endif
		#if		qPlatform_Windows
			template	<>
				inline	void	_NoReturn_	DoThrow (const Platform::Windows::HRESULTErrorException& e2Throw)
					{
						DbgTrace ("Throwing Platform::Windows::HRESULTErrorException: HRESULT = 0x%x", static_cast<HRESULT> (e2Throw));
						throw e2Throw;
					}
		#endif
			template	<>
				inline	void	_NoReturn_	DoThrow (const IO::FileFormatException& e2Throw)
					{
						DbgTrace (TSTR ("Throwing FileFormatException: fFileName = '%s'"), e2Throw.fFileName.c_str ());
						throw e2Throw;
					}
			template	<>
				inline	void	_NoReturn_	DoThrow (const SilentException& e2Throw)
					{
						DbgTrace (TSTR ("Throwing SilentException"));
						throw e2Throw;
					}
			template	<>
				inline	void	_NoReturn_	DoThrow (const UserCanceledException& e2Throw)
					{
						DbgTrace (TSTR ("Throwing UserCanceledException"));
						throw e2Throw;
					}
			template	<>
				inline	void	_NoReturn_	DoThrow (const IO::FileBusyException& e2Throw)
					{
						DbgTrace (TSTR ("Throwing FileBusyException: fFileName = '%s'"), e2Throw.fFileName.c_str ());
						throw e2Throw;
					}



		#if		qPlatform_Windows
			inline	void	ThrowIfFalseGetLastError (bool test)
				{
					if (not test) {
						Platform::Windows::Exception::DoThrow (::GetLastError ());
					}
				}
		#endif
			#if		qPlatform_Windows
			inline	void	ThrowIfFalseGetLastError (BOOL test)
				{
					if (not test) {
						Platform::Windows::Exception::DoThrow (::GetLastError ());
					}
				}
			#endif
			#if		qPlatform_Windows
			inline	void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
				{
					if (win32ErrCode != ERROR_SUCCESS) {
						Platform::Windows::Exception::DoThrow (win32ErrCode);
					}
				}
			#endif
			#if		qPlatform_Windows
			inline	void	ThrowIfErrorHRESULT (HRESULT hr)
				{
					if (not SUCCEEDED (hr)) {
						DoThrow (Platform::Windows::HRESULTErrorException (hr));
					}
				}
			#endif
			#if		qPlatform_Windows
			inline	void	ThrowIfError_errno_t (errno_t e)
				{
					if (e != 0) {
						errno_ErrorException::DoThrow (e);
					}
				}
			#endif
			inline	void	ThrowIfNull (const void* p)
				{
					if (p == nullptr) {
						DbgTrace ("ThrowIfNull (nullptr) - throwing bad_alloc");
						DoThrow (bad_alloc ());
					}
				}
			template	<typename E>
				inline	void	ThrowIfNull (const void* p, const E& e)
					{
						if (p == nullptr) {
							DbgTrace ("ThrowIfNull (nullptr,X) - throwing X");
							DoThrow (e);
						}
					}
			#if		qPlatform_Windows
			template<>
				inline	void	ThrowIfNull<HRESULT> (const void* p, const HRESULT& hr)
					{
						ThrowIfNull (p, Platform::Windows::HRESULTErrorException (hr));
					}
			#endif
		}
	}
}

#endif	/*_Stroia_Foundation_Execution_Exceptions_inl_*/
