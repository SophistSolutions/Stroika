/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Queue_CircularArray__
#define	__Queue_CircularArray__

/*
 * $Header: /fuji/lewis/RCS/Queue_CircularArray.hh,v 1.14 1992/11/19 05:05:39 lewis Exp $
 *
 * Description:
 *		Use the CircularArray template to implement a Queue abstraction.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Queue_CircularArray.hh,v $
 *		Revision 1.14  1992/11/19  05:05:39  lewis
 *		Rename QueueRep_CircularArray->Queue_CircularArrayRep.
 *
 *		Revision 1.13  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.12  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.11  1992/10/15  20:20:56  lewis
 *		Syntax error.
 *
 *		Revision 1.10  1992/10/15  13:22:53  lewis
 *		Moved inlines in class declaration down to implemenation detail section,
 *		moved concrete rep classes to .cc file, and related inlines.
 *		Got rid of CollectionRep from QueueRep, and so mix in in each concrete
 *		rep class now.
 *
 *		Revision 1.9  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.8  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.6  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default src
 *		if no arg given.
 *
 *		Revision 1.3  1992/09/11  16:11:36  sterling
 *		use new Shared implementation
 *
 *		Revision 1.2  1992/09/05  03:38:42  lewis
 *		Rename class from Queue_Array to Queue_CircularArray - was not totally consistent.
 *
 *
 */

#include	"Queue.hh"



#if		qRealTemplatesAvailable

template	<class T> class	Queue_CircularArrayRep;
template	<class T> class	Queue_CircularArray : public Queue<T> {
	public:
		Queue_CircularArray ();
		Queue_CircularArray (const Queue<T>& src);
		Queue_CircularArray (const Queue_CircularArray<T>& src);

		nonvirtual	Queue_CircularArray<T>& operator= (const Queue_CircularArray<T>& src);

		nonvirtual	size_t	GetSlotsAlloced () const;
		nonvirtual	void	SetSlotsAlloced (size_t slots);

	private:
		nonvirtual	const Queue_CircularArrayRep<T>*	GetRep () const;
		nonvirtual	Queue_CircularArrayRep<T>*			GetRep ();
};

#endif	/*qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
	template	<class T>	inline	Queue_CircularArray<T>::Queue_CircularArray (const Queue_CircularArray<T>& src) :
		Queue<T> (src)
	{
	}
	template	<class T>	inline	Queue_CircularArray<T>& Queue_CircularArray<T>::operator= (const Queue_CircularArray<T>& src)
	{
		Queue<T>::operator= (src);
		return (*this);
	}
#endif	/*qRealTemplatesAvailable*/



#if		qRealTemplatesAvailable && !qTemplatesHasRepository
	/*
	 *		We must include our .cc file here because of this limitation.
	 *	But, under some systems (notably UNIX) we cannot compile some parts
	 *	of our .cc file from the .hh.
	 *
	 *		The other problem is that there is sometimes some untemplated code
	 *	in the .cc file, and to detect this fact, we define another magic
	 *	flag which is looked for in those files.
	 */
	#define qIncluding_Queue_CircularArrayCC	1
		#include	"../Sources/Queue_CircularArray.cc"
	#undef  qIncluding_Queue_CircularArrayCC
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Queue_CircularArray__*/

