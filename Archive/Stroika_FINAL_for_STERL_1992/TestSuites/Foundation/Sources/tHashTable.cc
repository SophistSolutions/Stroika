/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tHashTable.cc,v 1.7 1992/12/03 07:46:40 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tHashTable.cc,v $
 *		Revision 1.7  1992/12/03  07:46:40  lewis
 *		Work around qGCC_SimpleClassStaticInitializerBug.
 *
 *		Revision 1.6  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.4  1992/10/08  01:18:45  sterling
 *		Got to work with GenClass instead of Declare macros.
 *
 *		Revision 1.3  1992/09/15  17:53:40  lewis
 *		Update for container changes - Get rid of Ab classes.
 *		And get rid of cerr - use cout.
 *
 *		Revision 1.2  1992/09/11  13:47:27  lewis
 *		Lots of work to debug hashtable stuff with gcc 2.2.1 - eventually adding bug workaround
 *		qGCC_BadDefaultCopyConstructorGeneration
 *
 *
 *
 *
 */


/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include	"OSInit.hh"
#pragma	push
	#pragma	force_active	on
	#if		qGCC_SimpleClassStaticInitializerBug
		static	OSInit	sOSInit;
	#else
		static	OSInit	sOSInit	=	OSInit ();
	#endif
#pragma	pop

#define		qUseAllGlobals		0
#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>

#include	"Debug.hh"
#include	"Format.hh"
#include	"HashTable.hh"
#include	"Time.hh"

#include	"SimpleClass.hh"
#include	"TestSuite.hh"

#define		qPrintTimings		!qDebug



	struct	DictElt {
		SimpleClass	fKey;
		UInt32	fElt;

		DictElt (int key, UInt32 elt) :
			fKey (key),
			fElt (elt)
		{
		}
#if		qGCC_BadDefaultCopyConstructorGeneration
		DictElt (const DictElt& from) :
			fKey (from.fKey),
			fElt (from.fElt)
		{
		}
#endif
		~DictElt () {}

	};
	Boolean	operator== (const DictElt& lhs, const DictElt& rhs)
	{
		return (Boolean (lhs.fKey == rhs.fKey));
	}

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include HashTableOfDictElt_cc
#endif	/*!qRealTemplatesAvailable*/


	ostream&	operator<< (ostream& on, const DictElt& elt)
	{
		on << "(" << elt.fKey.GetValue () << ", " << elt.fElt << ")";
		return (on);
	}


static	void	PrintHashTable (const HashTable(DictElt)& hTable, ostream& on);
static	void	PrintHashTable (const HashTable(DictElt)& hTable, ostream& on)
{
	on << "(" << endl;
	for (size_t i = 1; i <= hTable.GetTableSize (); i++) {
		on << tab << "[" << i << "]";
		if (hTable.GetIthTableHead (i) == Nil) {
			on << tab << "=>	Nil" << endl;
		}
		else {
			for (register HashTableElement(DictElt)* curT = hTable.GetIthTableHead (i); curT != Nil; curT = curT->fNext) {
				on << tab << "=>	(" << curT->fElement << ")" << endl;
			}
		}
	}
}




static	void	Test1 ();
static	void	Test2 ();




int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing HashTables..." << endl;

#if		qPrintTimings
	Time t = GetCurrentTime ();
	cout << tab << "timing HashTables..." << endl;
#endif
	Test1 ();
#if		qPrintTimings
	t = GetCurrentTime () - t;
	cout << tab << "finished timing HashTables; time elapsed = " << t << endl;
#endif

	Test2 ();
	cout << "Tested HashTables!" << endl;
	return (0);
}

static	size_t	MyHashFunction (const DictElt& elt)
{
	return (elt.fKey.GetValue ());
}


static	void	Test1 ()
{
#if		qRealTemplatesAvailable
	HashTable <DictElt>	someHashTable(MyHashFunction, 101);
#else
	HashTable (DictElt)	someHashTable(MyHashFunction, 101);
#endif

	const	int	kTestSize	=	100;
	
	for (long i = 1; i <= kTestSize; i++) {
		DictElt de (i, i-1);
		TestCondition (someHashTable.Enter (de));
		TestCondition (de.fKey == i);
		TestCondition (de.fElt == i-1);
	}
	for (i = 1; i <= kTestSize; i++) {
		DictElt de (i, i-1);
		TestCondition (someHashTable.Lookup (de));
		TestCondition (de.fKey == i);
		TestCondition (de.fElt == i-1);
	}
	for (i = 1; i <= kTestSize; i++) {
		DictElt de (i, i-1);
		someHashTable.Remove (de);
		TestCondition (not someHashTable.Lookup (de));
		if (i < kTestSize) {
			DictElt de1 (i+1, i);
			TestCondition (someHashTable.Lookup (de1));
		}
	}
}

static	void	Test2 ()
{
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

