/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Exceptions_h_
#define	_Stroia_Foundation_Execution_Exceptions_h_	1

#include	"../StroikaPreComp.h"

#include	<exception>
#include	<string>
#include	<cerrno>

#include	"../Characters/StringUtils.h"
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
					auto_ptr<RequiredComponentMissingException>			fRequiredComponentMissingException;
					auto_ptr<StringException>							fStringException;
					auto_ptr<IO::FileFormatException>					fFileFormatException;
					auto_ptr<IO::FileBusyException>						fFileBusyException;
					auto_ptr<SilentException>							fSilentException;
				#if		qPlatform_Windows
					auto_ptr<Platform::Windows::HRESULTErrorException>	fHRESULTErrorException;
					auto_ptr<Platform::Windows::Exception>				fWin32ErrorException;
				#endif
			};



			#if		qPlatform_Windows
				#define	CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)\
					catch (const Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException& e) {\
						(CE).fHRESULTErrorException = auto_ptr<Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException> (DEBUG_NEW Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException (e));\
					}\
					catch (const Stroika::Foundation::Execution::Platform::Windows::Exception& e) {\
						(CE).fWin32ErrorException = auto_ptr<Stroika::Foundation::Execution::Platform::Windows::Exception> (DEBUG_NEW Stroika::Foundation::Execution::Platform::Windows::Exception (e));\
					}
			#else
				#define	CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)
			#endif


			#define	CATCH_AND_CAPTURE_CATCH_BLOCK(CE)\
				catch (const Stroika::Foundation::Execution::RequiredComponentMissingException& e) {\
					(CE).fRequiredComponentMissingException = auto_ptr<Stroika::Foundation::Execution::RequiredComponentMissingException> (DEBUG_NEW Stroika::Foundation::Execution::RequiredComponentMissingException (e));\
				}\
				catch (const Stroika::Foundation::Execution::StringException& e) {\
					(CE).fStringException = auto_ptr<Stroika::Foundation::Execution::StringException> (DEBUG_NEW Stroika::Foundation::Execution::StringException (e));\
				}\
				CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)\
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

