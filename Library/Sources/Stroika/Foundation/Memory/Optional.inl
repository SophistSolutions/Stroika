/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_Optional_inl_
#define	_Stroika_Foundation_Memory_Optional_inl_	1


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
#endif	/*_Stroika_Foundation_Memory_Optional_inl_*/
