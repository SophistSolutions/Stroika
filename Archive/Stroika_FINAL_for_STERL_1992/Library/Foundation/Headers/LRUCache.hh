/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__LRUCache__
#define	__LRUCache__

/*
 * $Header: /fuji/lewis/RCS/LRUCache.hh,v 1.8 1992/11/11 03:55:13 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LRUCache.hh,v $
 *		Revision 1.8  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.7  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.6  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.5  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.4  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.3  1992/10/07  22:11:05  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		SharedOfLRUCacheElementOf<Key>_<T>_hh include instead of ## stuff in CollectionInfo.
 *
 *		Revision 1.2  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *
 *
 */
 
#include	"Config-Foundation.hh"
#include	"Debug.hh"

#include	"Collection.hh"
#include	"Iterator.hh"



#if		qRealTemplatesAvailable
	
template	<class Key, class T> class	LRUCacheElement {
	public:
		LRUCacheElement (Key key, T elt) :
			fKey (key),
			fElt (elt)
		{
		}

		Key	fKey;
		T	fElt;
};
template	<class Key, class T> Boolean	operator== (const LRUCacheElement<Key,T>& lhs, const LRUCacheElement<Key,T>& rhs)
	{ if (not (lhs.fKey == rhs.fKey)) return False; return (Boolean (lhs.fElt == rhs.fElt)); }
template	<class Key, class T> Boolean	operator!= (const LRUCacheElement<Key,T>& lhs, const LRUCacheElement<Key,T>& rhs)
	{	return (not operator== (lhs, rhs));	}

#if		!qRealTemplatesAvailable
		#include	"TFileMap.hh"
		#include	SharedOfLRUCacheElementOf<Key>_<T>_hh
#endif

template	<class Key, class T>	class	LRUCacheRep : public CollectionRep {
	protected:
		LRUCacheRep ()	{}

	public:
		virtual	LRUCacheRep<Key,T>*	Clone () const		= Nil;
		virtual	size_t	GetLength () const 		= Nil;
		virtual	void	Compact ()						= Nil;
	 	virtual	void	RemoveAll () 					= Nil;

		virtual	Boolean	Contains (Key k) const	= Nil;
		virtual	T		Fetch (Key k)			= Nil;

		virtual	IteratorRep<LRUCacheElement<Key,T>>* MakeElementIterator () = Nil;
};

template	<class Key, class T> class	LRUCache {
	public:
		LRUCache (const LRUCache<Key,T>& src)	: fRep (src.fRep)	{}
		LRUCache ();

	protected:
		LRUCache (LRUCacheRep<Key,T>* src) : fRep (&Clone, src)	{}

	public:
		LRUCache<Key,T>& operator= (const LRUCache<Key,T>& src)	{ fRep = src.fRep; return (*this); }

		nonvirtual	size_t	GetLength () const		{ return (fRep->GetLength ());	}
		nonvirtual	Boolean	IsEmpty () const				{ return (Boolean (GetLength () == 0));	}
		nonvirtual	void	RemoveAll ()					{ fRep->RemoveAll (); }
		nonvirtual	void	Compact ()						{ fRep->Compact ();	}

		nonvirtual	Boolean	Contains (Key k) const	{ return (fRep->Contains (k));	}
		nonvirtual	T		Fetch (Key k)			{  return (fRep->Fetch (k));	}

	 	nonvirtual	operator IteratorRep<T>* () const;	
	 	nonvirtual	operator IteratorRep<Key>* () const;
	 	nonvirtual	operator IteratorRep<LRUCacheElement<Key,T> >* () const	{ return (((LRUCache<Key,T>*) this)->fRep->MakeElementIterator ()); }
	
	protected:
		nonvirtual	const LRUCacheRep<Key,T>*	GetRep () const	{ return ((const LRUCacheRep<Key,T>*) fRep.GetPointer ());	}
		nonvirtual	LRUCacheRep<Key,T>*			GetRep ()		{ return ((LRUCacheRep<Key,T>*) fRep.GetPointer ());	}

	private:
		Shared<LRUCacheRep<Key,T> >	fRep;

		static	LRUCacheRep<Key,T>*	Clone (const LRUCacheRep<Key,T>& src)	{ return (src.Clone ()); }

	friend	Boolean	operator== (const LRUCache<Key,T>& lhs, const LRUCache<Key,T>& rhs);
};

template	<class Key, class T> Boolean	operator== (const LRUCache<Key,T>& lhs, const LRUCache<Key,T>& rhs);
template	<class Key, class T> inline	Boolean	operator!= (const LRUCache<Key,T>& lhs, const LRUCache<Key,T>& rhs)
	{	return (not operator== (lhs, rhs));	}


#endif	/*qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
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
	#define	qIncluding_LRUCacheCC	1
		#include	"../Sources/LRUCache.cc"
	#undef	qIncluding_LRUCacheCC
#endif

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__LRUCache__*/

