/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	__IdleManager_h__
#define	__IdleManager_h__	1

#include	"../../Foundation/StroikaPreComp.h"

/*
@MODULE:	IdleManager
@DESCRIPTION:	<p>Note that this implemenation is for SYNCRONOUS, and not THREADED idle time
			processing.
			</p>

 */


#include	<map>
#include	<vector>

#include	"Led_Support.h"




namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {




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
			Assert (IdleManager::Get ().fNonIdleContextCount > 0);
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



		}
	}
}



#endif	/*__IdleManager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
