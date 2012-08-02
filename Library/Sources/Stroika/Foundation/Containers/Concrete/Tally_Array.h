/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Tally_Array_h_   1


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

                template    <typename T>    class   Tally_ArrayRep;
                template    <typename T>    class   Tally_Array : public Tally<T> {
                public:
                    Tally_Array ();
                    Tally_Array (const Tally<T>& src);
                    Tally_Array (const Tally_Array<T>& src);
                    Tally_Array (const T* items, size_t size);

                    nonvirtual  Tally_Array<T>& operator= (const Tally_Array<T>& src);

                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                private:
                    nonvirtual  const Tally_ArrayRep<T>*    GetRep () const;
                    nonvirtual  Tally_ArrayRep<T>*          GetRep ();
                };


            }
        }
    }
}





#endif  /*_Stroika_Foundation_Containers_Concrete_Tally_Array_h_*/




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Tally_Array.inl"
