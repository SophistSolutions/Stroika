/*
 * Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>


#include    "Stroika/Foundation/Containers/Private/LinkedList.h"

#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"

#include	"../TestHarness/SimpleClass.h"
#include	"../TestHarness/TestHarness.h"





using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Containers::Private;




namespace	{
static	void	Test1()
{
	LinkedList_Patch<size_t>	someLL;
	const	size_t	kBigSize	=	1001;



	Assert (kBigSize > 100);
	VerifyTestResult(someLL.GetLength() == 0);
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }
	someLL.RemoveAll();
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }
	{ for (size_t i = 1; i <= kBigSize-10; i++) { someLL.RemoveFirst (); } }
	someLL.RemoveAll();														//	someLL.SetLength(kBigSize, 0);
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }


	VerifyTestResult(someLL.GetLength() == kBigSize);
	someLL.SetAt (55, 55);																										//	someLL [55] = 55;
	VerifyTestResult (someLL.GetAt (55) == 55);																					//	VerifyTestResult(someArray [55] == 55);
	VerifyTestResult (someLL.GetAt (55) != 56);																					//	VerifyTestResult(someArray [55] != 56);
	{ size_t i = 1; size_t cur; for (LinkedListMutator_Patch<size_t> it (someLL); it.More(&cur, true); i++) { if (i==100) {it.AddAfter (1); break;} } }//	someLL.InsertAt(1, 100);

	VerifyTestResult(someLL.GetLength() == kBigSize+1);
	VerifyTestResult (someLL.GetAt (100) == 1);																					//	VerifyTestResult(someArray [100] == 1);

	someLL.SetAt (someLL.GetAt (100) + 5, 101);

	VerifyTestResult (someLL.GetAt (101) == 6);
	someLL.RemoveFirst ();
	VerifyTestResult (someLL.GetAt (100) == 6);
}

static	void	Test2()
{
	LinkedList_Patch<SimpleClass>	someLL;
	const	size_t	kBigSize	=	1000;

	VerifyTestResult(someLL.GetLength() == 0);

	Assert (kBigSize > 10);
	VerifyTestResult(someLL.GetLength() == 0);
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }
	someLL.RemoveAll();
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }
	{ for (size_t i = 1; i <= kBigSize-10; i++) { someLL.RemoveFirst (); } }
	someLL.RemoveAll();														//	someLL.SetLength(kBigSize, 0);
	{ for (size_t i = 1; i <= kBigSize; i++) { someLL.Prepend (0); } }


	VerifyTestResult(someLL.GetLength() == kBigSize);

	someLL.SetAt (55, 55);																										//	someLL [55] = 55;
	VerifyTestResult(someLL.GetAt (55) == 55);
	VerifyTestResult(not(someLL.GetAt(55) == 56));

	someLL.RemoveAll ();
	VerifyTestResult(someLL.GetLength() == 0);

	for (size_t i = kBigSize; i >= 1; --i) {
		VerifyTestResult(not someLL.Contains(i));
		someLL.Prepend(i);
		VerifyTestResult(someLL.GetFirst () == i);
		VerifyTestResult(someLL.Contains(i));
	}
	for (size_t i = 1; i <= kBigSize; ++i) {
		VerifyTestResult(someLL.GetFirst () == i);
		someLL.RemoveFirst ();
		VerifyTestResult(not someLL.Contains(i));
	}
	VerifyTestResult(someLL.GetLength() == 0);

	for (size_t i = kBigSize; i >= 1; --i) {
		someLL.Prepend(i);
	}
	for (size_t i = kBigSize; i >= 1; --i) {
//cerr << "i, getat(i-1) = " << i << ", " << someLL.GetAt (i-1).GetValue () << endl;
		VerifyTestResult(someLL.GetAt(i-1) == i);
	}
}

}


namespace	{

	void	DoRegressionTests_ ()
		{
		    Test1();
		    Test2();
		}
}



#if qOnlyOneMain
extern  int Test_LinkedList ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}



