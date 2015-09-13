/*
	File:		Collections.h

	Contains:	Sample collection functions & classes

	Written by:	Steve Smith

	Copyright:	© 1995 by Apple Computer, Inc., all rights reserved.

	Description:
				CList: Generic unordered list
				COrderedList: Generic ordered list
				CFrameList: Unordered list of frames -
							frames automatically refcounted when 
							added/removed from list.
				CQueue: Generic queue collection
				CStack: Generic stack collection
*/


#ifndef _SAMPLECOLLECTIONS_
#define _SAMPLECOLLECTIONS_

// --- OpenDoc Includes ---

#ifndef _ODTYPES_
#include <ODTypes.h>
#endif

#ifndef SOM_ODFrame_xh
#include <Frame.xh>
#endif

// --- OpenDoc Utilities ---

#ifndef _ODNEW_
#include <ODNew.h>
#endif

#ifndef _LINKLIST_
#include "LinkList.h"
#endif

enum {
	kItemNotFound = -1,
	kListIsEmpty = 0
};

//------------------------------------------------------------------------------
// Collection Class Definitions
//------------------------------------------------------------------------------

class CGenericLink: public Link {
	public:
		CGenericLink();
		CGenericLink(void* value);
		virtual ~CGenericLink();
		
		void*	GetValue();
		void	SetValue(void* value);
		
	protected:
		void*	fValue;
};

class CListIterator {
	public:
		CListIterator() {}
		virtual ~CListIterator();
			
		void*		First();
		void*		Next();
		void*		Previous();
		void*		Last();
		void*		Current();
		ODBoolean 	IsNotComplete();
		void		RemoveCurrent();
		void		DeleteCurrent();

	protected:
		LinkedListIterator*	fIter;
};

class COrderedList {
	public:
		COrderedList();
		COrderedList(COrderedList *list);
		virtual ~COrderedList();
	
	  	ODBoolean	IsEmpty() const;
		ODULong		Count() const;
	  	ODBoolean	Contains(const void* value);
	  	ODUShort	Position(const void* value);
	  	void		DeleteAllLinks();
	  	void		RemoveAllLinks();
	  	void		Delete(void* valeu);
	  	void		Remove(void* value);
	  	void*		RemoveFirst();
	  	void*		RemoveLast();
	  	void		AddBefore(const void* existing, void* value);
	  	void		AddAfter(const void* existing, void* value);
	  	void		AddFirst(void* value);
	  	void		AddLast(void* value);
	  	void*		After(const void* value) const;
	  	void*		Before(const void* value) const;
	  	void*		First() const;
	  	void*		Last() const;
  	  	
  	private:
  		LinkedList		fList;
  	
  	friend class COrdListIterator;
};

class COrdListIterator : public CListIterator {
	public:
	COrdListIterator(COrderedList* list);
	virtual ~COrdListIterator();
};

#endif //_SAMPLECOLLECTIONS_

