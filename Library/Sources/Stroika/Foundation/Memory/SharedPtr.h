/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedPtr_h_
#define	_Stroika_Foundation_Memory_SharedPtr_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Execution/AtomicOperations.h"
#include	"../Execution/Exceptions.h"


/*
 * GETTING RID OF SharedPtrBase/UsesSharedPtrBase
 *
 *		(o)	I'm not happy with the design of SharedPtrBase/UsesSharedPtrBase. In practice - its awkward to use.
 *
 *			But there is one fairly import use of smart pointers it seems hard to avoid.
 *
 *			Consider a letter/envelope pair DB (envelope) and DBRep. Consider that there are LOTS of subclasses of DBRep, and the API
 *			is rich. Consider that there are many modules - which use the class DB (such as Import/Export).
 *
 *			DB is a simple envelope that uses SharedPtr<DBrep>.
 *
 *			How - in a subclass of DBRep - I want to call some API - such as Export - with DB (this). That would be BAD - as it would
 *			create a shared pointer out of this, and delete it when DB goes out of scope.
 *
 *			It is principally to solve THIS problem - that SharedPtrBase/UsesSharedPtrBase still exists for. There is as common base class
 *			(so you can always find the shared reference count).
 *
 *			For example:
 *
				void	PHRDBBaseRep::DoSave (ProgressStatusCallback* progressCallback)
				{
					wstring	soapBody;
					{
						BLOBs::BLOB	phrBLOB;
						{
							stringstream	tmpOut;
							ImportExport::Export (PHRDB::DB (SharedPtr<IDBRep> (SharedPtr<IDBRep>::eUsesSharedPtrBase, this)), MimeTypes::Predefined::HealthFramePHR2_CT (), ImportExport::ExportOptions (), tmpOut, ProgressSubTask (progressCallback, 0.0f, .5f));
							phrBLOB = StreamUtils::ReadStreamAsBLOB (tmpOut);
						}
						CreateSOAPEnvelope_PUTPHR (&soapBody, NarrowSDKStringToWide (Cryptography::EncodeBase64 (phrBLOB)), fUserName, fPassword, fOverridePHRName);
					}
					const wstring	kSoapAction	=	 wstring (kHealthFrameWorksURLNamespaceURI_V10) + L"UploadPHR";
					(void)HTTPSupport::SimpleSOAPSendReceive (L"POST", fUploadURL, kSoapAction, soapBody, CalcSendBestTimeout2Use (soapBody), sOurUserAgent, ProgressSubTask (progressCallback, 0.5f, 1.0f));
				}

 *
 *			ALTERNATIVES:
 *
 *			(A)		DELETER argument to SharedPtr<>. Basically - we would have a TEMPLATD ARG to SharedPtr - the default of
 *					which would take up zero size (calling regular operaotr delete on T arg to SharedPtr).
 *
 *					That has the advantage that if someone delcared SharedPtr<T,T::SPECIALDELETER> and tried to assign something of
 *					type SharedPtr<T> - that would not compile (in contrast with existing practice where SharedPtr<IDBRep> (this) would compile
 *					and just crash badly).
 *
 *			(B)		DELETER function pointer - in EVERY SharedPtr<> - passed in for orignal CTOR call. Stores extra pointer with every underlying SharedPtrREP.
 *					But then have specail one that does NOTHING. And then 
 *
 *			(C)		ScopedNODELETER
 *					This would be a specail class (or parameter to SharedPtr<>) which initializes the sharedptr with a specail count
 *					with - perhaps - the value 1, and then asserts at the end - when the envoleope is desroyed, that hte value is still 1,
 *					and THEN delete the COUNTER (cuz 1 was magically like zero - means all refernces went away).
 *
 *					It would be a BUG if the originating ScopeNoDeleter/SharedPtr didn't left behind any references. All the cases I have currently
 *					that would be fine for - and it works pretty well - but could be create buggy code if called code with this special pointer
 *					stored those poitners. Really just doesnt work for those cases. Maybe not too bad.
 *
 *			(D)		Partial specialization with SharedPtrBase.
 *					If I had faith in this always working - it would probably be the best choice. I guess I should try it...
 *					Basically - if you have a class you want to pull this trick on - then inheirt from a specail base class, and the rest
 *					happens fully automatically.
 *					[attractive enuf - IF I can make the partial-specailation stuff work well enuf - then its perhaps the best way to go]
 *
 *					This MAYBE as simple as template specialize of
 *									template	<typename T>
										inline	SharedPtr<T>::~SharedPtr ()
							and
								op=
							(or bette ryet - but the delete part in its own template and template partial specialize that

 *
 */



/*
 * Description:
 *
 *	...
 *
 * TODO:
 *		+	CAREFULLY writeup differences between this class and shared_ptr<>
 *			+	I DONT BELIEVE weak_ptr<T> makes sense, and seems likely to generate bugs in multithreaded
 *				applications. Maybe I'm missing something. Ask around a bit...
 *			+	SEE GETTING RID OF SharedPtrBase/UsesSharedPtrBase ABOVE...
 *
 *
				Then - we COULD get rid of SharedPtr<> altoegher and just not use the weak_ptr stuff if its a bad idea. I'm undecided on that
				later point. More thought required
						--	LGP 2011-09-09
 *
 *
 */


