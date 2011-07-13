/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DequeTests.cc,v 1.10 1992/12/04 16:05:46 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DequeTests.cc,v $
 *		Revision 1.10  1992/12/04  16:05:46  lewis
 *		Renamed most of the API - the names were so aweful it was hard
 *		to map them (had to look at old source) from what was done here
 *		to the new names.
 *
 *		Revision 1.9  1992/11/20  18:55:39  lewis
 *		Added a couple of TestConditions () should have minimal speed impact.
 *
 *		Revision 1.8  1992/11/19  05:12:27  lewis
 *		Fix for use of qRealTemplatesAvailable.
 *		Use cout instead of cerr.
 *		Cleanup how loop was done conditionally for qDebug.
 *
 *		Revision 1.7  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.6  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.4  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"DequeTests.hh"


#define		qPrintTimings		!qDebug

#if		qRealTemplatesAvailable
void	DequeIteratorTests (Deque<UInt32>& s)
#else
void	DequeIteratorTests (Deque(UInt32)& s)
#endif
{
	const	int	kTestSize	=	50;
	
	TestCondition (s.GetLength () == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for (int i = 1; i <= kTestSize; i++) {
			s.AddTail (i);
		}

		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
			TestCondition (s.GetLength () == kTestSize - i + 1);
			UInt32 oldElement = s.RemoveHead ();
			TestCondition (oldElement == i);
			i++;
			TestCondition (s.GetLength () == kTestSize - i + 1);
		}
		TestCondition (s.IsEmpty ());
		TestCondition (s.GetLength () == 0);

		for (i = 1; i <= kTestSize; i++) {
			s.AddHead (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		ForEach (UInt32, it2, s) {
			s.RemoveHead ();
		}
		TestCondition (s.GetLength () == 0);
	}

	/*
	 * Try removes multiple iterators present.
	 */
	{
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.AddHead (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
		  ForEach (UInt32, it2, s) {
			ForEach (UInt32, it3, s) {
				 if (s.GetLength () != 0) {
						UInt32 element = s.RemoveHead ();
						s.AddHead (i);
						element = s.RemoveHead ();
					}
				}
			}
		}
	}
}


#if		qRealTemplatesAvailable
static	void	DumpDeque (Deque<UInt32>& s)
#else
static	void	DumpDeque (Deque(UInt32)& s)
#endif
{
	cout << tab << "DQ:";
	ForEach (UInt32, it, s) {
		cout << " " << it.Current ();	
	}
	cout << endl;	
}


#if		qRealTemplatesAvailable
void	SimpleDequeTests (Deque<UInt32>& s)
#else
void	SimpleDequeTests (Deque(UInt32)& s)
#endif
{
	int	three = 3;
	
#if		qRealTemplatesAvailable
	Deque<UInt32>	s1 (s);
#else
	Deque(UInt32)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Deque<UInt32>	s2 = s1;
#else
	Deque(UInt32)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.AddTail (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);

	TestCondition (s.IsEmpty ());
	s.AddTail (three);
	TestCondition (s.Head () == three);
	s.AddTail (three);
	TestCondition (s.Head () == three);
	TestCondition (s.RemoveHead () == three);
	TestCondition (s.RemoveHead () == three);
	TestCondition (s.IsEmpty ());
	s.AddTail (1);
	s.AddTail (2);
	s.AddTail (3);
	s.AddTail (4);
	TestCondition (s.RemoveHead () == 1);
	TestCondition (s.RemoveHead () == 2);
	s.AddTail (5);
	s.AddTail (6);
	TestCondition (s.RemoveHead () == 3);
	TestCondition (s.RemoveHead () == 4);
	s.AddTail (1);
	s.AddTail (2);
	s.AddTail (3);
	s.AddTail (4);
	TestCondition (s.RemoveHead () == 5);
	TestCondition (s.RemoveHead () == 6);
	TestCondition (s.RemoveHead () == 1);
	TestCondition (s.RemoveHead () == 2);
	TestCondition (s.RemoveHead () == 3);
	TestCondition (s.RemoveHead () == 4);
	TestCondition (s.IsEmpty ());
#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "testing small Deque<UInt32>..." << endl;
#endif

#if		qDebug
	const	K = 10;
#else
	const	K = 1000;
#endif
	#if		qDebug
		const	kLoopCount	=	1;
	#else
		const	kLoopCount	=	10;
	#endif
	for (int loop = 1; loop <= kLoopCount; loop++) {
		TestCondition (s.IsEmpty ());
		for (int i = 1; i <= K*5; i++) {
			TestCondition (s.IsEmpty ());
			for (int j = 1; j <= 10; j++) {
				s.AddTail (j);
			}
			for (j = 1; j <= 10; j++) {
				TestCondition (s.RemoveHead () == j);
			}
			for (j = 1; j <= 10; j++) {
				s.AddHead (j);
			}
			for (j = 1; j <= 10; j++) {
				TestCondition (s.RemoveTail () == j);
			}
		}
		TestCondition (s.IsEmpty ());
		for (i = 1; i <= 100; i++) {
			for (int j = 1; j <= 10; j++) {
				s.AddHead (j);
			}
			for (j = 1; j <= 9; j++) {
				s.RemoveHead ();
			}
		}
	
		while (not s.IsEmpty ()) {
			s.RemoveHead ();
		}
		TestCondition (s.IsEmpty ());
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing small Deque<UInt32>...; time elapsed = " << t << endl;
	cout << tab << "start testing large Deque<UInt32>" << endl;
	t = GetCurrentTime ();
#endif

	TestCondition (s.IsEmpty ());
	for (loop = 1; loop <= kLoopCount; loop++) {
		for (int i = 1; i <= K; i++) {
			s.AddTail (i);
			TestCondition (s.Head () == 1);
		}
		for (i = 1; i <= K; i++) {
			TestCondition (s.Head () == i);
			TestCondition (s.RemoveHead () == i);
			TestCondition ((i == K) or (s.Head () == (i+1)));
		}
		TestCondition (s.IsEmpty ());
		for (i = 1; i <= K; i++) {
			s.AddHead (i);
			TestCondition (s.Tail () == 1);
		}
		for (i = 1; i <= K; i++) {
			TestCondition (s.Tail () == i);
			TestCondition (s.RemoveTail () == i);
		}
		TestCondition (s.IsEmpty ());
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing large Deque<UInt32>; time elapsed = " << t << endl;
	t = GetCurrentTime ();
#endif

	DequeIteratorTests (s);
}

#if		qRealTemplatesAvailable
void	SimpleDequeTests (Deque<SimpleClass>& s)
#else
void	SimpleDequeTests (Deque(SimpleClass)& s)
#endif
{
	SimpleClass	three = 3;
	
#if		qRealTemplatesAvailable
	Deque<SimpleClass>	s1 (s);
#else
	Deque(SimpleClass)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Deque<SimpleClass>	s2 = s1;
#else
	Deque(SimpleClass)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.AddTail (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);

	TestCondition (s.IsEmpty ());
	s.AddHead (three);
	TestCondition (s.Head () == three);
	s.AddHead (three);
	TestCondition (s.Head () == three);
	TestCondition (s.Tail () == three);
	TestCondition (s.RemoveHead () == three);
	TestCondition (s.RemoveHead () == three);
	TestCondition (s.IsEmpty ());
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

