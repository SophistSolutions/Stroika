/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */

#include "Private/Array.h"

#include "../Memory/BlockAllocated.h"


namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {

		    using namespace Private;


            // class Bag_Array<T>
            template	<typename T>	inline	Bag_Array<T>::Bag_Array (const Bag_Array<T>& bag) :
                Bag<T> (bag)
            {
            }
            template	<typename T>	inline	Bag_Array<T>&	Bag_Array<T>::operator= (const Bag_Array<T>& bag)
            {
                Bag<T>::operator= (bag);
                return (*this);
            }


            template	<typename T>	class	Bag_ArrayMutatorRep;
            template	<typename T>	class	Bag_ArrayRep : public BagRep<T> {
                public:
                    Bag_ArrayRep ();
                    ~Bag_ArrayRep ();

                    virtual	size_t		GetLength () const override;
                    virtual	bool		Contains (T item) const override;
                    virtual	void		Compact () override;
                    virtual	BagRep<T>*	Clone () const override;

                    virtual	IteratorRep<T>*		MakeIterator () override;
                    virtual	BagIteratorRep<T>*	MakeBagIterator () override;
                    virtual	BagMutatorRep<T>*	MakeBagMutator () override;

                    virtual	void	Add (T item) override;
                    virtual	void	Remove (T item) override;
                    virtual	void	RemoveAll () override;

                    static	void*	operator new (size_t size);
                    static	void	operator delete (void* p);

                private:
                    Array_Patch<T>	fData;

                friend	class	Bag_Array<T>;
                friend	class	Bag_ArrayMutatorRep<T>;
            };



            template	<typename T> class	Bag_ArrayMutatorRep : public BagMutatorRep<T> {
                public:
                    Bag_ArrayMutatorRep (Bag_ArrayRep<T>& owner);

                    virtual	bool	Done () const override;
                    virtual	bool	More (T* current) override;
                    virtual	T		Current () const override;

                    virtual	IteratorRep<T>*		Clone () const override;

                    virtual	void	RemoveCurrent () override;
                    virtual	void	UpdateCurrent (T newValue) override;

                    static	void*	operator new (size_t size);
                    static	void	operator delete (void* p);

                private:
                    ForwardArrayMutator_Patch<T>	fIterator;
                friend	class	Bag_ArrayRep<T>;
            };





            /*
             ********************************************************************************
             ******************************* Bag_ArrayMutatorRep ****************************
             ********************************************************************************
             */

            template	<class	T>	inline	void*	Bag_ArrayMutatorRep<T>::operator new (size_t size)
            {
                return (Memory::BlockAllocated<Bag_ArrayMutatorRep<T> >::operator new (size));
            }

            template	<class	T>	inline	void	Bag_ArrayMutatorRep<T>::operator delete (void* p)
            {
                Memory::BlockAllocated<Bag_ArrayMutatorRep<T> >::operator delete (p);
            }

            template	<class	T>	Bag_ArrayMutatorRep<T>::Bag_ArrayMutatorRep (Bag_ArrayRep<T>& owner) :
                BagMutatorRep<T> (),
                fIterator (owner.fData)
            {
            }

            template	<class	T>	bool	Bag_ArrayMutatorRep<T>::Done () const
            {
                return (fIterator.Done ());
            }

            template	<class	T>	bool	Bag_ArrayMutatorRep<T>::More (T* current)
            {
                return (fIterator.More (current));
            }

            template	<class	T>	T	Bag_ArrayMutatorRep<T>::Current () const
            {
                return (fIterator.Current());
            }

            template	<typename T>	IteratorRep<T>*	Bag_ArrayMutatorRep<T>::Clone () const
            {
                return (new Bag_ArrayMutatorRep<T> (*this));
            }

            template	<class	T>	void	Bag_ArrayMutatorRep<T>::RemoveCurrent ()
            {
                fIterator.RemoveCurrent ();
            }

            template	<class	T>	void	Bag_ArrayMutatorRep<T>::UpdateCurrent (T newValue)
            {
                fIterator.UpdateCurrent (newValue);
            }






            /*
             ********************************************************************************
             ********************************* Bag_ArrayRep *********************************
             ********************************************************************************
             */

            template	<class	T>	inline	void*	Bag_ArrayRep<T>::operator new (size_t size)
            {
                return (Memory::BlockAllocated<Bag_ArrayRep<T> >::operator new (size));
            }

            template	<class	T>	inline	void	Bag_ArrayRep<T>::operator delete (void* p)
            {
                Memory::BlockAllocated<Bag_ArrayRep<T> >::operator delete (p);
            }

            template	<typename T>	inline	Bag_ArrayRep<T>::Bag_ArrayRep () :
                fData ()
            {
            }

            template	<typename T>	Bag_ArrayRep<T>::~Bag_ArrayRep ()
            {
            }

            template	<typename T>	size_t	Bag_ArrayRep<T>::GetLength () const
            {
                return (fData.GetLength ());
            }

            template	<typename T>	void	Bag_ArrayRep<T>::Compact ()
            {
                fData.Compact ();
            }

            template	<typename T> bool Bag_ArrayRep<T>::Contains (T item) const
            {
                return (fData.Contains (item));
            }

            template	<typename T>	IteratorRep<T>*	Bag_ArrayRep<T>:: MakeIterator ()
            {
                return (new Bag_ArrayMutatorRep<T> (*this));
            }

            template	<typename T>	BagIteratorRep<T>*	Bag_ArrayRep<T>::MakeBagIterator ()
            {
                return (new Bag_ArrayMutatorRep<T> (*this));
            }

            template	<typename T>	BagMutatorRep<T>*	Bag_ArrayRep<T>::MakeBagMutator ()
            {
                return (new Bag_ArrayMutatorRep<T> (*this));
            }

            template	<typename T>	void	Bag_ArrayRep<T>::Add (T item)
            {
                fData.InsertAt (item, GetLength ());
            }


            template	<typename T>	void	Bag_ArrayRep<T>::Remove (T item)
            {
                /*
                 *		Iterate backwards since removing from the end of an array
                 *	will be faster.
                 */
#if qIteratorsRequireNoArgContructorForT
                T temp;
#endif
                for (BackwardArrayIterator<T> it (fData); it.More (&temp);) {
                    if (temp == item) {
                        fData.RemoveAt (it.CurrentIndex ());
                        return;
                    }
                }
            }

            template	<typename T>	void	Bag_ArrayRep<T>::RemoveAll ()
            {
                fData.RemoveAll ();
            }






            /*
             ********************************************************************************
             ************************************** Bag_Array *******************************
             ********************************************************************************
             */

            template	<typename T>	Bag_Array<T>::Bag_Array () :
                Bag<T> (new Bag_ArrayRep<T> ())
            {
            }

            template	<typename T>	Bag_Array<T>::Bag_Array (const Bag<T>& bag) :
                Bag<T> (new Bag_ArrayRep<T> ())
            {
                SetSlotsAlloced (bag.GetLength ());
                operator+= (bag);
            }

            template	<typename T>	Bag_Array<T>::Bag_Array (const T* items, size_t size) :
                Bag<T> (new Bag_ArrayRep<T> ())
            {
                SetSlotsAlloced (size);
                AddItems (items, size);
            }

            template	<typename T>	inline	const Bag_ArrayRep<T>*	Bag_Array<T>::GetRep () const
            {
                /*
                 * This cast is safe since we are very careful to assure we always have a pointer
                 * to this type. We could keep extra pointer, but that would cost space and time,
                 * and with care we don't need to.
                 */
                return ((const Bag_ArrayRep<T>*) Bag<T>::GetRep ());
            }

            template	<typename T>	inline	Bag_ArrayRep<T>*	Bag_Array<T>::GetRep ()
            {
                /*
                 * This cast is safe since we are very careful to assure we always have a pointer
                 * to this type. We could keep extra pointer, but that would cost space and time,
                 * and with care we don't need to.
                 */
                return ((Bag_ArrayRep<T>*) Bag<T>::GetRep ());
            }

            template	<typename T>	size_t	Bag_Array<T>::GetSlotsAlloced () const
            {
                return (GetRep ()->fData.GetSlotsAlloced ());
            }

            template	<typename T>	void	Bag_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
            {
                GetRep ()->fData.SetSlotsAlloced (slotsAlloced);
            }

            template	<typename T>	BagRep<T>*	Bag_ArrayRep<T>::Clone () const
            {
                return (new Bag_ArrayRep<T> (*this));
            }







		}
	}
}
