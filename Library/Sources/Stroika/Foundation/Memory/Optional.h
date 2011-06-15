/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_Optional_h_
#define	_Stroika_Foundation_Memory_Optional_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Basics.h"

//TEMPORARILY - WE MUST RE-IMPLEMENT NOT USING REFCNTPTR
#include	"RefCntPtr.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {

			template	<typename T>
				class	Opt {
					public:
						Opt ();
						Opt (const T& from);
						Opt (const Opt<T>& from);
			
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
						nonvirtual	bool	operator< (const Opt<T>& rhs) const;
						nonvirtual	bool	operator<= (const Opt<T>& rhs) const;
						nonvirtual	bool	operator> (const Opt<T>& rhs) const;
						nonvirtual	bool	operator>= (const Opt<T>& rhs) const;
						nonvirtual	bool	operator== (const Opt<T>& rhs) const;
						nonvirtual	bool	operator!= (const Opt<T>& rhs) const;

					private:
						/*
						 * Note: the implementation here is safe via copying - because we never WRITE to the value stored in the
						 * RefCntPtr<T> and never return a copy to someone who could, so the value can never change. One changes
						 * an Opt<T> by creating a whole new value object and assigning it.
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


	//	class	Optional::Opt<T>
	template	<typename T>
		inline	Opt<T>::Opt ()
			: fValue (NULL)
			{
			}
	template	<typename T>
		inline	Opt<T>::Opt (const T& from)
			: fValue (new T (from))
			{
			}
	template	<typename T>
		inline	Opt<T>::Opt (const Opt<T>& from)
			: fValue (from.fValue)
			{
			}
	template	<typename T>
		inline	void	Opt<T>::clear ()
			{
				fValue.clear ();
			}
	template	<typename T>
		inline	const typename T*	Opt<T>::get () const
			{
				return fValue.IsNull ()? NULL : fValue.get ();
			}
	template	<typename T>
		inline	bool	Opt<T>::empty () const
			{
				return fValue.IsNull ();
			}
	template	<typename T>
		inline	const T* Opt<T>::operator-> () const
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return &fValue.GetRep ();
			}
	template	<typename T>
		inline	T* Opt<T>::operator-> ()
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return &fValue.GetRep ();
			}
	template	<typename T>
		inline	const T& Opt<T>::operator* () const
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return fValue.GetRep ();
			}
	template	<typename T>
		inline	T& Opt<T>::operator* ()
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return fValue.GetRep ();
			}
	template	<typename T>
		inline	Opt<T>::operator T () const
			{
				if (fValue.IsNull ()) {
					throw bad_alloc ();
				}
				return *fValue;
			}
	template	<typename T>
		bool	Opt<T>::operator< (const Opt<T>& rhs) const
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
		bool	Opt<T>::operator<= (const Opt<T>& rhs) const
			{
				return *this < rhs or *this == rhs;
			}
	template	<typename T>
		inline	bool	Opt<T>::operator> (const Opt<T>& rhs) const
			{
				return rhs < *this;
			}
	template	<typename T>
		bool	Opt<T>::operator>= (const Opt<T>& rhs) const
			{
				return *this > rhs or *this == rhs;
			}
	template	<typename T>
		bool	Opt<T>::operator== (const Opt<T>& rhs) const
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
		inline	bool	Opt<T>::operator!= (const Opt<T>& rhs) const
			{
				return not (*this == rhs);
			}


		}
	}
}
#endif	/*_Stroika_Foundation_Memory_Optional_h_*/
