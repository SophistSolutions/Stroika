/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Sequence_Array__
#define	__Sequence_Array__

/*
 * $Header: /fuji/lewis/RCS/Sequence_Array.hh,v 1.34 1992/12/10 05:57:28 lewis Exp $
 *
 * Description:
 *		Use the Array backend to implement the Seqeuence abstraction.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Sequence_Array.hh,v $
 *		Revision 1.34  1992/12/10  05:57:28  lewis
 *		Got rid of friend declarations for iterators - not needed.
 *
 *		Revision 1.33  1992/11/12  08:09:23  lewis
 *		Add template declares to quite some CFront 3.0 errors with templates.
 *		Still not working on any template implementation, however.
 *
 *		Revision 1.32  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.31  1992/11/07  16:30:37  lewis
 *		Try new arrayiteratorbase stuff suggested by kdj, and use
 *		in various mixins that use Array.
 *
 *		Revision 1.30  1992/11/01  01:26:35  lewis
 *		New Array Iterator support with separate subclass for patching.
 *
 *		Revision 1.29  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.28  1992/10/29  15:54:25  lewis
 *		Use qCFront_InlinesOpNewDeleteMustDeclareInline to work around MPW bug.
 *
 *		Revision 1.27  1992/10/22  04:17:47  lewis
 *		Add dtor for SequenceRep_Array.
 *
 *		Revision 1.26  1992/10/20  21:24:50  lewis
 *		Add operator new/delete overloads for Sequence_ArrayRep.
 *
 *		Revision 1.25  1992/10/20  17:52:18  lewis
 *		Cleanup comments and made GetLength,IsEmpty, and GetAt() for rep and
 *		Sequence_Array to be inline - thus avoiding (virtual) function overhead
 *		when we know the static type for these operations.
 *
 *		Revision 1.24  1992/10/15  02:09:12  lewis
 *		Move Get/Setslots alloced into .cc files.
 *
 *		Revision 1.23  1992/10/14  16:21:59  lewis
 *		Delete CTOR with hintSize.
 *
 *		Revision 1.22  1992/10/13  05:28:15  lewis
 *		Lots of reworking, including moving inlines to the end of the
 *		file, and moving lots to the .cc file. Changed to using new
 *		Array iterators (inline, not using CollectionView, etc).
 *
 *		Revision 1.21  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.20  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.19  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.18  1992/09/25  21:14:26  lewis
 *		Fix lots of templates problems and get working using new GenClass stuff.
 *
 *		Revision 1.17  1992/09/25  01:28:14  lewis
 *		Started working on conversion to new psuedo-template mechanism.
 *
 *		Revision 1.16  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.15  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default rep
 *		if no arg given.
 *
 *		Revision 1.14  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.13  1992/09/11  16:14:29  sterling
 *		use new Shared implementation
 *
 *		Revision 1.12  1992/09/04  20:36:54  sterling
 *		Added Contains () override.
 *
 *		Revision 1.10  1992/07/21  06:32:41  lewis
 *		Declare operator new overload for iterator inline in struct definition
 *		to silice buggy apply compiler. Also, use SizeInContainer instead of
 *		Container::Size as a workaround for CFront 2.1 bugs with nested types,
 *		and needed for macros - we do it in the template here too so we can automate
 *		coversion from templates to macros more easily.
 *		In UpdateCurrent: cast fData to be able to call SetAt.
 *
 *		Revision 1.9  1992/07/19  08:53:45  lewis
 *		Fix typos.
 *
 *		Revision 1.8  1992/07/19  07:28:28  lewis
 *		Sped up implementations of iterator::Done () and corrected incorrect
 *		implementation of PatchAdd/PatchRemove - how painful analyzing all
 *		the cases....
 *
 *		Revision 1.7  1992/07/19  03:53:31  lewis
 *		A few more minor cleanups, including assuring that macro version and
 *		template version identical, and testing macros under GCC/AUX.
 *
 *		Revision 1.6  1992/07/17  18:24:40  lewis
 *		Replaced old qTemplatesHasRepository with a new, much more elaborate, and
 *		hopefully more correct version.
 *
 *		Revision 1.5  1992/07/17  03:52:59  lewis
 *		Got rid of class Collection/SequenceMutator, and consider non-const
 *		CollectionIterator to be a Mutator. Very careful line by line
 *		analysis, and bugfixing, and implemented and tested (compiling)
 *		the macro version under GCC. In macro version got rid of overload
 *		of new/delete in iterator cuz BlockAllocated<> declare not done
 *		automatically.
 *
 *		Revision 1.4  1992/07/16  07:44:46  lewis
 *		Fixed typo - sb !qTemplatesHasRepository.
 *
 *		Revision 1.3  1992/07/16  07:09:37  lewis
 *		Change magic include at end to be depend on "qRealTemplatesAvailable && qTemplatesHasRepository"
 *		instead of qBorlandCPlus.
 *
 *		Revision 1.2  1992/07/08  05:35:03  lewis
 *		Changed file name to include for template .cc stuff to UNIX pathname, and
 *		use PCName mapper to rename to appropriate PC File - cuz we may need to do
 *		this with gcc/aux too.
 *		Also, so I have PC Names (mapped to names) in just one place - so they are
 *		easier to change.
 *
 *		Revision 1.16  1992/06/10  04:13:24  lewis
 *		Got rid of Sort function - do later as an external object operating on a sequence, ala
 *		Stroustrups second edition.
 *
 *		Revision 1.15  92/05/23  00:56:10  00:56:10  lewis (Lewis Pringle)
 *		Implemented first cut at Sequence_ArrayMutator.
 *		
 *		Revision 1.14  92/05/21  17:28:16  17:28:16  lewis (Lewis Pringle)
 *		Changed template version to react to overhaul of Collection/Sequence definitions.
 *		Also, use BlockAllocated instead of manually keeping my own for Sequence_ArrayIterator.
 *		
 *		Revision 1.13  92/05/09  01:01:33  01:01:33  lewis (Lewis Pringle)
 *		Ported to BC++/Tempates/PC.
 *		
 *		Revision 1.12  92/04/02  20:03:15  20:03:15  lewis (Lewis Pringle)
 *		Templated implementation.
 *		
 *		Revision 1.10  1992/02/27  20:59:17  lewis
 *		Note some future optimizations, and do a little of that (cache length rather
 *		than recomputing in loop).
 *		Fixed bug where is qsort call accidnetly returned -11 instead of -1.
 *		Added support for new sequence notion of current index in seqeunce iterators.
 *
 *		Revision 1.9  1992/02/21  17:59:51  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem.
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *		Revision 1.8  1992/01/22  05:27:38  lewis
 *		Fix for changes in interface of Collection.
 *
 *		Revision 1.7  1992/01/22  02:25:07  lewis
 *		Add support for sorting, using QSort library function.
 *
 *		Revision 1.6  1992/01/21  20:56:17  lewis
 *		Use ForEach macro.
 *
 *
 *
 */

