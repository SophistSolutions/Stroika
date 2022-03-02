/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BagTests.cc,v 1.16 1992/12/04 23:10:01 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BagTests.cc,v $
 *		Revision 1.16  1992/12/04  23:10:01  lewis
 *		*** empty log message ***
 *
 *		Revision 1.15  1992/12/04  18:31:48  lewis
 *		Put back #if qRealTemplatesAvailable stuff - trick with #defines
 *		failed with BCC - not sure why - investigate later...
 *		Also, made helper routines static, and at end of file.
 *
 *		Revision 1.14  1992/12/04  16:05:20  lewis
 *		Renamed OccurencesOf() to TallyOf()
 *
 *		Revision 1.13  1992/12/03  19:23:51  lewis
 *		 Using new macro for Bag(T) expands to Bag<T> as a trial. See if
 *		it works on all targets. If so, we get get rid of a bunch of
 *		ifdefs.
 *
 *		Revision 1.12  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.11  1992/11/15  04:08:12  lewis
 *		Fix a few places where we needed <> for templates, and get
 *		rid of space before '(' in function call to see what it
 *		looks like.
 *
 *		Revision 1.10  1992/10/10  04:39:01  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.9  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.8  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.6  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.4  1992/09/11  13:19:07  lewis
 *		Use endl isntead of newline.
 *
 *		Revision 1.2  1992/09/04  01:10:54  lewis
 *		Deleted extra implements. We get these from CollectionTests.
 *
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"BagTests.hh"


#define		qPrintTimings		!qDebug



#if		qRealTemplatesAvailable
static	void	BagTimings(Bag<UInt32>& s);
static	void	BagIteratorTests(Bag<UInt32>& s);
#else
static	void	BagTimings(Bag(UInt32)& s);
static	void	BagIteratorTests(Bag(UInt32)& s);
#endif


#if		qRealTemplatesAvailable
void	BagTests(Bag<UInt32>& s)
#else
void	BagTests(Bag(UInt32)& s)
#endif
{
	UInt32	three = 3;

#if		qRealTemplatesAvailable
	Bag<UInt32>	s1(s);
#else
	Bag(UInt32)	s1(s);
#endif
	TestCondition(s1 == s);
	TestCondition(s1 == s);
#if		qRealTemplatesAvailable
	Bag<UInt32>	s2 = s1;
#else
	Bag(UInt32)	s2 = s1;
#endif
	TestCondition(s2 == s);
	TestCondition(s2 == s1);

	s2.Add(three);
	TestCondition(s1 == s);
	TestCondition(s2 != s1);
	
	BagIteratorTests(s);

#if		qDebug
	const	size_t	K = 200;
#else
	const	size_t	K = 500;
#endif
	int i;
	
	TestCondition(s.IsEmpty());
	s.Add(three);
	TestCondition(s.GetLength() == 1);
	s += three;
	TestCondition(s.GetLength() == 2);
	TestCondition(s.Contains(three));
	s.Remove(three);
	TestCondition(s.GetLength() == 1);
	s -= three;
	TestCondition(s.IsEmpty());
	s.RemoveAll();
	TestCondition(s.IsEmpty());
	
	for(i = 1; i <= K; i++) {
		s.Add(i);
	}
	BagTimings(s);
	TestCondition(s.IsEmpty());
	
#if		qPrintTimings
	Time t = GetCurrentTime();
	cout << tab << "testing Bag<UInt32>..." << endl;
#endif

	for(i = 1; i <= K; i++) {
		s.Add(i);
		TestCondition(s.Contains(i));
		TestCondition(s.TallyOf(i) == 1);
		TestCondition(s.GetLength() == i);
	}
	for(i = K; i > 0; i--) {
		s -= i;
		TestCondition(not s.Contains(i));
		TestCondition(s.GetLength() ==(i-1));
	}
	TestCondition(s.IsEmpty());

	for(i = 1; i <= K/2; i++) {
		s += 1;
		TestCondition(s.TallyOf(1) == i);
	}
	size_t oldLength = s.GetLength();
	s += s;
	TestCondition(s.GetLength() == oldLength*2);
	s -= s;
	TestCondition(s.GetLength() == 0);
	
#if		qPrintTimings
	t = GetCurrentTime() - t;
	cout << tab << "finished testing Bag<UInt32>; time elapsed = " << t << endl;
#endif
}

