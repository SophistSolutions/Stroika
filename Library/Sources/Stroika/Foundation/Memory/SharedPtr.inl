/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedPtr_inl_
#define	_Stroika_Foundation_Memory_SharedPtr_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Execution/AtomicOperations.h"
#include	"BlockAllocated.h"


namespace	Stroika {
	namespace	Foundation {
		namespace	Memory {


			/*
			 * IMPLEMENTATION NOTES:
			 *
			 *		We have Decrement() return a boolean and if it returns true to delete in the caller because otehrwise the PRIVATE
			 *	stuff (if you use SharedRep<> of a private type) - generates erorrs. At least it causes problems on Visual Studio.Net 2010.
			 *	I'm not sure if this is MY bug or a compiler  bug. Anyhow - this is fine for now...
			 *			-- LGP 2012-05-15
			 *
			 */


			namespace	Private {
				namespace	SharedPtr_Default_Traits_Helpers_ {

					struct	ReferenceCounterContainerType_ {
						ReferenceCountType_	fCount;
						ReferenceCounterContainerType_ ()
							: fCount (0)
							{
							}
						DECLARE_USE_BLOCK_ALLOCATION(ReferenceCounterContainerType_);
					};

					template	<typename	T>
						class	Envelope_ {
							private:
								T*								fPtr_;
								ReferenceCounterContainerType_*	fCountHolder_;
							public:
								Envelope_ (T* ptr, ReferenceCounterContainerType_* countHolder)
									: fPtr_ (ptr)
									, fCountHolder_ (countHolder)
									{
										if (fPtr_ != nullptr and countHolder == nullptr) {
											fCountHolder_ = new ReferenceCounterContainerType_ ();
										}
									}
								template <typename T2>
									inline	Envelope_ (const Envelope_<T2>& from)
										: fPtr_ (from.GetPtr ())
										, fCountHolder_ (from.fCountHolder_)
									{
									}
								inline	T*		GetPtr () const 
									{
										return fPtr_;
									}
								inline	void	SetPtr (T* p)
									{
										fPtr_ = p;
									}
								inline	ReferenceCountType_	CurrentRefCount () const
									{
										return fCountHolder_==nullptr? 0: fCountHolder_->fCount;
									}
								inline	void	Increment ()
									{
										RequireNotNull (fCountHolder_);
										Execution::AtomicIncrement (&fCountHolder_->fCount);
									}
								inline	bool	Decrement ()
									{
										Require (CurrentRefCount () > 0);
										if (Execution::AtomicDecrement (&fCountHolder_->fCount) == 0) {
											DoDeleteCounter ();
											return true;
										}
										return false;
									}
								inline	void	DoDeleteCounter ()
									{
										delete fCountHolder_;
										fCountHolder_ = nullptr;
									}
								inline	ReferenceCounterContainerType_*	GetCounterPointer () const
									{
										return fCountHolder_;
									}
							private:
								template	<typename T2>
									friend	class	Envelope_;
						};
				}
			}
			

			namespace	Private {
				namespace	enable_shared_from_this_Traits_Helpers_ {

					template	<typename	T>
						class	Envelope_ {
							private:
								T*		fPtr;
							public:
								Envelope_ (T* ptr, T* ptr2)
										: fPtr (ptr)
									{
										// Either they must be the same, or hte ptr2 (counter object) must be null - telling us to make a new one...
										Require (ptr == ptr2 or ptr2 == nullptr);
									}
								template <typename T2>
									Envelope_ (const Envelope_<T2>& from)
											: fPtr (from.fPtr)
										{
										}
								T*	GetPtr () const
									{
										return fPtr;
									}
								void	SetPtr (T* p)
									{
										fPtr = p;
									}
								ReferenceCountType_	CurrentRefCount () const
									{
										return fPtr==nullptr? 0: fPtr->fCount_;
									}
								void	Increment ()
									{
										RequireNotNull (fPtr);
										Execution::AtomicIncrement (&fPtr->fCount_);
									}
								bool	Decrement ()
									{
										Require (CurrentRefCount () > 0);
										if (Execution::AtomicDecrement (&fPtr->fCount_) == 0) {
											DoDeleteCounter ();
											return true;
										}
										return false;
									}
								inline	void	DoDeleteCounter ()
									{
									}
								enable_shared_from_this<T>*	GetCounterPointer () const
									{
										return fPtr;
									}
						};

				}
			}







