/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_Propagate_h_
#define	_Stroia_Foundation_Execution_Propagate_h_	1

#include	"../StroikaPreComp.h"

#include	"../IO/FileBusyException.h"
#include	"../IO/FileFormatException.h"
#include	"../Memory/SharedPtr.h"

#include	"IRunnable.h"

#include	"Exceptions.h"


/*
 * TODO:
 *		(o)	Re-implement using new C++11 feature:
			Copying and rethrowing exceptions
			How do you catch an exception and then rethrow it on another thread? Use a bit of library magic as described in the standard 18.8.5 Exception Propagation:
			exception_ptr current_exception(); Returns: An exception_ptr object that refers to the currently handled exception (15.3) or a copy of the currently handled exception, or a null exception_ptr object if no exception is being handled. The referenced object shall remain valid at least as long as there is an exception_ptr object that refers to it. ...
			void rethrow_exception(exception_ptr p);
			template<class E> exception_ptr copy_exception(E e); Effects: as if

				try {
					throw e;
				} catch(...) {
					return current_exception();
				}
			This is particularly useful for transmitting an exception from one thread to another
*/



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			/*
			@CLASS:			ExceptionPropagateHelper
			@DESCRIPTION:	<p>Utility class to support catch and re-throw of exceptions.
							</p>
			*/
			struct	ExceptionPropagateHelper {
				public:
					ExceptionPropagateHelper ();

				public:
					virtual	void	DoRunWithCatchRePropagate (IRunnable* callback);

				public:
					virtual	bool	AnyExceptionCaught () const;
					virtual	void	RethrowIfAnyCaught () const;

				public:
					Memory::SharedPtr<RequiredComponentMissingException>		fRequiredComponentMissingException;
					Memory::SharedPtr<StringException>							fStringException;
					Memory::SharedPtr<IO::FileFormatException>					fFileFormatException;
					Memory::SharedPtr<IO::FileBusyException>					fFileBusyException;
					Memory::SharedPtr<SilentException>							fSilentException;
				#if		qPlatform_Windows
					Memory::SharedPtr<Platform::Windows::HRESULTErrorException>	fHRESULTErrorException;
					Memory::SharedPtr<Platform::Windows::Exception>				fWin32ErrorException;
				#endif
			};





			/*
			@METHOD:		CATCH_AND_CAPTURE_CATCH_BLOCK(CE)
			@DESCRIPTION:	<p>Is given an argument isntance of ExceptionPropagateHelper - and any throws done in the above this use try block will be caught
							in this series of catcehs, and teh values stored in teh CE instance.
							</p>
							<p>This typically doesn't need to be called directly, except when you extend/subclass ExceptionPropagateHelper and provide a new 
							DoRunWithCatchRePropagate ()</p>
			*/
			//#define	CATCH_AND_CAPTURE_CATCH_BLOCK(CE)\
			//	catch (const Stroika::Foundation::Execution::RequiredComponentMissingException& e) {
			//..




		}
	}
}
#endif	/*_Stroia_Foundation_Execution_Propagate_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ExceptionPropagate.inl"

