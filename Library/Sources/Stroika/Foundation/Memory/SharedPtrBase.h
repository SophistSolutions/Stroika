/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Memory_SharedPtrBase_h_
#define	_Stroika_Foundation_Memory_SharedPtrBase_h_	1

#include	"../StroikaPreComp.h"

#include	"SharedPtr.h"





/*
 *	TODO:
 *			(o)		Cleanup documentation, especially about the purpose/point, and how to use.
 *
 */



/*
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

namespace	Stroika {	
	namespace	Foundation {
		namespace	Memory {



			namespace	Private {
				namespace	SharedPtrBase_Default_Traits_Helpers_ {
					// 32 bits of counter should be enough for any reasonable applicaiton
					typedef	uint32_t	ReferenceCountType_;
				}
				namespace	SharedPtrBase_Default_Traits_Helpers_ {
					template	<typename	T>
						struct	Envelope_;
				}
			}



			// An OPTIONAL class you can mix into 'T', and use with SharedPtr<>. If the 'T' used in SharedPtr<T> inherits
			// from this, then you can re-constitute a SharedPtr<T> from it's T* (since the count is pulled along-side).
			// This is sometimes handy if you wish to take a SharedPtr<> object, and pass the underlying pointer through
			// a layer of code, and then re-constitute the SharedPtr<> part later.
			struct	SharedPtrBase {
				private:
//public://fix to lose this with friends -- LGP 2012-05-11
					Private::SharedPtr_Default_Traits_Helpers_::ReferenceCountType_	fCount_;

				public:
					SharedPtrBase ();
					virtual ~SharedPtrBase ();

			private:
					template	<typename	T>
						friend	struct	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_;
			};



			namespace	Private {
				namespace	SharedPtrBase_Default_Traits_Helpers_ {
					template	<typename	T>
						struct	Envelope_ {
							T*		fPtr;

							Envelope_ (T* ptr, T* ptr2);
							template <typename T2>
								Envelope_ (const Envelope_<T2>& from);

							T*					GetPtr () const;
							void				SetPtr (T* p);
							ReferenceCountType_	CurrentRefCount () const;
							void				Increment ();
							bool				Decrement ();
							SharedPtrBase*		GetCounterPointer () const;
						};
				}
			}



			/*
			 * SharedPtr_SharedPtrBase_Traits is the TRAITS object to use with SharedPtr, and T must already inherit from SharedPtrBase.
			 *
			 *	Example usage:
			 *
			 *		class	VeryFancyObj : SharedPtrBase {
			 *		};
			 *
			 *		typedef	SharedPtr<VeryFancyObj,SharedPtr_SharedPtrBase_TraitsVeryFancyObj>>	VeryFancySmartPointer;
			 *
			 *		THEN - VeryFancySmartPointer will work like a regular smart pointer - EXCEPT THAT IN ADDITION, you can ALWAYS safely create
			 *		a VeryFancySmartPointer from an already existing VeryFancyObj - just by wrapping/construciting the smart pointer (because the reference count is
			 *		already in the base wrapped data type).
			 */
			template	<typename	T>
				struct	SharedPtr_SharedPtrBase_Traits {
					typedef	Private::SharedPtrBase_Default_Traits_Helpers_::ReferenceCountType_		ReferenceCountType;
					typedef	SharedPtrBase															ReferenceCounterContainerType;
					typedef	Private::SharedPtrBase_Default_Traits_Helpers_::Envelope_<T>			Envelope;
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