#include	"Array.hh"
#include	"Sequence.hh"


#if		qRealTemplatesAvailable

	template	<class T> class	SequenceRep_Array;
	template	<class T> class	Sequence_Array : public Sequence<T> {
		public:
			Sequence_Array ();
			Sequence_Array (const Sequence<T>& seq);
			Sequence_Array (const Sequence_Array<T>& seq);
			Sequence_Array (const T* items, size_t size);

			nonvirtual	Sequence_Array<T>& operator= (const Sequence_Array<T>& seq);

			nonvirtual	size_t	GetSlotsAlloced () const;
			nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);


		/*
		 * Repeats of stuff in Sequence, just so we can inline them here...
		 */
		public:
			inline	size_t	GetLength () const;
			inline	Boolean	IsEmpty () const;
			inline	T		GetAt (size_t index) const;

		private:
			nonvirtual	const SequenceRep_Array<T>*	GetRep () const;
			nonvirtual	SequenceRep_Array<T>*		GetRep ();
	};

#endif	/*qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable

	template	<class T> class	SequenceRep_Array : public SequenceRep<T> {
		public:
			SequenceRep_Array ();
			~SequenceRep_Array ();
	
			override	size_t	GetLength () const;
			override	void	Compact ();		
			override	SequenceRep<T>*	Clone () const;
			override	T		GetAt (size_t index) const;
			override	void	SetAt (T item, size_t index);
			override	void	InsertAt (T item, size_t index);
			override	void	RemoveAt (size_t index, size_t amountToRemove);
			override	void	Remove (T item);
			override	void	RemoveAll ();
	
			override	IteratorRep<T>* 		MakeIterator ();
			override	SequenceIteratorRep<T>*	MakeSequenceIterator (SequenceDirection d);
			override	SequenceMutatorRep<T>*	MakeSequenceMutator (SequenceDirection d);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
			inline
#endif
			static	void*	operator new (size_t size);

#if		qCFront_InlinesOpNewDeleteMustDeclareInline
			inline
#endif
			static	void	operator delete (void* p);

		private:
			Array_Patch<T>	fData;

		friend	class	Sequence_Array<T>;
	};

	// class SequenceRep_Array<T>
	template	<class T>	inline	size_t SequenceRep_Array<T>::GetLength () const
	{
		return (fData.GetLength ());
	}
	
	template	<class T>	inline	T SequenceRep_Array<T>::GetAt (size_t index) const
	{
		return (fData.GetAt (index));
	}


	// class Sequence_Array<T>
	template	<class	T>	inline	Sequence_Array<T>::Sequence_Array (const Sequence_Array<T>& seq) :
		Sequence<T> (seq)
	{
	}
	template	<class	T>	inline	Sequence_Array<T>& Sequence_Array<T>::operator= (const Sequence_Array<T>& seq)
	{
		Sequence<T>::operator= (seq);
		return (*this);
	}
	template	<class	T>	inline	const SequenceRep_Array<T>*	Sequence_Array<T>::GetRep () const
	{
		return ((const SequenceRep_Array<T>*) Sequence<T>::GetRep ());
	}
	template	<class	T>	inline	SequenceRep_Array<T>*	Sequence_Array<T>::GetRep ()
	{
		return ((SequenceRep_Array<T>*) Sequence<T>::GetRep ());
	}
	template	<class T>	inline	size_t	Sequence_Array<T>::GetLength () const
	{
		return (GetRep ()->GetLength ());
	}
	template	<class T>	inline	Boolean	Sequence_Array<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class T>	inline	T		Sequence_Array<T>::GetAt (size_t index) const
	{
		return (GetRep ()->GetAt (index));
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
	#define	qIncluding_Sequence_ArrayCC	1
		#include	"../Sources/Sequence_Array.cc"
	#undef	qIncluding_Sequence_ArrayCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Sequence_Array__*/

