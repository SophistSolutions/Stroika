/*
 * Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"
#include	"Stroika/Foundation/Memory/SharedPtr.h"
#include	"Stroika/Foundation/Memory/SharedPtrBase.h"

#include	"../TestHarness/SimpleClass.h"
#include	"../TestHarness/TestHarness.h"





using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;




namespace	{
	void	Test1 ()
		{
			struct	T1 {
				string x;
			};
			struct	T2 : T1 {
			};
			SharedPtr<T1>	t1 (new T1 ());
			SharedPtr<T2>	t2 (new T2 ());
			t1 = t2;
		}
	void	Test2 ()
		{
			struct	TTT : SharedPtrBase {
				string x;
			};
			typedef	SharedPtr<TTT,SharedPtr_SharedPtrBase_Traits<TTT>>	TTT_SP;


			TTT_SP	t1 (new TTT ());
			TTT_SP	t2 (new TTT ());
			t1 = t2;
		}
}


namespace	{

	void	DoRegressionTests_ ()
		{
		    Test1();
		}
}



int main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}



