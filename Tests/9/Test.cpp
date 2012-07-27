/*
* Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
*/
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include    "Stroika/Foundation/Containers/Bag.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"

#include	"../TestHarness/SimpleClass.h"
#include	"../TestHarness/TestHarness.h"

#include    "Stroika/Foundation/Containers/Concrete/Bag_Array.h"
#include    "Stroika/Foundation/Containers/Concrete/Bag_LinkedList.h"


using   namespace   Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;

using	Concrete::Bag_Array;
using	Concrete::Bag_LinkedList;




namespace	{
	namespace	Test1_ {
		void	Test1_BagIteratorTests_ (Bag<size_t>& s)
		{
			const	size_t	kTestSize	= 100;

			const	Bag<size_t>& sCont = s;

			VerifyTestResult(s.GetLength() == 0);
			/*
			* Try removes while iterating forward.
			*/
			{
				for(size_t i = 1; i <= kTestSize; i++) {
					s.Add(i);
					VerifyTestResult(s.Contains(i));
				}

				VerifyTestResult(s.GetLength() == kTestSize);
				For (it, sCont) {
					size_t	oldLength = s.GetLength ();
					VerifyTestResult(s.Contains(it.Current ()));
					VerifyTestResult(s.Contains(s.GetLength ()));
					s.Remove (s.GetLength ());
					VerifyTestResult(s.GetLength () == oldLength-1);
				}

				s.RemoveAll ();
				for(size_t i = 1; i <= kTestSize; i++) {
					s.Add(i);
				}

				{
					For (it, s) {
						for(size_t i = 1; i <= kTestSize; i++) {
							VerifyTestResult(s.Contains(i));
							VerifyTestResult(s.GetLength() == kTestSize - i + 1);
							s.Remove(i);
							VerifyTestResult(not s.Contains(i-1));
							VerifyTestResult(s.GetLength() == kTestSize - i);
						}
					}
					VerifyTestResult(s.IsEmpty());
					VerifyTestResult(s.GetLength() == 0);
				}

				for(size_t i = 1; i <= kTestSize; i++) {
					s.Add(i);
				}
				VerifyTestResult(s.GetLength() == kTestSize);
				{
					For (it, s) {
						it.RemoveCurrent();
					}

					VerifyTestResult(s.IsEmpty());
					VerifyTestResult(s.GetLength() == 0);
				}

				for(size_t i = 1; i <= kTestSize; i++) {
					s.Add(i);
				}
				VerifyTestResult(s.GetLength() == kTestSize);

				For(it2, s) {
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
				for(size_t i = 1; i <= kTestSize; i++) {
					s.Add(i);
				}
				VerifyTestResult(s.GetLength() == kTestSize);

				size_t i =	1;
				For(it, s) {
					For(it2, s) {
						For(it3, s) {
							it3.UpdateCurrent(i);
							it3.RemoveCurrent();
							s.Add(i);
							s.Remove(i);
							i++;
						}
					}
				}
			}

			s.RemoveAll ();
		}

		void	Test1_BagTimings_ (Bag<size_t>& s)
		{
#if		qPrintTimings
			Time t = GetCurrentTime();
			cout << tab << "testing Bag<size_t> of length " << s.GetLength() << endl;
#endif

			for(size_t i = 1; i <= s.GetLength(); i++) {
				VerifyTestResult(s.Contains(i));
				VerifyTestResult(not s.Contains(0));
			}

			for(size_t i = 1; i <= s.GetLength(); i++) {
				For(it, s) {
					if(it.Current() == i) {
						break;
					}
				}
			}
			For(it, s) {
				For(it1, s) {
					s.RemoveAll();
				}
			}
			VerifyTestResult(s.IsEmpty());
			VerifyTestResult(s.GetLength() == 0);

			For(it1, s) {
				For(it2, s) {
					VerifyTestResult(false);
				}
			}

#if		qPrintTimings
			t = GetCurrentTime() - t;
			cout << tab << "finished testing Bag<size_t>; time elapsed = " << t << endl;
#endif
		}

		void	Test1_On_Container_ (Bag<size_t>& s)
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

			Test1_BagIteratorTests_ (s);

#if		qDebug
			const	size_t	K = 200;
#else
			const	size_t	K = 500;
#endif
			size_t i;

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
			Test1_BagTimings_(s);
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

		void	Test1_On_Container_ (Bag<SimpleClass>& s)
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

		template	<typename	CONCRETE_TYPE>
		void	RunBasicBagTestsOnEachConcreteType_ ()
		{
			CONCRETE_TYPE	s;
			Test1_On_Container_ (s);
		}

		void	RunBasicBagTestsOnEachConcreteType_ ()
		{
			RunBasicBagTestsOnEachConcreteType_<Bag<size_t>> ();
			RunBasicBagTestsOnEachConcreteType_<Bag<SimpleClass>> ();
			RunBasicBagTestsOnEachConcreteType_<Bag_LinkedList<size_t>> ();
			RunBasicBagTestsOnEachConcreteType_<Bag_LinkedList<SimpleClass>> ();
			RunBasicBagTestsOnEachConcreteType_<Bag_Array<size_t>> ();
			RunBasicBagTestsOnEachConcreteType_<Bag_Array<SimpleClass>> ();
		}

	}
}





namespace	{
	namespace	Test2_TallyOf_ {


		template	<typename	CONCRETE_TYPE, typename	ELEMENT_TYPE>
		void	SimpleTallyTest_ ()
		{
			CONCRETE_TYPE	bag;
			ELEMENT_TYPE	t1	=	1;
			ELEMENT_TYPE	t2	=	2;
			ELEMENT_TYPE	t3	=	3;
			VerifyTestResult (bag.IsEmpty());
			bag.Add (t1);
			bag.Add (t1);
			VerifyTestResult (not bag.IsEmpty());
			VerifyTestResult (bag.TallyOf (t3) == 0);
			VerifyTestResult (bag.TallyOf (t1) == 2);
			{
				CONCRETE_TYPE	bag2	=	bag;
				VerifyTestResult (bag2.TallyOf (t3) == 0);
				VerifyTestResult (bag2.TallyOf (t1) == 2);
				bag.Add (t1);
				VerifyTestResult (bag2.TallyOf (t1) == 2);
				VerifyTestResult (bag.TallyOf (t1) == 3);
			}
		}

		void	RunBasicBagTestsOnEachConcreteType_ ()
		{
			SimpleTallyTest_<Bag<size_t>,size_t> ();
			SimpleTallyTest_<Bag<SimpleClass>,SimpleClass> ();
			SimpleTallyTest_<Bag_LinkedList<size_t>,size_t> ();
			SimpleTallyTest_<Bag_LinkedList<SimpleClass>,SimpleClass> ();
			SimpleTallyTest_<Bag_Array<size_t>,size_t> ();
			SimpleTallyTest_<Bag_Array<SimpleClass>,SimpleClass> ();
		}

	}
}


namespace	{

	void	DoRegressionTests_ ()
	{
		Test1_::RunBasicBagTestsOnEachConcreteType_ ();
		Test2_TallyOf_::RunBasicBagTestsOnEachConcreteType_ ();
	}

}




int main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

