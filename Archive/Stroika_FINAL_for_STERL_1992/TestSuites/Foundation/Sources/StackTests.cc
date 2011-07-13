/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/StackTests.cc,v 1.14 1992/12/08 17:22:15 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: StackTests.cc,v $
 *		Revision 1.14  1992/12/08  17:22:15  lewis
 *		*** empty log message ***
 *
 *		Revision 1.13  1992/12/03  06:42:18  lewis
 *		*** empty log message ***
 *
 *		Revision 1.12  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.11  1992/10/19  20:57:25  lewis
 *		#include StreamUtils for tab manipulator.
 *
 *		Revision 1.10  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.9  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.7  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.4  1992/09/04  01:12:46  lewis
 *		Deleted extra implements. We get these from CollectionTests.
 *
 *
 *		
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"StackTests.hh"


#define		qPrintTimings		!qDebug




#if		qRealTemplatesAvailable
void	SimpleTests (Stack<UInt32>& s)
#else
void	SimpleTests (Stack(UInt32)& s)
#endif
{
	int	three = 3;
	
#if		qRealTemplatesAvailable
	Stack<UInt32>	s1 (s);
#else
	Stack(UInt32)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Stack<UInt32>	s2 = s1;
#else
	Stack(UInt32)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Push (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);

	TestCondition (s.IsEmpty ());
	s.Push (three);
	TestCondition (s.Top () == three);
	s.Push (three);
	TestCondition (s.Top () == three);
	s.Pop ();
	TestCondition (s.Top () == three);
	s.Pop ();
	TestCondition (s.IsEmpty ());
	
#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "testing large Stack<UInt32>..." << endl;
#endif

#if		qDebug
	const	K = 5;
#else
	const	K = 1000;
#endif

	int i;
	#if		qDebug
		const	kLoopCount	=	1;
	#else
		const	kLoopCount	=	10;
	#endif
	for (int loop = 1; loop <= kLoopCount; loop++) {
		for (i = 1; i <= K; i++) {
			s.Push (i);
			TestCondition (s.Top () == i);
			s.Pop ();
			s.Push (i);
			TestCondition (s.Top () == i);
			s.Pop ();
			s.Push (i);
			TestCondition (s.Top () == i);
		}
	
		for (i = K; i > 0; i--) {
			TestCondition (s.Top () == i);
			s.Pop ();
		}
		TestCondition (s.IsEmpty ());
		
		for (i = 1; i <= K; i++) {
			s.Push (i);
			TestCondition (s.Top () == i);
			s.Pop ();
			s.Push (i);
			TestCondition (s.Top () == i);
			s.Pop ();
			s.Push (i);
			TestCondition (s.Top () == i);
		}
		for (i = K; i > 0; i--) {
			TestCondition (s.Top () == i);
			s.Pop ();
		}
		TestCondition (s.IsEmpty ());
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing large Stack<UInt32>; time elapsed = " << t << endl;
	cout << tab << "start testing small Stack<UInt32>... " << endl;
	t = GetCurrentTime ();
#endif

	for (loop = 1; loop <= kLoopCount; loop++) {
		for (i = 1; i <= K*5; i++) {
			TestCondition (s.IsEmpty ());
			for (int j = 1; j <= 5; j++) {
				s.Push (j);
			}
			for (j = 1; j <= 5; j++) {
				s.Pop ();
			}
		}
	}

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished testing small Stack<UInt32>; time elapsed = " << t << endl;
	t = GetCurrentTime ();
#endif
}

#if		qRealTemplatesAvailable
void	SimpleTests (Stack<SimpleClass>& s)
#else
void	SimpleTests (Stack(SimpleClass)& s)
#endif
{
	SimpleClass	three = 3;
		
#if		qRealTemplatesAvailable
	Stack<SimpleClass>	s1 (s);
#else
	Stack(SimpleClass)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Stack<SimpleClass>	s2 = s1;
#else
	Stack(SimpleClass)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Push (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);

	TestCondition (s.IsEmpty ());
	s.Push (three);
	TestCondition (s.Top () == three);
	s.Push (three);
	TestCondition (s.Top () == three);
	s.Pop ();
	TestCondition (s.Top () == three);
	s.Pop ();
	TestCondition (s.IsEmpty ());
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

