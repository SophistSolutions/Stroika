/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_inl_
#define _Stroika_Foundation_Containers_Tally_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"
#include    "Tally.h"

#include    "Concrete/Tally_Array.h"  // needed for default constructor


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             * Protected helper class to convert from an iterator of TallyEntries
             * to an iterator over individual items.
             *
             *  @todo   BUG - _TallyEntryToItemIteratorHelperRep should stop N times where N is the value of count
             *          in the source iterator!
             *
             *          But document and verify the calling code is expecting this.
             */
            template    <typename T>
            class  Tally<T>::_IRep::_TallyEntryToItemIteratorHelperRep : public Iterator<T>::IRep {
            public:
                _TallyEntryToItemIteratorHelperRep (const Iterator<TallyEntry<T>>& fDelegateTo);

                virtual bool                                    More (T* current, bool advance) override;
                virtual shared_ptr<typename Iterator<T>::IRep>  Clone () const override;
                virtual bool                                    StrongEquals (const typename Iterator<T>::IRep* rhs) const override;

            private:
                Iterator<TallyEntry<T>> fDelegateTo_;
            };


            /*
             ********************************************************************************
             ****************************** TallyEntry<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  TallyEntry<T>::TallyEntry (T item)
                : fItem (item)
                , fCount (1)
            {
            }
            template    <typename T>
            inline  TallyEntry<T>::TallyEntry (T item, size_t count)
                : fItem (item)
                , fCount (count)
            {
            }
            template    <typename T>
            inline bool   TallyEntry<T>::operator== (const TallyEntry<T>& rhs)  const
            {
                return (fCount == rhs.fCount and fItem == rhs.fItem);
            }
            template    <typename T>
            bool   operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs)
            {
                if (not (lhs.fItem == rhs.fItem))  {
                    return false;
                }
                return (bool (lhs.fCount == rhs.fCount));
            }
            template    <typename T>
            inline bool   TallyEntry<T>::operator!= (const TallyEntry<T>& rhs)  const
            {
                return not (operator== (rhs));
            }


            /*
             ********************************************************************************
             **************************** Tally<T>::_IRep ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Tally<T>::_IRep::_IRep ()
            {
            }


            /*
             ********************************************************************************
             *********************************** Tally<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            Tally<T>::Tally ()
                : inherited (Concrete::Tally_Array<T> ())
            {
            }
            template    <typename T>
            inline  Tally<T>::Tally (const Tally<T>& src)
                : inherited (src)
            {
            }
            template    <typename T>
            inline  Tally<T>::Tally (const _SharedPtrIRep& rep)
                : inherited (typename Iterable<TallyEntry<T>>::_SharedByValueRepType (rep))
            {
            }
            template    <typename T>
            Tally<T>::Tally (const T* start, const T* end)
                : inherited (Concrete::Tally_Array<T> (start, end))
            {
            }
            template    <typename T>
            Tally<T>::Tally (const TallyEntry<T>* start, const TallyEntry<T>* end)
                : inherited (Concrete::Tally_Array<T> (start, end))
            {
            }
            template    <typename T>
            void   Tally<T>::RemoveAll (T item)
            {
                Remove (item, TallyOf (item));
            }
            template    <typename T>
            size_t Tally<T>::TotalTally () const
            {
                size_t sum = 0;
                for (auto i = this->begin (); i != this->end (); ++i) {
                    sum += (*i).fCount;
                }
                return sum;
            }
            template    <typename T>
            Tally<T>&  Tally<T>::operator+= (const Tally<T>& t)
            {
                for (auto i = t.begin (); i != t.end (); ++i) {
                    Add (i->fItem, i->fCount);
                }
                return (*this);
            }
            template    <typename T>
            void   Tally<T>::Add (const T* begin, const T* end)
            {
                for (const T* i = begin; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T>
            inline  bool    Tally<T>::Contains (T item) const
            {
                return (_GetRep ().Contains (item));
            }
            template    <typename T>
            inline  void    Tally<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            inline  void    Tally<T>::Compact ()
            {
                _GetRep ().Compact ();
            }
            template    <typename T>
            inline  Iterator<T>    Tally<T>::MakeBagIterator () const
            {
                return _GetRep ().MakeBagIterator ();
            }
            template    <typename T>
            inline  Iterator<T>    Tally<T>::bagbegin () const
            {
                return MakeBagIterator ();
            }
            template    <typename T>
            inline  Iterator<T>       Tally<T>::bagend () const
            {
                return (Iterator<T>::GetEmptyIterator ());
            }
            template    <typename T>
            inline  void    Tally<T>::Add (T item)
            {
                _GetRep ().Add (item, 1);
            }
            template    <typename T>
            inline  void    Tally<T>::Add (T item, size_t count)
            {
                _GetRep ().Add (item, count);
            }
            template    <typename T>
            inline  void    Tally<T>::Remove (T item)
            {
                _GetRep ().Remove (item, 1);
            }
            template    <typename T>
            inline  void    Tally<T>::Remove (T item, size_t count)
            {
                _GetRep ().Remove (item, count);
            }
            template    <typename T>
            inline  void    Tally<T>::Remove (const Iterator<TallyEntry<T>>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T>
            inline  void    Tally<T>::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
            {
                _GetRep ().UpdateCount (i, newCount);
            }
            template    <typename T>
            inline  size_t  Tally<T>::TallyOf (T item) const
            {
                return (_GetRep ().TallyOf (item));
            }
            template    <typename T>
            inline  Tally<T>&   Tally<T>::operator+= (T item)
            {
                _GetRep ().Add (item, 1);
                return (*this);
            }
            template    <typename T>
            inline  const typename Tally<T>::_IRep&  Tally<T>::_GetRep () const
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), Tally<T>::_IRep);
                return *static_cast<const Tally<T>::_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename Tally<T>::_IRep&        Tally<T>::_GetRep ()
            {
                // Unsure - MAY need to use dynamic_cast here - but I think static cast performs better, so try...
                EnsureMember (&inherited::_GetRep (), Tally<T>::_IRep);
                return *static_cast<Tally<T>::_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline bool    operator!= (const Tally<T>& lhs, const Tally<T>& rhs)
            {
                return (not operator== (lhs, rhs));
            }
            template    <typename T>
            bool   operator== (const Tally<T>& lhs, const Tally<T>& rhs)
            {
                if (&lhs._GetRep () == &rhs._GetRep ()) {
                    return (true);
                }
                if (lhs.GetLength () != rhs.GetLength ()) {
                    return (false);
                }
                for (auto i = rhs.begin (); i != rhs.end (); ++i) {
                    if (i->fCount != rhs.TallyOf (i->fItem)) {
                        return (false);
                    }
                }
                return (true);
            }


            /*
             ********************************************************************************
             ************* Tally<T>::_IRep::_TallyEntryToItemIteratorHelperRep **************
             ********************************************************************************
             */
            template    <typename T>
            Tally<T>::_IRep::_TallyEntryToItemIteratorHelperRep::_TallyEntryToItemIteratorHelperRep (const Iterator<TallyEntry<T>>& delegateTo)
                : fDelegateTo_ (delegateTo)
            {
            }
            template    <typename T>
            inline bool    Tally<T>::_IRep::_TallyEntryToItemIteratorHelperRep::More (T* current, bool advance)
            {
                if (current == nullptr) {
                    return not fDelegateTo_.Done ();
                }
                else {
                    fDelegateTo_++;
                    bool done = fDelegateTo_.Done ();
                    if (current != nullptr and not done) {
                        *current = (*fDelegateTo_).fItem;
                    }
                    return (not done);
                }
            }
            template    <typename T>
            inline shared_ptr<typename Iterator<T>::IRep>  Tally<T>::_IRep::_TallyEntryToItemIteratorHelperRep::Clone () const
            {
                return shared_ptr<typename Iterator<T>::IRep> (new _TallyEntryToItemIteratorHelperRep (Iterator<TallyEntry<T>> (fDelegateTo_)));
            }
            template    <typename T>
            inline  bool    Tally<T>::_IRep::_TallyEntryToItemIteratorHelperRep::StrongEquals (const typename Iterator<T>::IRep* rhs) const
            {
                AssertNotImplemented ();
                return false;
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Tally_inl_ */


