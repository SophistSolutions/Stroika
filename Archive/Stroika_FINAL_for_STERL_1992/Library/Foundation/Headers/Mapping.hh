/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Mapping__
#define	__Mapping__

/*
 * $Header: /fuji/lewis/RCS/Mapping.hh,v 1.22 1992/11/23 21:23:27 lewis Exp $
 *
 * Description:
 *		Mapping which allows for the association of two elements, and key and
 *	a value. The key UNIQUELY specifies its associated value.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Mapping.hh,v $
 *		Revision 1.22  1992/11/23  21:23:27  lewis
 *		Return Iterator<T> instead of IteratorRep<T>*.
 *
 *		Revision 1.21  1992/11/17  05:29:58  lewis
 *		Work around qBCC_InlinesBreakTemplates.
 *
 *		Revision 1.20  1992/11/12  08:06:05  lewis
 *		Inlclude shared, and fix >> template syntax error.
 *
 *		Revision 1.19  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.17  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.16  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.15  1992/10/10  20:19:00  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.14  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.13  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.12  1992/10/07  22:13:27  sterling
 *		For !qRealTemplatesAvailable and using new GenClass stuff add
 *		IteratorOfMapElementOf<Key>_<T>_hh include instead of ## stuff in CollectionInfo.
 *		Made op== out of line.
 *
 *		Revision 1.11  1992/10/02  21:42:19  lewis
 *		Fix macro names for GenClass - and heavy-duty hack - comment ou;
 *		MapElement declaration (!!qTemplatesAvailable) so in GenClass it
 *		does not get copied, and instead we do its defintion as raw-text.
 *		Needed cuz of other files we include here depending on that defintion(
 *		eg iterator).
 *
 *		Revision 1.10  1992/10/02  04:11:33  lewis
 *		MappingKeyIterator macro should take 2 arguemnts. Fixed stuff for GenClass.
 *		Still not working.
 *
 *		Revision 1.9  1992/09/29  15:01:39  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.8  1992/09/26  05:14:02  lewis
 *		Fixed bad (redundent) name in name2 () macro on non-MPW
 *		hacked side.
 *
 *		Revision 1.7  1992/09/26  03:49:16  lewis
 *		Renamed operator IteratorRep<Key>* and operator IteratorRep<T>* to Make
 *		Image/KeyIterator () because cannot be conversion operators if
 *		(common case really) Key and T are same type!
 *		Also, cleaned up for new GenClass tool.
 *
 *		Revision 1.6  1992/09/23  01:14:26  lewis
 *		Added missing inlines on op== (MapElt,MapElt).
 *
 *		Revision 1.5  1992/09/21  05:36:02  sterling
 *		Use Mapping_Array as default implemenation.
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
 *		Revision 1.11  1992/02/21  18:24:06  lewis
 *		Comment, and remove qMPW_SymbolTableOverflowProblem.
 *
 *		Revision 1.9  1992/01/25  02:25:57  lewis
 *		Added missing trialing backslash after op== for Dict-elt.
 *
 *		Revision 1.8  1992/01/22  06:47:48  lewis
 *		Implenemted op== for MappingElement - now needed by Collection.
 *
 *		Revision 1.7  1992/01/22  05:26:32  lewis
 *		Get rid of pure virtuals that are inherited, and remove const from a few non-pointer args.
 *
 *		Revision 1.6  1992/01/16  00:28:55  lewis
 *		Made a few ctors/dtors out-of-line to hopefully avoid bugs in mac pcxlate. Didn't work (compiler still crashes).
 *
 *
 *
 */
 
#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Iterator.hh"
#include	"Shared.hh"



#if		qRealTemplatesAvailable



template	<class Key, class T> class	MapElement {
	public:
		MapElement (Key key, T elt);
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStarPlusPlusBug
		MapElement (const MapElement<Key,T>& from);
#endif
	
		Key	fKey;
		T	fElt;
};

#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	IteratorOfMapElementOf<Key>_<T>_hh
#endif

template	<class Key, class T> 	Boolean	operator== (const MapElement<Key,T>& lhs, const MapElement<Key,T>& rhs);
template	<class Key, class T>	Boolean	operator!= (const MapElement<Key,T>& lhs, const MapElement<Key,T>& rhs);


template	<class Key, class T>	class	MappingRep;
#if		!qRealTemplatesAvailable
		#include	"TFileMap.hh"
		#include	SharedOfMappingRepOf<Key>_<T>_hh
#endif

template	<class Key, class T> class	Mapping {
	public:
		Mapping ();
		Mapping (const Mapping<Key,T>& src);

	protected:
		Mapping (MappingRep<Key,T>* src);

	public:
		nonvirtual	Mapping<Key,T>& operator= (const Mapping<Key,T>& src);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	Boolean	IsEmpty () const;
		nonvirtual	void	RemoveAll ();
		nonvirtual	void	Compact ();

		nonvirtual	Boolean	Lookup (Key key,T* item) const;
		nonvirtual	void	Enter (Key key,T newElt);
		nonvirtual	void	Enter (const Mapping<Key,T>& items);
		nonvirtual	void	Remove (T elt);
		nonvirtual	void	Remove (const Mapping<Key,T>& items);
		nonvirtual	void	RemoveAt (Key key);

		nonvirtual	Mapping<Key,T>&	operator+= (const Mapping<Key,T>& items);
		nonvirtual	Mapping<Key,T>&	operator-= (const Mapping<Key,T>& items);

	 	nonvirtual	operator Iterator<Key> () const;
	 	nonvirtual	operator Iterator<MapElement<Key,T> > () const;
	
	protected:
		nonvirtual	const MappingRep<Key,T>*	GetRep () const;
		nonvirtual	MappingRep<Key,T>*			GetRep ();

	private:
		Shared<MappingRep<Key,T> >	fRep;

		static	MappingRep<Key,T>*	Clone (const MappingRep<Key,T>& src);

	friend	Boolean	operator== (const Mapping<Key,T>& lhs, const Mapping<Key,T>& rhs);
};

