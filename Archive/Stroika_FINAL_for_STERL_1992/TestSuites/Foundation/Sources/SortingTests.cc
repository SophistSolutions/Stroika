/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SortingTests.cc,v 1.6 1992/12/08 17:23:19 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SortingTests.cc,v $
 *		Revision 1.6  1992/12/08  17:23:19  lewis
 *		*** empty log message ***
 *
 *		Revision 1.5  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.4  1992/10/15  21:12:46  lewis
 *		Updated for SimpleClass and GenClass stuff.
 *
 *		Revision 1.3  1992/10/10  04:41:32  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.2  1992/09/21  04:31:30  lewis
 *		Move CollectionTests stuff here, got rif of tests for MyIntPtr, and
 *		got working with new collection stuff (no more class collection).
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"SortingTests.hh"


#define		qPrintTimings		!qDebug


#if		qRealTemplatesAvailable
	static	void	TestStillSorted (Sorting<UInt32>& s);
	static	void	TestStillSorted (Sorting<SimpleClass>& s);
	static	void	Timings (Sorting<UInt32>& s);
#else
	static	void	TestStillSorted (Sorting(UInt32)& s);
	static	void	TestStillSorted (Sorting(SimpleClass)& s);
	static	void	Timings (Sorting(UInt32)& s);
#endif