/*
 * Description:
 *		<<<<OBSOLETE/WRONG - FROM OLD STROIKA SHARED CLASS - JUSTPLACE HOLDER DOC TIL WE CAN WRITE GOOD STUFF>>>>>
 *
 *             This class is for keeping track of a data structure with reference counts,
 *     and disposing of that structure when the reference count drops to zero.
 *     Copying one of these Shared<T> just increments the referce count,
 *     and destroying/overwriting one decrements it.
 *
 *             You can have a ptr having a nullptr value, and it can be copied.
 *     (Implementation detail - the reference count itself is NEVER nil except upon
 *     failure of alloction of memory in ctor and then only valid op on class is
 *     destruction). You can access the value with GetPointer () but this is not
 *     advised - only if it may be legitimately nullptr do you want to do this.
 *     Generaly just use ptr-> to access the data, and this will do the
 *     RequireNotNull (POINTER) for you.
 *
 *             This class can be enourmously useful in implementing letter/envelope -
 *     type data structures - see String, or Shapes, for examples.
 *
 *
 * TODO:
 *
 *
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {


			// An OPTIONAL class you can mix into 'T', and use with SharedPtr<>. If the 'T' used in SharedPtr<T> inherits
			// from this, then you can re-constitute a SharedPtr<T> from it's T* (since the count is pulled along-side).
			// This is sometimes handy if you wish to take a SharedPtr<> object, and pass the underlying pointer through
			// a layer of code, and then re-constitute the SharedPtr<> part later.
			struct	SharedPtrBase {
				// we really want to treat this fCount_ as PRIVATE!!! DONT ACCESS IN SUBCLASSES - but it needs
				// access from the SharedPtr<> template
				private:
				public:
					size_t	fCount_DONT_ACCESS;

				public:
					SharedPtrBase ();
					virtual ~SharedPtrBase ();

				public:
					// called to delete the 'SharedPtrBase'. But - if this gets mixed into another object, just override
					// to ignore (cuz the actual object will get deleted too)
					virtual	void	DO_DELETE_REF_CNT () = 0;
			};


			/*
			 */
			template	<typename	T>
				struct	SharedPtr_Default_Traits {
					typedef	T	TTYPE;
				};



			/*
			@CLASS:			SharedPtr<T,T_TRAITS>
			@DESCRIPTION:	<p>SharedPtr<T> is a simple utility class - very much akin to the C++11 class
				std::shared_ptr<T>. SharedPtr<T> contains the following basic differences:

					<li>There is no std::weak_ptr - or at least if there is - we must document it clearly how/why via extra sharedPTR tmeplate arg(to be worked out)</li>
					<li>There is an extra template T_TRAITS that allows for solving special problems that come up with shared_ptr<> - namely recovering the
						'shared' version of 'T' when only given a plain copy of 'T'
					</li>

					Otherwise, the intention is that they should operate very similarly, and SharedPtr<T> should work with most classes that expect shared_ptr<T> (so long
				as they are templated, and not looking for the particular type name 'shared_ptr').

			
			(much more to discuss - details of T_TRAITs and the WHY for these differences)
			
			
			OLD DOCS:
			<p>A very simple reference counted pointer implementation. Alas - there doesn't seem to
				be anything in STL which provides this functionality! std::auto_ptr is closest, but no cigar.
				For one thing, std::auto_ptr doesn't work with std::vector.</p>
					<p>This implementation would be slightly more efficient, and slightly less flexible, if we assumed
				T had a base-class which contained the reference count. Right now, places where I'm using this don't warrant
				the worry about efficiency.</p>
					<p>NB: - we do allow for a refCntPtr to be nullptr. But any call to GetRep() assert its non-null. Check with call
				to IsNull() first if you aren't sure.</p>
					<P>Since this class is strictly more powerful than the STL template auto_ptr<>, but many people may already be
				using auto_ptr<>, I've provided some mimicing routines to make the transition easier if you want to make some of your
				auto_ptr's be SharedPtr<T>.</p>
					<ul>
						<li>@'SharedPtr<T>::get'</li>
						<li>@'SharedPtr<T>::release'</li>
						<li>@'SharedPtr<T>::reset'</li>
					</ul>
			*/
			template	<typename	T, typename T_TRAITS = SharedPtr_Default_Traits<T>>	class	SharedPtr {
				public:
					typedef	T_TRAITS	TRAITS;
				public:
					SharedPtr ();
					explicit SharedPtr (T* from);
					enum UsesSharedPtrBase { eUsesSharedPtrBase };
					explicit SharedPtr (UsesSharedPtrBase, T* from);
					explicit SharedPtr (T* from, SharedPtrBase* useCounter);
					SharedPtr (const SharedPtr<T,T_TRAITS>& from);

//TODO: UNCLEAR how to handle T2TRAITS. This probably isnt safe (unless we are very careful) going across TRAITSTYPES
					template <typename T2>
					/*
					@METHOD:		SharedPtr::SharedPtr
					@DESCRIPTION:	<p>This CTOR is meant to allow for the semantics of assigning a sub-type pointer to a pointer
								to the base class. There isn't any way to express this in template requirements, but this template
								will fail to compile (error assigning to its fPtr_ member in the CTOR) if its ever used to
								assign inappropriate pointer combinations.</p>
					*/
						SharedPtr (const SharedPtr<T2>& from)
							: fPtr_ (from.get ())
							, fCountHolder_ (from._PEEK_CNT_PTR_ ())
							{
								if (fPtr_ != nullptr) {
									RequireNotNull (fCountHolder_);
									Execution::AtomicIncrement (&fCountHolder_->fCount_DONT_ACCESS);
								}
							}

				public:
					nonvirtual		SharedPtr<T,T_TRAITS>& operator= (const SharedPtr<T,T_TRAITS>& rhs);
				public:
					~SharedPtr ();

				public:
					nonvirtual	bool		IsNull () const;
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::GetRep
					@DESCRIPTION:	<p>Requires that the pointer is non-nullptr.</p>
					*/
					nonvirtual	T&			GetRep () const;

				public:
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::operator->
					@DESCRIPTION:	<p>Note - this CAN NOT return nullptr (because -> semantics are typically invalid for a logically null pointer)</p>
					*/
					nonvirtual	T* operator-> () const;
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::operator*
					@DESCRIPTION:	<p></p>
					*/
					nonvirtual	T& operator* () const;
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::operator T*
					@DESCRIPTION:	<p>Note - this CAN return nullptr</p>
					*/
					nonvirtual	operator T* () const;

				public:
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::get
					@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Just return the pointed to object, with no
								asserts about it being non-null.</p>
					*/
					nonvirtual	T*		get () const;
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::release
					@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer nullptr, but first return the
								pre-existing pointer value. Note - if there were more than one references to the underlying object, its not destroyed.
								<br>
								NO - Changed API to NOT return old pointer, since COULD have been destroyed, and leads to buggy coding.
								If you want the pointer before release, explicitly call get () first!!!
								</p>
					*/
					nonvirtual	void	release ();
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::clear
					@DESCRIPTION:	<p>Synonymn for SharedPtr<T,T_TRAITS>::release ()
								</p>
					*/
					nonvirtual	void	clear ();
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::reset
					@DESCRIPTION:	<p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer 'p', but first return the
								pre-existing pointer value. Unreference any previous value. Note - if there were more than one references to the underlying object, its not destroyed.</p>
					*/
					nonvirtual	void	reset (T* p = nullptr);

				public:
					template <typename T2>
					/*
					@METHOD:		SharedPtr::Dynamic_Cast
					@DESCRIPTION:	<p>Similar to SharedPtr<T2> () CTOR - which does base type. NB couldn't call this dynamic_cast - thats a reserved word.</p>
					*/
						SharedPtr<T2> Dynamic_Cast ()
							{
								return SharedPtr<T2> (dynamic_cast<T2*> (get ()), _PEEK_CNT_PTR_ ());
							}

				private:
					T*				fPtr_;
					SharedPtrBase*	fCountHolder_;

				public:
					// Returns true iff reference count of owned pointer is 1 (false if 0 or > 1)
					nonvirtual	bool	IsUnique () const;
					// Alias for IsUnique()
					nonvirtual	bool	unique () const;
					/*
					@METHOD:		SharedPtr<T,T_TRAITS>::CurrentRefCount
					@DESCRIPTION:	<p>I used to keep this available only for debugging, but I've found a few cases where its handy outside the debugging context
					so not its awlays available (it has no cost to keep available).</p>
					*/
					nonvirtual	size_t	CurrentRefCount () const;

				public:
					nonvirtual	bool	operator< (const SharedPtr<T,T_TRAITS>& rhs) const;
					nonvirtual	bool	operator<= (const SharedPtr<T,T_TRAITS>& rhs) const;
					nonvirtual	bool	operator> (const SharedPtr<T,T_TRAITS>& rhs) const;
					nonvirtual	bool	operator>= (const SharedPtr<T,T_TRAITS>& rhs) const;
					nonvirtual	bool	operator== (const SharedPtr<T,T_TRAITS>& rhs) const;
					nonvirtual	bool	operator!= (const SharedPtr<T,T_TRAITS>& rhs) const;

				public:
					nonvirtual	SharedPtrBase*		_PEEK_CNT_PTR_ () const;
			};

		}


		namespace	Execution {
			template	<typename	T>
				void	ThrowIfNull (const Memory::SharedPtr<T>& p);
			template	<typename	T, typename T_TRAITS>
				void	ThrowIfNull (const Memory::SharedPtr<T, T_TRAITS>& p);
		}

	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtr_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SharedPtr.inl"
