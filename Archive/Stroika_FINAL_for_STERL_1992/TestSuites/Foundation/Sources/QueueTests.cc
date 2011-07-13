/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/QueueTests.cc,v 1.12 1992/12/08 17:20:22 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: QueueTests.cc,v $
 *		Revision 1.12  1992/12/08  17:20:22  lewis
 *		*** empty log message ***
 *
 *		Revision 1.11  1992/12/04  19:47:31  lewis
 *		Fix <> for qRealTemplates for DumpQueue, and put static helper
 *		functions at the end of the file.
 *
 *		Revision 1.10  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.9  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.8  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.6  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cout - use cout.
 *
 *		Revision 1.3  1992/09/04  21:19:20  sterling
 *		If debug on, use smaller K for test. And implemnted helper DumpQueue.
 *
 *		Revision 1.2  1992/09/04  01:12:11  lewis
 *		Deleted extra implements. We get these from CollectionTests.
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"QueueTests.hh"


#define		qPrintTimings		!qDebug


#if		qRealTemplatesAvailable
static	void	QueueIteratorTests (Queue<UInt32>& s);
static	void	DumpQueue (Queue<UInt32>& s);
#else
static	void	QueueIteratorTests (Queue(UInt32)& s);
static	void	DumpQueue (Queue(UInt32)& s);
#endif



#if		qRealTemplatesAvailable
void	SimpleQueueTests (Queue<UInt32>& s)
#else
void	SimpleQueueTests (Queue(UInt32)& s)
#endif
{
	int	three = 3;
	
#if		qRealTemplatesAvailable
	Queue<UInt32>	s1 (s);
#else
	Queue(UInt32)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Queue<UInt32>	s2 = s1;
#else
	Queue(UInt32)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Enqueue (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);

	TestCondition (s.IsEmpty ());
	s.Enqueue (three);
	TestCondition (s.Head () == three);
	s.Enqueue (three);
	TestCondition (s.Head () == three);
	TestCondition (s.Dequeue () == three);
	TestCondition (s.Dequeue () == three);
	TestCondition (s.IsEmpty ());
	s.Enqueue (1);
	s.Enqueue (2);
	s.Enqueue (3);
	s.Enqueue (4);
	TestCondition (s.Dequeue () == 1);
	TestCondition (s.Dequeue () == 2);
	s.Enqueue (5);
	s.Enqueue (6);
	TestCondition (s.Dequeue () == 3);
	TestCondition (s.Dequeue () == 4);
	s.Enqueue (1);
	s.Enqueue (2);
	s.Enqueue (3);
	s.Enqueue (4);
	TestCondition (s.Dequeue () == 5);
	TestCondition (s.Dequeue () == 6);
	TestCondition (s.Dequeue () == 1);
	TestCondition (s.Dequeue () == 2);
	TestCondition (s.Dequeue () == 3);
	TestCondition (s.Dequeue () == 4);
	TestCondition (s.IsEmpty ());
#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "testing small Queue<UInt32>..." << endl;
#endif

#if qDebug
	const	K = 10;
#else
	const	K = 1000;
#endif
	int i;

	#if		qDebug
		const	kLoopCount	=	1;
	#else
		const 	kLoopCount	=	10;
	#endif
	for (int loop = 1; loop <= kLoopCount; loop++) {
		TestCondition (s.IsEmpty ());
		for (i = 1; i <= K*5; i++) {
			TestCondition (s.IsEmpty ());
			for (int j = 1; j <= 10; j++) {
				s.Enqueue (j);
			}
			for (j = 1; j <= 10; j++) {
				s.Dequeue ();
			}
		}
		TestCondition (s.IsEmpty ());
		for (i = 1; i <= 100; i++) {
			for (int j = 1; j <= 10; j++) {
				s.Enqueue (j);
			}
			for (j = 1; j <= 9; j++) {
				s.Dequeue ();
			}
		}
	
		while (not s.IsEmpty ()) {
			s.Dequeue ();
		}
		TestCondition (s.IsEmpty ());
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing small Queue<UInt32>...; time elapsed = " << t << endl;
	cout << tab << "start testing large Queue<UInt32>" << endl;
	t = GetCurrentTime ();
#endif

	for (loop = 1; loop <= kLoopCount; loop++) {
		for (i = 1; i <= K; i++) {
			s.Enqueue (i);
			TestCondition (s.Head () == 1);
		}
		for (i = 1; i <= K; i++) {
			TestCondition (s.Head () == i);
			TestCondition (s.Dequeue () == i);
			TestCondition ((i == K) or (s.Head () == (i+1)));
		}
		TestCondition (s.IsEmpty ());
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing large Queue<UInt32>; time elapsed = " << t << endl;
	t = GetCurrentTime ();
#endif

	QueueIteratorTests (s);
}

#if		qRealTemplatesAvailable
void	SimpleQueueTests (Queue<SimpleClass>& s)
#else
void	SimpleQueueTests (Queue(SimpleClass)& s)
#endif
{
	SimpleClass	three = 3;
	
#if		qRealTemplatesAvailable
	Queue<SimpleClass>	s1 (s);
#else
	Queue(SimpleClass)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Queue<SimpleClass>	s2 = s1;
#else
	Queue(SimpleClass)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Enqueue (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);

	TestCondition (s.IsEmpty ());
	s.Enqueue (three);
	TestCondition (s.Head () == three);
	s.Enqueue (three);
	TestCondition (s.Head () == three);
	TestCondition (s.Dequeue () == three);
	TestCondition (s.Dequeue () == three);
	TestCondition (s.IsEmpty ());
}



#if		qRealTemplatesAvailable
static	void	QueueIteratorTests (Queue<UInt32>& s)
#else
static	void	QueueIteratorTests (Queue(UInt32)& s)
#endif
{
	const	int	kTestSize	=	50;
	
	TestCondition (s.GetLength () == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for (int i = 1; i <= kTestSize; i++) {
			s.Enqueue (i);
		}

		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
//			TestCondition (s.Contains (i));
			TestCondition (s.GetLength () == kTestSize - i + 1);
			UInt32 oldElement = s.Dequeue ();
			TestCondition (oldElement == i);
			i++;
			TestCondition (s.GetLength () == kTestSize - i + 1);
//			TestCondition (not s.Contains (i-1));
		}
		TestCondition (s.IsEmpty ());
		TestCondition (s.GetLength () == 0);

		for (i = 1; i <= kTestSize; i++) {
//			TestCondition (not s.Contains (i));
		}
		for (i = 1; i <= kTestSize; i++) {
			s.Enqueue (i);
		}
		TestCondition (s.GetLength () == kTestSize);
		for (i = 1; i <= kTestSize; i++) {
//			TestCondition (s.Contains (i));
		}

		ForEach (UInt32, it2, s) {
			s.Dequeue ();
		}
		TestCondition (s.GetLength () == 0);
	}
	/*
	 * Try removes multiple iterators present.
	 */
	{
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.Enqueue (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
		  ForEach (UInt32, it2, s) {
			ForEach (UInt32, it3, s) {
					UInt32 element = s.Dequeue ();
					s.Enqueue (i);
					element = s.Dequeue ();
				}
			}
		}
	}
}

#if		qRealTemplatesAvailable
static	void	DumpQueue (Queue<UInt32>& s)
#else
static	void	DumpQueue (Queue(UInt32)& s)
#endif
{
	cout << tab << "Q:";
	ForEach (UInt32, it, s) {
		cout << " " << it.Current ();	
	}
	cout << endl;	
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

