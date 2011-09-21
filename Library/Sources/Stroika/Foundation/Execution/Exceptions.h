/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Exceptions_h_
#define	_Stroia_Foundation_Execution_Exceptions_h_	1

#include	"../StroikaPreComp.h"

#include	<exception>
#include	<string>
#include	<cerrno>

#include	"../Configuration/Common.h"
#include	"../Debug/Trace.h"
#include	"../IO/FileBusyException.h"
#include	"../IO/FileFormatException.h"

#include	"StringException.h"

#if		qPlatform_Windows
	#include	"Platform/Windows/Exception.h"
	#include	"Platform/Windows/HRESULTErrorException.h"
	#include	"Platform/Windows/StructuredException.h"
#endif



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			using	Characters::TString;



			// Throw this when an error has already been reported - so that it isn't reported again
			class	SilentException {
			};

			// mostly treat the same as SilentException
			class	UserCanceledException : public SilentException {
			};



			#if		!qCompilerAndStdLib_Supports_errno_t
				typedef	int	errno_t;
			#endif

			class	errno_ErrorException : public StringException {
				public:
					explicit errno_ErrorException (errno_t e);

					operator errno_t () const;

				public:
					static	TString	LookupMessage (errno_t e);
					nonvirtual	TString	LookupMessage () const;

				public:
					// throw errno_ErrorException () - unless I can find another Win32Exception, or bad_alloc() or some such which is
					// as good a fit.
					static	void	DoThrow (errno_t error);

				private:
					errno_t	fError;
			};



			class	RequiredComponentMissingException : public StringException {
				public:
					static	const	wchar_t	kJava[];
					static	const	wchar_t	kPDFViewer[];
					static	const	wchar_t	kPDFOCXViewer[];
				public:
					RequiredComponentMissingException (const wstring& component);

				public:
					wstring	fComponent;
			};

			class	RequiredComponentVersionMismatchException : public StringException {
				public:
					RequiredComponentVersionMismatchException (const wstring& component, const wstring& requiredVersion = wstring ());
			};

			class	FeatureNotSupportedInThisVersionException : public StringException {
				public:
					FeatureNotSupportedInThisVersionException (const wstring& feature);
				public:
					wstring	fFeature;
			};



			// Utility to call a Trace message (hopefully an appropriate one) for an exception being thrown...
			template	<typename T>
				void	 _NoReturn_	DoThrow (const T& e2Throw);
			template	<typename T>
				void	_NoReturn_	DoThrow (const T& e2Throw, const char* traceMsg);
			template	<typename T>
				void	_NoReturn_ 	DoThrow (const T& e2Throw, const wchar_t* traceMsg);

			// Just a regular C++ rethrow, but with a DbgTrace message...
			void	_NoReturn_	DoReThrow ();
			void	_NoReturn_	DoReThrow (const char* traceMsg);
			void	_NoReturn_	DoReThrow (const wchar_t* traceMsg);



			void	ThrowIfFalseGetLastError (bool test);
			void	ThrowIfError_errno_t (errno_t e = errno);
		#if		qPlatform_Windows
			void	ThrowIfFalseGetLastError (BOOL test);
			void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);
			void	ThrowIfErrorHRESULT (HRESULT hr);
			void	ThrowIfShellExecError (HINSTANCE r);
		#endif

			template	<typename E>
				void	ThrowIfNull (const void* p, const E& e = E ());
		#if		qPlatform_Windows
			template<>
				void	ThrowIfNull (const void* p, const HRESULT& hr);
		#endif
			void	ThrowIfNull (const void* p);


			#define	IgnoreExceptionsForCall(theCode)		try {theCode;} catch (...) {}


		#if		qPlatform_Windows
			#define	CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION()\
				catch (HRESULT hr) {\
					return hr;\
				}\
				catch (const Stroika::Foundation::Execution::Platform::Windows::Exception& we) {\
					return (HRESULT_FROM_WIN32 (we));\
				}\
				catch (const Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException& h) {\
					return static_cast<HRESULT> (h);\
				}\
				catch (const bad_alloc&) {\
					return E_OUTOFMEMORY;\
				}\
				catch (...) {\
					return DISP_E_EXCEPTION;\
				}\

		#endif





			#if		qPlatform_Windows
				// these map invalid parameters etc to Execution (with assertions and appropriate
				// logging)
				void	RegisterDefaultHandler_invalid_parameter ();
				void	RegisterDefaultHandler_pure_function_call ();
			#endif

		}
	}
}
#endif	/*_Stroia_Foundation_Execution_Exceptions_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Exceptions.inl"

