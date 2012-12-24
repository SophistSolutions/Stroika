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
 *
 *      (o)         Tally<T> probably Iterable<TallyEntry<T>>.
 *      o           Correctly implement override of Iterator<T>::IRep::StrongEquals ()
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            template    <typename T>    class   TallyEntry {
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
            class  Tally;
            template    <typename T>
            class  TallyRep;
            template    <typename T>
            class  TallyIteratorRep;
            template    <typename T>
            class  TallyMutatorRep;

            template    <typename T>
            bool   operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs);

            template    <typename T>
            bool    operator== (const Tally<T>& lhs, const Tally<T>& rhs);
            template    <typename T>
            bool    operator!= (const Tally<T>& lhs, const Tally<T>& rhs);


            template    <typename T>
            class  TallyMutator : public Iterator<TallyEntry<T>> {
            public:
                TallyMutator (TallyMutatorRep<T>* it);

                nonvirtual  void    RemoveCurrent ();

                // if newCount == 0, equivilent to RemoveCurrent().
                nonvirtual  void    UpdateCount (size_t newCount);
            };


            /*
            I THINK OBSOLETE - GET RID OF THIS..

                Support for ranged for syntax: for (it : v) { it.Current (); }
                This typedef lets you easily construct iterators other than the basic
                iterator for the container.
                Sample usage:
                typedef RangedForIterator<Tally<T>, TallyMutator<T> >       Mutator;
            */
            template    <typename Container, typename IteratorClass>
            class   RangedForIterator {
            public:
                RangedForIterator (Container& t) :
                    fIt (t) {
                }

                RangedForIterator (const Container& t) :
                    fIt (t) {
                }
                nonvirtual  IteratorClass    begin () const {
                    return fIt;
                }

                IteratorClass end () const {
                    return (IteratorClass::GetEmptyIterator ());
                }

            private:
                IteratorClass   fIt;
            };



            template    <typename T>
            class  Tally {
            public:
                Tally ();
                Tally (const Tally<T>& src);
                Tally (const T* items, size_t size);

            protected:
                Tally (TallyRep<T>* rep);

            public:
                nonvirtual  Tally<T>& operator= (const Tally<T>& src);


            public:
                nonvirtual  size_t  GetLength () const;
                nonvirtual  bool    IsEmpty () const;
                nonvirtual  bool    Contains (T item) const;
                nonvirtual  void    RemoveAll ();
                nonvirtual  void    Compact ();

                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (T item, size_t count);
                nonvirtual  void    Add (const T* begin, const T* end);
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (T item, size_t count);
                nonvirtual  void    RemoveAll (T item);
                nonvirtual  size_t  TallyOf (T item) const;
                nonvirtual  size_t  TotalTally () const;

                nonvirtual  Tally<T>&   operator+= (T item);
                nonvirtual  Tally<T>&   operator+= (const Tally<T>& t);

            public:
                nonvirtual  operator Iterator<T> () const;
                nonvirtual  operator Iterator<TallyEntry<T> > () const;
                nonvirtual  operator TallyMutator<T> ();

                // Support for ranged for, and stl syntax in general
                nonvirtual  Iterator<T> begin () const;
                nonvirtual  Iterator<T> end () const;

                nonvirtual  TallyMutator<T> begin ();
                nonvirtual  TallyMutator<T> end ();

                // by default, you only iterator over T, not TallyEntry<T>
                // that is unfortunate in most uses of Tally, so you can modify your for call
                // for (it, t) -> for (it, Tally::It (t))
                // to get the more useful iterator
                typedef RangedForIterator<Tally<T>, Iterator<TallyEntry<T>> >   It;

            protected:

                nonvirtual  const TallyRep<T>*  GetRep () const;
                nonvirtual  TallyRep<T>*        GetRep ();

            private:
                struct  Rep_Cloner_ {
                    inline  static  TallyRep<T>*    Copy (const TallyRep<T>& t) {
                        return Tally::Clone (t);
                    }
                };
                Memory::SharedByValue<TallyRep<T>, Rep_Cloner_>  fRep;

            private:
                static  TallyRep<T>*    Clone (const TallyRep<T>& rep);

            private:
                friend  bool    operator==<T> (const Tally<T>& lhs, const Tally<T>& rhs);
            };

            template    <typename T> bool   operator== (const Tally<T>& lhs, const Tally<T>& rhs);
            template    <typename T> bool   operator!= (const Tally<T>& lhs, const Tally<T>& rhs);



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





