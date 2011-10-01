/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_IRunnable_h_
#define	_Stroika_Foundation_Execution_IRunnable_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Memory/SharedPtr.h"

#include	"CriticalSection.h"
#include	"Exceptions.h"
#include	"Event.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			/// More TODO - Improve use of RUNNABLE CLASS - SEE http://bugzilla/show_bug.cgi?id=746
			class	IRunnable {
				public:
					virtual ~IRunnable ();
				public:
					virtual	void	Run () = 0;
			};


			// Probably want to do variants on this - maybe templated think like STL-style functor... Maybe move to separate files(s)
			class	SimpleRunnable : public IRunnable {
				public:
					SimpleRunnable (void (*fun2CallOnce) (void* arg), void* arg)
						: fFun2CallOnce (fun2CallOnce)
						, fArg (arg)
						{
						}
				public:
					virtual	void	Run () override
						{
							(fFun2CallOnce) (fArg);
						}

			private:
				void (*fFun2CallOnce) (void* arg);
				void* fArg;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_IRunnable_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"IRunnable.inl"
