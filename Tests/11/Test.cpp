/*
 * Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>



#include    "Stroika/Foundation/Containers/Tally.h"
#include    "Stroika/Foundation/Containers/Tally_Array.h"
#include    "Stroika/Foundation/Containers/Tally_LinkedList.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"


#include	"../TestHarness/SimpleClass.h"
#include	"../TestHarness/TestHarness.h"



using   namespace   Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;





namespace	{


static	void	TallyIteratorTests(Tally<size_t>& s)
{
	const	size_t	kTestSize	=	6;

	VerifyTestResult (s.GetLength () == 0);

	For (it, s ) {
		VerifyTestResult (false);
	}

	/*
	 * Try removes while iterating forward.
	 */
	{
		for (size_t i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}


		For (it, Tally<size_t>::Mutator (s)) {
			it.UpdateCount (1);
		}

		VerifyTestResult (s.GetLength () == kTestSize);

		{
			For (it, s) {
				for (size_t i = 1; i <= kTestSize; i++) {
					VerifyTestResult (s.Contains (i));
					VerifyTestResult (s.GetLength () == kTestSize - i + 1);
					s.Remove (i);
					VerifyTestResult (not s.Contains (i-1));
				}
			}
			VerifyTestResult (s.IsEmpty ());
			VerifyTestResult (s.GetLength () == 0);
		}

		for (size_t i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		VerifyTestResult (s.GetLength () == kTestSize);
		{
			For (it, Tally<size_t>::Mutator (s)) {
				it.RemoveCurrent ();
			}
			VerifyTestResult (s.IsEmpty ());
			VerifyTestResult (s.GetLength () == 0);
		}

		for (size_t i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		VerifyTestResult (s.GetLength () == kTestSize);
		For (it2, s) {
			s.Remove (it2.Current ().fItem);
		}
		VerifyTestResult (s.GetLength () == 0);
	}

	/*
	 * Try removes multiple iterators present.
	 */
	{
		s.RemoveAll ();
		VerifyTestResult (s.GetLength () == 0);
		for (size_t i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		VerifyTestResult (s.GetLength () == kTestSize);
		size_t i =	1;

		for (Iterator<TallyEntry<size_t> > it = s.begin (); it != s.end (); ++it) {
			For (it2, Tally<size_t>::Mutator (s)) {
				For (it3, Tally<size_t>::Mutator (s)) {
					if (s.GetLength () != 0) {
						it3.UpdateCount (3);
						it3.RemoveCurrent ();
						s.Add (i);
						s.Remove (i);
					}
				}
			}
		}
	}
}

void	SimpleTallyTests (Tally<size_t>& s)

{
	size_t	three = 3;

	Tally<size_t>	s1 (s);

	VerifyTestResult (s1 == s);
	VerifyTestResult (s1 == s);
	Tally<size_t>	s2 = s1;

	VerifyTestResult (s2 == s);
	VerifyTestResult (s2 == s1);
	s2.Add (three);
	VerifyTestResult (s1 == s);
	VerifyTestResult (s2 != s1);

	TallyIteratorTests (s);

	const	size_t	K = 500;

	VerifyTestResult (s.IsEmpty ());
	s.Add (three);
	VerifyTestResult (s.GetLength () == 1);
	s += three;
	VerifyTestResult (s.GetLength () == 1);
	VerifyTestResult (s.Contains (three));
	VerifyTestResult (s.TallyOf (three) == 2);
	s.Remove (three);
	VerifyTestResult (s.GetLength () == 1);
	VerifyTestResult (s.Contains (three));
	VerifyTestResult (s.TallyOf (three) == 1);
	s.Remove (three);
	VerifyTestResult (s.IsEmpty ());
	s.RemoveAll ();
	VerifyTestResult (s.IsEmpty ());
	for (size_t i = 1; i <= K; i++) {
		s.Add (i);
	}

	for (size_t i = 1; i <= s.GetLength (); i++) {
		VerifyTestResult (s.Contains (i));
		VerifyTestResult (not s.Contains (0));
	}

	for (size_t i = 1; i <= s.GetLength (); i++) {
		For (it, Tally<size_t>::Mutator (s)) {
			if (it.Current ().fItem == i) {
				break;
			}
		}
	}
	For (it, s) {
		For (it1, Tally<size_t>::KeyIterator (s)) {
			s.RemoveAll ();
		}
	}
	VerifyTestResult (s.IsEmpty ());
	VerifyTestResult (s.GetLength () == 0);

	For (it1, s) {
		For (it2, s) {
			VerifyTestResult (false);
		}
	}
	VerifyTestResult (s.IsEmpty ());


	for (size_t i = 1; i <= K; i++) {
		s.Add (i);
		VerifyTestResult (s.Contains (i));
		VerifyTestResult (s.TallyOf (i) == 1);
		VerifyTestResult (s.GetLength () == i);
	}
	for (size_t i = K; i > 0; i--) {
		s.Remove (i);
		VerifyTestResult (not s.Contains (i));
		VerifyTestResult (s.GetLength () == (i-1));
	}
	VerifyTestResult (s.IsEmpty ());

	for (size_t i = 1; i <= K/2; i++) {
		s += 1;
		VerifyTestResult (s.TallyOf (1) == i);
	}
	size_t oldLength = s.GetLength ();
	size_t oldTotal = s.TotalTally ();
	s += s;
	VerifyTestResult (s.GetLength () == oldLength);
	VerifyTestResult (s.TotalTally () == oldTotal*2);
}

void	SimpleTallyTests (Tally<SimpleClass>& s)
{
	SimpleClass	three = 3;

	Tally<SimpleClass>	s1 (s);

	VerifyTestResult (s1 == s);
	VerifyTestResult (s1 == s);
	Tally<SimpleClass>	s2 = s1;

	VerifyTestResult (s2 == s);
	VerifyTestResult (s2 == s1);
	s2.Add (three);
	VerifyTestResult (s1 == s);
	VerifyTestResult (s2 != s1);

	VerifyTestResult (s.IsEmpty ());
	s.Add (three);
	VerifyTestResult (s.GetLength () == 1);
	s += three;
	VerifyTestResult (s.GetLength () == 1);
	VerifyTestResult (s.Contains (three));
	VerifyTestResult (s.TallyOf (three) == 2);
	s.Remove (three);
	VerifyTestResult (s.GetLength () == 1);
	VerifyTestResult (s.Contains (three));
	VerifyTestResult (s.TallyOf (three) == 1);
	s.Remove (three);
	VerifyTestResult (s.IsEmpty ());
	s.RemoveAll ();
	VerifyTestResult (s.IsEmpty ());
}

}


namespace	{

	void	DoRegressionTests_ ()
		{

            {
            Tally_LinkedList<size_t>	s;
			SimpleTallyTests (s);
            }

			{
            Tally_LinkedList<SimpleClass>	s;
			SimpleTallyTests (s);
            }

			{
            Tally_Array<size_t>	s;
			SimpleTallyTests (s);
            }

            {
 		    Tally_Array<SimpleClass>	s;
			SimpleTallyTests (s);
            }

            {
            	// just proof that they can be constructed
				Tally<size_t> t;
				Tally<SimpleClass>	s1;
            }
	}
}


#if qOnlyOneMain
extern  int Test_Tallys ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

