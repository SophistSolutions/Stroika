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

		#if		qPlatform_Windows
		//	class	Win32ErrorException
			inline	Win32ErrorException::Win32ErrorException (DWORD error):
				fError (error)
				{
				}
			inline	Win32ErrorException::operator DWORD () const
				{
					return fError;
				}
			inline	TString	Win32ErrorException::LookupMessage () const
				{
					return LookupMessage (fError);
				}
		#endif

		//	class	Win32StructuredException
		#if		qPlatform_Windows
			inline	Win32StructuredException::Win32StructuredException (unsigned int seCode):
				fSECode (seCode)
				{
				}
			inline	Win32StructuredException::operator unsigned int () const
				{
					return fSECode;
				}
			inline	TString	Win32StructuredException::LookupMessage () const
				{
					return LookupMessage (fSECode);
				}
		#endif



		//	class	HRESULTErrorException
		#if		qPlatform_Windows
			inline	HRESULTErrorException::HRESULTErrorException (HRESULT hresult):
				fHResult (hresult)
				{
				}
			inline	HRESULTErrorException::operator HRESULT () const
				{
					return fHResult;
				}
			inline	TString	HRESULTErrorException::LookupMessage () const
				{
					return LookupMessage (fHResult);
				}
		#endif

	
		//	class	errno_ErrorException
		#if		qPlatform_Windows
			inline	errno_ErrorException::operator errno_t () const
				{
					return fError;
				}
			inline	TString	errno_ErrorException::LookupMessage () const
				{
					return LookupMessage (fError);
				}
		#endif



			template	<typename T>
				inline		void	 DoThrow (const T& e2Throw) 
					{
						DbgTrace ("Throwing exception: %s", typeid (T).name ());
						throw e2Throw;
					}
			template	<typename T>
				inline	void		DoThrow (const T& e2Throw, const char* traceMsg)
					{
						DbgTrace ("%s", traceMsg);
						throw e2Throw;
					}
			template	<typename T>
				inline	void		DoThrow (const T& e2Throw, const wchar_t* traceMsg)
					{
						DbgTrace (L"%s", traceMsg);
						throw e2Throw;
					}
			inline	void	DoReThrow ()
				{
					DbgTrace ("DoReThrow");
					throw;
				}
			inline	void	DoReThrow (const char* traceMsg)
				{
					DbgTrace ("DoReThrow: %s", traceMsg);
					throw;
				}
			inline	void	DoReThrow (const wchar_t* traceMsg)
				{
					DbgTrace (L"DoReThrow: %s", traceMsg);
					throw;
				}


			#if		qPlatform_Windows
			template	<>
				inline	void	DoThrow (const Win32ErrorException& e2Throw)
					{
						DbgTrace ("Throwing Win32ErrorException: DWORD = 0x%x", static_cast<DWORD> (e2Throw));
						throw e2Throw;
					}
			#endif
			template	<>
				inline	void	DoThrow (const StringException& e2Throw)
					{
						DbgTrace (L"Throwing StringException: '%s'", static_cast<wstring> (e2Throw).substr (0, 20).c_str ());
						throw e2Throw;
					}
		#if		qPlatform_Windows
			template	<>
				inline	void	_DoThrow (const Win32StructuredException& e2Throw)
					{
						DbgTrace ("Throwing Win32StructuredException: fSECode = 0x%x", static_cast<int> (e2Throw));
						throw e2Throw;
					}
		#endif
		#if		qPlatform_Windows
			template	<>
				inline	void	__atttribute__((noreturn))	DoThrow (const HRESULTErrorException& e2Throw)
					{
						DbgTrace ("Throwing HRESULTErrorException: HRESULT = 0x%x", static_cast<HRESULT> (e2Throw));
						throw e2Throw;
					}
		#endif
			template	<>
				inline	void	DoThrow (const IO::FileFormatException& e2Throw)
					{
						DbgTrace (_T ("Throwing FileFormatException: fFileName = '%s'"), e2Throw.fFileName.c_str ());
						throw e2Throw;
					}
			template	<>
				inline	void	DoThrow (const SilentException& e2Throw)
					{
						DbgTrace (_T ("Throwing SilentException"));
						throw e2Throw;
					}
			template	<>
				inline	void	DoThrow (const UserCanceledException& e2Throw)
					{
						DbgTrace (_T ("Throwing UserCanceledException"));
						throw e2Throw;
					}
			template	<>
				inline	void	DoThrow (const IO::FileBusyException& e2Throw)
					{
						DbgTrace (_T ("Throwing FileBusyException: fFileName = '%s'"), e2Throw.fFileName.c_str ());
						throw e2Throw;
					}



		#if		qPlatform_Windows
			inline	void	ThrowIfFalseGetLastError (bool test)
				{
					if (not test) {
						Win32ErrorException::DoThrow (::GetLastError ());
					}
				}
		#endif
			#if		qPlatform_Windows
			inline	void	ThrowIfFalseGetLastError (BOOL test)
				{
					if (not test) {
						Win32ErrorException::DoThrow (::GetLastError ());
					}
				}
			#endif
			#if		qPlatform_Windows
			inline	void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
				{
					if (win32ErrCode != ERROR_SUCCESS) {
						Win32ErrorException::DoThrow (win32ErrCode);
					}
				}
			#endif
			#if		qPlatform_Windows
			inline	void	ThrowIfErrorHRESULT (HRESULT hr)
				{
					if (not SUCCEEDED (hr)) {
						DoThrow (HRESULTErrorException (hr));
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
					if (p == NULL) {
						DoThrow (bad_alloc (), _T ("ThrowIfNull (NULL) - throwing bad_alloc"));
					}
				}
			template	<typename E>
				inline	void	ThrowIfNull (const void* p, const E& e)
					{
						if (p == NULL) {
							DoThrow (e, _T ("ThrowIfNull (NULL,X) - throwing X"));
						}
					}
			#if		qPlatform_Windows
			template<>
				inline	void	ThrowIfNull<HRESULT> (const void* p, const HRESULT& hr)
					{
						ThrowIfNull (p, HRESULTErrorException (hr));
					}
			#endif
		}
	}
}

#endif	/*_Stroia_Foundation_Execution_Exceptions_inl_*/