#if		qRealTemplatesAvailable
void	BagTests(Bag<SimpleClass>& s)
#else
void	BagTests(Bag(SimpleClass)& s)
#endif
{
	SimpleClass	three = 3;
	
#if		qRealTemplatesAvailable
	Bag<SimpleClass>	s1(s);
#else
	Bag(SimpleClass)	s1(s);
#endif
	TestCondition(s1 == s);
	TestCondition(s1 == s);
#if		qRealTemplatesAvailable
	Bag<SimpleClass>	s2 = s1;
#else
	Bag(SimpleClass)	s2 = s1;
#endif
	TestCondition(s2 == s);
	TestCondition(s2 == s1);
	s2.Add(three);
	TestCondition(s1 == s);
	TestCondition(s2 != s1);
		
	TestCondition(s.IsEmpty());
	s.Add(three);
	TestCondition(s.GetLength() == 1);
	s.Add(three);
	TestCondition(s.GetLength() >= 1);
	TestCondition(s.Contains(three));
	s.Remove(three);
	TestCondition(s.GetLength() <= 1);
	s.RemoveAll();
	TestCondition(s.IsEmpty());
}





#if		qRealTemplatesAvailable
static	void	BagTimings(Bag<UInt32>& s)
#else
static	void	BagTimings(Bag(UInt32)& s)
#endif
{
#if		qPrintTimings
	Time t = GetCurrentTime();
	cout << tab << "testing Bag<UInt32> of length " << s.GetLength() << endl;
#endif

	for(int i = 1; i <= s.GetLength(); i++) {
		TestCondition(s.Contains(i));
		TestCondition(not s.Contains(0));
	}

	for(i = 1; i <= s.GetLength(); i++) {
		ForEach(UInt32, it, s) {
			if(it.Current() == i) {
				break;
			}
		}
	}
	ForEach(UInt32, it, s) {
		ForEach(UInt32, it1, s) {
			s.RemoveAll();
		}
	}
	TestCondition(s.IsEmpty());
	TestCondition(s.GetLength() == 0);
	
	ForEach(UInt32, it1, s) {
		ForEach(UInt32, it2, s) {
			TestCondition(False);
		}
	}

	#if		qPrintTimings
		t = GetCurrentTime() - t;
		cout << tab << "finished testing Bag<UInt32>; time elapsed = " << t << endl;
	#endif
}

#if		qRealTemplatesAvailable
static	void	BagIteratorTests(Bag<UInt32>& s)
#else
static	void	BagIteratorTests(Bag(UInt32)& s)
#endif
{
	const	int	kTestSize	=	100;
	
	TestCondition(s.GetLength() == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for(int i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}

		TestCondition(s.GetLength() == kTestSize);
		
		{
			ForEach(UInt32, it, s) {
				for(i = 1; i <= kTestSize; i++) {
					TestCondition(s.Contains(i));
					TestCondition(s.GetLength() == kTestSize - i + 1);
					s.Remove(i);
					TestCondition(not s.Contains(i-1));
				}
			}
			TestCondition(s.IsEmpty());
			TestCondition(s.GetLength() == 0);
		}

		for(i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}
		TestCondition(s.GetLength() == kTestSize);
		{
			ForEachT(BagMutator, UInt32, it, s) {
				it.RemoveCurrent();
			}
			TestCondition(s.IsEmpty());
			TestCondition(s.GetLength() == 0);
		}

		for(i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}
		TestCondition(s.GetLength() == kTestSize);

		ForEach(UInt32, it2, s) {
			s.Remove(it2.Current());
		}
		TestCondition(s.GetLength() == 0);
	}
	/*
	 * Try removes multiple iterators present.
	 */
	{
		s.RemoveAll();
		TestCondition(s.GetLength() == 0);
		for(int i = 1; i <= kTestSize; i++) {
			s.Add(i);
		}
		TestCondition(s.GetLength() == kTestSize);

		i =	1;
		ForEach(UInt32, it, s) {
			ForEach(UInt32, it2, s) {
				ForEachT(BagMutator, UInt32, it3, s) {
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



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

