/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/Led_Gtk.cpp,v 2.11 2003/05/15 12:50:29 lewis Exp $
 *
 * Changes:
 *	$Log: Led_Gtk.cpp,v $
 *	Revision 2.11  2003/05/15 12:50:29  lewis
 *	SPR#1487: Add a 'GetEnabled' method to TextInteractor::CommandUpdater and subclasses (to help with SPR)
 *	
 *	Revision 2.10  2003/04/04 14:41:44  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.9  2003/03/28 12:58:36  lewis
 *	SPR#1388: destruction order of IdleManager and OSIdleManagerImpls is arbitrary - since done
 *	by static file scope DTORs. Could work around this other ways (with .h file with ref count
 *	based DTOR - as old days of iostream hack) - but easier to just make IdleManager::SetIdleManagerOSImpl
 *	static method and careful not to recreate IdleManager if setting impl property to NULL
 *	
 *	Revision 2.8  2003/03/21 13:59:40  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to TextInteractor::ProcessSimpleClick
 *	(adding extra arg). Also lots of other small code cleanups
 *	
 *	Revision 2.7  2003/03/20 16:21:52  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2003/03/20 15:53:51  lewis
 *	SPR#1360 - added IdleManagerOSImpl_Gtk GTK support for new Idler classes (still untested)
 *	
 *	Revision 2.5  2002/05/06 21:33:45  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.4  2001/11/27 00:29:51  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.3  2001/08/28 18:43:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  2001/05/22 21:41:41  lewis
 *	implement SPR#0924- Led_Gtk_TmpCmdUpdater. Also a number of small cleanups to g_messsage trace calls
 *	
 *	Revision 2.1  2000/09/05 22:35:49  lewis
 *	new preliminary support for the X-Windows GTK class library (for Gnone)
 *	
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<gtk/gtkmenuitem.h>

#include	"IdleManager.h"

#include	"Led_Gtk.h"


#if		qLedUsesNamespaces
	namespace	Led {
#endif


/*
@CLASS:			IdleManagerOSImpl_Gtk
@BASES:			@'IdleManager::IdleManagerOSImpl'
@ACCESS:		public
@DESCRIPTION:	<p>Implemenation detail of the idle-task management system. This can generally be ignored by Led users.
			</p>
*/
class	IdleManagerOSImpl_Gtk : public IdleManager::IdleManagerOSImpl {
	public:
		IdleManagerOSImpl_Gtk ();
		~IdleManagerOSImpl_Gtk ();

	public:
		override	void	StartSpendTimeCalls ();
		override	void	TerminateSpendTimeCalls ();
		override	float	GetSuggestedFrequency () const;
		override	void	SetSuggestedFrequency (float suggestedFrequency);


		static	gint	Static_OnIdle (gpointer data)
			{
				IdleManagerOSImpl_Gtk*	pThis	=	reinterpret_cast<IdleManagerOSImpl_Gtk*> (data);
				GDK_THREADS_ENTER ();
				pThis->CallSpendTime ();
				GDK_THREADS_LEAVE ();
			}


	private:
		float	fSuggestedFrequency;
		gint	fIdleTimerID;
};



namespace	{
	/*
	 *	Code to automatically install and remove our idle manager.
	 */
	struct	IdleMangerSetter {
		IdleMangerSetter ()
			{
				IdleManager::SetIdleManagerOSImpl (&fIdleManagerOSImpl);
			}
		~IdleMangerSetter ()
			{
				IdleManager::SetIdleManagerOSImpl (NULL);
			}
		IdleManagerOSImpl_Gtk	fIdleManagerOSImpl;
	};
	struct	IdleMangerSetter	sIdleMangerSetter;
}






/*
 ********************************************************************************
 **************************** IdleManagerOSImpl_Gtk ******************************
 ********************************************************************************
 */
IdleManagerOSImpl_Gtk::IdleManagerOSImpl_Gtk ():
	fSuggestedFrequency (0),
	fIdleTimerID (0)
{
}

IdleManagerOSImpl_Gtk::~IdleManagerOSImpl_Gtk ()
{
}

void	IdleManagerOSImpl_Gtk::StartSpendTimeCalls ()
{
	Led_Verify (fIdleTimerID = ::gtk_idle_add (Static_OnIdle, this));
}

void	IdleManagerOSImpl_Gtk::TerminateSpendTimeCalls ()
{
	if (fIdleTimerID != 0) {
		gtk_idle_remove (fIdleTimerID);
	}
}

float	IdleManagerOSImpl_Gtk::GetSuggestedFrequency () const
{
	return fSuggestedFrequency;
}

void	IdleManagerOSImpl_Gtk::SetSuggestedFrequency (float suggestedFrequency)
{
	if (fSuggestedFrequency != suggestedFrequency) {
		fSuggestedFrequency = suggestedFrequency;
	}
}









/*
 ********************************************************************************
 ************************* Led_Gtk_TmpCmdUpdater ********************************
 ********************************************************************************
 */

Led_Gtk_TmpCmdUpdater::Led_Gtk_TmpCmdUpdater (GtkMenuItem* m, Led_Gtk_TmpCmdUpdater::CommandNumber cmdNum):
	fCommand (cmdNum),
	fMenuItem (m)
{
	Led_Assert (GTK_IS_MENU_ITEM (fMenuItem));
}

bool	Led_Gtk_TmpCmdUpdater::GetEnabled () const
{
	return GTK_WIDGET_IS_SENSITIVE (GTK_WIDGET (fMenuItem));
}

void	Led_Gtk_TmpCmdUpdater::SetEnabled (bool enabled)
{
	gtk_widget_set_sensitive (GTK_WIDGET (fMenuItem), enabled);
}

void	Led_Gtk_TmpCmdUpdater::SetChecked (bool checked)
{
	Led_Require (not checked or GTK_IS_CHECK_MENU_ITEM (fMenuItem));
	if (GTK_IS_CHECK_MENU_ITEM (fMenuItem)) {
		/*
		 *	NB: we just set the active field instead of calling gtk_check_menu_item_set_active ()
		 *	cuz for some weird reason - thats all that seems to work. I'm guessing its cuz calling the
		 *	method has other nasty side effects?? This applies to Gtk-1.2 - LGP 2001-05-22
		 */
		GTK_CHECK_MENU_ITEM (fMenuItem)->active = checked;
	}
}

void	Led_Gtk_TmpCmdUpdater::SetText (const Led_SDK_Char* /*text*/)
{
}




#if		qLedUsesNamespaces
	}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


