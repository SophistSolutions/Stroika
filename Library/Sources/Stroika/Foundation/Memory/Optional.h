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
						nonvirtual	const typename T*	get () const;
			
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



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


	//	class	Optional::Optional<T>
	template	<typename T>
		inline	Optional<T>::Optional ()
			: fValue (NULL)
			{
			}
	template	<typename T>
		inline	Optional<T>::Optional (const T& from)
			: fValue (new T (from))
			{
			}
	template	<typename T>
		inline	Optional<T>::Optional (const Optional<T>& from)
			: fValue (from.fValue)
			{
			}
	template	<typename T>
		inline	void	Optional<T>::clear ()
			{
				fValue.clear ();
			}
	template	<typename T>
		inline	const typename T*	Optional<T>::get () const
			{
				return fValue.IsNull ()? NULL : fValue.get ();
			}
	template	<typename T>
		inline	bool	Optional<T>::empty () const
			{
				return fValue.IsNull ();
			}
	template	<typename T>
		inline	const T* Optional<T>::operator-> () const
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return &fValue.GetRep ();
			}
	template	<typename T>
		inline	T* Optional<T>::operator-> ()
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return &fValue.GetRep ();
			}
	template	<typename T>
		inline	const T& Optional<T>::operator* () const
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return fValue.GetRep ();
			}
	template	<typename T>
		inline	T& Optional<T>::operator* ()
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return fValue.GetRep ();
			}
	template	<typename T>
		inline	Optional<T>::operator T () const
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return *fValue;
			}
	template	<typename T>
		bool	Optional<T>::operator< (const Optional<T>& rhs) const
			{
				if (fValue.IsNull ()) {
					return rhs.fValue.IsNull ()? false: true;	// arbitrary choice - but assume if lhs is empty thats less than any T value
				}
				if (rhs.fValue.IsNull ()) {
					return false;
				}
				return *fValue < rhs.fValue;
			}
	template	<typename T>
		bool	Optional<T>::operator<= (const Optional<T>& rhs) const
			{
				return *this < rhs or *this == rhs;
			}
	template	<typename T>
		inline	bool	Optional<T>::operator> (const Optional<T>& rhs) const
			{
				return rhs < *this;
			}
	template	<typename T>
		bool	Optional<T>::operator>= (const Optional<T>& rhs) const
			{
				return *this > rhs or *this == rhs;
			}
	template	<typename T>
		bool	Optional<T>::operator== (const Optional<T>& rhs) const
			{
				if (fValue.IsNull ()) {
					return rhs.fValue.IsNull ();
				}
				if (rhs.fValue.IsNull ()) {
					return false;
				}
				return *fValue == *rhs.fValue;
			}
	template	<typename T>
		inline	bool	Optional<T>::operator!= (const Optional<T>& rhs) const
			{
				return not (*this == rhs);
			}


		}
	}
}
#endif	/*_Stroika_Foundation_Memory_Optional_h_*/
