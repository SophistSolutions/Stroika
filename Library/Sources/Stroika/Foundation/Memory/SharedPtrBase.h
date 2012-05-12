/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedPtrBase_h_
#define	_Stroika_Foundation_Memory_SharedPtrBase_h_	1

#include	"../StroikaPreComp.h"

#include	"SharedPtr.h"



// TODO: MOST OF DOCS IN THIS FILE OBSOLETE - REDO



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


// MOVE SharedPtrBase/SharedPtr_SharedPtrBase_Traits to new file
// SharedPtrBase (dont like name but need a name).
// Document clearly that htis isnt NEEDED BY OR USED BY SharedPtr<> but CAN OPTIONALLY be used to
// get better size (avoid extra pointer) - and better re-constitute semantics

			// An OPTIONAL class you can mix into 'T', and use with SharedPtr<>. If the 'T' used in SharedPtr<T> inherits
			// from this, then you can re-constitute a SharedPtr<T> from it's T* (since the count is pulled along-side).
			// This is sometimes handy if you wish to take a SharedPtr<> object, and pass the underlying pointer through
			// a layer of code, and then re-constitute the SharedPtr<> part later.
			struct	SharedPtrBase {
				private:
public://fix to lose this with friends -- LGP 2012-05-11
					Private::ReferenceCountType	fCount;

				public:
					SharedPtrBase ();
					virtual ~SharedPtrBase ();
			};



			namespace	Private {
				template	<typename	T>
					struct	SharedPtrBase_Envelope_ {
						T*		fPtr;

						SharedPtrBase_Envelope_ (T* ptr, T* ptr2);
						template <typename T2>
							SharedPtrBase_Envelope_ (const SharedPtrBase_Envelope_<T2>& from)
								: fPtr (from.fPtr)
							{
							}
						T*		GetPtr () const;
						void	SetPtr (T* p);
						ReferenceCountType	CurrentRefCount () const;
						void	Increment ();
						bool	Decrement ();
						SharedPtrBase*	GetCounterPointer () const;
					};
			}



			// this is the TRAITS object to use with SharedPtr, and T must already inherit from SharedPtrBase
			template	<typename	T>
				struct	SharedPtr_SharedPtrBase_Traits {
					typedef	Private::ReferenceCountType			ReferenceCountType;
					typedef	SharedPtrBase						ReferenceCountObjectType;
					typedef	T									TTYPE;
					typedef	Private::SharedPtrBase_Envelope_<T>	Envelope;
				};




		}


	}
}
#endif	/*_Stroika_Foundation_Memory_SharedPtrBase_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SharedPtrBase.inl"
