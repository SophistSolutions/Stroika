/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_SimpleRunnable_h_
#define	_Stroika_Foundation_Execution_SimpleRunnable_h_	1

#include	"../StroikaPreComp.h"

#include	"../Memory/BlockAllocated.h"

#include	"IRunnable.h"


/*
 * TODO:
 *		o	Probably want to do variants on this - maybe templated think like STL-style functor...
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			/*
			 * SimpleRunnable is an easy way to map a simple object or function (optionally with args) to an IRunnable - the
			 * basic type used in threading and background processing classes.
			 */
			class	SimpleRunnable : public IRunnable {
				public:
					SimpleRunnable (void (*fun2CallOnce) ());
					SimpleRunnable (void (*fun2CallOnce) (void* arg), void* arg);

				public:
					virtual	void	Run () override;

				public:
					static	Memory::SharedPtr<IRunnable>	MAKE (void (*fun2CallOnce) ());
					static	Memory::SharedPtr<IRunnable>	MAKE (void (*fun2CallOnce) (void* arg), void* arg);

				private:
					static	void	FakeZeroArg_ (void* arg);

				private:
					void (*fFun2CallOnce) (void* arg);
					void* fArg;

				public:
					DECLARE_USE_BLOCK_ALLOCATION(SimpleRunnable);
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
