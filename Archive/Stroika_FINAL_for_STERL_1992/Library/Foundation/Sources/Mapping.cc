/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository
#ifndef	__Mapping__cc
#define	__Mapping__cc

/*
 * $Header: /fuji/lewis/RCS/Mapping.cc,v 1.22 1992/12/03 07:21:27 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Mapping.cc,v $
 *		Revision 1.22  1992/12/03  07:21:27  lewis
 *		Get rid of unneeded DTOR - let compiler autogen fine.
 *		Override Done() to delegate for MappingKeyIterator<Key,T>::Done
 *
 *		Revision 1.21  1992/11/23  21:52:55  lewis
 *		Have conversion operator return Iterator<T>* instead of IteratorRep<T>*.
 *
 *		Revision 1.20  1992/11/20  19:24:40  lewis
 *		Use new it.More() stuff instead of Next/Done.
 *
 *		Revision 1.19  1992/11/15  04:15:30  lewis
 *		Use explicit Iterator<T> instead of typedef as workaround for
 *		ForEach() of something with comma problem, since CFront 3.0 barfs
 *		on typedefs in template functions.
 *
 *		Revision 1.18  1992/11/13  03:42:32  lewis
 *		Be more careful about mutual include conflicts resulting from
 *		qRealTemplatesAvailable && !qTemplatesHasRepository hacks.
 *		Do hack to work around macro problem with templates together
 *		with our ForEach macro. Needs investigation.
 *
 *		Revision 1.17  1992/11/12  08:20:07  lewis
 *		Fix bad template declare, and work around crock bug
 *		with templates and ForEach macro and MapElement<Key,T> - maybe
 *		only trouble with gcc cpp???
 *
 *		Revision 1.16  1992/11/11  03:55:13  lewis
 *		Gcc 2.3.1 and ifndefs around .cc files (for templates) and fix
 *		#define qIncludings in .hhs.
 *
 *		Revision 1.15  1992/11/05  15:23:51  lewis
 *		MappingKeyIterator(MappingKeyIterator&) now const reference.
 *
 *		Revision 1.14  1992/10/30  22:37:35  lewis
 *		Support for copy CTOR and op= for iterators - and therefore clone method
 *		for IteratorRep.
 *
 *		Revision 1.13  1992/10/29  20:42:28  lewis
 *		Get rid of <T> at end of class name in template declaration,
 *		and end of CTOR name and DTOR name - for real template (and bcc)
 *		compatability - now genclass fixes this for us with macros.
 *
 *		Revision 1.12  1992/10/23  17:18:58  lewis
 *		Moved inlines to implemenation section, and concrete reps to the .cc files.
 *		Other minor cleanups for GenClass. Got rid of CollectionRep as base
 *		class of MappingRep and moved it to each concrete rep.
 *
 *		Revision 1.11  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.10  1992/10/09  14:16:06  lewis
 *		Added #include        IteratorOfMapElementOf<Key>_<T>_cc.
 *
 *		Revision 1.9  1992/10/09  04:44:35  lewis
 *		Added #include        SharedOfMappingRepOf<Key>_<T>_cc.
 *
 *		Revision 1.8  1992/10/07  23:20:03  sterling
 *		Lots of diffs - ?? - LGP checked in.
 *
 *		Revision 1.7  1992/10/02  21:47:37  lewis
 *		Fixed problems with op== and MappingKeyIterator for templates.
 *
 *		Revision 1.6  1992/10/02  04:15:19  lewis
 *		Stuff to try to get GenClass working.
 *
 *		Revision 1.5  1992/09/29  15:20:26  sterling
 *		Cleaned up template versions - looking at macros.
 *
 *		Revision 1.4  1992/09/26  03:49:16  lewis
 *		Renamed operator IteratorRep<Key>* and operator IteratorRep<T>* to Make
 *		Image/KeyIterator () because cannot be conversion operators if
 *		(common case really) Key and T are same type!
 *		Also, cleaned up for new GenClass tool.
 *
 *		Revision 1.3  1992/09/21  06:11:41  sterling
 *		Use Mapping_Array as default implemnetation (lgp checked in).
 *
 *		Revision 1.2  1992/09/15  17:24:02  lewis
 *		Merged AbXXX with XXX class for containers.
 *		Also, got rid of rep files.
 *
 *		Revision 1.2  1992/09/11  16:32:52  sterling
 *		used new Shared implementaiton
 *
 *
 */



#include	"Debug.hh"
#include	"Mapping.hh"			// Include first to avoid circularity problems with qRealTemplatesAvailable && !qTemplatesHasRepository
#include	"Mapping_Array.hh"		// default implementation





#if		qRealTemplatesAvailable


template	<class Key, class T> class	MappingKeyIterator : public IteratorRep<Key> {
	public:
		MappingKeyIterator (MappingRep<Key,T>& owner);
		MappingKeyIterator (const MappingKeyIterator<Key,T>& from);
		~MappingKeyIterator ();

		override	Boolean	Done () const;
		override	Boolean	More ();
		override	Key		Current () const;

		override	IteratorRep<Key>*	Clone () const;

	private:
		IteratorRep<MapElement<Key,T> >*	fIterator;
};




