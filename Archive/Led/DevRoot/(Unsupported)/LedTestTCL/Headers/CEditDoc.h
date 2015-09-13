/****
 * CEditDoc.h
 *
 *	Document class for a tiny editor.
 *
 ****/

#define	_H_CEditDoc			/* Include this file only once */
#include <CDocument.h>
#include <CApplication.h>  

struct CEditDoc : CDocument {

									/** Construction/Destruction **/
                                        
    void        IEditDoc(CApplication *aSupervisor, Boolean printable);

	void		NewFile(void);
	void		OpenFile(SFReply *macSFReply);
	void		BuildWindow (Handle theData, StScrpHandle styleInfo, const char* fileName);
	
									/** Filing **/
	Boolean		DoSave(void);
	Boolean		DoSaveAs(SFReply *macSFReply);
};