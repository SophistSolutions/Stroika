/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_

/**
 *  \file
 *
 *  TODO:
 *
 *  Description:
 *      This module genericly wraps STL containers (such as map, vector etc), and facilitates
 *      using them as backends for Stroika containers.
 *
 *
 *      NOTE FOR USES OF THIS CLASS
 *          o            OK to use non-patchable API inside lock - like directly calling map<>::find()
 *
 *
 *  @todo   VERY INCOMPLETE Patch support. Unclear if/how I can do patch support generically - perhaps using
 *          some methods only called by array impls, and some only by returing iteratore on erase impls, etc,
 *          or perhaps with template specialization.
 *
 */

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"

#include    "../../Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /**
                     *  Use this to wrap an underlying stl container (like std::vector or stl:list, etc) to adapt
                     *  it for Stroika containers.
                     *
                     *  This code is NOT threadsafe. It assumes a wrapper layer provides thread safety, but it
                     *  DOES provide 'deletion'/update safety.
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    class   STLContainerWrapper : public CONTAINER_OF_T {
                    public:
                        STLContainerWrapper ();
                        STLContainerWrapper (const STLContainerWrapper<T, CONTAINER_OF_T>& from);

                    public:
                        ~STLContainerWrapper ();

                    public:
                        nonvirtual  STLContainerWrapper<T, CONTAINER_OF_T>& operator= (const STLContainerWrapper<T, CONTAINER_OF_T>& rhs);

                    public:
                        /**
                         * Are there any iterators to be patched?
                         */
                        nonvirtual  bool    HasActiveIterators () const;

                    public:
                        /**
                         *  call after add
                         */
                        nonvirtual  void    PatchAfter_insert (typename CONTAINER_OF_T::iterator i) const;

                    public:
                        /**
                         *  call before remove/erase
                         */
                        nonvirtual  void    PatchBefore_erase (typename CONTAINER_OF_T::iterator i) const;

                    public:
                        /**
                         *  call after removeall/clear
                         */
                        nonvirtual  void    PatchAfter_clear () const;

                    public:
                        /**
                         *  call after realloc could have happened (such as reserve)
                         */
                        nonvirtual  void    PatchAfter_Realloc () const;


                    public:
                        nonvirtual  void    Invariant () const;
#if     qDebug
                    protected:
                        nonvirtual  void    _Invariant () const;
#endif

                    public:
                        class IteratorPatchHelper;

                    private:
                        IteratorPatchHelper*    fActiveIteratorsListHead_;

                    private:
                        friend  class   IteratorPatchHelper;
                    };


                    /**
                     *      STLContainerWrapper::IteratorPatchHelper is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    class   STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper {
                    public:
                        IteratorPatchHelper (STLContainerWrapper<T, CONTAINER_OF_T>* data);
                        IteratorPatchHelper (const IteratorPatchHelper& from);

                    public:
                        ~IteratorPatchHelper ();

                    public:
                        nonvirtual  IteratorPatchHelper& operator= (const IteratorPatchHelper& rhs);

                    public:
                        inline  bool    Done () const;
                    public:
                        inline  bool    More (T* current, bool advance);


                    public:
                        /**
                         * Only legal to call if underlying iterator is random_access
                         */
                        nonvirtual  size_t  CurrentIndex () const;

                    public:
                        /**
                         */
                        nonvirtual  void    RemoveCurrent ();


                    public:
                        //  call after add
                        nonvirtual  void    PatchAfter_insert (typename CONTAINER_OF_T::iterator i) const;

                    public:
                        //  call before remove
                        nonvirtual  void    PatchBefore_erase (typename CONTAINER_OF_T::iterator i) const;

                    public:
                        //  call after removeall
                        nonvirtual  void    PatchAfter_clear () const;

                    public:
                        //  call after realloc could have happened (such as reserve)
                        nonvirtual  void    PatchAfter_Realloc () const;

                    public:
                        nonvirtual  void    Invariant () const;
#if     qDebug
                    protected:
                        nonvirtual  void    _Invariant () const;
#endif

                    public:
                        STLContainerWrapper<T, CONTAINER_OF_T>*   fData;
                        typename CONTAINER_OF_T::iterator         fStdIterator;
                        IteratorPatchHelper*                      fNextActiveIterator;
                        bool                                      fSuppressMore;  // for removealls
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
#include    "STLContainerWrapper.inl"


#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_ */
