/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Collection.cc,v 1.11 1992/10/10 20:22:10 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Collection.cc,v $
 *		Revision 1.11  1992/10/10  20:22:10  lewis
 *		Renamed Iterator->IteratorRep, IndirectIterator->Iterator, and
 *		similarly for subclasses (eg SequenceIterator, etc).
 *
 *		Revision 1.10  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.9  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.8  1992/09/11  16:37:59  sterling
 *		use new Shared implementation
 *
 *		Revision 1.7  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/04  21:00:50  sterling
 *		Got rid of Contains ().
 *
 *		Revision 1.4  1992/07/21  06:06:13  lewis
 *		Fixed bug in CollectionView DTOR and added CTOR/DTOR for CollectionIterator.
 *
 *		Revision 1.3  1992/07/17  03:48:11  lewis
 *		Got rid of class Mutator, and consider non-const CollectionIterator to
 *		be a Mutator. Very careful line by line analysis, and bugfixing, and
 *		implemented and tested (compiling) the macro version under GCC.
 *
 *		Revision 1.2  1992/06/25  02:11:05  lewis
 *		Fixed template syntax errors.
 *
 *		Revision 1.4  1992/05/21  17:24:13  lewis
 *		Fixed syntax errors after compiling for BC++.
 *		Then, added CollectionView, CollectionMutator, etc...
 *
 *		Revision 1.3  92/05/19  11:51:14  11:51:14  lewis (Lewis Pringle)
 *		Implemented template iostream inserter/extractor for Collections.
 *		
 *		Revision 1.2  92/05/13  00:30:59  00:30:59  lewis (Lewis Pringle)
 *		Implement op==/op!= templates.
 *
 */



#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Memory.hh"

#include	"Collection.hh"


#if		!qRealTemplatesAvailable && 0
	Implement (IteratorRep, VoidStar);
	BlockAllocatedImplement (CollectionIteratorOfVoidStar);
#endif


// CollectionRep
void	CollectionRep::PatchViewsAdd (CollectionItemKey key) const
{
	for (CollectionView* v = fViews; v != Nil; v = v->fNext) {
		v->PatchAdd (key);
	}
}

void	CollectionRep::PatchViewsRemove (CollectionItemKey key) const
{
	for (CollectionView* v = fViews; v != Nil; v = v->fNext) {
		v->PatchRemove (key);
	}
}
	
void	CollectionRep::PatchViewsRemoveAll () const
{
	for (CollectionView* v = fViews; v != Nil; v = v->fNext) {
		v->PatchRemoveAll ();
	}
}
	
// CollectionView
CollectionView::CollectionView (const CollectionRep& owner) :
	fOwner (owner),
	fNext (((CollectionRep&)owner).fViews)
{
	((CollectionRep&)owner).fViews = this;
}

CollectionView::~CollectionView ()
{
	CollectionRep& owner = (CollectionRep&) fOwner;
	if (owner.fViews == this) {
		owner.fViews = fNext;
	}
	else {
		for (CollectionView* v = owner.fViews; v->fNext != this; v = v->fNext) {
			AssertNotNil (v);
			AssertNotNil (v->fNext);
		}
		AssertNotNil (v);
		Assert (v->fNext == this);
		v->fNext = fNext;
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

