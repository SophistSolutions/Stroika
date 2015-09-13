/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/SampleCollections.cpp,v 2.5 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: SampleCollections.cpp,v $
 *	Revision 2.5  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.4  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1995/12/13  06:26:23  lewis
 *	Lose unused collection types
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *
 *
 *
 */

#include	"SampleCollections.h"

// --- OpenDoc Includes ---

#ifndef _ODTYPES_
#include <ODTypes.h>
#endif

#ifndef SOM_ODFrame_xh
#include <Frame.xh>
#endif

// --- OpenDoc Utilities ---

#ifndef _EXCEPT_
#include <Except.h>
#endif

#ifndef _ODNEW_
#include <ODNew.h>
#endif

#ifndef _LINKLIST_
#include <LinkList.h>
#endif

#ifndef _LIMITS_
#include <Limits.h>
#endif

#ifndef _ODUTILS_
#include <ODUtils.h>
#endif


//====================================================================
// CGenericLink
//====================================================================

CGenericLink::CGenericLink()
	:Link()
{
	fValue = kODNULL;
}

CGenericLink::CGenericLink(void* value)
	:Link()
{
	fValue = value;
}

CGenericLink::~CGenericLink()
{
}

void* CGenericLink::GetValue()
{
	return fValue;
}

void CGenericLink::SetValue(void* value)
{
	fValue = value;
}












CListIterator::~CListIterator()
{
	delete fIter;
}
	
void* CListIterator::First()
{
	CGenericLink* link = (CGenericLink*) fIter->First();

	//$$$$$ for empty list, link will be null
	return (void*) link ? link->GetValue() : kODNULL;
}

void* CListIterator::Next()
{
	CGenericLink* link = (CGenericLink*) fIter->Next();
	
	//$$$$$ At end of list, link will be null
	return (void*) link ? link->GetValue() : kODNULL;
}

void* CListIterator::Previous()
{
	CGenericLink* link = (CGenericLink*) fIter->Previous();
	
	//$$$$$ At head of list, link will be null
	return (void*) link ? link->GetValue() : kODNULL;
}

void* CListIterator::Last()
{
	CGenericLink* link = (CGenericLink*) fIter->Last();

	//$$$$$ for empty list, link will be null
	return (void*) link ? link->GetValue() : kODNULL;
}

void* CListIterator::Current()
{
	CGenericLink* link = (CGenericLink*) fIter->Current();

	//$$$$$ for empty list, link will be null
	return (void*) link ? link->GetValue() : kODNULL;
}

ODBoolean CListIterator::IsNotComplete()
{
	return fIter->IsNotComplete();
}

void CListIterator::RemoveCurrent()
{
	fIter->RemoveCurrent();
}

void CListIterator::DeleteCurrent()
{
	CGenericLink* link = (CGenericLink*) fIter->Current();
	fIter->RemoveCurrent();
	
	//$$$$$ for empty list, link is null
	if (link)
		delete link->GetValue();
		
	delete link;
}

//====================================================================
// COrderedList
//====================================================================

COrderedList::COrderedList()
{
}

COrderedList::~COrderedList()
{
	fList.DeleteAllLinks();
}

COrderedList::COrderedList(COrderedList *list)
{
	// (MH)  Copy constructor for COrderedList
	COrdListIterator iter(list);
	for (void* elem = iter.First(); iter.IsNotComplete(); elem = iter.Next())
	{
		this->AddLast(elem);
	}
}

ODBoolean COrderedList::IsEmpty() const
{
	return fList.IsEmpty();
}

ODULong COrderedList::Count() const
{
	return fList.Count();
}

ODBoolean COrderedList::Contains(const void* value)
{
	if ( fList.IsEmpty() ) return kODFalse;
	
	LinkedListIterator	iter(&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		if ( ((CGenericLink*)link)->GetValue() == value )
			return kODTrue;
	}

	return kODFalse;
}

