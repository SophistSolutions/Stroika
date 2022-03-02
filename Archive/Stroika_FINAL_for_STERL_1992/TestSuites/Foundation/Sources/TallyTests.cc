/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TallyTests.cc,v 1.13 1992/12/08 17:21:18 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TallyTests.cc,v $
 *		Revision 1.13  1992/12/08  17:21:18  lewis
 *		*** empty log message ***
 *
 *		Revision 1.12  1992/12/04  20:08:53  lewis
 *		Fix a few typos in template code.
 *
 *		Revision 1.11  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.9  1992/10/23  06:09:58  lewis
 *		Get to compile again.
 *
 *		Revision 1.8  1992/10/19  20:51:30  lewis
 *		got rid of redundent Time declaration.
 *
 *		Revision 1.7  1992/10/16  22:55:28  lewis
 *		Got rid of printfs, and TEntry->TallyEntry rename.
 *
 *		Revision 1.6  1992/10/10  04:42:31  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.5  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.4  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"TallyTests.hh"


#define		qPrintTimings		!qDebug


#if		qRealTemplatesAvailable
void	TallyIteratorTests (Tally<UInt32>& s)
#else
void	TallyIteratorTests (Tally(UInt32)& s)
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

		TestCondition (s.GetLength () == kTestSize);
		
		{
			ForEach (UInt32, it, s) {
				for (i = 1; i <= kTestSize; i++) {
					TestCondition (s.Contains (i));
					TestCondition (s.GetLength () == kTestSize - i + 1);
					s.Remove (i);
					TestCondition (not s.Contains (i-1));
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
			ForEachT (TallyMutator, UInt32, it, s) {
				it.RemoveCurrent ();
			}
			TestCondition (s.IsEmpty ());
			TestCondition (s.GetLength () == 0);
		}

		for (i = 1; i <= kTestSize; i++) {
			s.Add (i);
		}
		TestCondition (s.GetLength () == kTestSize);

#if		qRealTemplatesAvailable
		for (Iterator<TallyEntry<UInt32> > it2 (s); it2.More (); )
#else
		ForEach (TallyEntry(UInt32), it2, s)
#endif
		{
			s.Remove (it2.Current ().fItem);
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
		  ForEachT (TallyMutator, UInt32, it2, s) {
			ForEachT (TallyMutator, UInt32, it3, s) {
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


#if		qRealTemplatesAvailable
void	SimpleTallyTests (Tally<UInt32>& s)
#else
void	SimpleTallyTests (Tally(UInt32)& s)
#endif
{
	UInt32	three = 3;

#if		qRealTemplatesAvailable
	Tally<UInt32>	s1 (s);
#else
	Tally(UInt32)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Tally<UInt32>	s2 = s1;
#else
	Tally(UInt32)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Add (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);
	
	TallyIteratorTests (s);

#if qDebug
	const	size_t	K = 100;
#else
	const	size_t	K = 500;
#endif
	int i;
	
	TestCondition (s.IsEmpty ());
	s.Add (three);
	TestCondition (s.GetLength () == 1);
	s += three;
	TestCondition (s.GetLength () == 1);
	TestCondition (s.Contains (three));
	TestCondition (s.TallyOf (three) == 2);
	s.Remove (three);
	TestCondition (s.GetLength () == 1);
	TestCondition (s.Contains (three));
	TestCondition (s.TallyOf (three) == 1);
	s.Remove (three);
	TestCondition (s.IsEmpty ());
	s.RemoveAll ();
	TestCondition (s.IsEmpty ());
	for (i = 1; i <= K; i++) {
		s.Add (i);
	}

#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "testing Tally<UInt32> of length " << s.GetLength () << endl;
#endif
	for (i = 1; i <= s.GetLength (); i++) {
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
#if		qRealTemplatesAvailable
	for (Iterator<TallyEntry<UInt32> > it (s); it.More (); )
#else
	ForEach (TallyEntry (UInt32), it, s)
#endif
	{
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
	cout << tab << "finished testing Tally<UInt32>; time elapsed = " << t << endl;
#endif

	TestCondition (s.IsEmpty ());
	
#if		qPrintTimings
	t = GetCurrentTime ();
	cout << tab << "testing Tally<UInt32>..." << endl;
#endif

	for (i = 1; i <= K; i++) {
		s.Add (i);
		TestCondition (s.Contains (i));
		TestCondition (s.TallyOf (i) == 1);
		TestCondition (s.GetLength () == i);
	}
	for (i = K; i > 0; i--) {
		s.Remove (i);
		TestCondition (not s.Contains (i));
		TestCondition (s.GetLength () == (i-1));
	}
	TestCondition (s.IsEmpty ());

	for (i = 1; i <= K/2; i++) {
		s += 1;
		TestCondition (s.TallyOf (1) == i);
	}
	size_t oldLength = s.GetLength ();
	size_t oldTotal = s.TotalTally ();
	s += s;
	TestCondition (s.GetLength () == oldLength);
	TestCondition (s.TotalTally () == oldTotal*2);
//	s -= s;
//	TestCondition (s.GetLength () == 0);
	
#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing Tally<UInt32>; time elapsed = " << t << endl;
#endif
}

#if		qRealTemplatesAvailable
void	SimpleTallyTests (Tally<SimpleClass>& s)
#else
void	SimpleTallyTests (Tally(SimpleClass)& s)
#endif
{
	SimpleClass	three = 3;

#if		qRealTemplatesAvailable
	Tally<SimpleClass>	s1 (s);
#else
	Tally(SimpleClass)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Tally<SimpleClass>	s2 = s1;
#else
	Tally(SimpleClass)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Add (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);
		
	TestCondition (s.IsEmpty ());
	s.Add (three);
	TestCondition (s.GetLength () == 1);
	s += three;
	TestCondition (s.GetLength () == 1);
	TestCondition (s.Contains (three));
	TestCondition (s.TallyOf (three) == 2);
	s.Remove (three);
	TestCondition (s.GetLength () == 1);
	TestCondition (s.Contains (three));
	TestCondition (s.TallyOf (three) == 1);
	s.Remove (three);
	TestCondition (s.IsEmpty ());
	s.RemoveAll ();
	TestCondition (s.IsEmpty ());
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

