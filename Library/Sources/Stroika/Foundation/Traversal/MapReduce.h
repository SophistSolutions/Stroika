/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_MapReduce_h_
#define _Stroika_Foundation_Traversal_MapReduce_h_  1

#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"
#include    "../Containers/Sequence.h"  //tmp impl hack

#include    "Iterator.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  NOTES:
 *
 *  TODO:
 *      @todo   MAJOR REDESIGN NEEDED (COLLABORATION WITH KDJ 2013-10-05)
 *
 *              >   Consider something like java8 streams - have STREAM<T> - where you have input and
 *                  output iterator like things.
 *
 *                  In fact - better model MAYBE Iteraotr<T> (really input iteraotr) and OutputIterator<T>
 *
 *                  Key is defered evalution - and not materializiang inpout and oputput until needed.
 *
 *                  Proabbly way to do this is abstract MAPPER engine - and one engine might be trivial thing
 *                  I have impelmtend below. But another might extenralize - mutltihread, thread pool etc
 *                  do execute in and out.
 *
 *                  See example from http://java.dzone.com/articles/exploring-java8-lambdas-part-1
 *                  List<String> result =  Arrays.asList("Larry", "Moe", "Curly")
 *                  .stream()
 *                  .map(s -> "Hello " + s)
 *                  .collect(Collectors.toList());
 *
 *                  result will be a List<String> containing "Hello Larry", "Hello Moe" and "Hello Curly"
 *
 *                  In my model this would be:
 *                  Mapper m = BasicMapper (or threadedpoolmapper);
 *                  Sequence<String>  s = {"Larry", "Moe", "Curly"};
 *                  s = m.Map (s, [] (String s) { return "Hello " + s ; });
 *
 *                  as above - result will be String list of "Hello Larry", "Hello Moe" and "Hello Curly"
 *
 *                  Key is Map() - thogh not obvious above - takes ITERASTOR as arg, and returns another INOPUT ITERATOR as result.
 *
 *                  So we can add Mapper::Filter(InputItarotr, Lambda) - which skips some values, and then chain mappings as in:
 *                  s = m.Map(m.filter (s, [] { static int i = 0; ++i; return i & 1; }),  [] (String s) { return "Hello " + s ; });
 *                  That would return "Hello Larry", and "Hello Curly" (skips one in middle cuz i & 1 returns false);
 *
 *                  KDJ really wants chaining sort of syntax - like in languages like python wiht ORMs.
 *
 *                  e.g. cout >> b >> c >> d;
 *                  or
 *                  personTable.select(name.equals('jim')).order_by(date);
 *
 *                  Perhaps use << with Mapper.
 *                  Mapper m;
 *                  m(s).
 *                  Filter ([] { static int i = 0; ++i; return i & 1; }).
 *                  Map (s, [] (String s) { return "Hello " + s ; }).
 *                  s;
 *                  (could use operator . effecitly )methods) or operator -> or whatever.
 *
 *                  does the same thing. m(s) (function operator applied to Iterator or ocntianer) produces intermeidate object
 *                  which combines mapper and resulting iterator.
 *                  Filter (lambda) returns specail object - packing name of function todo iwth arg (lambda)
 *                  Then the operator>> calls the mapper.thatFunction filter).
 *
 *
 *
 *      @todo   Placeholder - so I know its work todo... but not really even started
 *
 *      @todo   Consider renaming this module to
 *                  >   FunctionalUtilties
 *                  >   FunctionalSupport
 *                  >   FunctionalAlgorithms
 *                  >   Or - Migrate this code directly into Iterable<T>!!!
 *
 *      @todo   See http://underscorejs.org/#filter - and add filter, and probably others - but not sure what
 *              to call these (functional helpers?)
 *              Same for http://underscorejs.org/#pluck
 *
 *      @todo   Consider replacing ElementType with value_type - after I verify taht alwways works in STL - and then
 *              this code will work with Stroika containers and STL containers
 *
 *      @todo   MAYBE use
 *              Default_MapReduce_Traits<CONTAINER_OF_T> {
 *                  typedef typename CONTAINER_OF_T::ElementType    ElementType;
 *
 *                  UPDATE_TARGET_CONTAINER (CONTAINER_OF_T* result, ElementType newElt);// UNCLEAR IF THIS APPENDS, or takes iteraotr???
 *              ...
 *              }
 *
 *      @todo   Consider having Map/Reduce (ETC) methods take Iterable<T> as the parameter, and then a templated
 *              param for resulting container (and method to add to it - maybe stdfunction?) - Maybe something like
 *              an STL output-iterator (but safe - using stroika containers)?
 *
 *      @todo   CONCEPTS!
 *                  o   use std::function<>... with particular arguments to advertise the
 *                      required type signature,
 *                  o   OR use typename FUNCTION - better performance performance - TEST.
 *
 *              We should verify if there is truely a performance difference, and if there is, then find
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  @see http://en.wikipedia.org/wiki/MapReduce
             *  @see http://www.ruby-doc.org/core-2.0/Array.html#method-i-map
             */
            template    < typename CONTAINER_OF_T, typename APPLY_TO_EACH_FUNCTION_TYPE/* = std::function<typename CONTAINER_OF_T::ElementType (typename CONTAINER_OF_T::ElementType)>*/ >
            CONTAINER_OF_T  Map (const CONTAINER_OF_T& containerOfT, const APPLY_TO_EACH_FUNCTION_TYPE& do2Each);

            /**
             *  @see http://en.wikipedia.org/wiki/MapReduce
             *  @see http://underscorejs.org/#reduce
             */
            template    < typename CONTAINER_OF_T, typename APPLY_TO_EACH_FUNCTION_TYPE/* = std::function<typename CONTAINER_OF_T::ElementType (typename CONTAINER_OF_T::ElementType memo, typename CONTAINER_OF_T::ElementType i)>*/ >
            typename CONTAINER_OF_T::ElementType    Reduce (const CONTAINER_OF_T& containerOfT, const APPLY_TO_EACH_FUNCTION_TYPE& do2Each, typename CONTAINER_OF_T::ElementType memo = typename CONTAINER_OF_T::ElementType ());


            namespace SuperPreDraftExperiment_ {

                /**
                 */

                /**
                 *
                 * DRAFT API - this is envelope and simple concrete impl all in one to evaluate the approach (see @todo item
                 *  above from 2013-10-05).
                 *
                 *  NOTE - reason for mapper abstraction - is one mapper will just walk and iterator - and other might use threadpools
                 *  dodo multiple compuations (runing do2eachs) at a time.
                 */
                struct  BasicMapper {
                    template    <typename IN_T, typename OUT_T>
                    Iterable<OUT_T>     Map (const Iterable<IN_T>& from, const function<OUT_T(IN_T)>& do2Each) const {
                        Containers::Sequence<OUT_T>  result;
                        for (IN_T i : from) {
                            // unsure if we update in place, or create a new container? May need traits param to define how todo this!
                            result.Append (do2Each (i));
                        }
                        return result;
                    }
                    template    <typename IN_T, typename OUT_T>
                    OUT_T                   Reduce (const Iterable<IN_T>& from, const function<OUT_T(IN_T, OUT_T)>& do2Each, OUT_T memo = OUT_T ()) const {
                        OUT_T    result  =   memo;
                        for (IN_T i : from) {
                            result = do2Each (i, result);
                        }
                        return result;
                    }
                    template    <typename T>
                    Iterable<T>         Filter (const Iterable<T>& from, const function<bool(T)>& includeTest) const {
                        Containers::Sequence<T>  result;
                        for (T i : from) {
                            if (includeTest (i)) {
                                result.Append (i);
                            }
                        }
                        return result;
                    }
                };


                template    <typename T>
                struct  FuncIterator : public Iterable<T> {
                    typedef Iterable<T> inherited;
                    BasicMapper fM;

                    FuncIterator (BasicMapper m, Iterable<T> i)
                        : inherited (i)
                        , fM (m) {
                    }

                    template    <typename OUT_T>
                    FuncIterator<OUT_T>     Map (const function<OUT_T(T)>& do2Each) const {
                        return FuncIterator<OUT_T>  (fM, fM.Map (inherited (*this), do2Each));
                    }
                    template    <typename OUT_T>
                    T                       Reduce (const function<OUT_T(T, OUT_T)>& do2Each, OUT_T memo = OUT_T ()) const {
                        return  fM.Reduce (inherited (*this), do2Each, memo);
                    }
                    FuncIterator<T>         Filter (const function<bool(T)>& includeTest) const {
                        return FuncIterator<T>  (fM, fM.Filter (inherited (*this), includeTest));
                    }
                };
                template    <typename T>
                FuncIterator<T>    DoIt(BasicMapper m, Iterable<T> i)
                {
                    return FuncIterator<T>  (m, i);
                }

                // Then example call
                //      BasicMapper     m;
                //      Sequence<String> s = { L"a", L"b", L"c" };
                //      DoIt(m, s).
                //      Filter (([] { static int i = 0; ++i; return i & 1; }).
                //      Map<String> ([] (String s) { return "Hello " + s ; }).
                //      Reduce<size_t> ([] (String s, size_t memo) { return memo + 1; });
                //  result prints 2! (count).
                ///
            }


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "MapReduce.inl"

#endif  /*_Stroika_Foundation_Traversal_MapReduce_h_ */
