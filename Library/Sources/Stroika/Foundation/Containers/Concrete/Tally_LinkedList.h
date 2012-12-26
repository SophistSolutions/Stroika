/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_h_  1


#include    "../../StroikaPreComp.h"

#include    "../Tally.h"


/*
 *  TODO:
 *      o   Correctly implement override of Iterator<T>::IRep::StrongEquals ()
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {

                /**
                 *
                 */
                template    <typename T>
                class  Tally_LinkedList : public Tally<T> {
                private:
                    typedef Tally<T> inherited;

                public:
                    Tally_LinkedList ();
                    Tally_LinkedList (const Tally<T>& src);
                    Tally_LinkedList (const Tally_LinkedList<T>& src);
                    Tally_LinkedList (const T* items, size_t size);

                public:
                    nonvirtual  Tally_LinkedList<T>& operator= (const Tally_LinkedList<T>& src);

                private:
                    class   Rep_;
                    class   MutatorRep_;
                };

            }
        }
    }
}


#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_LinkedList_h_ */


#include    "Tally_LinkedList.inl"

