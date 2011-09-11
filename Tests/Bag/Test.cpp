/*
 * Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>


#include    "Stroika/Foundation/Containers/Bag.h"
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

static	void	BagIteratorTests(Bag<size_t>& s)
{
	const	int	kTestSize	=	100;

	VerifyTestResult(s.GetLength() == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for(int i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}

		VerifyTestResult(s.GetLength() == kTestSize);

		{
			ForEach(size_t, it, s) {
				for(int i = 1; i <= kTestSize; i++) {
					VerifyTestResult(s.Contains(i));
					VerifyTestResult(s.GetLength() == kTestSize - i + 1);
					s.Remove(i);
					VerifyTestResult(not s.Contains(i-1));
				}
			}
			VerifyTestResult(s.IsEmpty());
			VerifyTestResult(s.GetLength() == 0);
		}

		for(int i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}
		VerifyTestResult(s.GetLength() == kTestSize);
		{
			ForEachT(BagMutator, size_t, it, s) {
				it.RemoveCurrent();
			}
			VerifyTestResult(s.IsEmpty());
			VerifyTestResult(s.GetLength() == 0);
		}

		for(int i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}
		VerifyTestResult(s.GetLength() == kTestSize);

		ForEach(size_t, it2, s) {
			s.Remove(it2.Current());
		}
		VerifyTestResult(s.GetLength() == 0);
	}
	/*
	 * Try removes multiple iterators present.
	 */
	{
		s.RemoveAll();
		VerifyTestResult(s.GetLength() == 0);
		for(int i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}
		VerifyTestResult(s.GetLength() == kTestSize);

		int i =	1;
		ForEach(size_t, it, s) {
			ForEach(size_t, it2, s) {
				ForEachT(BagMutator, size_t, it3, s) {
					it3.UpdateCurrent(i);
					it3.RemoveCurrent();
					s.Add(i);
					s.Remove(i);
					i++;
				}
			}
		}
	}
}


static	void	BagTimings(Bag<size_t>& s)

{
#if		qPrintTimings
	Time t = GetCurrentTime();
	cout << tab << "testing Bag<size_t> of length " << s.GetLength() << endl;
#endif

	for(int i = 1; i <= s.GetLength(); i++) {
		VerifyTestResult(s.Contains(i));
		VerifyTestResult(not s.Contains(0));
	}

	for(int i = 1; i <= s.GetLength(); i++) {
		ForEach(size_t, it, s) {
			if(it.Current() == i) {
				break;
			}
		}
	}
	ForEach(size_t, it, s) {
		ForEach(size_t, it1, s) {
			s.RemoveAll();
		}
	}
	VerifyTestResult(s.IsEmpty());
	VerifyTestResult(s.GetLength() == 0);

	ForEach(size_t, it1, s) {
		ForEach(size_t, it2, s) {
			VerifyTestResult(false);
		}
	}

	#if		qPrintTimings
		t = GetCurrentTime() - t;
		cout << tab << "finished testing Bag<size_t>; time elapsed = " << t << endl;
	#endif
}


void	BagTests(Bag<size_t>& s)
{
	size_t	three = 3;

	Bag<size_t>	s1(s);

	VerifyTestResult(s1 == s);
	VerifyTestResult(s1 == s);

	Bag<size_t>	s2 = s1;

	VerifyTestResult(s2 == s);
	VerifyTestResult(s2 == s1);

	s2.Add(three);
	VerifyTestResult(s1 == s);
	VerifyTestResult(s2 != s1);

	BagIteratorTests(s);

#if		qDebug
	const	size_t	K = 200;
#else
	const	size_t	K = 500;
#endif
	int i;

	VerifyTestResult(s.IsEmpty());
	s.Add(three);
	VerifyTestResult(s.GetLength() == 1);
	s += three;
	VerifyTestResult(s.GetLength() == 2);
	VerifyTestResult(s.Contains(three));
	s.Remove(three);
	VerifyTestResult(s.GetLength() == 1);
	s -= three;
	VerifyTestResult(s.IsEmpty());
	s.RemoveAll();
	VerifyTestResult(s.IsEmpty());

	for(i = 1; i <= K; i++) {
		s.Add(i);
	}
	BagTimings(s);
	VerifyTestResult(s.IsEmpty());

#if		qPrintTimings
	Time t = GetCurrentTime();
	cout << tab << "testing Bag<size_t>..." << endl;
#endif

	for(i = 1; i <= K; i++) {
		s.Add(i);
		VerifyTestResult(s.Contains(i));
		VerifyTestResult(s.TallyOf(i) == 1);
		VerifyTestResult(s.GetLength() == i);
	}
	for(i = K; i > 0; i--) {
		s -= i;
		VerifyTestResult(not s.Contains(i));
		VerifyTestResult(s.GetLength() ==(i-1));
	}
	VerifyTestResult(s.IsEmpty());

	for(i = 1; i <= K/2; i++) {
		s += 1;
		VerifyTestResult(s.TallyOf(1) == i);
	}
	size_t oldLength = s.GetLength();
	s += s;
	VerifyTestResult(s.GetLength() == oldLength*2);
	s -= s;
	VerifyTestResult(s.GetLength() == 0);

#if		qPrintTimings
	t = GetCurrentTime() - t;
	cout << tab << "finished testing Bag<size_t>; time elapsed = " << t << endl;
#endif
}

void	BagTests(Bag<SimpleClass>& s)
{
	SimpleClass	three = 3;

	Bag<SimpleClass>	s1(s);

	VerifyTestResult(s1 == s);
	VerifyTestResult(s1 == s);

	Bag<SimpleClass>	s2 = s1;

	VerifyTestResult(s2 == s);
	VerifyTestResult(s2 == s1);
	s2.Add(three);
	VerifyTestResult(s1 == s);
	VerifyTestResult(s2 != s1);

	VerifyTestResult(s.IsEmpty());
	s.Add(three);
	VerifyTestResult(s.GetLength() == 1);
	s.Add(three);
	VerifyTestResult(s.GetLength() >= 1);
	VerifyTestResult(s.Contains(three));
	s.Remove(three);
	VerifyTestResult(s.GetLength() <= 1);
	s.RemoveAll();
	VerifyTestResult(s.IsEmpty());
}

}

namespace	{

	void	DoRegressionTests_ ()
		{
            {
            Bag<size_t>	s;
		    BagTests (s);
            }

            {
 		    Bag<SimpleClass>	s;
            BagTests (s);
            }
		}
}


#if qOnlyOneMain
extern  int Test_Bags ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

