/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

#ifndef _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_


/*
 *  TODO:
 *
 */





/*
 *
 *  Description:
 *
 *      NOTE FOR USES OF THIS CLASS
 *          o            OK to use non-patchable API inside lock - like directly calling map<>::find()
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
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    class   STLContainerWrapper : public CONTAINER_OF_T {
                    public:
                        STLContainerWrapper ()
                            : CONTAINER_OF_T () {
                        }
                        STLContainerWrapper (const STLContainerWrapper<T, CONTAINER_OF_T>& from)
                            : CONTAINER_OF_T (from) {
                        }
                    public:
                        ~STLContainerWrapper () {
                        }

                    public:
                        nonvirtual  STLContainerWrapper<T, CONTAINER_OF_T>& operator= (const STLContainerWrapper<T, CONTAINER_OF_T>& rhs) {
                            CONTAINER_OF_T::operator= (rhs);
                            return *this;
                        }

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perform patching.
                         */
                    public:
                        //  are there any iterators to be patched?
                        nonvirtual  bool    HasActiveIterators () const;

                    public:
                        //  call after add
                        nonvirtual  void    PatchAfter_insert (typename CONTAINER_OF_T::iterator i) const {
                        }

                    public:
                        //  call before remove
                        nonvirtual  void    PatchBefore_erase (typename CONTAINER_OF_T::iterator i) const {
                        }

                    public:
                        //  call after removeall
                        nonvirtual  void    PatchAfter_clear () const {
                        }

                    public:
                        //  call after realloc could have happened (such as reserve)
                        nonvirtual  void    PatchAfterRealloc () const {
                        }


                    public:
                        class IteratorPatchHelper;
                    private:
                        IteratorPatchHelper* fIterators;        // head of linked list of active iterators
                        friend  class   IteratorPatchHelper;
                    };


                    /*
                     *      STLContainerWrapper::IteratorPatchHelper is a private utility class designed
                     *  to promote source code sharing among the patched iterator implementations.
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    class   STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper {
                    public:
                        IteratorPatchHelper (STLContainerWrapper<T, CONTAINER_OF_T>* data)
                            : fData (data)
                            , fStdIterator (data->begin ())
                            , fNext (data->fIterators)
                            ,  fSuppressMore (true) {
                            fData->fIterators = this;
                        }
                        IteratorPatchHelper (const IteratorPatchHelper& from)
                            : fData (from.fData)
                            , fStdIterator (from.fStdIterator)
                            , fNext (from.fData->fIterators)
                            ,  fSuppressMore (from.fSuppressMore) {
                            fData->fIterators = this;
                        }
                    public:
                        ~IteratorPatchHelper () {
                            AssertNotNull (fData);
                            if (fData->fIterators == this) {
                                fData->fIterators = fNext;
                            }
                            else {
                                auto v = fData->fIterators;
                                for (; v->fNext != this; v = v->fNext) {
                                    AssertNotNull (v);
                                    AssertNotNull (v->fNext);
                                }
                                AssertNotNull (v);
                                Assert (v->fNext == this);
                                v->fNext = fNext;
                            }
                        }

                    public:
                        nonvirtual  IteratorPatchHelper& operator= (const IteratorPatchHelper& rhs) {
                            AssertNotImplemented ();
                            return *this;
                        }

                    public:
                        inline  bool    Done () const {
                            return fStdIterator == fData->end ();
                        }
                        inline  bool    More (T* current, bool advance) {
                            bool    done    =   Done ();
                            if (advance) {
                                if (not this->fSuppressMore and not done) {
                                    this->fStdIterator++;
                                }
                                this->fSuppressMore = false;
                                done = Done ();
                                if ((current != nullptr) and (not done)) {
                                    *current = *fStdIterator;
                                }
                            }
                            return (not done);
                        }
                    public:
                        //  call after add
                        nonvirtual  void    PatchAdd (typename CONTAINER_OF_T::iterator i);
                        //  call before remove
                        nonvirtual  void    PatchRemove (typename CONTAINER_OF_T::iterator i);
                        //  call after removeall
                        nonvirtual  void    PatchRemoveAll ();
                        //  call after realloc could have happened
                        nonvirtual  void    PatchRealloc ();

                    public:
                        STLContainerWrapper<T, CONTAINER_OF_T>*   fData;
                        typename CONTAINER_OF_T::iterator               fStdIterator;
                        IteratorPatchHelper*                            fNext;
                        bool                                            fSuppressMore;  // for removealls
                    };


                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "STLContainerWrapper.inl"

