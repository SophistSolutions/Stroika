/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_h_
#define _Stroika_Foundation_Containers_Bag_h_   1


/*
 *
 * Description:
 *
 *      A Bag is the simplest kind of collection. It allows addition and
 *  removal of elements, but makes no guarantees about element ordering. Two
 *  bags are considered equal if they contain the same items, even if iteration
 *  order is different.
 *
 *      Bags are typically designed to optimize item addition and iteration.
 *  They are fairly slow at item access (as they have no keys). Removing items
 *  is usually slow, except in the context of a Bag<T>::Mutator, where it is usually
 *  very fast. Bag comparison (operator==) is often very slow in the worst
 *  case (n^2) and this worst case is the relatively common case of identical
 *  bags.
 *
 *      Although Bag has an TallyOf () method, it is nonvirtual, and therefore
 *  not optimized for the various backends. There is a separate class, Tally,
 *  for cases where you are primarily interested in keeping an summary count
 *  of the occurences of each item.
 *
 *      Bags allow calls to Remove with an item not contained within the bag.
 *
 *      As syntactic sugar, using either functional (Add, Remove) or
 *  operator (+,-) is allowed.
 *
 *
 * TODO:
 *      +   Do CTOR () that takes ITERATOR<T> - but not til after next release....
 *
 *      +   Have Bag_Difference/Union/Interesection??? methods/?? Do research....
 *
 * Notes:
 *
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterable.h"
#include    "Iterator.h"



/*
 * TODO:
 *
 *      (o)                     // See about making these PROTECTED
                class   BagRep;
                class   BagMutatorRep;
 */




namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {



            /// NEEDED TEMPORARILY FOR BELOW FRIEND DECLARATION TIL I CAN FIND A BETTER WAY? - GCC ONLY ISSUE
            /// -- LGP 2012-07-22
            template    <typename T>
            class   Bag;
            template    <typename T>
            bool    operator== (const Bag<T>& lhs, const Bag<T>& rhs);


            template    <typename T>
            class   Bag {
            protected:
// See about making these PROTECTED
                class   _IRep;
            public:
                class   BagMutatorRep;

            public:
                Bag ();
                Bag (const Bag<T>& bag);
                explicit Bag (const T* start, const T* end);

            protected:
                explicit Bag (_IRep* rep);

            public:
                nonvirtual  Bag<T>& operator= (const Bag<T>& bag);

            public:
                nonvirtual  size_t  GetLength () const;

            public:
                nonvirtual  bool    IsEmpty () const;

            public:
                nonvirtual  bool    Contains (T item) const;

            public:
                nonvirtual  void    RemoveAll ();

            public:
                nonvirtual  void    Compact ();

            public:
                nonvirtual  Iterator<T> MakeIterator () const;

            public:
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (const Bag<T>& items);
                nonvirtual  void    Add (const T* begin, const T* end);


            public:
                /*
                 * It is legal to remove something that is not there.
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Bag<T>& items);

                // Have Bag_Difference/Union/Interesection??? methods/??


            public:
                /*
                 *      +=/-= are equivilent Add() and Remove(). They
                 *  are just syntactic sugar.
                 */
                nonvirtual  Bag<T>& operator+= (T item);
                nonvirtual  Bag<T>& operator+= (const Bag<T>& items);
                nonvirtual  Bag<T>& operator-= (T item);
                nonvirtual  Bag<T>& operator-= (const Bag<T>& items);


            public:
                class   Mutator : public Iterator<T> {
                public:
                    explicit Mutator (BagMutatorRep* it);

                public:
                    nonvirtual  void    RemoveCurrent ();
                    nonvirtual  void    UpdateCurrent (T newValue);

                private:
                    nonvirtual  BagMutatorRep*   GetMutatorRep_ ();
                };

            public:
                /*
                 *  Create a mutators : an Iterator<T> which also allows for updates/changes.
                 */
                nonvirtual  Mutator MakeMutator ();

            public:
                // Support for ranged for, and stl syntax in general
                nonvirtual  Iterator<T> begin () const;
                nonvirtual  Iterator<T> end () const;

                nonvirtual  Mutator begin ();
                nonvirtual  Mutator end ();

            public:
                nonvirtual  size_t  TallyOf (T item) const;

            protected:
                // Are these allowed to return 0??? If not, add asserts!!!
                nonvirtual  const _IRep*    GetRep () const;
                nonvirtual  _IRep*          GetRep ();

            private:
                struct  Rep_Cloner_ {
                    inline  static  _IRep*  Copy (const _IRep& t) {
                        return Bag::Clone_ (t);
                    }
                };
                Memory::SharedByValue<_IRep, Rep_Cloner_>    fRep_;

            private:
                static  _IRep*  Clone_ (const _IRep& rep);

            private:
                // SSW 9/16/2011: note weird syntax needed for friend declaration (and had to be forwarded above)
                friend  bool    operator==<T> (const Bag<T>& lhs, const Bag<T>& rhs);   // friend to check if reps equal...
            };

            template    <typename T>
            bool    operator== (const Bag<T>& lhs, const Bag<T>& rhs);
            template    <typename T>
            bool    operator!= (const Bag<T>& lhs, const Bag<T>& rhs);

            template    <typename T>
            Bag<T>  operator+ (const Bag<T>& lhs, const Bag<T>& rhs);
            template    <typename T>
            Bag<T>  operator- (const Bag<T>& lhs, const Bag<T>& rhs);


            template    <typename T>
            class   Bag<T>::BagMutatorRep : public Iterator<T>::Rep {
            protected:
                BagMutatorRep ();

            public:
                virtual void    RemoveCurrent ()            =   0;
                virtual void    UpdateCurrent (T newValue)  =   0;
            };

            template    <typename T>
            class   Bag<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual _IRep*     Clone () const                  =   0;
                virtual bool        Contains (T item) const         =   0;
                virtual size_t      GetLength () const              =   0;
                virtual void        Compact ()                      =   0;
                virtual void        RemoveAll ()                    =   0;

                virtual void        Add (T item)                    =   0;
                virtual void        Remove (T item)                 =   0;

                virtual typename Iterator<T>::Rep*      MakeIterator ()         =   0;
                virtual BagMutatorRep*                  MakeBagMutator ()       =   0;
            };

        }
    }
}


#endif  /*_Stroika_Foundation_Containers_Bag_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Bag.inl"

