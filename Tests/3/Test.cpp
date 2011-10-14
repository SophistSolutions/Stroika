/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/Execution/CriticalSection.h"
#include	"Stroika/Foundation/Execution/Event.h"
#include	"Stroika/Foundation/Execution/Lockable.h"
#include	"Stroika/Foundation/Execution/Thread.h"
#include	"Stroika/Foundation/Execution/Sleep.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;


using	Execution::CriticalSection;
using	Execution::Lockable;
using	Execution::AutoCriticalSection;
using	Execution::Thread;







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

			Thread	thread (&FRED::DoIt, const_cast<char*> ("foo"));
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
			Thread	thread1 (&FRED::DoIt, &updaterValue);
			Thread	thread2 (&FRED::DoIt, &updaterValue);
			thread1.Start ();
			thread2.Start ();
			thread1.WaitForDone ();
			thread2.WaitForDone ();
			VerifyTestResult (updaterValue == 2 * 10);
		}
}






namespace	{
	Execution::Event	sRegTest3Event_T1_;
	Execution::Event	sRegTest3Event_T2_;
	void	RegressionTest3_ ()
		{

			// Make 2 concurrent threads, which share a critical section object to take turns updating a variable
			struct	FRED1 {
				static	void	DoIt (void* ignored)
					{
						int*	argP	=	reinterpret_cast<int*> (ignored);
						for (int i = 0; i < 10; i++) {
							sRegTest3Event_T1_.Wait ();
							int	tmp = *argP;
							Execution::Sleep (.01);
							//DbgTrace ("FRED1: Updating value in of %d", tmp);
							*argP = tmp + 1;
							sRegTest3Event_T2_.Set ();
						}
					}
			};
			struct	FRED2 {
				static	void	DoIt (void* ignored)
					{
						int*	argP	=	reinterpret_cast<int*> (ignored);
						for (int i = 0; i < 10; i++) {
							sRegTest3Event_T2_.Wait ();
							int	tmp = *argP;
							Execution::Sleep (.01);
							//DbgTrace ("FRED2: Updating value in of %d", tmp);
							*argP = tmp + 1;
							sRegTest3Event_T1_.Set ();
						}
					}
			};

			sRegTest3Event_T1_.Reset ();
			sRegTest3Event_T2_.Reset ();
			int	updaterValue	=	0;
			Thread	thread1 (&FRED1::DoIt, &updaterValue);
			Thread	thread2 (&FRED2::DoIt, &updaterValue);
			thread1.Start ();
			thread2.Start ();
			sRegTest3Event_T1_.Set ();
			thread1.WaitForDone ();
			thread2.WaitForDone ();
			//DbgTrace ("Test3 - updaterValue = %d", updaterValue);
			VerifyTestResult (updaterValue == 2 * 10);
		}
}


namespace	{
	struct	data_ {};
	void	RegressionTest4_Lockable_ ()
		{
			{
				Lockable<data_>	x;
				Lockable<data_>	y = data_ ();
				x = data_ ();
			}
			{
				Lockable<int>	x;
				Lockable<int>	y = 3;
				x = 4;
			}
			{
				// Make 2 concurrent threads, which update a lockable variable
				struct	FRED {
					static	void	DoIt (void* ignored)
						{
							Lockable<int>*	argP	=	reinterpret_cast<Lockable<int>*> (ignored);
							for (int i = 0; i < 10; i++) {
								AutoCriticalSection	critSect (*argP);
								int	tmp = *argP;
								Execution::Sleep (.01);
								//DbgTrace ("Updating value in thread id %d", ::GetCurrentThreadId  ());
								*argP = tmp + 1;
							}
						}
				};
				Lockable<int>	updaterValue	=	0;
				Thread	thread1 (&FRED::DoIt, &updaterValue);
				Thread	thread2 (&FRED::DoIt, &updaterValue);
				thread1.Start ();
				thread2.Start ();
				thread1.WaitForDone ();
				thread2.WaitForDone ();
				VerifyTestResult (updaterValue == 2 * 10);
			}
		}
}



namespace	{

	void	DoRegressionTests_ ()
		{
			RegressionTest1_ ();
			RegressionTest2_ ();
			RegressionTest3_ ();
			RegressionTest4_Lockable_ ();
		}
}



#if qOnlyOneMain
extern  int TestThreads ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

