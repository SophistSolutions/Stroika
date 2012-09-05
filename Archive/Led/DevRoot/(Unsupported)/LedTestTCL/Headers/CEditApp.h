/*****
 * CEditApp.h
 *
 *	Application class for a tiny editor.
 *
 *****/

#define	_H_CEditApp		/* Include this file only once */
#include "CApplication.h"

struct CEditApp : CApplication {

	/* No instance variables */

	void	IEditApp(void);
	void	SetUpFileParameters(void);
	void	SetUpMenus(void);

	void	CreateDocument(void);
	void	OpenDocument(SFReply *macSFReply);

	void	UpdateMenus();
	void	DoCommand (long theCommand);
};
