/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_LockFreeDataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_LockFreeDataStructures_LinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Configuration/TypeHints.h"
#include    "../../Common/Compare.h"
#include    "../../Debug/AssertExternallySynchronizedLock.h"
#include    "../../Memory/BlockAllocated.h"
#include    "../../Memory/Optional.h"

#include    "../Common.h"


/**
 *
 *
 * TODO:
 *
 *      @todo   TOTALLY UNIMPLEMENTED
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   DataStructures {


                using   Configuration::ArgByValueType;


                /**
                 * VERY PRELIMINARY DRAFT OF HOW TO HANDLE THIS - UNSURE ABOUT ISSUE OF FORWARDABILITY AND COPYABILIUTY OF COMPARERES!!!!
                 */
                template    <typename T, typename EQUALS_COMPARER = Common::DefaultEqualsComparerOptionally <T>>
                struct   LinkedList_DefaultTraits {
                    using   EqualsCompareFunctionType   =   EQUALS_COMPARER;
                };

                /*
                 *      https://en.wikipedia.org/wiki/Non-blocking_linked_list
				 *
				 *		or better – Herb Sutter provides a C++ implementation:
				 *		https://www.infoq.com/news/2014/10/cpp-lock-free-programming
                 */
                template      <typename  T, typename TRAITS = LinkedList_DefaultTraits<T>>
                class   LinkedList : public Debug::AssertExternallySynchronizedLock {
                public:
                    using   value_type  =   T;

                public:
                    LinkedList ();
                    LinkedList (const LinkedList<T, TRAITS>& from);
                    ~LinkedList ();


				public:

           
                };


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_LockFreeDataStructures_LinkedList_h_ */
