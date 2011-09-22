/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Tally_h_
#define	_Stroika_Foundation_Containers_Tally_h_	1


#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Memory/SharedByValue.h"

#include    "Iterator.h"



namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {

			template	<typename T>	class	TallyEntry {
				public:
					TallyEntry (T item);
					TallyEntry (T item, size_t count);

					T		fItem;
					size_t	fCount;
			};


			template	<typename T> class	Tally;
			template	<typename T> class	TallyRep;
			template	<typename T> class	IteratorRep;
			template	<typename T> class	TallyMutatorRep;

			template	<typename T> bool	operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs);

            template	<typename T>	bool	operator== (const Tally<T>& lhs, const Tally<T>& rhs);
            template	<typename T>	bool	operator!= (const Tally<T>& lhs, const Tally<T>& rhs);


			// Must be more careful about copying TallyMutators...Now that iterators are copyable...
			template	<typename T> class	TallyMutator : public Iterator<TallyEntry<T> > {
				public:
					TallyMutator (TallyMutatorRep<T>* it);

					nonvirtual	void	RemoveCurrent ();

				/*
				 *		Update the
				 * NB: if newCount == 0, equivilent to RemoveCurrent().
				 */
				public:
					nonvirtual	void	UpdateCount (size_t newCount);
			};


			template	<typename T> class	Tally {
				public:
					Tally ();
					Tally (const Tally<T>& src);
					Tally (const T* items, size_t size);

				protected:
					Tally (TallyRep<T>* rep);

				public:
					nonvirtual	Tally<T>& operator= (const Tally<T>& src);


				public:
					nonvirtual	size_t	GetLength () const;
					nonvirtual	bool	IsEmpty () const;
					nonvirtual	bool	Contains (T item) const;
					nonvirtual	void	RemoveAll ();
					nonvirtual	void	Compact ();

					nonvirtual	void	Add (T item);
					nonvirtual	void	Add (T item, size_t count);
					nonvirtual	void	Remove (T item);
					nonvirtual	void	Remove (T item, size_t count);
					nonvirtual	void	RemoveAll (T item);
					nonvirtual	size_t	TallyOf (T item) const;
					nonvirtual	size_t	TotalTally () const;

					nonvirtual	Tally<T>&	operator+= (T item);
					nonvirtual	Tally<T>&	operator+= (Tally<T> t);

				public:
					nonvirtual	operator Iterator<T> () const;
					nonvirtual	operator Iterator<TallyEntry<T> > () const;
					nonvirtual	operator TallyMutator<T> ();

					// Support for ranged for, and stl syntax in general
                    nonvirtual  Iterator<T>    begin () const;
                    nonvirtual  IterationState end () const;

				protected:
					nonvirtual	void	AddItems (const T* items, size_t size);

					nonvirtual	const TallyRep<T>*	GetRep () const;
					nonvirtual	TallyRep<T>*		GetRep ();

				private:
					Memory::SharedByValue<TallyRep<T> >	fRep;

					static	TallyRep<T>*	Clone (const TallyRep<T>& rep);

				friend	bool	operator==<T> (const Tally<T>& lhs, const Tally<T>& rhs);
			};

			template	<typename T> bool	operator== (const Tally<T>& lhs, const Tally<T>& rhs);
			template	<typename T> bool	operator!= (const Tally<T>& lhs, const Tally<T>& rhs);



		}
    }
}


#endif	/*_Stroika_Foundation_Containers_Tally_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Tally.inl"





