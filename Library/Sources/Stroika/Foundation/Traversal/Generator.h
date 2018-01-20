/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Generator_h_
#define _Stroika_Foundation_Traversal_Generator_h_ 1

#include "../StroikaPreComp.h"

#include "Iterable.h"
#include "Iterator.h"

/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *      @todo   See if/how DiscreteRange<> can be made simpler using this generator code.
 *
 *      @todo   I have old docs comment:
 *
 *              "Writeup details on how todo generator – as lambda-from-iteator – like I did for
 *              revision with queie and lambdas – except we have we construct new object with
 *              updstream-get-lambda and pass it to downstream one. I think that works. Try draft…."
 *
 *              I'm now not quite sure what that means. This may have been more for the FunctionApplicaiton module?
 *              But keep for a little bit to see if it makes sense when I review this code later...
 *
 *              -- LGP 2013-10-14
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Traversal {

            /**
             *  Note - if you need to maintain context for the iterator (typically yes) - bind it into the
             *  std::function lambda closure (with smart pointers).
             */
            template <typename T>
            Iterator<T> CreateGeneratorIterator (const function<Memory::Optional<T> ()>& getNext);

            /**
             *  Note - if you need to maintain context for the iterator (typically yes) - bind it into the
             *  std::function lambda closure (with smart pointers).
             *
             *  \par Example Usage
             *      \code
             *      constexpr int kMin = 1;
             *      constexpr int kMax = 10;
             *      auto myContext = make_shared<int> (kMin - 1);
             *      auto getNext = [myContext] () -> Memory::Optional<int> {
             *          (*myContext)++;
             *          if (*myContext > 10)
             *          {
             *              return Memory::Optional<int> ();
             *          }
             *          return *myContext;
             *      };
             *
             *      int sum = 0;
             *      for (auto i : CreateGenerator<int> (getNext)) {
             *          VerifyTestResult (1 <= i and i <= 10);
             *          sum += i;
             *      }
             *      VerifyTestResult (sum == (kMax - kMin + 1) * (kMax + kMin) / 2);
             *      \endcode
             */
            template <typename T>
            Iterable<T> CreateGenerator (const function<Memory::Optional<T> ()>& getNext);
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Generator.inl"

#endif /*_Stroika_Foundation_Traversal_Generator_h_ */
