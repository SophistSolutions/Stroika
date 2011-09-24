/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */

#ifndef	_Stroika_Foundation_Containers_LinkedList_h_
#define	_Stroika_Foundation_Containers_LinkedList_h_

/*
 *
 * TODO:
 *			+	Consider adding LinkedListMutator<T> since otherwise we cannot do InsertAt,
 *				/ AddAfter() etc without using a LinkedList_Patch.
 *
 * Notes:
 *
 *		Slightly unusual behaviour for LinkedListMutator_Patch<T>::AddBefore () -
 *	allow it to be called when we are Done() - otherwise there is NO
 *	WAY TO APPEND TO A LINK LIST!!!
 *
 *
 *
 */


#include	"../../StroikaPreComp.h"

#include	"../../Configuration/Common.h"

#include "../Common.h"



namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {
		    namespace Private {

				template	<typename	T>	class	LinkedListIterator;
				template	<typename	T>	class	LinkedListIterator_Patch;
				template	<typename	T>	class	LinkedListMutator_Patch;

				template	<typename	T>	class	Link {
					public:
						Link (T item, Link<T>* next);

						static	void*	operator new (size_t size);
						static	void	operator delete (void* p);

					public:
						T			fItem;
						Link<T>*	fNext;
				};



				/*
				 *		LinkedList<T> is a generic link (non-intrusive) list implementation.
				 *	It keeps a length count so access to its length is rapid. We provide
				 *	no public means to access the links themselves.
				 *
				 *		Since this class provides no patching support, it is not generally
				 *	used - more likely you want to use LinkedList_Patch<T>. Use this if you
				 *	will manage all patching, or know that none is necessary.
				 */
				template	<typename	T>	class	LinkedList {
					public:
						LinkedList ();
						LinkedList (const LinkedList<T>& from);
						~LinkedList ();

					public:
						nonvirtual	LinkedList<T>& operator= (const LinkedList<T>& list);

					public:
						nonvirtual	size_t	GetLength () const;

					/*
					 *		Basic access to the head of the list. This is what is most
					 *	commonly used, and is most efficient.
					 */
					public:
						nonvirtual	T		GetFirst () const;
						nonvirtual	void	Prepend (T item);
						nonvirtual	void	RemoveFirst ();

					/*
					 *	Utility to search the list for the given item using operator==
					 */
					public:
						nonvirtual	bool	Contains (T item) const;


					public:
						nonvirtual	void	Remove (T item);
						nonvirtual	void	RemoveAll ();


					/*
					 *		Not alot of point in having these, as they are terribly slow,
					 *	but the could be convienient.
					 */
					public:
						nonvirtual	T		GetAt (size_t i) const;
						nonvirtual	void	SetAt (T item, size_t i);

					public:
						nonvirtual	void	Invariant () const;

					protected:
						size_t		fLength;
						Link<T>*	fFirst;

				#if		qDebug
						virtual		void	Invariant_ () const;
				#endif

					friend	class	LinkedListIterator<T>;
					friend	class	LinkedListIterator_Patch<T>;
					friend	class	LinkedListMutator_Patch<T>;
				};



				/*
				 *		LinkedListIterator<T> allows you to iterate over a LinkedList<T>. Its API
				 *	is designed to make easy implemenations of subclasses of IteratorRep<T>.
				 *	It is unpatched - use LinkedListIterator_Patch<T> or LinkedListMutator_Patch<T>
				 *	for that.
				 */
				template	<class T>	class	LinkedListIterator {
					public:
						LinkedListIterator (const LinkedListIterator<T>& from);
						LinkedListIterator (const LinkedList<T>& data);

					public:
						nonvirtual	LinkedListIterator<T>& operator= (const LinkedListIterator<T>& list);

					public:
						nonvirtual	bool	Done () const;
						nonvirtual	bool	More (T* current, bool advance);
						nonvirtual	T		Current () const;

						nonvirtual	void	Invariant () const;

					protected:
						const Link<T>*	fCurrent;
						bool			fSuppressMore;	// Indicates if More should do anything, or if were already Mored...

				#if		qDebug
						virtual	void	Invariant_ () const;
				#endif
				};






				/*
				 *	Patching Support:
				 *
				 *		Here we provide Patching Versions of each iterator, and for convienience
				 *	versions of LinkedList that maintain a list of all Patching iterators.
				 */



				/*
				 *		LinkedList_Patch<T> is a LinkedList<T> with the ability to keep track of
				 *	owned patching iterators. These patching iterators will automatically be
				 *	adjusted when the link list is adjusted. This is the class of LinkedList
				 *	most likely to be used in implementing a concrete container class.
				 */
				template	<class T>	class	LinkedListIterator_Patch;
				template	<typename	T>	class	LinkedList_Patch : public LinkedList<T> {
					public:
						LinkedList_Patch ();
						LinkedList_Patch (const LinkedList_Patch<T>& from);
						~LinkedList_Patch ();

						nonvirtual	LinkedList_Patch<T>& operator= (const LinkedList_Patch<T>& list);

					/*
					 * Methods we shadow so that patching is done. If you want to circumvent the
					 * patching, thats fine - use scope resolution operator to call this's base
					 * class version.
					 */
					public:
						nonvirtual	void	Prepend (T item);
						nonvirtual	void	Remove (T item);
						nonvirtual	void	RemoveFirst ();
						nonvirtual	void	RemoveAll ();

					/*
					 * Methods to do the patching yourself. Iterate over all the iterators and
					 * perfrom patching.
					 */
					public:
						nonvirtual	bool	HasActiveIterators () const;					//	are there any iterators to be patched?
						nonvirtual	void	PatchViewsAdd (const Link<T>* link) const;		//	call after add
						nonvirtual	void	PatchViewsRemove (const Link<T>* link) const;	//	call before remove
						nonvirtual	void	PatchViewsRemoveAll () const;					//	call after removeall

					/*
					 *	Check Invariants for this class, and all the iterators we own.
					 */
					public:
						nonvirtual	void	Invariant () const;

					protected:
						LinkedListIterator_Patch<T>*	fIterators;

					friend	class	LinkedListIterator_Patch<T>;
				#if		qDebug
						virtual		void	Invariant_ () const override;
						nonvirtual	void	InvariantOnIterators_ () const;
				#endif
				};




				/*
				 *		LinkedListIterator_Patch<T> is a LinkedListIterator_Patch<T> that allows
				 *	for updates to the LinkedList<T> to be dealt with properly. It maintains a
				 *	link list of iterators headed by the LinkedList_Patch<T>, and takes care
				 *	of all patching details.
				 */
				template	<class T>	class	LinkedListIterator_Patch : public LinkedListIterator<T> {
					public:
						LinkedListIterator_Patch (const LinkedList_Patch<T>& data);
						LinkedListIterator_Patch (const LinkedListIterator_Patch<T>& from);
						~LinkedListIterator_Patch ();

						nonvirtual	LinkedListIterator_Patch<T>&	operator= (const LinkedListIterator_Patch<T>& rhs);

						/*
						 * Shadow more to keep track of prev.
						 */
						nonvirtual	bool	More (T* current, bool advance);

						nonvirtual	void	PatchAdd (const Link<T>* link);		//	call after add
						nonvirtual	void	PatchRemove (const Link<T>* link);	//	call before remove
						nonvirtual	void	PatchRemoveAll ();					//	call after removeall


					protected:
						const LinkedList_Patch<T>*	fData;
						LinkedListIterator_Patch<T>*	fNext;
						const Link<T>*				fPrev;		// keep extra previous link for fast patchremove
																// Nil implies fCurrent == fData->fFirst or its invalid,
																// and must be recomputed (it was removed itself)...

					friend	class	LinkedList_Patch<T>;
				#if		qDebug
						virtual	void	Invariant_ () const	override;
				#endif
				};




				/*
				 *		LinkedListMutator_Patch<T> is a LinkedListIterator_Patch<T> that allows
				 *	for changes to the LinkedList<T> relative to the position we are at
				 *	currently in the iteration. This is be used most commonly in
				 *	implementing concrete iterators for Stroika containers.
				 */
				template	<class T>	class	LinkedListMutator_Patch : public LinkedListIterator_Patch<T> {
					public:
						LinkedListMutator_Patch (LinkedList_Patch<T>& data);
						LinkedListMutator_Patch (const LinkedListMutator_Patch<T>& from);

						nonvirtual	LinkedListMutator_Patch<T>&	operator= (LinkedListMutator_Patch<T>& rhs);

						nonvirtual	void	RemoveCurrent ();
						nonvirtual	void	UpdateCurrent (T newValue);
						nonvirtual	void	AddBefore (T item);
						nonvirtual	void	AddAfter (T item);
				};

		    }
		}
	}
}



#endif	/*_Stroika_Foundation_Containers_LinkedList_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"LinkedList.inl"
