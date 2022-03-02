/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SequenceTests.cc,v 1.14 1992/11/26 02:54:03 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SequenceTests.cc,v $
 *		Revision 1.14  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.13  1992/11/16  21:46:51  lewis
 *		Got rid of unused UInt32CmpFunct.
 *		Cleanup template usage.
 *		1 assert -> TestCondition.
 *
 *		Revision 1.11  1992/10/10  04:40:42  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.10  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.9  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.6  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cout - use cout.
 *
 *		Revision 1.4  1992/06/11  00:08:03  lewis
 *		Got rid of sort stuff - redo later with different approach.
 *
 *		Revision 1.3  92/06/09  22:22:07  22:22:07  lewis (Lewis Pringle)
 *		Changed SequenceForEach to ForEachS.
 *		
 *		Revision 1.2  92/05/10  00:50:49  00:50:49  lewis (Lewis Pringle)
 *		Get working on PC/BC++/templates.
 *		
 *		Revision 1.6  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cout instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.5  1992/01/22  04:10:28  lewis
 *		Cleanup code and add test for sorting.
 *
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"


#include	"SequenceTests.hh"


#define		qPrintProgress	0
#define		qPrintTimings	!qDebug


#if		qRealTemplatesAvailable
void	SequenceTimings (Sequence<UInt32>& s)
#else
void	SequenceTimings (Sequence(UInt32)& s)
#endif
{
#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "testing Sequence<UInt32> of length " << s.GetLength () << endl;
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
		cout << tab << "finished testing Sequence<UInt32>; time elapsed = " << t << endl;
	#endif
}

#if		qRealTemplatesAvailable
static	void	TestSimpleIteration (Sequence<UInt32>& s)
#else
static	void	TestSimpleIteration (Sequence(UInt32)& s)
#endif
{
    UInt32 index = 0;
	{
		ForEach (UInt32, it, s) {
			index++;
			TestCondition (it.Current () == index);
			TestCondition (s.IndexOf (it.Current ()) == index);
		}
	}
	TestCondition (s.GetLength () == index);

	{
		ForEach (UInt32, it, s.MakeSequenceIterator (eSequenceBackward)) {
			TestCondition (it.Current () == index);
			TestCondition (s.IndexOf (it.Current ()) == index);
			index--;
		}
	}
}

#if		qRealTemplatesAvailable
void	SimpleSequenceTimings (Sequence<UInt32>& s)
#else
void	SimpleSequenceTimings (Sequence(UInt32)& s)
#endif
{

#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "timing Sequence<UInt32>..." << endl;
#endif

	const	K = 500;
	for (int i = 1; i <= K; i++) {
		s.Append (i);
		TestCondition (s.Contains (i));
		TestCondition (s.GetLength () == i);
	}
	for (i = K; i > 0; i--) {
		s.Remove (i);
		TestCondition (not s.Contains (i));
		TestCondition (s.GetLength () == (i-1));
	}
	for (i = 1; i <= K; i++) {
		s.Append (i);
	}
	i = 0;
	ForEach (UInt32, it, s) {
		TestCondition (++i == it.Current ());
	}
	s.RemoveAll ();
	TestCondition (s.IsEmpty ());

#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished timing Sequence<UInt32>; time elapsed = " << t << endl;
#endif
}

