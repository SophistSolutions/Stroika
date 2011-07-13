/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Mapping_HashTable__
#define	__Mapping_HashTable__

/*
 * $Header: /fuji/lewis/RCS/Mapping_HashTable.hh,v 1.16 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *		Use a hashtable to implement a Mapping. This is probably the most
 *	natural represenation for a Mapping, since it gives you the fastest lookup,
 *	and adding items, so long as the hash function is good, and the sizes of
 *	the has table is big enuf to avoid collisions. We allow you to specify
 *	the hash function, and hash table size, which should help assure this is
 *	true.
 *
 *		The down side, is that the implementation is expensive in terms of
 *	memory allocated for a particular level of performance, and the worst
 *	case is quite bad (linked list bad) if you do start getting collisions.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Mapping_HashTable.hh,v $
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.14  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.13  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.12  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.11  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.10  1992/09/26  03:50:48  lewis
 *		First cut at cleanups for new GenClass tool - untested yet.
 *
 *		Revision 1.9  1992/09/23  01:16:24  lewis
 *		G++ refuses to compile some cases of const cheating, so I must
 *		cast explicitly - see if there is some better way!
 *
 *		Revision 1.8  1992/09/21  05:43:26  sterling
 *		Sterl made LOTS of changes here - not sure what so I lost
 *		my fix for gcc- guess have to make it again later...
 *		(LGP checked in).
 *
 *		Revision 1.7  lewis
 *		Explicitly cast away const in two usages of ForEach macros here,
 *		cuz g++ fails to compile it otherwise. Should consider better
 *		alternatives, however.
 *
 *		Revision 1.6  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.5  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.4  1992/09/11  16:03:31  sterling
 *		use new Shared implementation; fixed bug in IteratorRep
 *
 *		Revision 1.2  1992/09/04  20:09:55  sterling
 *		Added Contains () method.
 *
 *		Revision 1.13  1992/06/10  04:03:47  lewis
 *		Changed default Mapping_HashTable hash table size to 101 (was 1 - a very
 *		bad default - better to have none).
 *
 *		Revision 1.11  1992/02/21  18:29:51  lewis
 *		Comment, and remove qMPW_SymbolTableOverflowProblem.
 *
 *		Revision 1.10  1992/02/21  17:29:04  lewis
 *		No longer support qGPlus_FunctParamBug.
 *
 *		Revision 1.8  1992/01/21  20:53:43  lewis
 *		Use ForEach () macro.
 *
 *
 *
 */

#include	"Mapping.hh"



#if		qRealTemplatesAvailable

template	<class Key, class T>	class Mapping_HashTable : public Mapping<Key,T> {
	public:
		Mapping_HashTable (
			size_t (*hashFunction) (const Key&), 
			size_t hashTableSize = 101);

		Mapping_HashTable (
			const Mapping<Key,T>&,
			size_t (*hashFunction) (const Key&), 
			size_t hashTableSize = 101);
			
		nonvirtual	Mapping_HashTable<Key,T> (const Mapping_HashTable<Key,T>& map);
};

#endif	/*qRealTemplatesAvailable*/



// not sure this is a good place for this???
class	String;
size_t	DefaultStringHashFunction (const String& key);




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	template	<class Key, class T>	inline	Mapping_HashTable<Key,T>::Mapping_HashTable (const Mapping_HashTable<Key,T>& map) :
		Mapping<Key,T> (map)
	{
	}
#endif	/*qRealTemplatesAvailable*/



#if		qRealTemplatesAvailable && !qTemplatesHasRepository
	/*
	 * 		We must include our .cc file here because of this limitation.
	 *	But, under some systems (notably UNIX) we cannot compile some parts
	 *	of our .cc file from the .hh.
	 *
	 *		The other problem is that there is sometimes some untemplated code
	 *	in the .cc file, and to detect this fact, we define another magic
	 *	flag which is looked for in those files.
	 */
	#define	qIncluding_Mapping_HashTableCC	1
		#include	"../Sources/Mapping_HashTable.cc"
	#undef	qIncluding_Mapping_HashTableCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Mapping_HashTable__*/


