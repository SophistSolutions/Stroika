/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_h_
#define _Stroika_Foundation_Containers_Tally_h_ 1


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterator.h"
#include    "Iterable.h"

/*
 *
 *  TODO:
 *      @todo   consider if this should inherit from Iterable<TallyEntry<T>>. Be sure and document
 *              why we chose one way or the other
 *				(did switch but still must document why)
 *
 *      @todo   Lose mutator code - instead do like I did in Bag<T> code - with update methods taking iteraotr
 *              arugment.
 *
 *      @todo   Consider if MakeBagIterator/bagbegin/bagend should  be replaced with
 *              As<Bag<T>>(), and then As<Bag<T>>().begin ()? Or some such?
 *
 *      @todo   Need Tally_Tree<T> implementaiton - where we use btree to keep tally's sorted,
 *              so faster lookup. PROBLEM with that impl is it requires an ordering on T, which the others dont
 *
 *      @todo   redo APIs so all (or most) using shared_ptr not IRep*  - eg for MakeIterator and CTOR.
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            template    <typename T>
            class   TallyEntry {
            public:
#if qIteratorsRequireNoArgContructorForT
                TallyEntry () {}
#endif
                TallyEntry (T item);
                TallyEntry (T item, size_t count);

                nonvirtual  bool    operator!= (const TallyEntry<T>& rhs) const;
                nonvirtual  bool    operator== (const TallyEntry<T>& rhs) const;

                T       fItem;
                size_t  fCount;
            };
            template    <typename T>
            bool   operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs);


			/**
			 *	A Tally<T> a collection of <T> elements, but where each time you add something, the tally tracks the number
			 *	of times that thing has been entered. This is not a commonly used class, but handy when you want to count
			 *	things.
			 *
			 *	A Tally<T> makes no promises about ordering of elements in iteration.
			 */
            template    <typename T>
            class  Tally : public Iterable<TallyEntry<T>> {
            private:
                typedef Iterable<TallyEntry<T>> inherited;

            protected:
                class _IRep;

            public:
                Tally ();
                Tally (const Tally<T>& src);
                Tally (const T* start, const T* end);
                Tally (const TallyEntry<T>* start, const TallyEntry<T>* end);

            protected:
                explicit Tally (_IRep* rep);

            public:
                nonvirtual  bool    Contains (T item) const;
                nonvirtual  void    RemoveAll ();
                nonvirtual  void    Compact ();

            public:
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (T item, size_t count);
                nonvirtual  void    Add (const T* begin, const T* end);

            public:
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (T item, size_t count);

            public:
                nonvirtual  void    RemoveAll (T item);

            public:
                nonvirtual  size_t  TallyOf (T item) const;

            public:
                nonvirtual  size_t  TotalTally () const;

            public:
                nonvirtual  Tally<T>&   operator+= (T item);
                nonvirtual  Tally<T>&   operator+= (const Tally<T>& t);

            public:
                /**
                 * Return an iterator over individual items in the tally - as if the Tally was a Bag,
                 * not a Tally.
                 */
                nonvirtual  Iterator<T> MakeBagIterator () const;
                nonvirtual  Iterator<T> bagbegin () const;
                nonvirtual  Iterator<T> bagend () const;


                // SOON TO BE OBSOLETE MUTATOR CODE - DO LIKE WITH BAG<T>
            public:
                class TallyMutator;
            public:
                nonvirtual  TallyMutator begin ();
                nonvirtual  TallyMutator end ();

                // NOTE - once we lose TallyMutator code - we can lose this method too, since
                // its inherited from Iterable.
            public:
                nonvirtual  Iterator<TallyEntry<T>> begin () const;
                nonvirtual  Iterator<TallyEntry<T>> end () const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();

            protected:
                class _TallyEntryToItemIterator;

            private:
                friend  bool    operator==<T> (const Tally<T>& lhs, const Tally<T>& rhs);
            };

            template    <typename T>
            bool   operator== (const Tally<T>& lhs, const Tally<T>& rhs);
            template    <typename T>
            bool   operator!= (const Tally<T>& lhs, const Tally<T>& rhs);


            /**
             */
            template    <typename T>
            class   Tally<T>::_IRep : public Iterable<TallyEntry<T>>::_IRep {
            private:
                typedef typename Iterable<TallyEntry<T>>::_IRep  inherited;

            protected:
                _IRep ();

            public:
                virtual bool    Contains (T item) const                         =   0;
                virtual size_t  GetLength () const                              =   0;
                virtual void    Compact ()                                      =   0;
                virtual void    RemoveAll ()                                    =   0;
                virtual void    Add (T item, size_t count)                      =   0;
                virtual void    Remove (T item, size_t count)                   =   0;
                virtual size_t  TallyOf (T item) const                          =   0;

            public:
                virtual shared_ptr<typename Iterator<T>::IRep>  MakeBagIterator () const        =   0;

                virtual typename Tally<T>::TallyMutator                            MakeTallyMutator ()             =   0;
            };


            /**
             * SOON TO BE OBSOLETE
             */
            template    <typename T>
            class  Tally<T>::TallyMutator : public Iterator<TallyEntry<T>> {
            public:
                class IRep;
            public:
                TallyMutator (IRep* it);

            public:
                nonvirtual  void    RemoveCurrent ();

            public:
                // if newCount == 0, equivilent to RemoveCurrent().
                nonvirtual  void    UpdateCount (size_t newCount);
            };


        }
    }
}


#endif  /*_Stroika_Foundation_Containers_Tally_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Tally.inl"





