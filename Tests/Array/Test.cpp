/*
 * Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>


#include    "Stroika/Foundation/Containers/Array.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"

#include	"../TestHarness/TestHarness.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;




namespace	{


class	SimpleClass {
	public:
		SimpleClass (size_t v);
		SimpleClass (const SimpleClass& f);
		~SimpleClass ();

		nonvirtual	size_t	GetValue () const;
		static		size_t	GetTotalLiveCount ();

	private:
		size_t	fValue;
		int		fConstructed;
		static	size_t	sTotalLiveObjects;

	friend	bool	operator== (const SimpleClass& lhs, const SimpleClass& rhs);
	friend	bool	operator< (const SimpleClass& lhs, const SimpleClass& rhs);
};
bool	operator== (const SimpleClass& lhs, const SimpleClass& rhs);
bool	operator< (const SimpleClass& lhs, const SimpleClass& rhs);


/*
 ********************************************************************************
 ********************************** SimpleClass *********************************
 ********************************************************************************
 */
static	const	int kFunnyValue = 1234;

size_t	SimpleClass::sTotalLiveObjects	=	0;

SimpleClass::SimpleClass (size_t v) :
	fValue (v),
	fConstructed (kFunnyValue)
{
	sTotalLiveObjects++;
}

SimpleClass::SimpleClass (const SimpleClass& f) :
	fValue (f.fValue),
	fConstructed (kFunnyValue)
{
	sTotalLiveObjects++;
	VerifyTestResult (f.fConstructed == kFunnyValue);
}

SimpleClass::~SimpleClass ()
{
	VerifyTestResult (fConstructed == kFunnyValue);
	VerifyTestResult (sTotalLiveObjects != 0);
	sTotalLiveObjects--;
	fConstructed = 0;
	VerifyTestResult (fConstructed != kFunnyValue);
}

size_t	SimpleClass::GetValue () const
{
	VerifyTestResult (fConstructed == kFunnyValue);
	return (fValue);
}

size_t	SimpleClass::GetTotalLiveCount ()
{
	return (sTotalLiveObjects);
}


/*
 ********************************************************************************
 ******************************* comparison operators  **************************
 ********************************************************************************
 */
bool	operator== (const SimpleClass& lhs, const SimpleClass& rhs)
{
	VerifyTestResult (lhs.fConstructed == kFunnyValue);
	VerifyTestResult (rhs.fConstructed == kFunnyValue);
	return (bool (lhs.fValue == rhs.fValue));
}

bool	operator< (const SimpleClass& lhs, const SimpleClass& rhs)
{
	VerifyTestResult (lhs.fConstructed == kFunnyValue);
	VerifyTestResult (rhs.fConstructed == kFunnyValue);
	return (bool (lhs.fValue < rhs.fValue));
}



}



namespace	{
static	void	Test1()
{
    Array<size_t>	someArray;// if debug on, new C compiler on mac chokes here - should fix better but hack it for now...


	const	size_t	kBigSize	=	1001;

	VerifyTestResult(someArray.GetLength() == 0);
	someArray.SetLength(kBigSize, 0);
	someArray.RemoveAll();
	someArray.SetLength(kBigSize, 0);
	someArray.SetLength(10, 0);
	someArray.SetLength(kBigSize, 0);

	VerifyTestResult(someArray.GetLength() == kBigSize);
	someArray [55] = 55;
	VerifyTestResult(someArray [55] == 55);
	VerifyTestResult(someArray [55] != 56);
	someArray.InsertAt(1, 100);
	VerifyTestResult(someArray.GetLength() == kBigSize+1);
	VerifyTestResult(someArray [100] == 1);

	someArray [101] = -someArray [100];
	someArray.RemoveAt(1);
	VerifyTestResult(someArray [100] == -1);
}

static	void	Test2()
{
	Array<SimpleClass>	someArray;

	const	size_t	kBigSize	=	1000;

	VerifyTestResult(someArray.GetLength() == 0);
	someArray.SetLength(kBigSize, 0);
	someArray.SetLength(0, 0);
	someArray.SetLength(kBigSize, 0);
	someArray.SetLength(10, 0);
	someArray.SetLength(kBigSize, 0);

	VerifyTestResult(someArray.GetLength() == kBigSize);
	someArray [55] = 55;
	VerifyTestResult(someArray [55] == 55);
	VerifyTestResult(not(someArray [55] == 56));

	someArray.RemoveAt(100);
	while(someArray.GetLength() > 0) {
		someArray.RemoveAt(1);
	}
	while(someArray.GetLength() < kBigSize) {
		someArray.InsertAt(1, someArray.GetLength()+1);
	}

	someArray.InsertAt(1, 100);
	VerifyTestResult(someArray.GetLength() == kBigSize+1);
	VerifyTestResult(someArray [100] == 1);
	someArray [101] = 1 + someArray [100].GetValue();
	someArray.RemoveAt(1);
	VerifyTestResult(someArray [100].GetValue() == 2);
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
extern  int Test_Arrays ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}



