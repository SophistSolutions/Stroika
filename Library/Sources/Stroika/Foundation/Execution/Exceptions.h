/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Exceptions_h_
#define	_Stroia_Foundation_Execution_Exceptions_h_	1

#include	"../StroikaPreComp.h"

#include	<exception>
#include	<string>
#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Characters/StringUtils.h"
#include	"../Configuration/Common.h"
#include	"../Debug/Trace.h"
#include	"../IO/FileBusyException.h"
#include	"../IO/FileFormatException.h"

#include	"StringException.h"


// in gcc the syntax is sadly after the function name, not before: __attribute__ ((noreturn)
// we will probably have to use two separate defines to achieve this, very ugly
#if qPlatform_Windows
	#define	__atttribute__((noreturn)) __declspec(noreturn)
#else
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


		#if		qPlatform_Windows
			class	Win32ErrorException {
				public:
					Win32ErrorException (DWORD error);

					operator DWORD () const;

				public:
					// throw Win32ErrorException () - if error is a real error, and map SOME (like #8 to bad_alloc) - but ALWAYS throw
					// someting, regardless of error#
					static	void	DoThrow (DWORD error);

				public:
					static	TString	LookupMessage (DWORD hr);
					nonvirtual	TString	LookupMessage () const;

				private:
					DWORD	fError;

				public:
					static	const	DWORD	kERROR_INTERNET_TIMEOUT						=	12002;
					static	const	DWORD	kERROR_INTERNET_INVALID_URL					=	12005;
					static	const	DWORD	kERROR_INTERNET_UNRECOGNIZED_SCHEME			=	12006;
					static	const	DWORD	kERROR_INTERNET_NAME_NOT_RESOLVED			=	12007;
					static	const	DWORD	kERROR_INTERNET_PROTOCOL_NOT_FOUND			=	12008;
					static	const	DWORD	kERROR_INTERNET_CANNOT_CONNECT				=	12029;
			};
		#endif


		#if		qPlatform_Windows
			class	Win32StructuredException {
				private:
					unsigned int fSECode;

				public:
					explicit Win32StructuredException (unsigned int n);
					operator unsigned int () const;

				public:
					static	TString	LookupMessage (unsigned int n);
					nonvirtual	TString	LookupMessage () const;

				public:
					static	void	RegisterHandler ();
				private:
					static	void	trans_func (unsigned int u, EXCEPTION_POINTERS* pExp);
			};
		#endif


		#if		qPlatform_Windows
			class	HRESULTErrorException {
				public:
					HRESULTErrorException (HRESULT hresult);

					operator HRESULT () const;

				public:
					static	TString	LookupMessage (HRESULT hr);
					nonvirtual	TString	LookupMessage () const;

				private:
					HRESULT	fHResult;
			};
		#endif


		#if		qPlatform_Windows
			class	errno_ErrorException : public StringException {
				public:
					errno_ErrorException (errno_t e);

					operator errno_t () const;

				public:
					static	TString	LookupMessage (errno_t e);
					nonvirtual	TString	LookupMessage () const;

				public:
					// throw errno_ErrorException () - unless I can find another Win32Exception, or bad_alloc() or some such which is
					// as good a fit.
					static	void	DoThrow (DWORD error);

				private:
					errno_t	fError;
			};
		#endif



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
				void	 __attribute__((noreturn))	DoThrow (const T& e2Throw) ;
			template	<typename T>
				void	__attribute__((noreturn))	DoThrow (const T& e2Throw, const char* traceMsg);
			template	<typename T>
				void	__attribute__((noreturn)) 	DoThrow (const T& e2Throw, const wchar_t* traceMsg);

			// Just a regular C++ rethrow, but with a DbgTrace message...
			void	__attribute__((noreturn))	DoReThrow ();
			void	__attribute__((noreturn))	DoReThrow (const char* traceMsg);
			void	__attribute__((noreturn))	DoReThrow (const wchar_t* traceMsg);



			void	ThrowIfFalseGetLastError (bool test);
		#if		qPlatform_Windows
			void	ThrowIfFalseGetLastError (BOOL test);
			void	ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);
			void	ThrowIfErrorHRESULT (HRESULT hr);
			void	ThrowIfError_errno_t (errno_t e = errno);
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


			#define	CATCH_AND_HANDLE_EXCEPTIONS_IN_HRESULT_FUNCTION()\
				catch (HRESULT hr) {\
					return hr;\
				}\
				catch (const Stroika::Foundation::Execution::Win32ErrorException& we) {\
					return (HRESULT_FROM_WIN32 (we));\
				}\
				catch (const Stroika::Foundation::Execution::HRESULTErrorException& h) {\
					return static_cast<HRESULT> (h);\
				}\
				catch (const bad_alloc&) {\
					return E_OUTOFMEMORY;\
				}\
				catch (...) {\
					return DISP_E_EXCEPTION;\
				}\



			/*
			 *	Utility class to support catch and re-throw of exceptions.
			 */
			struct	CatchAndCaptureExceptionHelper {
				public:
					CatchAndCaptureExceptionHelper ();

				public:
					// This function need not be run, but CAN be run to allow subclasses to specify additional 
					struct	Callback {
						virtual	void	DoItInsideCatcher () = 0;
					};
					virtual	void	DoRunWithCatchRePropagate (Callback* callback);

				public:
					virtual	bool	AnyExceptionCaught () const;
					virtual	void	RethrowIfAnyCaught () const;

				public:
		#if		qPlatform_Windows
					auto_ptr<HRESULTErrorException>					fHRESULTErrorException;
					auto_ptr<Win32ErrorException>					fWin32ErrorException;
		#endif
					auto_ptr<RequiredComponentMissingException>		fRequiredComponentMissingException;
					auto_ptr<StringException>						fStringException;
					auto_ptr<IO::FileFormatException>				fFileFormatException;
					auto_ptr<IO::FileBusyException>					fFileBusyException;
					auto_ptr<SilentException>						fSilentException;
			};


			#define	CATCH_AND_CAPTURE_CATCH_BLOCK(CE)\
				catch (const Stroika::Foundation::Execution::RequiredComponentMissingException& e) {\
					(CE).fRequiredComponentMissingException = auto_ptr<Stroika::Foundation::Execution::RequiredComponentMissingException> (DEBUG_NEW Stroika::Foundation::Execution::RequiredComponentMissingException (e));\
				}\
				catch (const Stroika::Foundation::Execution::StringException& e) {\
					(CE).fStringException = auto_ptr<Stroika::Foundation::Execution::StringException> (DEBUG_NEW Stroika::Foundation::Execution::StringException (e));\
				}\
				catch (const Stroika::Foundation::Execution::HRESULTErrorException& e) {\
					(CE).fHRESULTErrorException = auto_ptr<Stroika::Foundation::Execution::HRESULTErrorException> (DEBUG_NEW Stroika::Foundation::Execution::HRESULTErrorException (e));\
				}\
				catch (const Stroika::Foundation::Execution::Win32ErrorException& e) {\
					(CE).fWin32ErrorException = auto_ptr<Stroika::Foundation::Execution::Win32ErrorException> (DEBUG_NEW Stroika::Foundation::Execution::Win32ErrorException (e));\
				}\
				catch (const Stroika::Foundation::IO::FileFormatException& e) {\
					(CE).fFileFormatException = auto_ptr<Stroika::Foundation::IO::FileFormatException> (DEBUG_NEW Stroika::Foundation::IO::FileFormatException (e));\
				}\
				catch (const Stroika::Foundation::IO::FileBusyException& e) {\
					(CE).fFileBusyException = auto_ptr<Stroika::Foundation::IO::FileBusyException> (DEBUG_NEW Stroika::Foundation::IO::FileBusyException (e));\
				}\
				catch (const Stroika::Foundation::Execution::SilentException& e) {\
					(CE).fSilentException = auto_ptr<Stroika::Foundation::Execution::SilentException> (DEBUG_NEW Stroika::Foundation::Execution::SilentException (e));\
				}\
				catch (...) {\
					(CE).fStringException = auto_ptr<Stroika::Foundation::Execution::StringException> (DEBUG_NEW Stroika::Foundation::Execution::StringException (L"Unknown Exception"));\
				}\



				// these map invalid parameters etc to Execution (with assertions and appropriate
				// logging)
				void	RegisterDefaultHandler_invalid_parameter ();
				void	RegisterDefaultHandler_pure_function_call ();

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

