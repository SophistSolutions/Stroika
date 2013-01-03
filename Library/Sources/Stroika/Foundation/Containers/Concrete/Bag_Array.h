/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Bag_Array_h_

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
                 * \brief   Bag_Array<T> is an Array-based concrete implementation of the Bag<T> container pattern.
                 */
                template    <class T>
                class   Bag_Array : public Bag<T> {
                private:
                    typedef     Bag<T>  inherited;

                public:
                    Bag_Array ();
                    Bag_Array (const Bag<T>& bag);
                    Bag_Array (const Bag_Array<T>& bag);
                    Bag_Array (const T* start, const T* end);

                    nonvirtual  Bag_Array<T>& operator= (const Bag_Array<T>& bag);

                public:
                    /*
                     * This optional API allows pre-reserving space as an optimizaiton.
                     */
                    nonvirtual  size_t  GetCapacity () const;
                    nonvirtual  void    SetCapacity (size_t slotsAlloced);

                private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
                public:
#endif
                    class   Rep_;
                    class   IteratorRep_;

                private:
                    nonvirtual  const Rep_&  GetRep_ () const;
                    nonvirtual  Rep_&        GetRep_ ();
                };


            }
        }
    }
}




#endif  /*_Stroika_Foundation_Containers_Concrete_Bag_Array_h_ */


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Bag_Array.inl"



