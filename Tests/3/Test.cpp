/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/Execution/CriticalSection.h"
#include	"Stroika/Foundation/Execution/Event.h"
#include	"Stroika/Foundation/Execution/SimpleThread.h"
#include	"Stroika/Foundation/Execution/Sleep.h"


using	namespace	Stroika::Foundation;


using	Execution::CriticalSection;
using	Execution::AutoCriticalSection;
using	Execution::SimpleThread;


namespace	{
	void	_ASSERT_HANDLER_(const char* fileName, int lineNum)
		{
			cerr << "FAILED: " << fileName << ": " << lineNum << endl;
			_exit (EXIT_FAILURE);
		}
}




namespace	{
	void	RegressionTest1_ ()
		{
			struct	FRED { 
				static	void	DoIt (void* ignored)
					{
						for (int i = 1; i < 10; i++) {
							Execution::Sleep (.1);
						}
					}
			};

			SimpleThread	thread (&FRED::DoIt, "foo");
			thread.Start ();
			thread.WaitForDone ();
		}
}







namespace	{
	CriticalSection	sharedCriticalSection_;
	void	RegressionTest2_ ()
		{

			// Make 2 concurrent threads, which share a critical section object to take turns updating a variable
			struct	FRED { 
				static	void	DoIt (void* ignored)
					{
						int*	argP	=	reinterpret_cast<int*> (ignored);
						for (int i = 0; i < 10; i++) {
							AutoCriticalSection	critSect (sharedCriticalSection_);
							int	tmp = *argP;
							Execution::Sleep (.01);
							//DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
							*argP = tmp + 1;
						}
					}
			};

			int	updaterValue	=	0;
			SimpleThread	thread1 (&FRED::DoIt, &updaterValue);
			SimpleThread	thread2 (&FRED::DoIt, &updaterValue);
			thread1.Start ();
			thread2.Start ();
			thread1.WaitForDone ();
			thread2.WaitForDone ();
			Assert (updaterValue == 2 * 10);
		}
}



namespace	{

	void	DoRegressionTests_ ()
		{
			try {
				RegressionTest1_ ();
				RegressionTest2_ ();
			}
			catch (...) {
				cerr << "FAILED: REGRESSION TEST EXCEPTION" << endl;
				_exit (EXIT_FAILURE);
			}
		}
}




int main(int argc, const char* argv[])
{
#if		defined (_DEBUG)
	Stroika::Foundation::Debug::SetAssertionHandler (_ASSERT_HANDLER_);
#endif
	DoRegressionTests_ ();

	cout << "Succeeded" << endl;
	return EXIT_SUCCESS;
}
