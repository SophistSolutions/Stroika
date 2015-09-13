/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__IdleManager_h__
#define	__IdleManager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/IdleManager.h,v 2.6 2004/02/10 03:05:51 lewis Exp $
 */

/*
@MODULE:	IdleManager
@DESCRIPTION:	<p>Note that this implemenation is for SYNCRONOUS, and not THREADED idle time
			processing.
			</p>

 */


/*
 * Changes:
 *	$Log: IdleManager.h,v $
 *	Revision 2.6  2004/02/10 03:05:51  lewis
 *	SPR#1634: Added EnterIdler class to get notified on OnEnterIdle messages. Also - added IdleManager::NonIdleContext class to notify idle-manager of non-idle (eg executing a command) contexts (lots of these usages).
 *	
 *	Revision 2.5  2003/11/05 01:46:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2003/03/28 12:58:34  lewis
 *	SPR#1388: destruction order of IdleManager and OSIdleManagerImpls is arbitrary - since done by static
 *	file scope DTORs. Could work around this other ways (with .h file with ref count based DTOR - as old
 *	days of iostream hack) - but easier to just make IdleManager::SetIdleManagerOSImpl static method and
 *	careful not to recreate IdleManager if setting impl property to NULL
 *	
 *	Revision 2.3  2003/03/24 14:32:48  lewis
 *	Added qBCCStaticVCLDTORLibBug BCC bug workaround
 *	
 *	Revision 2.2  2003/03/21 14:57:23  lewis
 *	SPR#1368 - IdleManager had small memory leak
 *	
 *	Revision 2.1  2003/03/20 15:46:44  lewis
 *	SPR#1360- add new Idler and IdleManager classes
 *	
 *	
 *	
 *
 *
 */
#include	<map>
#include	<vector>

#include	"LedSupport.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			Idler
@DESCRIPTION:	<p>Simple mixin interface you inherit from if you're object would like to be
			ticked (notified periodically) at idle time.
			</p>
*/
class	Idler {
	protected:
		Idler ();

	public:
		virtual	void	SpendIdleTime ();
};



/*
@CLASS:			EnterIdler
@DESCRIPTION:	<p></p>
*/
class	EnterIdler {
	protected:
		EnterIdler ();

	public:
		virtual	void	OnEnterIdle ();
};





/*
@CLASS:			IdleManager
@DESCRIPTION:	<p>
			</p>
*/
class	IdleManager {
	private:
		IdleManager ();

	public:
		static	IdleManager&	Get ();
	private:
		static	IdleManager*	sThe;

	public:
		virtual	void	AddIdler (Idler* idler);
		virtual	void	RemoveIdler (Idler* idler);

	public:
		virtual	void	AddEnterIdler (EnterIdler* enterIdler);
		virtual	void	RemoveEnterIdler (EnterIdler* enterIdler);

	public:
		nonvirtual	bool	GetInIdleMode () const;
		nonvirtual	void	SetInIdleMode (bool inIdleMode);
	private:
		bool	fInIdleMode;

	public:
		static	float	kNeverCallIdler;
		virtual	float	GetIdlerFrequncy (Idler* idler);
		virtual	void	SetIdlerFrequncy (Idler* idler, float idlerFrequency);


	public:
		class	NonIdleContext;
	protected:
		unsigned int	fNonIdleContextCount;
	private:
		friend	class	NonIdleContext;

	public:
		class	IdleManagerOSImpl;
	public:
		static	void	SetIdleManagerOSImpl (IdleManagerOSImpl* impl);
	private:
		IdleManagerOSImpl*	fIdleManagerOSImpl;
	private:
		friend	class	IdleManagerOSImpl;

	private:
		nonvirtual	void	UpdateIdleMgrImplState ();

	protected:
		virtual	void	CallSpendTime ();
		virtual	void	CallEnterIdle ();

	private:
		struct	IdlerInfo {
			IdlerInfo ();
			float	fIdlerFrequency;
			float	fLastCalledAt;
		};
		map<Idler*, IdlerInfo>	fIdlers;
		bool					fNeedMgrIdleCalls;

	private:
		vector<EnterIdler*>	fEnterIdlers;

	public:
		class	Cleanup;
	private:
		friend	class	Cleanup;
};





/*
@CLASS:			IdleManager::NonIdleContext
@DESCRIPTION:	<p></p>
*/
class	IdleManager::NonIdleContext {
	public:
		NonIdleContext ();
		~NonIdleContext ();
};





/*
@CLASS:			IdleManager::IdleManagerOSImpl
@ACCESS:		public
@DESCRIPTION:	<p>
			</p>
*/
class	IdleManager::IdleManagerOSImpl {
	protected:
		IdleManagerOSImpl ();

	public:
		virtual	void	StartSpendTimeCalls ()									=	0;
		virtual	void	TerminateSpendTimeCalls ()								=	0;
		virtual	float	GetSuggestedFrequency () const							=	0;
		virtual	void	SetSuggestedFrequency (float suggestedFrequency) 		=	0;

	protected:
		virtual	void	CallSpendTime ();
};




class	IdleManager::Cleanup {
	public:
		~Cleanup ();
};




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	Idler
	inline	Idler::Idler ()
		{
		}

//	class	EnterIdler
	inline	EnterIdler::EnterIdler ()
		{
		}


//	class	IdleManager::NonIdleContext
	inline	IdleManager::NonIdleContext::NonIdleContext ()
		{
			IdleManager::Get ().SetInIdleMode (false);
			IdleManager::Get ().fNonIdleContextCount ++;
		}
	inline	IdleManager::NonIdleContext::~NonIdleContext ()
		{
			Led_Assert (IdleManager::Get ().fNonIdleContextCount > 0);
			IdleManager::Get ().fNonIdleContextCount --;
		}



//	class	IdleManager
	inline	IdleManager::IdleManager ():
		fInIdleMode (false),
		fNonIdleContextCount (0),
		fIdleManagerOSImpl (NULL),
		fIdlers (),
		fNeedMgrIdleCalls (false),
		fEnterIdlers ()
		{
		}
	inline	IdleManager&	IdleManager::Get ()
		{
			if (sThe == NULL) {
				sThe = new IdleManager ();
			}
			return *sThe;
		}
	inline	bool	IdleManager::GetInIdleMode () const
		{
			return fInIdleMode;
		}
	inline	void	IdleManager::SetInIdleMode (bool inIdleMode)
		{
			bool	effectiveInIdleMode	=	inIdleMode and fNonIdleContextCount == 0;
			bool	enteringIdle	=	not fInIdleMode and effectiveInIdleMode;
			fInIdleMode = effectiveInIdleMode;
			if (enteringIdle) {
				CallEnterIdle ();
			}
		}



//	class	IdleManager::IdleManagerOSImpl
	inline	IdleManager::IdleManagerOSImpl::IdleManagerOSImpl ()
		{
		}



#if		qLedUsesNamespaces
}
#endif



#endif	/*__IdleManager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
