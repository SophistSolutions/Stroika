/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileBusyException_inl_
#define	_Stroika_Foundation_IO_FileBusyException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Trace.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	IO {

		//	class	FileBusyException
			inline	FileBusyException::FileBusyException (const TString& fileName)
				: fFileName (fileName)
				{
				}

		}
		namespace	Execution {
			template	<>
				inline	void	_NoReturn_	DoThrow (const IO::FileBusyException& e2Throw)
					{
						DbgTrace (TSTR ("Throwing FileBusyException: fFileName = '%s'"), e2Throw.fFileName.c_str ());
						throw e2Throw;
					}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_FileBusyException_inl_*/
