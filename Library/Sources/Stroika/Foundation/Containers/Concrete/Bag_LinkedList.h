/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Bag.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {

                template    <typename T>
				class  Bag_LinkedList : public Bag<T> {
                public:
                    Bag_LinkedList ();
                    Bag_LinkedList (const T* start, const T* end);
                    Bag_LinkedList (const Bag<T>& bag);
                    Bag_LinkedList (const Bag_LinkedList<T>& bag);

                    nonvirtual  Bag_LinkedList<T>&  operator= (const Bag_LinkedList<T>& bag);

				
                public:
                public:
					class   Rep_;
					class   MutatorRep_;
				};

            }
        }
    }
}


#endif  /*_Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include	"Bag_LinkedList.inl"


