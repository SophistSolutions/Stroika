/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Tally_LinkedList_h_
#define	_Stroika_Foundation_Containers_Tally_LinkedList_h_	1


#include	"../StroikaPreComp.h"


#include	"Tally.h"




namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {


			template	<typename T> class	Tally_LinkedList : public Tally<T> {
				public:
					Tally_LinkedList ();
					Tally_LinkedList (const Tally<T>& src);
					Tally_LinkedList (const Tally_LinkedList<T>& src);
					Tally_LinkedList (const T* items, size_t size);

					nonvirtual	Tally_LinkedList<T>& operator= (const Tally_LinkedList<T>& src);
			};

		}
	}
}


#endif	/*_Stroika_Foundation_Containers_Tally_LinkedList_h_ */


#include "Tally_LinkedList.inl"

