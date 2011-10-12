/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_SimpleRunnable_h_
#define	_Stroika_Foundation_Execution_SimpleRunnable_h_	1

#include	"../StroikaPreComp.h"

#include	"IRunnable.h"




namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			// Probably want to do variants on this - maybe templated think like STL-style functor... Maybe move to separate files(s)
			class	SimpleRunnable : public IRunnable {
				public:
					SimpleRunnable (void (*fun2CallOnce) ());
					SimpleRunnable (void (*fun2CallOnce) (void* arg), void* arg);

				public:
					virtual	void	Run () override;

				private:
					static	void	FakeZeroArg_ (void* arg);

				private:
					void (*fFun2CallOnce) (void* arg);
					void* fArg;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_SimpleRunnable_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SimpleRunnable.inl"
