/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_Optional_h_
#define	_Stroika_Foundation_Memory_Optional_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"

//TEMPORARILY - WE MUST RE-IMPLEMENT NOT USING REFCNTPTR
#include	"RefCntPtr.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {

			template	<typename T>
				class	Optional {
					public:
						Optional ();
						Optional (const T& from);
						Optional (const Optional<T>& from);
			
					public:
						nonvirtual	void	clear ();
						nonvirtual	bool	empty () const;	// means no value (it is optional!)

					public:
						// Unclear if we want a non-const version too?
						nonvirtual	const T*	get () const;
			
					public:
						nonvirtual	operator T () const;	// throws bad_alloc () if called when empty ()

					public:
						nonvirtual	const T* operator-> () const;
						nonvirtual	T* operator-> ();
						nonvirtual	const T& operator* () const;
						nonvirtual	T& operator* ();

					// Somewhat arbitrarily, treat NOT-PROVIDED (empty) as < any value of T
					public:
						nonvirtual	bool	operator< (const Optional<T>& rhs) const;
						nonvirtual	bool	operator<= (const Optional<T>& rhs) const;
						nonvirtual	bool	operator> (const Optional<T>& rhs) const;
						nonvirtual	bool	operator>= (const Optional<T>& rhs) const;
						nonvirtual	bool	operator== (const Optional<T>& rhs) const;
						nonvirtual	bool	operator!= (const Optional<T>& rhs) const;

					private:
						/*
						 * Note: the implementation here is safe via copying - because we never WRITE to the value stored in the
						 * RefCntPtr<T> and never return a copy to someone who could, so the value can never change. One changes
						 * an Optional<T> by creating a whole new value object and assigning it.
						 */
						RefCntPtr<T>	fValue;
				};
		}
	}
}
#endif	/*_Stroika_Foundation_Memory_Optional_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Optional.inl"
