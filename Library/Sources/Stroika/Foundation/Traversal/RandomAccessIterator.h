/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_RandomAccessIterator_h_
#define _Stroika_Foundation_Traversal_RandomAccessIterator_h_  1

#include    "../StroikaPreComp.h"

#include    <iterator>

#include    "../Configuration/Common.h"

#include    "Iterator.h"



/**
 *
 *  \file
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             */
            template    <typename T>
            class   RandomAccessIterator : public /*std::iterator<input_iterator_tag, T>,*/ Iterator<T> {
            private:
                using   inherited   =   typename    Iterator<T>;

            public:
                /**
                 *      \brief  ElementType is just a handly copy of the *T* template type which this
                 *              Iterator<T> parameterizes access to.
                 */
                using   ElementType =   T;

            public:
                class   IRep;
                using   SharedIRepPtr   =   shared_ptr<IRep>;

            private:
                struct  Rep_Cloner_ {
                    static  SharedIRepPtr  Copy (const IRep& t);
                };

            public:
                /**
                 *  \brief  Lazy-copying smart pointer mostly used by implementors (can generally be ignored
                 *          by users).
                 */
                using   SharedByValueRepType    =   Memory::SharedByValue<Memory::SharedByValue_Traits<IRep, SharedIRepPtr, Rep_Cloner_>>;

            private:
                /*
                 *  Mostly internal type to select a constructor for the special END iterator.
                 */
                enum    ConstructionFlagForceAtEnd_ {
                    ForceAtEnd
                };

            public:
                /**
                 *  \brief
                 *      This overload is usually not called directly. Instead, iterators are
                 *      usually created from a container (eg. Bag<T>::begin()).
                 *
                 *  Iterators are safely copyable, preserving their current position.
                 *
                 *  \req RequireNotNull (rep.get ())
                 */
                explicit RandomAccessIterator (const SharedIRepPtr& rep);
                RandomAccessIterator (const RandomAccessIterator<T>& from);
                RandomAccessIterator () = delete;

            private:
                Iterator (ConstructionFlagForceAtEnd_);

            public:
                /**
                 *  \brief  Iterators are safely copyable, preserving their current position.
                 */
                nonvirtual  RandomAccessIterator<T>&    operator= (const RandomAccessIterator<T>& rhs);


            public:
                /**
                 *  \brief
                 *      Used by *somecontainer*::end ()
                 *
                 *  GetEmptyIterator () returns a special iterator which is always empty - always 'at the end'.
                 *  This is handy in implementing STL-style 'if (a != b)' style iterator comparisons.
                 */
                static  Iterator<T>     GetEmptyIterator ();

            public:
                /**
                 *  \brief
                 *      Get a reference to the IRep owned by the iterator. This is an implementation detail,
                 *      mainly intended for implementors.
                 *
                 *  Get a reference to the IRep owned by the iterator.
                 *  This is an implementation detail, mainly intended for implementors.
                 */
                nonvirtual  IRep&               GetRep ();
                nonvirtual  const IRep&         GetRep () const;

            private:
                SharedByValueRepType    fIterator_;

            private:
                Memory::Optional<T>     fCurrent_;

            private:
                static  SharedIRepPtr    Clone_ (const IRep& rep);
            };


            /**
             *
             *  \brief  Implementation detail for iterator implementors.
             *
             *  IRep is a support class used to implement the @ref Iterator<T> pattern.
             *
             *  Subclassed by front-end container writers.
             *  Most of the work is done in More, which does a lot of work because it is the
             *  only virtual function called during iteration, and will need to lock its
             *  container when doing "safe" iteration. More does the following:
             *  iterate to the next container value if advance is true
             *  (then) copy the current value into current, if current is not null
             *  return true if iteration can continue (not done iterating)
             *
             *  typical uses:
             *
             *          it++ -> More (&ignoredvalue, true)
             *          *it -> More (&v, false); return *v;
             *          Done -> More (&v, false); return v.IsPresent();
             *
             *          (note that for performance and thread safety reasons the iterator envelope
             *           actually passes fCurrent_ into More when implenenting ++it
             */
            template    <typename T>
            class   RandomAccessIterator<T>::IRep {
            protected:
                IRep ();

            public:
                virtual ~IRep ();

            public:
                using  SharedIRepPtr    =   typename RandomAccessIterator<T>::SharedIRepPtr;

            public:
                /**
                 * Clone() makes a copy of the state of this iterator, which can separately be tracked with Equals ()
                 * and/or More() to get values and move forward through the iteration.
                 */
                virtual SharedIRepPtr   Clone () const                      = 0;
                /**
                 *  @see Iterator<T>::GetOwner
                 */
                virtual IteratorOwnerID GetOwner () const                   = 0;
                /**
                 *  More () takes two required arguments - one an optional result, and the other a flag about whether or
                 *  not to advance.
                 *
                 *  If advance is true, it moves the iterator to the next legal position.
                 *
                 *  It IS legal to call More () with advance true even when already at the end of iteration.
                 *  This design choice was made to be multi-threading friendly.
                 *
                 *  This function returns the current value in result if the iterator is positioned at a valid position,
                 *  and sets result to an empty value if at the end - its 'at end'.
                 *
                 *  \em Design Note
                 *      We chose to use a pointer parameter instead of a return value to avoid extra
                 *      initializaiton/copying. Note that the return value optimization rule doesn't apply
                 *      to assignment, but only initialization.
                 *
                 */
                virtual void    More (Memory::Optional<T>* result, bool advance)     = 0;
                /**
                 * \brief two iterators must be iterating over the same source, and be up to the same position.
                 *
                 *  \req rhs != nullptr
                 *
                 *  \req this and rhs must be of the same dynamic type, and come from the same iterable object
                 *
                 *  @see Iterator<T>::Equals for details
                 */
                virtual bool    Equals (const IRep* rhs) const            = 0;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

//#include    "RandomAccessIterator.inl"

#endif  /*_Stroika_Foundation_Traversal_RandomAccessIterator_h_ */
