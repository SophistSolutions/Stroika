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
 *		+	SERIOUSLY CONSIDER relationship between this class and Shared<>
 *
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
			@CLASS:			SharedPtr<T>
			@DESCRIPTION:	<p>A very simple reference counted pointer implementation. Alas - there doesn't seem to
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
			template	<typename	T>	class	SharedPtr {
				public:
					SharedPtr ();
					explicit SharedPtr (T* from);
					enum UsesSharedPtrBase { eUsesSharedPtrBase };
					explicit SharedPtr (UsesSharedPtrBase, T* from);
					explicit SharedPtr (T* from, SharedPtrBase* useCounter);
					SharedPtr (const SharedPtr<T>& from);

					template <typename T2>
					/*
					@METHOD:		SharedPtr::SharedPtr
					@DESCRIPTION:	<p>This CTOR is meant to allow for the semantics of assigning a sub-type pointer to a pointer
								to the base class. There isn't any way to express this in template requirements, but this template
								will fail to compile (error assigning to its fPtr member in the CTOR) if its ever used to
								assign inappropriate pointer combinations.</p>
					*/
						SharedPtr (const SharedPtr<T2>& from):
							fPtr (from.get ()),
							fCountHolder (from._PEEK_CNT_PTR_ ())
							{
								if (fPtr != nullptr) {
									RequireNotNull (fCountHolder);
									Execution::AtomicIncrement (&fCountHolder->fCount_DONT_ACCESS);
								}
							}

				public:
					nonvirtual		SharedPtr<T>& operator= (const SharedPtr<T>& rhs);
				public:
					~SharedPtr ();

				public:
					nonvirtual	bool		IsNull () const;
					nonvirtual	T&			GetRep () const;

				public:
					nonvirtual	T* operator-> () const;
					nonvirtual	T& operator* () const;
					nonvirtual	operator T* () const;

				public:
					nonvirtual	T*		get () const;
					nonvirtual	void	release ();
					nonvirtual	void	clear ();
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

#if 0
				public:
					#if		!qCompilerAndStdLib_Supports_lambda_default_argument
						inline	T*	DefaultElementCopier_ (const T& t)
							{
								return new T (t);
							}
					#endif

					/*
					 * Assure1Reference () can be called when implementing copy-on-write. A typical use would be to call Assure1Reference () on
					 * all non-const methods of envelope objects which use SharedPtr<> to share a common read-only copy and only clone it when they need write.
					 *
					 * The 'copier' function must make a logical copy (presumably suitable for update) of the given object already pointed to. A generally appropriate
					 * default implementation is provided.
					 */
					#if		qCompilerAndStdLib_Supports_lambda_default_argument
					nonvirtual	void	Assure1Reference (T* (*copier) (const T&) = [](const T& t) { return new T (t); });
					#else
					nonvirtual	void	Assure1Reference (T* (*copier) (const T&) = DefaultElementCopier_);
					#endif

				private:
					nonvirtual	void	BreakReferences_ (T* (*copier) (const T&));
#endif

				protected:
					T*				fPtr;
					SharedPtrBase*	fCountHolder;

				public:
					// Returns true iff reference count of owned pointer is 1 (false if 0 or > 1)
					nonvirtual	bool	IsUnique () const;
					// Alias for IsUnique()
					nonvirtual	bool	unique () const;
					nonvirtual	size_t	CurrentRefCount () const;

				public:
					nonvirtual	bool	operator< (const SharedPtr<T>& rhs) const;
					nonvirtual	bool	operator<= (const SharedPtr<T>& rhs) const;
					nonvirtual	bool	operator> (const SharedPtr<T>& rhs) const;
					nonvirtual	bool	operator>= (const SharedPtr<T>& rhs) const;
					nonvirtual	bool	operator== (const SharedPtr<T>& rhs) const;
					nonvirtual	bool	operator!= (const SharedPtr<T>& rhs) const;

				public:
					nonvirtual	SharedPtrBase*		_PEEK_CNT_PTR_ () const;
			};

		}


		namespace	Execution {
			// Re-declare so we can specialize (real declaration is in Execution/Excpetions.h)
			template	<typename	T>
				void	ThrowIfNull (const Memory::SharedPtr<T>& p);
			template	<typename	T>
				void	ThrowIfNull (const Memory::SharedPtr<T>& p);
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
