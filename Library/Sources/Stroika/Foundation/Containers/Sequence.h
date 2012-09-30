/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sequence_h_
#define _Stroika_Foundation_Containers_Sequence_h_  1

/*
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Sequence.hh
 *
 *      (o)         Should inherit from Iterable<T>
 *
 *		@todo		Must support SequenceIterator - and that SequenceIterator must work with qsort().
 *					In otehrwords, must act as random-access iterator so it can be used in algorithjms that use STL 
 *					random-access iterators.
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Sequence_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Sequence.inl"
