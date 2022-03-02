/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Changeable__
#define	__Changeable__
/*
 * $Header: /fuji/lewis/RCS/Changeable.hh,v 1.4 1992/10/09 17:32:29 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Changeable.hh,v $
 *		Revision 1.4  1992/10/09  17:32:29  lewis
 *		*** empty log message ***
 *
 *
 *		
 *
 */

#include	"Config-Foundation.hh"
#include	"Debug.hh"



/*
 * Maintains a change count, so one can keep tabs on whether a given object
 * has been modified.
 */
 
typedef	Int16	ChangeCount;	// should be scoped within class but need CFront 2.1 for this
class	Changeable {
	protected:
		Changeable ();				// abstract class

	public:
		virtual ~Changeable ();

		nonvirtual	ChangeCount	GetChangeCount () const;
		nonvirtual	void		SetChangeCount (ChangeCount newCount);

		nonvirtual	void		PlusChangeCount ();
		nonvirtual	void		MinusChangeCount ();
		
		nonvirtual	ChangeCount	GetLastSavedCount () const;
		nonvirtual	void		SetLastSavedCount (ChangeCount newCount);
		
		nonvirtual	void		ResetChangeCount ();	// set changecount and lastsaved count back to zero, no longer considered dirty

		nonvirtual	Boolean	GetDirty () const;
		nonvirtual	void	SetDirty ();	// only increments change count if not currently dirty
		nonvirtual	void	SetClean ();	

	protected:
		virtual		void	SetChangeCount_ (ChangeCount newCount);

	private:
		ChangeCount	fChangeCount;
		ChangeCount	fLastSavedCount;
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Changeable__*/