/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */
template	<class Key, class T> Boolean	operator== (const MapElement<Key,T>& lhs, const MapElement<Key,T>& rhs)
{
	// written so confusinglly cuz of CFront 2.1 not-yet-implementeds...
	if (not (lhs.fKey == rhs.fKey)) {
		return False;
	}
	return (Boolean (lhs.fElt == rhs.fElt));
}





/*
 ********************************************************************************
 ************************************* MappingKeyIterator ***********************
 ********************************************************************************
 */
template	<class Key, class T>	MappingKeyIterator<Key,T>::MappingKeyIterator (MappingRep<Key,T>& owner):
	fIterator (owner.MakeElementIterator ())
{
	EnsureNotNil (fIterator);
}

template	<class Key, class T>	MappingKeyIterator<Key,T>::MappingKeyIterator (const MappingKeyIterator<Key,T>& from):
	fIterator (Nil)
{
	RequireNotNil (from.fIterator);
	fIterator = from.fIterator->Clone ();
	EnsureNotNil (fIterator);
}

template	<class Key, class T>	MappingKeyIterator<Key,T>::~MappingKeyIterator ()
{
	delete fIterator;
}

template	<class Key, class T>	Boolean	MappingKeyIterator<Key,T>::Done () const
{
	RequireNotNil (fIterator);
	return (fIterator->Done());
}

template	<class Key, class T>	Boolean	MappingKeyIterator<Key,T>::More ()
{
	RequireNotNil (fIterator);
	return (fIterator->More());
}

template	<class Key, class T>	Key		MappingKeyIterator<Key,T>::Current () const
{
	RequireNotNil (fIterator);
	return (fIterator->Current ().fKey);
}

template	<class Key, class T>	IteratorRep<Key>*	MappingKeyIterator<Key,T>::Clone () const
{
	return (new MappingKeyIterator<Key,T> (*this));
}




/*
 ********************************************************************************
 **************************************** Mapping *******************************
 ********************************************************************************
 */
template	<class Key, class T>	class	Mapping;	// Tmp hack so GenClass will fixup following CTOR/DTORs
														// Harmless, but silly.

template	<class Key, class T> Mapping<Key,T>::Mapping () :
	fRep (&Clone, Nil)
{
	*this = Mapping_Array<Key,T> ();
}

template	<class Key, class T> Mapping<Key,T>::operator Iterator<Key> () const
{
	return (new MappingKeyIterator<Key,T> (*((Mapping<Key,T>*) this)->fRep));
}

template	<class Key, class T> void	Mapping<Key,T>::Enter (const Mapping<Key,T>& items)
{
#if		qRealTemplatesAvailable && 1
	for (Iterator<MapElement<Key,T> > it (items); it.More ();)
#else
	ForEach (MapElement<Key,T>, it, items)
#endif
	{
		fRep->Enter (it.Current ().fKey, it.Current ().fElt);
	}
}

template	<class Key, class T> void	Mapping<Key,T>::Remove (const Mapping<Key,T>& items)
{
	if (GetRep () == items.GetRep ()) {
		RemoveAll ();
	}
	else {
#if		qRealTemplatesAvailable && 1
		for (Iterator<MapElement<Key,T> > it (items); it.More ();)
#else
		ForEach (MapElement<Key,T>, it, items)
#endif
		{
			MapElement<Key,T> current = it.Current ();
			if (Lookup (current.fKey, &current.fElt)) {
				fRep->RemoveAt (current.fKey);
			}
		}
	}
}

template	<class Key, class T> MappingRep<Key,T>*	Mapping<Key,T>::Clone (const MappingRep<Key,T>& src)
{
	return (src.Clone ());
}




/*
 ********************************************************************************
 **************************************** operators *****************************
 ********************************************************************************
 */
template	<class Key, class T> Boolean	operator== (const Mapping<Key,T>& lhs, const Mapping<Key,T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}

#if		qRealTemplatesAvailable && 1
	for (Iterator<MapElement<Key,T> > it (lhs); it.More ();)
#else
	ForEach (MapElement<Key,T>, it, lhs)
#endif
	{
		T elt = it.Current ().fElt;
		if (not rhs.Lookup (it.Current ().fKey, &elt)) {
			return (False);
		}
	}
	return (True);
}

template	<class Key, class T> Mapping<Key,T>	operator+ (const Mapping<Key,T>& lhs, const Mapping<Key,T>& rhs)
{
	Mapping<Key,T>	temp	= lhs;
	temp.Enter (rhs);
	return (temp);
}

template	<class Key, class T> Mapping<Key,T>	operator- (const Mapping<Key,T>& lhs, const Mapping<Key,T>& rhs)
{
	Mapping<Key,T>	temp	= lhs;
	temp.Remove (rhs);
	return (temp);
}


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SharedOfMappingRepOf<Key>_<T>_cc
	#include	IteratorOfMapElementOf<Key>_<T>_cc
#endif


#endif	/*qRealTemplatesAvailable*/

#endif	// This ifndef/define/endif stuff only needed if qRealTemplatesAvailable && !qTemplatesHasRepository

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

