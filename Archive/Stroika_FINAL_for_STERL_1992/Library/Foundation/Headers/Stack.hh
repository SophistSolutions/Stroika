/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Stack__
#define	__Stack__

/*
 * $Header: /fuji/lewis/RCS/Stack.hh,v 1.20 1992/12/04 17:51:31 lewis Exp $
 *
 * Description:
 *		Standard FIFO (first in - first out) Stack. See Sedgewick, 30-31.
 *		Iteration proceeds from the top to the bottom of the stack. Top
 *		is the FIRST IN.
 *
 *
 * TODO:
 *			To CTOR() that takes Iterator<T>
 * Notes:
 *
 * Changes:
 *	$Log: Stack.hh,v $
 *		Revision 1.20  1992/12/04  17:51:31  lewis
 *		Add RemoveAll() since was only place we didn't have, and
 *		is a sensible thing to have - not sure - but Sterl insisted.
 *		RemoveTop -> Pop() and returns value.
 *
 *		Revision 1.19  1992/11/23  21:49:29  lewis
 *		Return Iterator<T> instead of IteratorRep<T>*.
 *
 *		Revision 1.18  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.17  1992/11/12  08:11:41  lewis
 *		#include Stack.hh
 *
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/11/05  22:11:06  lewis
 *		Get rid of extra ';' in class declaration - makes CFront 2.1 barf.
 *
 *		Revision 1.14  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.13  1992/10/22  04:13:22  lewis
 *		Add virtual DTOR for StackRep - accidentally deleted with CollectionRep.
 *
 *		Revision 1.12  1992/10/15  02:14:24  lewis
 *		Dont have rep subclass form CollectionRep - have subclasses do if it need
 *		be as an implementation detail.
 *		RETHINK which way iteration goes.
 *
 *		Revision 1.11  1992/10/14  06:04:02  lewis
 *		Cleanups - moved inlines to implementaion section at end of header
 *		file, and moved things we could to the .cc file. Got rid of method
 *		defintions from inside class declarations.
 *
 *		Revision 1.10  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.9  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.8  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.7  1992/10/07  23:05:18  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		SharedOfStackRepOf<T>_hh include instead of ## stuff in CollectionInfo.
 *
 *		Revision 1.6  1992/10/02  21:46:16  lewis
 *		Fix stack #defines (StackRep(T)) etc - for use with GenClass.
 *
 *		Revision 1.5  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.3  1992/09/15  17:05:42  lewis
 *		Got rid of Rep files. Merged AbX into X (Eg. Got rid of AbStack,
 *		and have variuous subclasses subclass from Stack, rather than
 *		AbStack. Then have Stack(as it did before) build a default src
 *		if no arg given.
 *
 *		Revision 1.3  1992/09/12  02:42:31  lewis
 *		Got rid of separate Rep.hh files - put into AbXX files inline.
 *
 *		Revision 1.2  1992/09/11  14:58:43  sterling
 *		used new Shared implementation
 *
 *		Revision 1.4  1992/05/09  01:15:50  lewis
 *		Ported to BC++ and fixed templates stuff.
 *
 *		Revision 1.3  92/04/23  16:53:28  16:53:28  lewis (Lewis Pringle)
 *		Did qRealTemplatesAvailable implementation.
 *		
 *
 */

#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"

	
#if		qRealTemplatesAvailable
		
template	<class	T>	class	StackRep;
#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfStackRepOf<T>_hh
#endif

template	<class T> class	Stack {
	public:
		Stack ();
		Stack (const Stack<T>& src);

	protected:
		Stack (StackRep<T>* src);

	public:
		nonvirtual	Stack<T>& operator= (const Stack<T>& src);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();
	 	nonvirtual	operator Iterator<T> () const;	

		nonvirtual	void	Push (T item);
		nonvirtual	T		Pop ();
		nonvirtual	T		Top () const;

	protected:
		nonvirtual	const StackRep<T>*	GetRep () const;
		nonvirtual	StackRep<T>*		GetRep ();

	private:
		Shared<StackRep<T> >	fRep;

		static	StackRep<T>*	Clone (const StackRep<T>& src);

	friend	Boolean	operator== (const Stack<T>& lhs, const Stack<T>& rhs);
};

template	<class T> Boolean	operator== (const Stack<T>& lhs, const Stack<T>& rhs);
template	<class T> Boolean	operator!= (const Stack<T>& lhs, const Stack<T>& rhs);


template	<class	T>	class	StackRep {
	protected:
		StackRep ();

	public:
		virtual ~StackRep ();

	public:
		virtual	StackRep<T>*	Clone () const		=	Nil;
		virtual	size_t			GetLength () const 	=	Nil;
		virtual	void			RemoveAll () 		=	Nil;
		virtual	void			Compact ()			=	Nil;
		virtual	IteratorRep<T>*	MakeIterator () 	=	Nil;

		virtual	void			Push (T item)		=	Nil;
		virtual	T				Pop ()				=	Nil;
		virtual	T				Top () const		=	Nil;
};


#endif	/*qRealTemplatesAvailable*/




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifdef	inline
			#error "Ooops - I guess my workaround wont work!!!"
		#endif
		#define	inline
	#endif

	// class StackRep<T>
	template	<class T>	inline	StackRep<T>::StackRep ()
	{
	}
	template	<class T>	inline	StackRep<T>::~StackRep ()
	{
	}


	// class Stack<T>
	template	<class T>	inline	Stack<T>::Stack (const Stack<T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class T>	inline	Stack<T>::Stack (StackRep<T>* src) :
		fRep (&Clone, src)
	{
	}
	template	<class T>	inline	Stack<T>& Stack<T>::operator= (const Stack<T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class T>	inline	const StackRep<T>*	Stack<T>::GetRep () const
	{
		return ((const StackRep<T>*) fRep.GetPointer ());
	}
	template	<class T>	inline	StackRep<T>*		Stack<T>::GetRep ()
	{
		return ((StackRep<T>*) fRep.GetPointer ());
	}
	template	<class T>	inline	size_t	Stack<T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class T>	inline	Boolean	Stack<T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class T>	inline	void	Stack<T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class T>	inline	Stack<T>::operator Iterator<T> () const
	{
		return (((Stack<T>*) this)->fRep->MakeIterator ());
	}		
	template	<class T>	inline	void	Stack<T>::Push (T item)
	{
		fRep->Push (item);
		Ensure (not IsEmpty ());
	}
	template	<class T>	inline	T		Stack<T>::Pop ()
	{
		Require (not IsEmpty ());
		return (fRep->Pop ());
	}
	template	<class T>	inline	T		Stack<T>::Top () const
	{
		Require (not IsEmpty ());
		return (fRep->Top ());
	}


	// operators
	template	<class T> inline	Boolean	operator!= (const Stack<T>& lhs, const Stack<T>& rhs)
	{
		return (not operator== (lhs, rhs));
	}

	#if		qRealTemplatesAvailable && qBCC_InlinesBreakTemplates
		#ifndef	inline
			#error	"How did it get undefined?"
		#endif
		#undef	inline
	#endif

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
	#define	qIncluding_StackCC	1
		#include	"../Sources/Stack.cc"
	#undef	qIncluding_StackCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Stack__*/
