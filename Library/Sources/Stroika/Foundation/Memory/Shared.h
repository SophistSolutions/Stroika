/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_Shared_h_
#define	_Stroika_Foundation_Memory_Shared_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

#include	"BlockAllocated.h"



/*
 * Description:
 *
 *		This class is for keeping track of a data structure with reference counts,
 *	and disposing of that structure when the reference count drops to zero.
 *	Copying one of these Shared<T> just increments the referce count,
 *	and destroying/overwriting one decrements it.
 *
 *		You can have a ptr having a nullptr value, and it can be copied.
 *	(Implementation detail - the reference count itself is NEVER nil except upon
 *	failure of alloction of memory in ctor and then only valid op on class is
 *	destruction). You can access the value with GetPointer () but this is not
 *	advised - only if it may be legitimately nullptr do you want to do this.
 *	Generaly just use ptr-> to access the data, and this will do the
 *	RequireNotNull (POINTER) for you.
 *
 *		This class can be enourmously useful in implementing letter/envelope -
 *	type data structures - see String, or Shapes, for examples.
 *
 *
 * TODO:
 *		+	Use BlockAllocated instead of our manual implementation
 *		+	SERIOUSLY CONSIDER relationship between this class and RefCntPtr<>
 *
 *
 */




namespace	Stroika {
	namespace	Foundation {
		namespace	Memory {

			template	<class T>	
				class	Shared {
					private:
						Shared ();	// illegal - dont call this...

					public:
						Shared (T* (*cloneFunction) (const T&), T* ptr);	// OK to pass nullptr for ptr but assign before use
						Shared (const Shared<T>& src);

						~Shared ();											// nonvirtual DTOR cuz we always keep these by value

						nonvirtual	Shared<T>& operator= (const Shared<T>& src);
						nonvirtual	Shared<T>& operator= (T* ptr);


						/*
						 * These operators require that the underlying ptr is non-nil.
						 */
						nonvirtual	const T*	operator-> () const;
						nonvirtual	T*			operator-> ();
						nonvirtual	const T&	operator* () const;
						nonvirtual	T&			operator* ();


						/*
						 * Counter is the type used for keeping track of how many references there are to a single
						 * object.
						 *
						 * CountReferences () returns how many references there are to the given object - typically
						 * we only care about the values 0, 1, and > 1.
						 *
						 * We define this method (CountReferences ()) here to avoid confusing over how to write
						 * the return type with broken, stupid compilers.
						 */
						typedef	uint32_t	Counter;
						nonvirtual	Counter	CountReferences () const	{ AssertNotNull (fCount); return (*fCount); }


						/*
						 * GetPointer () returns the real underlying ptr we store. It can be nullptr. This should
						 * rarely be used - use operator-> in preference. This is only for dealing with cases where
						 * the ptr could legitimately be nil.
						 */
						nonvirtual	const T*	GetPointer () const;
						nonvirtual	T*			GetPointer ();


						/*
						 * This routine is automatically called whenever a non-const method is called, to ensure
						 * that only the current copy, and no shared references, are affected by a modification.
						 * Users should never need to call this method directly, although class implementers may
						 * occasionally call this method directly. One plausible example of when this is needed
						 * is when the type T being wrapped does not make correct use of const.
						 */
						nonvirtual	void	Assure1Reference ();

					private:
						T*			fPtr;					// Pointer to the  thing being reference counted
						Counter*	fCount;					// Shared count - never nil - even for nil ptr
						T*			(*fCloner) (const T&);	// Called by BreakReferences to create new T

						nonvirtual	void	BreakReferences ();
				};

			template	<class	T>	bool	operator== (const Shared<T>& lhs, const Shared<T>& rhs);
			template	<class	T>	bool	operator!= (const Shared<T>& lhs, const Shared<T>& rhs);

		}
	}
}
#endif	/*_Stroika_Foundation_Memory_Shared_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Shared.inl"
