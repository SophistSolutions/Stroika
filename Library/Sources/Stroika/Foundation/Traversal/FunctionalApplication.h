/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_FunctionalApplication_h_
#define _Stroika_Foundation_Traversal_FunctionalApplication_h_ 1

#include "../StroikaPreComp.h"

#include <limits>
#include <optional>

#include "../Configuration/Common.h"

#include "Iterator.h"

/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  NOTES:
 *
 *  TODO:
 *
 *      @todo   add demo project so shows how to use (cleaned up versions of the regtest code)
 *              Maybe something using bignums (dont exist yet) and the multithreaded threadpool
 *              mapper (doesn't exist yet) - showing how to run multiple IsPrime's at a time in the
 *              context of this mapping stuff.
 *
 *      @todo   Consider that we COULD implement something like http://underscorejs.org/#pluck
 *              if we used VariantValue - instead of "T" types. That would have some merrits.
 *
 *              Also - using that style (along with ObjectVariantMapper) could allow us to have
 *              the mapping-engine's not be template based - which might make them easier to
 *              implement (private/.cpp file).
 *
 *      @todo    do impl of pull-based iterator stuff
 *
 *              (or at least document deisgn basics)
 *              .. mujst have filter etc funtions not do anything but acuulate lambdas.
 *              then construct inverted compuation as iterator.
 *
 *              MAYBE can do by having each peice that reutrns an iterator do the work
 *              of priducing apull based iteraotr?
 *                  THAT SEEM BEST!
 *
 *      @todo   DOCUMENT/think through the design choice if Map, and Filter etc - MUST return
 *              items IN-ORDER. By allowing them to return stuff out of order, they CAN
 *              by implemented much more efficeintly. Probaly have two functions - Map and Map_UO
 *              where they do the same thing but Map_UO is not necessarily ordreed.
 *
 *      @todo   NOTE that DirectPushMapEngine is bad, and we want a pull-based one ASAP.
 *
 *      @todo   Mapper's must be written in such a way that they are smart-pointers nad
 *              pass data from first call to last.
 *
 *      @todo   Need to simplify usage further. Example usage is too wordy. See if we can better use templates
 *              and defaulting to make more brief basic filter usage.
 *
 *      @todo   See http://underscorejs.org/#filter - and add filter, and probably others - but not sure what
 *              to call these (functional helpers?)
 *              Same for http://underscorejs.org/#pluck
 *
 *      @todo   CONCEPTS!
 *                  o   use std::function<>... with particular arguments to advertise the
 *                      required type signature,
 *                  o   OR use typename FUNCTION - better performance performance - TEST.
 *
 *              We should verify if there is truely a performance difference, and if there is, then find
 *
 *  \em Design Overview
 *      This code is based in large part on
 *          o   conversations with kdj 2013-10-05
 *          o   @see http://en.wikipedia.org/wiki/MapReduce
 *          o   @see http://www.ruby-doc.org/core-2.0/Array.html#method-i-map
 *          o   @see http://underscorejs.org/#reduce
 *          o   Python/SQL Alchemy (filter chains).
 *          o   The code is in part based on ideas for pre-release java 8 streams.
 *          o   http://java.dzone.com/articles/exploring-java8-lambdas-part-1
 *
 *      The gist is to have a MAPPING_ENGINE - which can either do direct applicaiton and buffering, or
 *      pull based compuation (where calls are done in final iterator as you pull results)
 *      or done with the first case - applicaiton context/buffering - but via thread pools, or some other strategy.
 *
 *      All this is to do the computation for traditional basic functional programming algorithms, like map/reduce, and filters
 *      etc.
 *
 *      To use, you simple instantiate a FunctionalApplicationContext object, and then chain together a series of filters.
 *      typically, the end result - last filter step - will be to produce an iterable (which can be trivially converted to another
 *      container type).
 *
 */

namespace Stroika::Foundation {
    namespace Traversal {

        /**
         *  This class should not be used directly, but as a parameter to FunctionalApplicationContext.
         *
         *  The DirectPushMapEngine implements a very simple, basic functional computation strategy, where results are
         *  immediately computed, and within a single thread. The results are buffered, and the various map
         *  functions return iterables which pull out pre-computed and cached results.
         *
         *  @see FunctionalApplicationContext
         */
        struct DirectPushMapEngine {
            template <typename IN_T, typename OUT_T>
            Iterable<OUT_T> Map (const Iterable<IN_T>& from, const function<OUT_T (IN_T)>& do2Each);

            template <typename IN_T, typename OUT_T>
            OUT_T Reduce (const Iterable<IN_T>& from, const function<OUT_T (IN_T, OUT_T)>& do2Each, OUT_T memo);

            template <typename IN_OUT_T>
            Iterable<IN_OUT_T> Filter (const Iterable<IN_OUT_T>& from, const function<bool(IN_OUT_T)>& includeTest);

            template <typename IN_OUT_T>
            optional<IN_OUT_T> Find (const Iterable<IN_OUT_T>& from, const function<bool(IN_OUT_T)>& thatPassesThisTest);
        };

        /**
         *  \em Design Overview
         *      The gist is to have a MAPPING_ENGINE - which can either do direct applicaiton and buffering, or
         *      pull based compuation (where calls are done in final iterator as you pull results)
         *      or done with the first case - applicaiton context/buffering - but via thread pools, or some other strategy.
         *
         *      All this is to do the computation for traditional basic functional programming algorithms, like map/reduce, and filters
         *      etc.
         *
         *      To use, you simple instantiate a FunctionalApplicationContext object, and then chain together a series of filters.
         *      typically, the end result - last filter step - will be to produce an iterable (which can be trivially converted to another
         *      container type).
         *
         *  \par Example Usage
         *      \code
         *      COMMON/SETUP:
         *          Sequence<int> s = { 1, 2, 3 };
         *      \endcode
         *
         *  \par DANGEROUS/BAD EXAMPLE
         *      \code
         *      {
         *          int countSoFar = 0;
         *          int answer =
         *              FunctionalApplicationContext<int>(s).
         *              Filter ([&countSoFar] (int) -> bool { ++countSoFar; return countSoFar & 1; }).
         *              Map<int> ([] (int s) { return s + 5; }).
         *              Reduce<size_t> ([] (int s, size_t memo) { return memo + 1; })
         *          ;
         *          VerifyTestResult (answer == 2);
         *      }
         *      \endcode
         *
         *      This will work, but ONLY because all resulting objects from the compuation will be destroyed before
         *      the countSoFar goes out of scope. Since FunctionalApplicationContext<> is often used to produce an
         *      iterable that is 'returned' - its best to use a smart-pointer to store any catpured values.
         *
         *  \par Example Usage
         *      \code
         *      {
         *          shared_ptr<int> countSoFar = shared_ptr<int> (new int (0));
         *          int answer =
         *              FunctionalApplicationContext<int>(s).
         *              Filter ([countSoFar] (int) -> bool { ++(*countSoFar); return (*countSoFar) & 1; }).
         *              Map<int> ([] (int s) { return s + 5; }).
         *              Reduce<size_t> ([] (int s, size_t memo) { return memo + 1; })
         *          ;
         *          VerifyTestResult (answer == 2);
         *      }
         *      \endcode
         *  \par Example Usage
         *      \code
         *      {
         *          shared_ptr<int> countSoFar = shared_ptr<int> (new int (0));
         *          Sequence<int> r = Sequence<int> (
         *              FunctionalApplicationContext<int>(s).
         *              Filter ([countSoFar] (int) -> bool { ++(*countSoFar); return (*countSoFar) & 1; }).
         *              Map<int> ([] (int s) { return s + 5; })
         *          );
         *          VerifyTestResult (r.length () == 2);
         *          VerifyTestResult (r[0] == 6 and r[1] == 8);
         *      }
         *      \endcode
         *
         *  So you create a FunctionalApplicationContext<> - with the template parameter refering to the type of
         *  the input container.
         *
         *  Then you chain together as many steps as you want (methods of FunctionalApplicationContext)
         *  and the final result is the end of the computation.
         *
         *  You can optionally create the initial FunctionalApplicationContext with a selected 'mapping engine' that
         *  will use thread pools, or whatever mechanism you choose to orchestrate the computation.
         *
         *  Note that we use std::function<> intead of typename FUNCTION - though I think this will perform
         *  the same or slower - just because I think using the explicit types will produce more
         *  comprehensible error messages from the compiler. We can always relax the definition to
         *  use typename FUNCTION in the future - hopefully - without breaking existing using code.
         *
         *  \em What Methods and why?
         *      Much of this list of methods is based on @see http://underscorejs.org/#reduce
         *      But much doesn't make sense in stroika. So here is a rough partial summary of what
         *      is NOT included from there and why
         *
         *          >   each() - not reason to include because Iterable has 'Apply' - use that instead.
         *          >   reject() - not included cuz its the same as filter() - except with NOT on lambda
         */
        template <typename T, typename MAPPER_ENGINE = DirectPushMapEngine>
        class FunctionalApplicationContext : public Iterable<T> {
        private:
            using inherited = Iterable<T>;

        private:
            MAPPER_ENGINE fMappingEngine_;

        public:
            /**
             */
            FunctionalApplicationContext (Iterable<T> i, MAPPER_ENGINE m = MAPPER_ENGINE ());

        public:
            /**
             *          o   @see http://en.wikipedia.org/wiki/MapReduce
             *          o   @see http://www.ruby-doc.org/core-2.0/Array.html#method-i-map
             */
            template <typename OUT_T>
            FunctionalApplicationContext<OUT_T, MAPPER_ENGINE> Map (const function<OUT_T (T)>& do2Each);

        public:
            /**
             *          o   @see http://en.wikipedia.org/wiki/MapReduce
             *          o   @see http://underscorejs.org/#reduce
             */
            template <typename OUT_T>
            OUT_T Reduce (const function<OUT_T (T, OUT_T)>& do2Each, OUT_T memo = OUT_T ());

        public:
            /**
             */
            template <typename INOUT_T>
            FunctionalApplicationContext<INOUT_T, MAPPER_ENGINE> Filter (const function<bool(INOUT_T)>& includeTest);

        public:
            /**
             */
            template <typename INOUT_T>
            optional<INOUT_T> Find (const function<bool(INOUT_T)>& thatPassesThisTest);
        };
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "FunctionalApplication.inl"

#endif /*_Stroika_Foundation_Traversal_FunctionalApplication_h_ */
