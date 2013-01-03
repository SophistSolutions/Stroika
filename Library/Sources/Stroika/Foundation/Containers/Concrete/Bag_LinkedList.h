/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Bag_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../Bag.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   Correctly implement override of Iterator<T>::IRep::StrongEquals()
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {

                /**
                 * \brief Bag_LinkedList<T> is an LinkedList-based concrete implementation of the Bag<T> container pattern.
                 */
                template    <typename T>
                class  Bag_LinkedList : public Bag<T> {
                private:
                	typedef		Bag<T>	inherited;

                public:
                    Bag_LinkedList ();
                    Bag_LinkedList (const T* start, const T* end);
                    Bag_LinkedList (const Bag<T>& bag);
                    Bag_LinkedList (const Bag_LinkedList<T>& bag);

                    nonvirtual  Bag_LinkedList<T>&  operator= (const Bag_LinkedList<T>& bag);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;
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
#include    "Bag_LinkedList.inl"


