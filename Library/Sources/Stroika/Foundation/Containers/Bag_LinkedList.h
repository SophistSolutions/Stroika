/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Bag_LinkedList_h_
#define	_Stroika_Foundation_Containers_Bag_LinkedList_h_

#include	"../StroikaPreComp.h"

#include	"Bag.h"



namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {

			template	<typename T> class	Bag_LinkedList : public Bag<T> {
				public:
					Bag_LinkedList ();
					Bag_LinkedList (const T* items, size_t size);
					Bag_LinkedList (const Bag<T>& bag);
					Bag_LinkedList (const Bag_LinkedList<T>& bag);

					nonvirtual	Bag_LinkedList<T>&	operator= (const Bag_LinkedList<T>& bag);
			};
		}
	}
}


#endif	/*_Stroika_Foundation_Containers_Bag_LinkedList_h_ */


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

 #include "Bag_LinkedList.inl"


