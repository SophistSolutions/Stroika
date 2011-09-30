/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_ErrNoException_inl_
#define	_Stroia_Foundation_Execution_ErrNoException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
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


			inline	void	ThrowErrNoIfNegative (int returnCode)
				{
					if (returnCode < 0) {
						errno_ErrorException::DoThrow (errno);
					}
				}


			template	<>
				inline	void	_NoReturn_	DoThrow (const errno_ErrorException& e2Throw)
					{
						// Go directly through class DoThrow() since that may remap to differnt kinds of exceptions, and already has trace messages
						errno_ErrorException::DoThrow (e2Throw);
					}
			inline	void	ThrowIfError_errno_t (errno_t e)
				{
					if (e != 0) {
						errno_ErrorException::DoThrow (e);
					}
				}

		}
	}
}

#endif	/*_Stroia_Foundation_Execution_ErrNoException_inl_*/
