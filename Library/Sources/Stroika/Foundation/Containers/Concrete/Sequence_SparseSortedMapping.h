/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_SparseSortedMapping_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_SparseSortedMapping_h_

#include    "../../StroikaPreComp.h"

#include    "../Sequence.h"



/**
 *  \file
 *
 *  TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /**
                 * \brief   Sequence_SparseSortedMapping<T> is a sparse-Array-based concrete implementation of the Sequence<T> container pattern.
                 */
                template    <class T>
                class   Sequence_SparseSortedMapping : public Sequence<T> {
                private:
                    typedef     Sequence<T>  inherited;

                public:
                    Sequence_SparseSortedMapping ();
                    Sequence_SparseSortedMapping (const Sequence_SparseSortedMapping<T>& s);
                    template <typename CONTAINER_OF_T>
                    explicit Sequence_SparseSortedMapping (const CONTAINER_OF_T& s);
                    template <typename COPY_FROM_ITERATOR>
                    explicit Sequence_SparseSortedMapping (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

                public:
                    nonvirtual  Sequence_SparseSortedMapping<T>& operator= (const Sequence_SparseSortedMapping<T>& s);


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




/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

//#include    "Sequence_SparseSortedMapping.inl"

#endif  /*_Stroika_Foundation_Containers_Concrete_Sequence_SparseSortedMapping_h_ */