ODUShort COrderedList::Position(const void* value)
{
	if ( fList.IsEmpty() ) return kListIsEmpty;
	
	ODUShort position = 0;
	LinkedListIterator	iter(&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		position++;
		if ( ((CGenericLink*)link)->GetValue() == value )
			return kODTrue;
	}

	return kItemNotFound;
}

void COrderedList::DeleteAllLinks()
{
	LinkedListIterator	iter(&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		delete (((CGenericLink*)link)->GetValue());
	}
	fList.RemoveAll();
}

void COrderedList::RemoveAllLinks()
{
	fList.RemoveAll();
}

void COrderedList::Delete(void* value)
{
	LinkedListIterator	iter(&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		if ( ((CGenericLink*)link)->GetValue() == value )
		{
			delete (((CGenericLink*)link)->GetValue());
			fList.Remove(*link);
			return;
		}
	}
}

void COrderedList::Remove(void* value)
{
	LinkedListIterator	iter(&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		if ( ((CGenericLink*)link)->GetValue() == value )
		{
			fList.Remove(*link);
			return;
		}
	}
}

void* COrderedList::RemoveFirst()
{
	CGenericLink* link = (CGenericLink*) fList.RemoveFirst();
	return link ? (void*) link->GetValue() : (void*)kODNULL;
}

void* COrderedList::RemoveLast()
{
	CGenericLink* link = (CGenericLink*) fList.RemoveLast();
	return link ? (void*) link->GetValue() : (void*)kODNULL;
}

void COrderedList::AddBefore(const void* existing, void* value)
{
	LinkedListIterator	iter(&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		if ( ((CGenericLink*)link)->GetValue() == existing )
		{
			CGenericLink* newLink = new CGenericLink(value);
			fList.AddBefore(*link,newLink);
			return;
		}
	}
}

void COrderedList::AddAfter(const void* existing, void* value)
{
	LinkedListIterator	iter(&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		if ( ((CGenericLink*)link)->GetValue() == existing )
		{
			CGenericLink* newLink = new CGenericLink(value);
			fList.AddAfter(*link,newLink);
			return;
		}
	}
}

void COrderedList::AddFirst(void* value)
{
	CGenericLink* link = new CGenericLink(value);
	fList.AddFirst(link);
}

void COrderedList::AddLast(void* value)
{
	CGenericLink* link = new CGenericLink(value);
	fList.AddLast(link);
}

void* COrderedList::After(const void* value) const
{
	LinkedListIterator	iter((LinkedList*)&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		if ( ((CGenericLink*)link)->GetValue() == value )
		{
			Link* after = fList.After(*link);
			// $$$$$ After may be null, if matched element was at end of list!
			return (void*) after ? ((CGenericLink*)after)->GetValue() : kODNULL;
		}
	}
	return kODNULL;
}

void* COrderedList::Before(const void* value) const
{
	LinkedListIterator	iter((LinkedList*)&fList);
	for (Link* link=iter.First();iter.IsNotComplete();link=iter.Next())
	{
		if ( ((CGenericLink*)link)->GetValue() == value )
		{
			//$$$$$ it would be nice to call the previous link 'before', not 'after'
			Link* before = fList.Before(*link);
			// $$$$$ before may be null, if matched element was at head of list!
			return (void*) before ? ((CGenericLink*)before)->GetValue() : kODNULL;
		}
	}
	return kODNULL;
}

void* COrderedList::First() const
{
	CGenericLink* link = (CGenericLink*) fList.First();
	
	//$$$$$ (MH) More empty list problems fixed
	return (void*) link ? link->GetValue() : kODNULL;
}

void* COrderedList::Last() const
{
	CGenericLink* link = (CGenericLink*) fList.Last();
	
	//$$$$$ (MH) More empty list problems fixed
	return (void*) link ? link->GetValue() : kODNULL;
}

//====================================================================
// COrdListIterator
//====================================================================

COrdListIterator::COrdListIterator(COrderedList* list)
{
	fIter = new LinkedListIterator(&list->fList);
}

COrdListIterator::~COrdListIterator()
{
}
