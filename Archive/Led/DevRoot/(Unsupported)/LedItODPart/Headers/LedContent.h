#ifndef __LedContent_h__
#define __LedContent_h__	1

#include "LedPartDef.h"



#include	"ChunkedArrayTextStore.h"
#include	"StyledTextImager.h"




/*
 *	Was helpful as debugging tool originally. But I can see no reason to continue
 *	supporting this - LGP 951230
 */
#ifndef	qSupportPictureAsNativeContent
	#define	qSupportPictureAsNativeContent	0
#endif





class	LedPart;
class	Embedding;
class	COrderedList;
class	ODStorageUnit;
class	ODFrame;
class	TextStore_;
class	ODDraft;
class	SimpleEmbeddedObjectStyleMarker;


class CloneInfo  {
	public:
		CloneInfo (ODDraftKey key, 
					ODDraft* fromDraft, 
					ODFrame* scopeFrame, 
					ODCloneKind kind
				);

		// Key returned from draft when clone is initiated, it is used when
		// referencing the cloning operation
		ODDraftKey 		fKey; 
		
		// Source of clone			
		ODDraft* 		fFromDraft; 


		// This is the containing scope of the clone operation
		// could be either destination or source.			
		ODFrame* 		fScopeFrame;
		
		// The type of clone operation, specified at time of clone
		// and used by OpenDoc			
		ODCloneKind		fCloneKind;

		// For internalize we replace this region and insert after from, and for
		// externalize, we externalize just this region...
		// if fTo==-1, then means all the way to end of document...
		size_t			fFrom;
		size_t			fTo;
};




class LedContent	{
	public:
		// -- Init --
		LedContent(LedPart* editor);
		~LedContent();
			
	public:
		//LedPart*			GetLedPart() const;
		
		COrderedList*		GetShapeList() const;
		
		// -- Modify Content
		void 		AddEmbedding (Embedding* shape);

		nonvirtual	Led_Array<SimpleEmbeddedObjectStyleMarker*>	CollectAllEmbeddingMarkersInRange (const TextStore_& textStore, size_t from, size_t to);


	public:
		nonvirtual	void	Externalize (ODStorageUnit* storageUnit, CloneInfo* info); 
		nonvirtual	void	ExternalizeNativeContent (ODStorageUnit* storageUnit, CloneInfo* info); 
		nonvirtual	void	ExternalizePlainText (ODStorageUnit* storageUnit, CloneInfo* info); 
		nonvirtual	void	ExternalizeSingleEmbeddedFrame (ODStorageUnit* storageUnit, CloneInfo* cloneInfo, ODFrame* embeddedFrame); 


	public:
		nonvirtual	void		InternalizeOurContent (ODStorageUnit* storageUnit,  CloneInfo* info, size_t insertAt); 
		nonvirtual	void		InternalizeSingleEmbeddedFrame (ODStorageUnit* storageUnit, CloneInfo* info);
		nonvirtual	void  		InternalizeForeignContent (ODStorageUnit* storage, CloneInfo* info);

	public:
		nonvirtual	void 		HandleInternalizeContent (ODStorageUnit* su, CloneInfo* info, size_t insertAt);
		nonvirtual	void 		HandleTranslateContent (ODStorageUnit* storage, CloneInfo* info, ODBoolean embedOrMerge);
	private:
		// should just throw on error, but some trouble with this on CW 1.4a3 - maybe use throw in later version - hack with bool for now...
		nonvirtual	bool		HelpInternalizeOldFormats1to3 (ODStorageUnit* storageUnit, CloneInfo* info, COrderedList* tCollection, size_t insertAt);

	public:
		ODULong Count();


	protected:
		LedPart*		fLedPart;
		COrderedList*	fShapeList;	

	public:
		nonvirtual		TextStore_&										GetTextStore ();
		nonvirtual		StandardStyledTextImager::SharedStyleDatabase&	GetStyleDatabase ();

	protected:
		nonvirtual	void	InternalizeFromStorageUnitReplacingRange_PlainTextFormatAllFocused_ (ODStorageUnit *storageUnit, size_t insertAt);
		nonvirtual	void	InternalizeFromStorageUnitReplacingRange_ExternalFileFormatAllFocused_ (ODStorageUnit *storageUnit, size_t insertAt);
#if		qSupportPictureAsNativeContent
		nonvirtual	void	InternalizeFromStorageUnitReplacingRange_FromPictureAllFocused_ (ODStorageUnit *storageUnit, size_t insertAt);
#endif

	private:
		ChunkedArrayTextStore							fTextStore;
		StandardStyledTextImager::SharedStyleDatabase	fStyleDatabase;
};












/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	CloneInfo::CloneInfo (ODDraftKey key, ODDraft* fromDraft,  ODFrame* scopeFrame, ODCloneKind kind)
	{
		fKey = key;
		fFromDraft  = fromDraft;
		fScopeFrame = scopeFrame;
		fCloneKind = kind;
		fFrom = qLedFirstIndex;
		fTo = kBadIndex;
	}

	inline	TextStore_&										LedContent::GetTextStore ()
		{
			return fTextStore;
		}
	inline	StandardStyledTextImager::SharedStyleDatabase&	LedContent::GetStyleDatabase ()
		{
			return fStyleDatabase;
		}


#endif	/*__LedContent_h__*/


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
