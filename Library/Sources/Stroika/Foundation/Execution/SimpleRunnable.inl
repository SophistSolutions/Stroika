/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_SimpleRunnable_inl_
#define	_Stroika_Foundation_Execution_SimpleRunnable_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			inline	SimpleRunnable::SimpleRunnable (void (*fun2CallOnce) ())
				: fFun2CallOnce (FakeZeroArg_)
				, fArg (reinterpret_cast<void*> (fun2CallOnce))
				{
				}

			inline	SimpleRunnable::SimpleRunnable (void (*fun2CallOnce) (void* arg), void* arg)
				: fFun2CallOnce (fun2CallOnce)
				, fArg (arg)
				{
				}

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_SimpleRunnable_inl_*/
