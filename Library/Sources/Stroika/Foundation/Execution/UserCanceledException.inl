/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroia_Foundation_Execution_UserCanceledException_inl_
#define	_Stroia_Foundation_Execution_UserCanceledException_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			template	<>
				void	_NoReturn_	DoThrow (const UserCanceledException& e2Throw)
					{
						DbgTrace (TSTR ("Throwing UserCanceledException"));
						throw e2Throw;
					}

		}
	}
}

#endif	/*_Stroia_Foundation_Execution_UserCanceledException_inl_*/
