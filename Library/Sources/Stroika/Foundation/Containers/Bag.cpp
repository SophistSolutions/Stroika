/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */


#include	"../StroikaPreComp.h"

#include    "Iterator.h"
#include	"../Memory/SharedByValue.h"

#include "Bag.h"

#if 0



namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {

/*
 ********************************************************************************
 *************************************** Bag ************************************
 ********************************************************************************
 */


template	<class T>	Bag<T>::Bag () :
	fRep (&Clone, 0)
{
	*this = Bag_Array<T> ();
}

template	<class T>	Bag<T>::Bag (const T* items, size_t size) :
	fRep (&Clone, 0)
{
	*this = Bag_Array<T> (items, size);
}

template	<class T> void	Bag<T>::Add (T item)
{
	GetRep ()->Add (item);
	Ensure (not IsEmpty ());
}

template	<class T> void	Bag<T>::Add (Bag<T> items)
{
	ForEach (T, it, items) {
		GetRep ()->Add (it.Current ());
	}
}

template	<class T> void	Bag<T>::Remove (T item)
{
	GetRep ()->Remove (item);
}

template	<class T> void	Bag<T>::Remove (const Bag<T>& items)
{
	if (GetRep () == items.GetRep ()) {
		RemoveAll ();
	}
	else {
		ForEach (T, it, items) {
			GetRep ()->Remove (it.Current ());
		}
	}
}

template	<class T> size_t	Bag<T>::TallyOf (T item) const
{
	size_t count = 0;
	ForEach (T, it, *this) {
		if (it.Current () == item) {
			count++;
		}
	}
	return (count);
}

template	<class T> void	Bag<T>::AddItems (const T* items, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		Add (items[i]);
	}
}

template	<class T>	BagRep<T>*	Bag<T>::Clone (const BagRep<T>& rep)
{
	return (rep.Clone ());
}



/*
 ********************************************************************************
 ************************************ operators *********************************
 ********************************************************************************
 */
template	<class T> Boolean	operator== (const Bag<T>& lhs, const Bag<T>& rhs)
{
	if (lhs.GetRep () == rhs.GetRep ()) {
		return (True);
	}
	if (lhs.GetLength () != rhs.GetLength ()) {
		return (False);
	}

	// n^2 algorithm!!!
	ForEach (T, it, lhs) {
		if (lhs.TallyOf (it.Current ()) != rhs.TallyOf (it.Current ())) {
			return (False);
		}
	}
	return (True);
}

template	<class T> Bag<T>	operator+ (const Bag<T>& lhs, const Bag<T>& rhs)
{
	Bag<T>	temp	= lhs;
	temp += rhs;
	return (temp);
}

template	<class T> Bag<T>	operator- (const Bag<T>& lhs, const Bag<T>& rhs)
{
	Bag<T>	temp	= lhs;
	temp -= rhs;
	return (temp);
}

		}
	}
}

#endif
