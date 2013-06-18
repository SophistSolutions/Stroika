/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Memory/BlockAllocated.h"

#include    "../../Common.h"


/**
 *
 * TODO:
 *
 *
 *
 *      @todo   Do RemoveAt(ForwardIterator), and other mutator methods as method of LinkedList<T>
 *              THen mask in Patching:::LinkedList<T> - and do patching there. Then get rid of
 *              mutator objects!
 *
 *      @todo   MAJOR cleanup needed - nearly an entire rewrite. This code is very old and worn...
 *
 *      @todo   Consider adding InsertAt,
 *              / AddAfter() etc without using a LinkedList_Patch, using iteartor params as reference.
 *
 * Notes:
 *
 *      Slightly unusual behaviour for LinkedListMutator_Patch<T>::AddBefore () -
 *  allow it to be called when we are Done() - otherwise there is NO
 *  WAY TO APPEND TO A LINK LIST!!!
 *
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /*
                     *      LinkedList<T> is a generic link (non-intrusive) list implementation.
                     *  It keeps a length count so access to its length is rapid. We provide
                     *  no public means to access the links themselves.
                     *
                     *      Since this class provides no patching support, it is not generally
                     *  used - more likely you want to use LinkedList_Patch<T>. Use this if you
                     *  will manage all patching, or know that none is necessary.
                     */
                    template    <typename   T>
                    class   LinkedList {
                    public:
                        LinkedList ();
                        LinkedList (const LinkedList<T>& from);
                        ~LinkedList ();

                    public:
                        nonvirtual  LinkedList<T>& operator= (const LinkedList<T>& list);

                    public:
                        class   ForwardIterator;

                    public:
                        nonvirtual  bool    IsEmpty () const;

                    public:
                        nonvirtual  size_t  GetLength () const;

                    public:
                        /**
                         *  Performance:
                         *      Worst Case: O(1)
                         *      Average Case: O(1)
                         */
                        nonvirtual  T       GetFirst () const;

                    public:
                        /**
                         *  Performance:
                         *      Worst Case: O(1)
                         *      Average Case: O(1)
                         */
                        nonvirtual  void    Prepend (T item);

                    public:
                        /**
                         *  Performance:
                         *      Worst Case: O(1)
                         *      Average Case: O(1)
                         */
                        nonvirtual  void    RemoveFirst ();

                    public:
                        /*
                         *  Performance:
                         *      Worst Case: O(N)
                         *      Average Case: O(N)
                         *
                         *  Utility to search the list for the given item using operator==
                         */
                        nonvirtual  bool    Contains (T item) const;


                    public:
                        /**
                         *  Performance:
                         *      Worst Case: O(1)
                         */
                        nonvirtual  void    RemoveAt (const ForwardIterator& i);

                    public:
                        /**
                         *  Performance:
                         *      Worst Case: O(1)
                         */
                        nonvirtual  void    SetAt (const ForwardIterator& i, T newValue);

                    public:
                        /**
                         *  Performance:
                         *      Worst Case: O(1)
                         *
                         *  NB: Can be called if done
                         */
                        nonvirtual  void    AddBefore (const ForwardIterator& i, T item);

                    public:
                        /**
                         *  Performance:
                         *      Worst Case: O(1)
                         */
                        nonvirtual  void    AddAfter (const ForwardIterator& i, T item);

                    public:
                        /**
                         *  Note - does nothing if item not found.
                         */
                        nonvirtual  void    Remove (T item);

                    public:
                        nonvirtual  void    RemoveAll ();


                    public:
                        /*
                         *  Performance:
                         *      Worst Case: O(N)
                         *      Average Case: O(N)
                         *
                         *      Not alot of point in having these, as they are terribly slow,
                         *  but the could be convienient.
                         */
                        nonvirtual  T       GetAt (size_t i) const;

                    public:
                        /*
                         *  Performance:
                         *      Worst Case: O(N)
                         *      Average Case: O(N)
                         *
                         *      Not alot of point in having these, as they are terribly slow,
                         *  but the could be convienient.
                         */
                        nonvirtual  void    SetAt (T item, size_t i);


                    public:
                        nonvirtual  void    Invariant () const;

                    public:
                        class   Link;


                    public://////WORKRARBOUND - NEED MUTATOR TO ACCESS THIS SO OUR PROTECTED STUFF NOT NEEDED BY PATCHING CODE
                        //protected:
                        Link*       fFirst;

#if     qDebug
                    protected:
                        virtual     void    Invariant_ () const;
#endif

                    private:
                        friend  class   ForwardIterator;
                    };


                    template    <typename   T>
                    class   LinkedList<T>::Link {
                    public:
                        DECLARE_USE_BLOCK_ALLOCATION (Link);
                    public:
                        Link (T item, Link* next);

                    public:
                        T        fItem;
                        Link*    fNext;
                    };


                    /*
                     *      ForwardIterator<T> allows you to iterate over a LinkedList<T>. Its API
                     *  is designed to make easy implemenations of subclasses of IteratorRep<T>.
                     *  It is unpatched - use LinkedListIterator_Patch<T> or LinkedListMutator_Patch<T>
                     *  for that.
                     */
                    template    <typename   T>
                    class   LinkedList<T>::ForwardIterator {
                    public:
                        ForwardIterator (const ForwardIterator& from);
                        ForwardIterator (const LinkedList& data);

                    public:
                        nonvirtual  ForwardIterator& operator= (const ForwardIterator& it);

                    public:
                        nonvirtual  bool    Done () const;
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  T       Current () const;

                        nonvirtual  void    Invariant () const;

                    protected:
                        const typename LinkedList<T>::Link*     fCachedPrev;        // either nullptr or valid cached prev
                    protected:
                    public:
                        ///@todo - tmphack - this SB protected probably???
                        const typename LinkedList<T>::Link*     fCurrent;
                    protected:
                        bool                                    fSuppressMore;      // Indicates if More should do anything, or if were already Mored...

#if     qDebug
                        virtual void    Invariant_ () const;
#endif
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_LinkedList_h_ */