template	<class Key, class T> Boolean	operator== (const Mapping<Key,T>& lhs, const Mapping<Key,T>& rhs);
template	<class Key, class T> Boolean	operator!= (const Mapping<Key,T>& lhs, const Mapping<Key,T>& rhs);

template	<class Key, class T> Mapping<Key,T>	operator+ (const Mapping<Key,T>&, const Mapping<Key,T>&);
template	<class Key, class T> Mapping<Key,T>	operator- (const Mapping<Key,T>&, const Mapping<Key,T>&);



template	<class Key, class T>	class	MappingRep {
	protected:
		MappingRep ();

	public:
		virtual ~MappingRep ();

	public:
		virtual	MappingRep<Key,T>*	Clone () const						=	Nil;
		virtual	size_t	GetLength () const 								=	Nil;
		virtual	void	Compact ()										=	Nil;
	 	virtual	void	RemoveAll () 									=	Nil;

		virtual	Boolean	Lookup (Key key, T* eltResult) const			=	Nil;
		virtual	void	Enter (Key key, T newElt)						=	Nil;
		virtual	void	Remove (T elt)									=	Nil;
		virtual	void	RemoveAt (Key key)								=	Nil;

		virtual	IteratorRep<MapElement<Key,T> >* MakeElementIterator ()	=	Nil;
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

	// class MapElement<Key,T>
	template	<class Key, class T> inline	MapElement<Key,T>::MapElement (Key key, T elt) :
		fKey (key),
		fElt (elt)
	{
	}
#if		qGCC_BadDefaultCopyConstructorGeneration || qGCC_OperatorNewAndStarPlusPlusBug
	template	<class Key, class T> inline	MapElement<Key,T>::MapElement (const MapElement<Key,T>& from) :
		fKey (from.fKey),
		fElt (from.fElt)
	{
	}
#endif

	// operator!=
	template	<class Key, class T> inline	Boolean	operator!= (const MapElement<Key,T>& lhs, const MapElement<Key,T>& rhs)
	{
		return (not operator== (lhs, rhs));
	}
	template	<class Key, class T> inline	Boolean	operator!= (const Mapping<Key,T>& lhs, const Mapping<Key,T>& rhs)
	{
		return (not operator== (lhs, rhs));
	}

	// class	Mapping<Key,T>
	template	<class Key, class T> inline	MappingRep<Key,T>::MappingRep ()
	{
	}
	template	<class Key, class T> inline	MappingRep<Key,T>::~MappingRep ()
	{
	}

	// class	Mapping<Key,T>
	template	<class Key, class T> inline	Mapping<Key,T>::Mapping (const Mapping<Key,T>& src) :
		fRep (src.fRep)
	{
	}
	template	<class Key, class T> inline	Mapping<Key,T>::Mapping (MappingRep<Key,T>* src) :
		fRep (&Clone, src)
	{
	}
	template	<class Key, class T> inline	Mapping<Key,T>& Mapping<Key,T>::operator= (const Mapping<Key,T>& src)
	{
		fRep = src.fRep;
		return (*this);
	}
	template	<class Key, class T> inline	size_t	Mapping<Key,T>::GetLength () const
	{
		return (fRep->GetLength ());
	}
	template	<class Key, class T> inline	Boolean	Mapping<Key,T>::IsEmpty () const
	{
		return (Boolean (GetLength () == 0));
	}
	template	<class Key, class T> inline	void	Mapping<Key,T>::RemoveAll ()
	{
		fRep->RemoveAll ();
	}
	template	<class Key, class T> inline	void	Mapping<Key,T>::Compact ()
	{
		fRep->Compact ();
	}
	template	<class Key, class T> inline	Boolean	Mapping<Key,T>::Lookup (Key key,T* item) const
	{
		return (fRep->Lookup (key, item));
	}
	template	<class Key, class T> inline	void	Mapping<Key,T>::Enter (Key key,T newElt)
	{
		fRep->Enter (key, newElt);
	}
	template	<class Key, class T> inline	void	Mapping<Key,T>::Remove (T elt)
	{
		fRep->Remove (elt);
	}
	template	<class Key, class T> inline	void	Mapping<Key,T>::RemoveAt (Key key)
	{
		fRep->RemoveAt (key);
	}
	template	<class Key, class T> inline	Mapping<Key,T>&	Mapping<Key,T>::operator+= (const Mapping<Key,T>& items)
	{
		Enter (items);
		return (*this);
	}
	template	<class Key, class T> inline	Mapping<Key,T>&	Mapping<Key,T>::operator-= (const Mapping<Key,T>& items)
	{
		Remove (items); return (*this);
	}
	template	<class Key, class T> inline	Mapping<Key,T>::operator Iterator<MapElement<Key,T> > () const
	{
		return (((Mapping<Key,T>*) this)->fRep->MakeElementIterator ());
	}
	template	<class Key, class T> inline	const MappingRep<Key,T>*	Mapping<Key,T>::GetRep () const
	{
		return ((const MappingRep<Key,T>*) fRep.GetPointer ());
	}
	template	<class Key, class T> inline	MappingRep<Key,T>*			Mapping<Key,T>::GetRep ()
	{
		return ((MappingRep<Key,T>*) fRep.GetPointer ());
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
	#define	qIncluding_MappingCC	1
		#include	"../Sources/Mapping.cc"
	#undef	qIncluding_MappingCC
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Mapping__*/

