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
 *
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



            template    <typename T>
            class  Tally : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class _IRep;

            public:
                Tally ();
                Tally (const Tally<T>& src);
                Tally (const T* items, size_t size);

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
                //TERRIBLE NAMES
                nonvirtual  Iterator<TallyEntry<T>> ebegin () const;
                nonvirtual  Iterator<TallyEntry<T>> eend () const;

            public:
                nonvirtual  Iterator<T> begin () const;
                nonvirtual  Iterator<T> end () const;
                nonvirtual  TallyMutator<T> begin ();
                nonvirtual  TallyMutator<T> end ();

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();

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
            class   Tally<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

                // from Iterable<T>::_IRep
            public:
                virtual  Iterator<T>    MakeIterator () const override;

            public:
                virtual bool    Contains (T item) const                         =   0;
                virtual size_t  GetLength () const                              =   0;
                virtual void    Compact ()                                      =   0;
                virtual void    RemoveAll ()                                    =   0;
                virtual void    Add (T item, size_t count)                      =   0;
                virtual void    Remove (T item, size_t count)                   =   0;
                virtual size_t  TallyOf (T item) const                          =   0;

            public:
                virtual typename Iterator<TallyEntry<T> >::IRep*    MakeTallyIterator () const    =   0;
                virtual TallyMutator<T>                         MakeTallyMutator ()     =   0;
            };

            /**
             */
            template    <typename T>
            class  TallyMutator : public Iterator<TallyEntry<T>> {
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





