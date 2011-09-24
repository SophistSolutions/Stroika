/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */

#ifndef	_Stroika_Foundation_Containers_Array_h_
#define	_Stroika_Foundation_Containers_Array_h_

/*
 *
 *	Description:
 *
 *		Array<T> is a backend implementation. It is not intended to be directly
 *	used by programmers, except in implementing concrete container reps.
 *
 *		Array<T> is a template which provides a dynamic array class. Elements
 *	of type T can be assigned, and accessed much like a normal array, except
 *	that when debug is on, accesses are range-checked.
 *
 *		Array<T> also provides a dynamic sizing capability. It reallocs its
 *	underlying storgage is such a ways as to keep a buffer of n(currently 5)%
 *	extra, so that reallocs on resizes only occur logn times on n appends.
 *	To save even this space, you can call Compact().
 *
 *		Unlike other dynamic array implementations, when an item is removed,
 *	it is destructed then. So the effects of buffering have no effects on the
 *	semantics of the Array.
 *
 *
 *
 *	Notes:
 *
 *	Warning:
 *		This implemenation prohibits use of internal pointers within T
 *	since we sometimes copy the contents of the array without excplicitly using
 *	the X(X&) and T::operator= functions.
 *
 *
 *	C++/StandardC arrays and segmented architectures:
 *
 *		Our iterators use address arithmatic since that is faster than
 *	array indexing, but that requires care in the presence of patching,
 *	and in iterating backwards.
 *
 *		The natural thing to do in iteration would be to have fCurrent
 *	point to the current item, but that would pose difficulties in the
 *	final test at the end of the iteration when iterating backwards. The
 *	final test would be fCurrent < fStart. This would be illegal in ANSI C.
 *
 *		The next possible trick is for backwards iteration always point one
 *	past the one you mean, and have it.Current () subtract one before
 *	dereferncing. This works pretty well, but makes source code sharing between
 *	the forwards and backwards cases difficult.
 *
 *		The next possible trick, and the one we use for now, is to have
 *	fCurrent point to the current item, and in the Next() code, when
 *	going backwards, reset fCurrent to fEnd - bizzare as this may seem
 *	then the test code can be shared among the forwards and backwards
 *	implemenations, all the patching code can be shared, with only this
 *	one minor check. Other potential choices are presented in the TODO
 *	below.
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

                /*
                 *		We use this class ArrayNode as a hack to work around bugs in many
                 *	C++ compilers (most notewory is CFront 2.1 - I'm not sure about others).
                 *	The trouble is we must be able to call T::~T on builtin types like
                 *	void* and int, and some compilers dont allow this. So we wrap a dummy
                 *	class around T, and our problems go away. Everything is inline so
                 *	this has no cost. When the last braindead compiler has been safely supplanted
                 *	on all the machines we care about, we can loose this class.
                 *	(LGP Tuesday, November 17, 1992 1:32:25 PM)
                 */
                template	<class	T>	class	ArrayNode {
                    public:
                        ArrayNode (T item);
                        ~ArrayNode ();


                    public:
                        T	fItem;
                };


                template	<class	T>	class	ArrayIteratorBase;
                template	<class	T>	class	ArrayIterator_PatchBase;


                /*
                 *		This class is the main core of the implementation. It provides
                 *	an array abstraction, where the size can be set dynamically, and
                 *	extra sluff is maintained off the end to reduce copying from reallocs.
                 *	Only items 0..GetLength ()-1 are kept constructed. The rest (GetLength()+1
                 *	..fSlotsAlloced) are uninitialized memory. This is important because
                 *	it means you can count on DTORs of your T being called when you
                 *	remove them from contains, not when the caches happen to empty.
                 */
                template      <class  T>      class   Array {
                    public:
                        Array ();
                        Array (const Array<T>& from);
                        ~Array ();

                        nonvirtual	Array<T>& operator= (const Array<T>& rhs);

                    public:
                        nonvirtual	T		GetAt (size_t i) const;
                        nonvirtual	void	SetAt (T item, size_t i);
                        nonvirtual	T&		operator[] (size_t i);
                        nonvirtual	T		operator[] (size_t i) const;

                        nonvirtual	size_t	GetLength () const;
                        nonvirtual	void	SetLength (size_t newLength, T fillValue);

                        nonvirtual	void	InsertAt (T item, size_t index);
                        nonvirtual	void	RemoveAt (size_t index);
                        nonvirtual	void	RemoveAll ();

                        nonvirtual	bool	Contains (T item) const;

                        /*
                         * Memory savings/optimization methods.  Use this to tune useage
                         * of arrays so that they dont waste time in Realloc's.
                         */
                        nonvirtual	size_t	GetSlotsAlloced () const;
                        nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);

                        nonvirtual	void	Compact ();

                        nonvirtual	void	Invariant () const;

                    protected:
                        size_t			fLength;			// #items advertised/constructed
                        size_t			fSlotsAllocated;	// #items allocated (though not necessarily initialized)
                        ArrayNode<T>*	fItems;

                #if		qDebug
                        virtual	void	Invariant_ () const;
                #endif

                    friend	class	ArrayIteratorBase<T>;
                    friend	class	ArrayIterator_PatchBase<T>;
                };



                /*
                 *		ArrayIteratorBase<T> is an un-advertised implementation
                 *	detail designed to help in source-code sharing among various
                 *	iterator implementations.
                 */
                template	<typename T>	class	ArrayIteratorBase {
                    private:
                        ArrayIteratorBase ();		// not defined - do not call.

                    public:
                        ArrayIteratorBase (const Array<T>& data);

                        nonvirtual	T		Current () const;			//	Error to call if Done (), otherwise OK
                        nonvirtual	size_t	CurrentIndex () const;		//	NB: This can be called if we are done - if so, it returns GetLength() + 1.
                        nonvirtual	bool	More (T* current, bool advance);
                        nonvirtual	bool	Done () const;

                        nonvirtual	void	Invariant () const;

                    protected:
                #if		qDebug
                        const Array<T>*		fData;
                #endif
                        const ArrayNode<T>*	fStart;			// points to FIRST elt
                        const ArrayNode<T>*	fEnd;			// points 1 PAST last elt
                        const ArrayNode<T>*	fCurrent;		// points to CURRENT elt (SUBCLASSES MUST INITIALIZE THIS!)
                        bool				fSuppressMore;	// Indicates if More should do anything, or if were already Mored...

                #if		qDebug
                        virtual	void	Invariant_ () const;
                #endif
                };



                /*
                 *		Use this iterator to iterate forwards over the array. Be careful
                 *	not to add or remove things from the array while using this iterator,
                 *	since it is not safe. Use ForwardArrayIterator_Patch for those cases.
                 */
                template	<typename T> class	ForwardArrayIterator : public ArrayIteratorBase<T> {
                    public:
                        ForwardArrayIterator (const Array<T>& data);

                        nonvirtual	bool	More (T* current, bool advance);
                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity

                    private:
                        typedef ArrayIteratorBase<T>    inherited;
                };




                /*
                 *		ForwardArrayMutator<T> is the same as ForwardArrayIterator<T> but
                 *	adds the ability to update the contents of the array as you go along.
                 */
                template	<typename T> class	ForwardArrayMutator : public ForwardArrayIterator<T> {
                    public:
                        ForwardArrayMutator (Array<T>& data);

                        nonvirtual	bool	More (T* current, bool advance);                // shadow to avoid scope ambiguity
                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity

                        nonvirtual	void	UpdateCurrent (T newValue);

                    private:
                        typedef ForwardArrayIterator<T>    inherited;
                };




                /*
                 *		Use this iterator to iterate backwards over the array. Be careful
                 *	not to add or remove things from the array while using this iterator,
                 *	since it is not safe. Use BackwardArrayIterator_Patch for those cases.
                 */
                template	<typename T> class	BackwardArrayIterator : public ArrayIteratorBase<T> {
                    public:
                        BackwardArrayIterator (const Array<T>& data);

                        nonvirtual	bool	More (T* current, bool advance);
                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity

                    private:
                        typedef ArrayIteratorBase<T>    inherited;
                };




                /*
                 *		BackwardArrayMutator<T> is the same as BackwardArrayIterator<T> but
                 *	adds the ability to update the contents of the array as you go along.
                 */
                template	<typename T> class	BackwardArrayMutator : public BackwardArrayIterator<T> {
                    public:
                        BackwardArrayMutator (Array<T>& data);

                        nonvirtual	bool	More (T* current, bool advance);      // shadow to avoid scope ambiguity
                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity

                        nonvirtual	void	UpdateCurrent (T newValue);

                    private:
                        typedef BackwardArrayIterator<T>    inherited;
                };



                /*
                 *	Patching Support:
                 *
                 *		Here we provide Patching Versions of each iterator, and for convienience
                 *	versions of array that maintain a list of all Patching iterators of a given
                 *	type.
                 */


                template	<typename T>	class	ArrayIterator_PatchBase;


                /*
                 *		Array_Patch<T> is an array implemantion that keeps a list of patchable
                 *	iterators, and handles the patching automatically for you. Use this if
                 *	you ever plan to use patchable iterators.
                 */
                template	<typename T>	class	Array_Patch : public Array<T> {
                    public:
                        Array_Patch ();
                        Array_Patch (const Array_Patch<T>& from);
                        ~Array_Patch ();

                        nonvirtual	Array_Patch<T>& operator= (const Array_Patch<T>& rhs);

                    /*
                     * Methods we shadow so that patching is done. If you want to circumvent the
                     * patching, thats fine - use scope resolution operator to call this's base
                     * class version.
                     */
                    public:
                        nonvirtual	void	SetLength (size_t newLength, T fillValue);
                        nonvirtual	void	InsertAt (T item, size_t index);
                        nonvirtual	void	RemoveAt (size_t index);
                        nonvirtual	void	RemoveAll ();
                        nonvirtual	void	SetSlotsAlloced (size_t slotsAlloced);
                        nonvirtual	void	Compact ();

                    /*
                     * Methods to do the patching yourself. Iterate over all the iterators and
                     * perform patching.
                     */
                    public:
                        nonvirtual	bool	HasActiveIterators () const;			//	are there any iterators to be patched?
                        nonvirtual	void	PatchViewsAdd (size_t index) const;		//	call after add
                        nonvirtual	void	PatchViewsRemove (size_t index) const;	//	call before remove
                        nonvirtual	void	PatchViewsRemoveAll () const;			//	call after removeall
                        nonvirtual	void	PatchViewsRealloc () const;				//	call after realloc could have happened


                    /*
                     *	Check Invariants for this class, and all the iterators we own.
                     */
                    public:
                        nonvirtual	void	Invariant () const;

                    private:
                        ArrayIterator_PatchBase<T>*	fIterators;

                        friend	class	ArrayIterator_PatchBase<T>;

                #if		qDebug
                        virtual	void	Invariant_ () const override;
                        nonvirtual	void	InvariantOnIterators_ () const;
                #endif
                };




                /*
                 *		ArrayIterator_PatchBase<T> is a private utility class designed
                 *	to promote source code sharing among the patched iterator implementations.
                 */
                template	<typename T>	class	ArrayIterator_PatchBase : public ArrayIteratorBase<T> {
                    public:
                        ArrayIterator_PatchBase (const Array_Patch<T>& data);
                        ArrayIterator_PatchBase (const ArrayIterator_PatchBase<T>& from);
                        ~ArrayIterator_PatchBase ();

                        nonvirtual	ArrayIterator_PatchBase<T>&	operator= (const ArrayIterator_PatchBase<T>& rhs);

                        nonvirtual	size_t	CurrentIndex () const;  // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity

                   public:
                        nonvirtual	void	PatchAdd (size_t index);		//	call after add
                        nonvirtual	void	PatchRemove (size_t index);		//	call before remove
                        nonvirtual	void	PatchRemoveAll ();				//	call after removeall
                        nonvirtual	void	PatchRealloc ();				//	call after realloc could have happened

                    protected:
                        const Array_Patch<T>* 		fData;
                        ArrayIterator_PatchBase<T>*	fNext;

                #if		qDebug
                        virtual	void	Invariant_ () const override;
                #endif

                        virtual		void	PatchRemoveCurrent ()	=	0;	// called from patchremove if patching current item...

                        friend	class	Array_Patch<T>;
                    private:
                        typedef ArrayIteratorBase<T>    inherited;
                };




                /*
                 *		ForwardArrayIterator_Patch<T> is forwards iterator that can be used
                 *	while modifing its owned array. It can only be used with Array_Patch<T>
                 *	since the classes know about each other, and keep track of each other.
                 *	This is intended to be a convienience in implementing concrete container
                 *	mixins.
                 */
                template	<typename T> class	ForwardArrayIterator_Patch : public ArrayIterator_PatchBase<T> {
                    public:
                        ForwardArrayIterator_Patch (const Array_Patch<T>& data);

                    public:
                        nonvirtual	bool	More (T* current, bool advance);
                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity

                    protected:
                        virtual	void	PatchRemoveCurrent () override;

                   private:
                        typedef ArrayIterator_PatchBase<T>    inherited;
                };




                /*
                 *		ForwardArrayMutator_Patch<T> is the same as ForwardArrayIterator_Patch<T> but
                 *	adds the ability to update the contents of the array as you go along.
                 */
                template	<typename T> class	ForwardArrayMutator_Patch : public ForwardArrayIterator_Patch<T> {
                    public:
                        ForwardArrayMutator_Patch (Array_Patch<T>& data);

                        nonvirtual	size_t	CurrentIndex () const;  // shadow to avoid scope ambiguity

                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity

                    public:
                        nonvirtual	void	RemoveCurrent ();
                        nonvirtual	void	UpdateCurrent (T newValue);
                        nonvirtual	void	AddBefore (T item);				//	NB: Can be called if done
                        nonvirtual	void	AddAfter (T item);

                   private:
                        typedef ForwardArrayIterator_Patch<T>    inherited;
                };




                /*
                 *		BackwardArrayIterator_Patch<T> is backwards iterator that can be used
                 *	while modifing its owned array. It can only be used with Array_Patch<T>
                 *	since the classes know about each other, and keep track of each other.
                 *	This is intended to be a convienience in implementing concrete container
                 *	mixins.
                 */
                template	<typename T> class	BackwardArrayIterator_Patch : public ArrayIterator_PatchBase<T> {
                    public:
                        BackwardArrayIterator_Patch (const Array_Patch<T>& data);

                    public:
                        nonvirtual	size_t	CurrentIndex () const;  // shadow to avoid scope ambiguity

                        nonvirtual	bool	More (T* current, bool advance);
                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity
                    protected:
                        virtual	void	PatchRemoveCurrent () override;

                   private:
                        typedef ArrayIterator_PatchBase<T>    inherited;
                };




                /*
                 *		BackwardArrayMutator_Patch<T> is the same as BackwardArrayIterator_Patch<T> but
                 *	adds the ability to update the contents of the array as you go along.
                 */
                template	<typename T> class	BackwardArrayMutator_Patch : public BackwardArrayIterator_Patch<T> {
                    public:
                        BackwardArrayMutator_Patch (Array_Patch<T>& data);

                        nonvirtual	size_t	CurrentIndex () const;  // shadow to avoid scope ambiguity
                        nonvirtual	bool	Done () const;          // shadow to avoid scope ambiguity

                        nonvirtual	void	Invariant () const;     // shadow to avoid scope ambiguity
                    public:
                        nonvirtual	void	RemoveCurrent ();
                        nonvirtual	void	UpdateCurrent (T newValue);
                        nonvirtual	void	AddBefore (T item);
                        nonvirtual	void	AddAfter (T item);				//	NB: Can be called if done

                   private:
                        typedef BackwardArrayIterator_Patch<T>    inherited;
                };
		    }
		}
	}
}


#endif	/*_Stroika_Foundation_Containers_Array_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Array.inl"

