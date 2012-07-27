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
 *      o   Because of the definition of operator== (Bag,Bag), we have an expensive implemenation.
 *          The underlying IRep should be enhanced to allow if we are talking to two like implementations
 *          we can produce a more efficient comparison.
 *
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






namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


#if     qCompilerAndStdLib_TemplateFriendFunctionsRequirePredeclaredTemplateFunction
            template    <typename T>
            class   Bag;
            template    <typename T>
            bool    operator== (const Bag<T>& lhs, const Bag<T>& rhs);
#endif


            /*
             *  A Bag<T> is a container pattern to manage an un-ordered collection of items. This is both an abstract interface, and
             *  but the Bag<T> class it actually concrete because it automatically binds to a default implementation.
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
             */
            template    <typename T>
            class   Bag {
            protected:
                class   _IRep;
                class   _IMutatorRep;

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

            public:
                /*
                 *      +=/-= are equivilent Add() and Remove(). They are just syntactic sugar.
                 */
                nonvirtual  Bag<T>& operator+= (T item);
                nonvirtual  Bag<T>& operator+= (const Bag<T>& items);
                nonvirtual  Bag<T>& operator-= (T item);
                nonvirtual  Bag<T>& operator-= (const Bag<T>& items);


            public:
                class   Mutator;

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
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();

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


            /*
             *  A Bag<T>::Mutator is an iterator, which also allows updating its owning bag.
             */
            template    <typename T>
            class   Bag<T>::Mutator : public Iterator<T> {
            public:
                explicit Mutator (_IMutatorRep* it);

            public:
                nonvirtual  void    RemoveCurrent ();
                nonvirtual  void    UpdateCurrent (T newValue);

            private:
                nonvirtual  _IMutatorRep*   GetMutatorRep_ ();
            };



            /*
             *  Protected abstract interface to support concrete implementations of
             *  the Bag<T> container API.
             */
            template    <typename T>
            class   Bag<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual _IRep*      Clone () const                  =   0;
                virtual bool        Contains (T item) const         =   0;
                virtual size_t      GetLength () const              =   0;
                virtual void        Compact ()                      =   0;
                virtual void        RemoveAll ()                    =   0;

                virtual void        Add (T item)                    =   0;
                virtual void        Remove (T item)                 =   0;

                virtual Iterator<T>	MakeIterator ()					=   0;
                virtual _IMutatorRep*                  MakeBagMutator ()       =   0;
            };


            /*
             *  Protected abstract interface to support concrete implementations of
             *  the Bag<T> container API.
             */
            template    <typename T>
            class   Bag<T>::_IMutatorRep : public Iterator<T>::Rep {
            protected:
                _IMutatorRep ();

            public:
                virtual void    RemoveCurrent ()            =   0;
                virtual void    UpdateCurrent (T newValue)  =   0;
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

