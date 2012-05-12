/*
 * Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/Memory/SharedPtr.h"

#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"

#include	"../TestHarness/SimpleClass.h"
#include	"../TestHarness/TestHarness.h"





using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;




namespace	{
	struct	T1 {
		string x;
	};
	struct	T2 : T1 {
	};
	void	Test1 ()
	{
		SharedPtr<T1>	t1 (new T1 ());
		SharedPtr<T2>	t2 (new T2 ());

		// tricky - uses special CTOR, but other way should fail
#if 1
		t1 = t2;
#endif
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