#if		qRealTemplatesAvailable
void	SequenceIteratorTests (Sequence<UInt32>& s)
#else
void	SequenceIteratorTests (Sequence(UInt32)& s)
#endif
{
	const	int	kTestSize	=	5;

	TestCondition (s.GetLength () == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for (int i = 1; i <= kTestSize; i++) {
			s.Append (i);
		}

		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
			TestCondition (s.Contains (i));
			TestCondition (s.GetLength () == kTestSize - i + 1);
			s.Remove (i++);
			TestCondition (s.GetLength () == kTestSize - i + 1);
			TestCondition (not s.Contains (i-1));
			if (i == kTestSize+1) {
				TestCondition (s.IsEmpty ());
			}
		}
		TestCondition (s.IsEmpty ());
		TestCondition (s.GetLength () == 0);

		for (i = 1; i <= kTestSize; i++) {
			s.Append (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	kTestSize;
		ForEach (UInt32, it1, s) {
			TestCondition (s.Contains (i));
			s.Remove (i--);
		}
		TestCondition (s.GetLength () == kTestSize/2);
		
		s.RemoveAll ();
		for (i = 1; i <= kTestSize; i++) {
			s.Append (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		ForEach (UInt32, it2, s) {
			s.Remove (it2.Current ());
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
			s.Append (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
			ForEachT (SequenceIterator, UInt32, it2, s.MakeSequenceIterator (eSequenceBackward)) {
				ForEachT (SequenceMutator, UInt32, it3, s.MakeSequenceMutator (eSequenceBackward)) {
					TestCondition (it2.GetDirection () == eSequenceBackward);
					TestCondition (it3.GetDirection () == eSequenceBackward);

					TestCondition (s.GetLength () > 0);

					it3.RemoveCurrent ();
					s.Append (i);
					s.Remove (i);
				}
			}
		}
	}
}

#if		qRealTemplatesAvailable
void	SimpleTests (Sequence<UInt32>& s)
#else
void	SimpleTests (Sequence(UInt32)& s)
#endif
{
	UInt32	three = 3;
	
#if		qRealTemplatesAvailable
	Sequence<UInt32>	s1 (s);
#else
	Sequence(UInt32)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Sequence<UInt32>	s2 = s1;
#else
	Sequence(UInt32)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Append (three);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);
	
	TestCondition (s.IsEmpty ());

	s.Append (three);
	TestCondition (s.GetLength () == 1);
	s.Append (4);
	TestCondition (s.GetLength () == 2);
	TestCondition (s.Contains (three));
	s.Remove (three);
	TestCondition (s.GetLength () == 1);
	s.RemoveAll ();
	TestCondition (s.IsEmpty ());
	s.Append (1);
	TestCondition (s.Contains (1));
	TestCondition (not s.Contains (3));
	s.Append (2);
	TestCondition (s.Contains (2));
	TestCondition (s.Contains (1));
	TestCondition (not s.Contains (3));
	TestCondition (s.GetLength () == 2);

	s.Remove (2);
	TestCondition (s.GetLength () == 1);

	s.RemoveAll ();

	const int kListLength = 50;

	size_t	count;
	for (count = 1; count <= kListLength; count++) {
	    s.Append (count);
	}
	SequenceTimings (s);
	s.RemoveAll ();
	

#if		qPrintProgress
	cout << "Simple tests of Sequence of UInt32..." << endl;
	cout << "testing empty sequence" << endl;
#endif

	TestCondition (s.GetLength () == 0);
	{
    	ForEach (UInt32, it, s.MakeSequenceIterator (eSequenceBackward)) {
			TestCondition (False);
		}
	}
	{
		ForEach (UInt32, it, s.MakeSequenceIterator (eSequenceForward)) {
			TestCondition (False);
		}
	}

#if		qPrintProgress
	cout << "testing iteration with appends" << endl;
#endif
	for (count = 1; count <= kListLength; count++) {
	    s.Append (count);
	    TestSimpleIteration (s);
	}

#if		qPrintProgress
	cout << "testing remove" << endl;
#endif
	TestCondition (s.GetLength () == kListLength);
	for (count = 1; count <= kListLength; count++) {
		s.Remove (count);
		TestCondition (s.GetLength () == (kListLength - count));
	}
	TestCondition (s.GetLength () == 0);

#if		qPrintProgress
	cout << "testing iteration with prepends" << endl;
#endif
	for (count = kListLength; count >= 1; count--) {
		s.Prepend (count);
	}
	TestSimpleIteration (s);
 
#if		qPrintProgress
	cout << "testing remove all" << endl;
#endif
	TestCondition (s.GetLength () == kListLength);
	s.RemoveAll ();
	TestCondition (s.GetLength () == 0);

#if		qPrintProgress
	cout << "testing array indexing with appends" << endl;
#endif
	for (count = 1; count <= kListLength; count++) {
		s.Append (count);
	    TestCondition (s[count] == count);
	} 
	s.RemoveAll ();
	TestCondition (s.GetLength () == 0);

	for (count = 1; count <= kListLength; count++) {
		s.Append ((UInt32)1);
	} 
	{
	    count = 0;
	    ForEach (UInt32, it, s) {
			count++;
		}
		TestCondition (count == kListLength);
	}
	for (count = kListLength; count >= 1; count--) {
		s.Remove (1);
	} 
	TestCondition (s.GetLength () == 0);

#if		qPrintProgress
	cout << "Finished simple tests of Sequence of UInt32..." << endl;
#endif

	SequenceIteratorTests (s);
	SimpleSequenceTimings (s);
}




#if		qRealTemplatesAvailable
void	Test_Deletions_During_Iteration (Sequence<UInt32>& s)
#else
void	Test_Deletions_During_Iteration (Sequence(UInt32)& s)
#endif
{
	SequenceIteratorTests (s);
	
	const	int	kTestSize	=	100;
	
	TestCondition (s.GetLength () == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for (int i = 1; i <= kTestSize; i++) {
			s.Append (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEachT (Iterator, UInt32, it, s) {
			s.Remove (i++);
		}
		TestCondition (s.GetLength () == 0);
	}

	/*
	 * Try removeAts while iterating forward.
	 */
	{
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.Append (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
			if (i > s.GetLength ()) {
				i = 1;
			}
			s.RemoveAt (i++);
		}
		TestCondition (s.GetLength () == (kTestSize/2) - 1);	/* since removing right of cursor */
		s.RemoveAll ();
		TestCondition (s.GetLength () == 0);
	}

	/*
	 * Try removes while iterating backward.
	 */
	{
		
	}

	/*
	 * Try removes multiple iterators present.
	 */
	{
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.Append (i);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (UInt32, it, s) {
			ForEach (UInt32, it2, s.MakeSequenceIterator (eSequenceBackward)) {
				ForEach (UInt32, it3, s) {
					if (s.GetLength () != 0) {
						if (i > s.GetLength ()) {
							i = 1;
						}
						s.RemoveAt (i++);
					}
				}
			}
		}
	}
}

#if		qRealTemplatesAvailable
static	void	TestSimpleIteration (Sequence<SimpleClass>& s)
#else
static	void	TestSimpleIteration (Sequence(SimpleClass)& s)
#endif
{
    UInt32 index = 0;
    {
      ForEach (SimpleClass, it, s) {
		index++;
	//	TestCondition (s.IndexOf (it.Current ()) == index);
     }
    }
    TestCondition (s.GetLength () == index);

    {
      ForEach (SimpleClass, it, s.MakeSequenceIterator (eSequenceBackward)) {
	//	TestCondition (s.IndexOf (it.Current ()) == index);
		index--;
      }
    }
}

#if		qRealTemplatesAvailable
void	SimpleTests (Sequence<SimpleClass>& s)
#else
void	SimpleTests (Sequence(SimpleClass)& s)
#endif
{
#if		qRealTemplatesAvailable
	Sequence<SimpleClass>	s1 (s);
#else
	Sequence(SimpleClass)	s1 (s);
#endif
	TestCondition (s1 == s);
	TestCondition (s1 == s);
#if		qRealTemplatesAvailable
	Sequence<SimpleClass>	s2 = s1;
#else
	Sequence(SimpleClass)	s2 = s1;
#endif
	TestCondition (s2 == s);
	TestCondition (s2 == s1);
	s2.Append (5);
	TestCondition (s1 == s);
	TestCondition (s2 != s1);
	
	const int kListLength = 10;
	size_t		count;

	for (count = 1; count <= kListLength; count++) {
		s.Append (1);
	}
	s.RemoveAll ();

#if		qPrintProgress
	cout << "Simple tests of Sequence of SimpleClass..." << endl;
	cout << "testing empty sequence" << endl;
#endif

	TestCondition (s.GetLength () == 0);
	{
		ForEach (SimpleClass, it, s) {
			TestCondition (False);
		}
	}
	{
		ForEach (SimpleClass, it, s.MakeSequenceIterator (eSequenceBackward)) {
			TestCondition (False);
		}
	}

#if		qPrintProgress
	cout << "testing iteration with appends" << endl;
#endif
	for (count = 1; count <= kListLength; count++) {
		s.Append (1);
		TestSimpleIteration (s);
	}

#if		qPrintProgress
	cout << "testing remove" << endl;
#endif
	TestCondition (s.GetLength () == kListLength);
	for (count = 1; count <= kListLength; count++) {
	    s.Remove (1);
		TestCondition (s.GetLength () == (kListLength - count));
	}
	TestCondition (s.GetLength () == 0);

#if		qPrintProgress
	cout << "testing iteration with prepends" << endl;
#endif
	for (count = kListLength; count >= 1; count--) {
		s.Prepend (2);
	}
	TestSimpleIteration (s);
 
#if		qPrintProgress
	cout << "testing remove all" << endl;
#endif
	TestCondition (s.GetLength () == kListLength);
	s.RemoveAll ();
	TestCondition (s.GetLength () == 0);

#if		qPrintProgress
	cout << "testing array indexing with appends" << endl;
#endif
	for (count = 1; count <= kListLength; count++) {
		s.Append (2);
		TestCondition (s[count] == 2);
	} 
	s.RemoveAll ();
	TestCondition (s.GetLength () == 0);

	for (count = 1; count <= kListLength; count++) {
		s.Append (3);
	} 
	{
		count = 0;
		ForEach (SimpleClass, it, s) {
			count++;
		}
		TestCondition (count == kListLength);
	}

	for (count = kListLength; count >= 1; count--) {
		s.Remove (3);
	} 
	TestCondition (s.GetLength () == 0);

#if		qPrintProgress
	cout << "Finished simple tests of Sequence of SimpleClass..." << endl;
#endif
}

#if		qRealTemplatesAvailable
void	Test_Deletions_During_Iteration (Sequence<SimpleClass>& s)
#else
void	Test_Deletions_During_Iteration (Sequence(SimpleClass)& s)
#endif
{
	const	int	kTestSize	=	100;
	
	TestCondition (s.GetLength () == 0);
	/*
	 * Try removes while iterating forward.
	 */
	{
		for (int i = 1; i <= kTestSize; i++) {
			s.Append (6);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (SimpleClass, it, s) {
			s.Remove (6);
		}
		TestCondition (s.GetLength () == 0);
	}

	/*
	 * Try removeAts while iterating forward.
	 */
	{
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.Append (6);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (SimpleClass, it, s) {
			if (i > s.GetLength ()) {
				i = 1;
			}
			s.RemoveAt (i++);
		}
		TestCondition (s.GetLength () == (kTestSize/2) - 1);	/* since removing right of cursor */
		s.RemoveAll ();
		TestCondition (s.GetLength () == 0);
	}

	/*
	 * Try removes while iterating backward.
	 */
	{
		
	}

	/*
	 * Try removes multiple iterators present.
	 */
	{
		TestCondition (s.GetLength () == 0);
		for (int i = 1; i <= kTestSize; i++) {
			s.Append (8);
		}
		TestCondition (s.GetLength () == kTestSize);

		i =	1;
		ForEach (SimpleClass, it, s) {
		  ForEach (SimpleClass, it2, s.MakeSequenceIterator (eSequenceBackward)) {
			ForEach (SimpleClass, it3, s) {
				 if (s.GetLength () != 0) {
						if (i > s.GetLength ()) {
							i = 1;
						}
						s.RemoveAt (i++);
					}
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

