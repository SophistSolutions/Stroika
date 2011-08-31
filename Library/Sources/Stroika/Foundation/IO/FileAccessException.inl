/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_IO_FileAccessException_inl_
#define	_Stroika_Foundation_IO_FileAccessException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Trace.h"
#include	"../Execution/Exceptions.h"

namespace	Stroika {
	namespace	Foundation	{
		namespace	Execution {

		template	<>
			inline	void	_NoReturn_	DoThrow (const IO::FileAccessException& e2Throw)
				{
					DbgTrace (TSTR ("Throwing FileAccessException: fFileName = '%s'; FileAccessMode=%d"), e2Throw.fFileName.c_str (), e2Throw.fFileAccessMode);
					throw e2Throw;
				}
		}
	}
}
#endif	/*_Stroika_Foundation_IO_FileAccessException_inl_*/