#if		qRealTemplatesAvailable
void	SortingIteratorTests (Sorting<UInt32>& s)
#else
void	SortingIteratorTests (Sorting(UInt32)& s)
#endif
{
	const	int	kTestSize	=	100;
	
	TestCondition (s.GetLength () == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for (int i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestStillSorted (s);

		TestCondition (s.GetLength () == kTestSize);
		
		{
			ForEach (UInt32, it, s) {
				for (i = 1; i <= kTestSize; i++) {
					TestCondition (s.Contains (i));
					TestCondition (s.GetLength () == kTestSize - i + 1);
					s.Remove (i);
					TestCondition (not s.Contains (i-1));
					TestStillSorted (s);
				}
			}
			TestCondition (s.IsEmpty ());
			TestCondition (s.GetLength () == 0);
		}

		for (i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestCondition (s.GetLength () == kTestSize);
		{
			ForEach (UInt32, it, s) {
				s.Remove (it.Current ());
			}
			TestCondition (s.IsEmpty ());
			TestCondition (s.GetLength () == 0);
		}

		for (i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		ForEach (UInt32, it2, s) {
			TestStillSorted (s);
			s.Remove (it2.Current ());
			TestStillSorted (s);
		}
		TestCondition (s.GetLength () == 0);
	}

	/*
	 * Try removes multiple iterators present.
	 */
	{
		s.RemoveAll ();
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
		  ForEach (UInt32, it2, s) {
			ForEach (UInt32, it3, s) {
				 if (s.GetLength () != 0) {
						s.Remove (it3.Current ());
						s.Add (i);
						s.Remove (i);
					}
				}
			}
		}
	}
}


#if		qRealTemplatesAvailable
void	SimpleSortingTests (Sorting<UInt32>& s)
#else
void	SimpleSortingTests (Sorting(UInt32)& s)
#endif
{
	UInt32	three = 3;

#if		qRealTemplatesAvailable
	Sorting<UInt32>	s1 (s);
#else
	Sorting(UInt32)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Sorting<UInt32>	s2 = s1;
#else
	Sorting(UInt32)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Add (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);
	
	SortingIteratorTests (s);

#if		qDebug
	const	size_t	K = 200;
#else
	const	size_t	K = 500;
#endif
	int i;
	
	TestCondition (s.IsEmpty ());
	s.Add (three);
	TestCondition (s.GetLength () == 1);
	s.Add (three);
	TestCondition (s.GetLength () == 2);
	TestCondition (s.Contains (three));
	s.Remove (three);
	TestCondition (s.GetLength () == 1);
	s.Remove (three);
	TestCondition (s.IsEmpty ());
	s.RemoveAll ();
	TestCondition (s.IsEmpty ());
	
	for (i = 1; i <= K; i++) {
		s.Add (i);
	}
	Timings (s);
	TestCondition (s.IsEmpty ());
	
#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "testing Sorting<UInt32>..." << endl;
#endif

	for (i = 1; i <= K; i++) {
		s.Add (i);
		TestCondition (s.Contains (i));
		TestCondition (s.GetLength () == i);
	}
	for (i = K; i > 0; i--) {
		s.Remove (i);
		TestCondition (not s.Contains (i));
		TestCondition (s.GetLength () == (i-1));
	}
	TestCondition (s.IsEmpty ());

	for (i = 1; i <= K/2; i++) {
		s.Add (1);
	}
#if 0
// we dont yet define an Add(Sorting) - maybe we should...
	size_t oldLength = s.GetLength ();
	s.Add (s);
	TestCondition (s.GetLength () == oldLength*2);
	s -= s;
	TestCondition (s.GetLength () == 0);
#endif
#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing Sorting<UInt32>; time elapsed = " << t << endl;
#endif
}

#if		qRealTemplatesAvailable
void	SimpleSortingTests (Sorting<SimpleClass>& s)
#else
void	SimpleSortingTests (Sorting(SimpleClass)& s)
#endif
{
	SimpleClass	three = 3;
	
#if		qRealTemplatesAvailable
	Sorting<SimpleClass>	s1 (s);
#else
	Sorting(SimpleClass)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Sorting<SimpleClass>	s2 = s1;
#else
	Sorting(SimpleClass)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Add (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);
	TestStillSorted (s);

	TestCondition (s.IsEmpty ());
	s.Add (three);
	TestCondition (s.GetLength () == 1);
	s.Add (three);
	TestCondition (s.GetLength () >= 1);
	TestCondition (s.Contains (three));
	s.Remove (three);
	TestCondition (s.GetLength () <= 1);
	s.RemoveAll ();
	TestCondition (s.IsEmpty ());
}

#if		qRealTemplatesAvailable
void	Timings (Sorting<UInt32>& s)
#else
void	Timings (Sorting(UInt32)& s)
#endif
{
#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "testing Sorting<UInt32> of length " << s.GetLength () << endl;
#endif

	for (int i = 1; i <= s.GetLength (); i++) {
		TestCondition (s.Contains (i));
		TestCondition (not s.Contains (0));
	}

	for (i = 1; i <= s.GetLength (); i++) {
		ForEach (UInt32, it, s) {
			if (it.Current () == i) {
				break;
			}
		}
	}
	ForEach (UInt32, it, s) {
		ForEach (UInt32, it1, s) {
			s.RemoveAll ();
		}
	}
	TestCondition (s.IsEmpty ());
	TestCondition (s.GetLength () == 0);
	
	ForEach (UInt32, it1, s) {
		ForEach (UInt32, it2, s) {
			TestCondition (False);
		}
	}

	#if		qPrintTimings
		t = GetCurrentTime () - t;
		cout << tab << "finished testing Sorting<UInt32>; time elapsed = " << t << endl;
	#endif
}







#if		qRealTemplatesAvailable
static	void	TestStillSorted (Sorting<UInt32>& s)
#else
static	void	TestStillSorted (Sorting(UInt32)& s)
#endif
{
	UInt32	prev		=	0;
	Boolean	prevGood	=	False;
	ForEach (UInt32, it, s) {
		if (prevGood) {
			TestCondition (prev <= it.Current ());
		}
		prev = it.Current ();
		prevGood = True;
	}
}

#if		qRealTemplatesAvailable
static	void	TestStillSorted (Sorting<SimpleClass>& s)
#else
static	void	TestStillSorted (Sorting(SimpleClass)& s)
#endif
{
	SimpleClass	prev		=	0;
	Boolean		prevGood	=	False;
	ForEach (SimpleClass, it, s) {
		if (prevGood) {
			TestCondition ((prev < it.Current ()) or (prev == it.Current ()));
		}
		prev = it.Current ();
		prevGood = True;
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