		//	class	SharedPtr<T,T_TRAITS>
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr ()
					: fEnvelope_ (nullptr, nullptr)
					{
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (T* from)
					: fEnvelope_ (from, nullptr)
					{
						if (fEnvelope_.GetPtr () != nullptr) {
							// NB: the fEnvelope_.CurrentRefCount () USUALLY == 0, but not necessarily, if the refcount is stored
							// in the 'from' - (see SharedPtrBase) - in which case the refcount might already be larger.
							fEnvelope_.Increment ();
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (const typename T_TRAITS::Envelope& from)
					: fEnvelope_ (from)
					{
						if (fEnvelope_.GetPtr () != nullptr) {
							fEnvelope_.Increment ();
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::SharedPtr (const SharedPtr<T,T_TRAITS>& from)
					: fEnvelope_ (from.fEnvelope_)
					{
						if (fEnvelope_.GetPtr () != nullptr) {
							fEnvelope_.Increment ();
						}
					}
			template	<typename T, typename T_TRAITS>
					template <typename T2, typename T2_TRAITS>
						SharedPtr<T,T_TRAITS>::SharedPtr (const SharedPtr<T2, T2_TRAITS>& from)
							: fEnvelope_ (from.fEnvelope_)
							{
								if (fEnvelope_.GetPtr () != nullptr) {
									fEnvelope_.Increment ();
								}
							}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>& SharedPtr<T,T_TRAITS>::operator= (const SharedPtr<T,T_TRAITS>& rhs)
					{
						if (rhs.fEnvelope_.GetPtr () != fEnvelope_.GetPtr ()) {
							if (fEnvelope_.GetPtr () != nullptr) {
								if (fEnvelope_.Decrement ()) {
									delete fEnvelope_.GetPtr ();
									fEnvelope_.SetPtr (nullptr);
								}
							}
							fEnvelope_ = rhs.fEnvelope_;
							if (fEnvelope_.GetPtr () != nullptr) {
								Assert (fEnvelope_.CurrentRefCount () > 0);
								fEnvelope_.Increment ();
							}
						}
						return *this;
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::~SharedPtr ()
					{
						if (fEnvelope_.GetPtr () != nullptr) {
							if (fEnvelope_.Decrement ()) {
								delete fEnvelope_.GetPtr ();
							}
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::IsNull () const
					{
						return fEnvelope_.GetPtr () == nullptr;
					}
			template	<typename T, typename T_TRAITS>
				inline	T&	SharedPtr<T,T_TRAITS>::GetRep () const
					{
						RequireNotNull (fEnvelope_.GetPtr ());
						Assert (fEnvelope_.CurrentRefCount () > 0);
						return *fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				inline	T* SharedPtr<T,T_TRAITS>::operator-> () const
					{
						return &GetRep ();
					}
			template	<typename T, typename T_TRAITS>
				inline	T& SharedPtr<T,T_TRAITS>::operator* () const
					{
						return GetRep ();
					}
			template	<typename T, typename T_TRAITS>
				inline	SharedPtr<T,T_TRAITS>::operator T* () const
					{
						return fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				inline	T*	SharedPtr<T,T_TRAITS>::get () const
					{
						return (fEnvelope_.GetPtr ());
					}
			template	<typename T, typename T_TRAITS>
				inline	void	SharedPtr<T,T_TRAITS>::release ()
					{
						*this = SharedPtr<T,T_TRAITS> (nullptr);
					}
			template	<typename T, typename T_TRAITS>
				inline	void	SharedPtr<T,T_TRAITS>::clear ()
					{
						release ();
					}
			template	<typename T, typename T_TRAITS>
				inline	void	SharedPtr<T,T_TRAITS>::reset (T* p)
					{
						if (fEnvelope_.GetPtr () != p) {
							*this = SharedPtr<T,T_TRAITS> (p);
						}
					}
			template	<typename T, typename T_TRAITS>
				template <typename T2>
					SharedPtr<T2> SharedPtr<T,T_TRAITS>::Dynamic_Cast ()
						{
							//return SharedPtr<T2> (dynamic_cast<T2*> (get ()), fEnvelope_.GetCounterPointer ());
							return SharedPtr<T2> (SharedPtr_Default_Traits<T2>::Envelope (dynamic_cast<T2*> (get ()), fEnvelope_.GetCounterPointer ()));
						}
			template	<typename T, typename T_TRAITS>
				inline	typename T_TRAITS::ReferenceCountType	SharedPtr<T,T_TRAITS>::CurrentRefCount () const
					{
						return fEnvelope_.CurrentRefCount ();
					}
			template	<typename T, typename T_TRAITS>
				inline	typename T_TRAITS::ReferenceCountType	SharedPtr<T,T_TRAITS>::use_count () const
					{
						return fEnvelope_.CurrentRefCount ();
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::IsUnique () const
					{
						return fEnvelope_.CurrentRefCount () == 1;
					}
			template	<typename T, typename T_TRAITS>
				inline	bool	SharedPtr<T,T_TRAITS>::unique () const
					{
						// respect the stl-ish names
						return IsUnique ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator< (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fEnvelope_.GetPtr () < rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator<= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fEnvelope_.GetPtr () <= rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator> (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fEnvelope_.GetPtr () > rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator>= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						// not technically legal to compare pointers this way, but its is legal to convert to int, and then compare, and
						// this does the same thing...
						//		-- LGP 2009-01-11
						return fEnvelope_.GetPtr () >= rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator== (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						return fEnvelope_.GetPtr () == rhs.fEnvelope_.GetPtr ();
					}
			template	<typename T, typename T_TRAITS>
				bool	SharedPtr<T,T_TRAITS>::operator!= (const SharedPtr<T,T_TRAITS>& rhs) const
					{
						return fEnvelope_.GetPtr () != rhs.fEnvelope_.GetPtr ();
					}






			//	class	enable_shared_from_this<T>
			template	<typename	T>
				inline	enable_shared_from_this<T>::enable_shared_from_this ():
					fCount_ (0)
					{
					}
			template	<typename	T>
				inline	enable_shared_from_this<T>::~enable_shared_from_this ()
					{
					}
			template	<typename	T>
				SharedPtr<T,SharedPtrFromThis_Traits<T>> enable_shared_from_this<T>::shared_from_this () 
					{
						/*
							* The Constructor for SharedPtr<T> expects a T*. However, we don't have a T*. But recall,
							* the ONLY legal way to use this enable_shared_from_this is:
							*
							* 		struct	TTT : Memory::enable_shared_from_this<TTT> {
							*			string x;
							*			....
							*		};
							*
							*	and so if we have a legal pointer to enable_shared_from_this<T>, then it MUST also be castable to a pointer to T*!!!
							*/
						T*	tStarThis	=	static_cast<T*> (this);
						return (SharedPtr<T,SharedPtrFromThis_Traits<T>> (tStarThis));
					}


		}




		namespace	Execution {
			template	<typename T>
				inline	void	ThrowIfNull (const Memory::SharedPtr<T>& p)
					{
						if (p.get () == nullptr) {
							Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<T> ()) - throwing bad_alloc ()");
						}
					}
			template	<typename T, typename T_TRAITS>
				inline	void	ThrowIfNull (const Memory::SharedPtr<T,T_TRAITS>& p)
					{
						if (p.get () == nullptr) {
							Execution::DoThrow (bad_alloc (), "ThrowIfNull (SharedPtr<T,T_TRAITS> ()) - throwing bad_alloc ()");
						}
					}
		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtr_inl_*/
