/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	__LineBasedPartition_h__
#define	__LineBasedPartition_h__	1

/*
@MODULE:	LineBasedPartition
@DESCRIPTION:
		<p>A LineBasedPartition is a concrete @'Partition' subclass in which the character '\n' is used to break
	up the text into peices (partition elements). This is the most basic, default, standard kind of Partition.</p>
 */


#include	"Led_PartitioningTextImager.h"


namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {

/*
@CLASS:			LineBasedPartition
@BASES:			virtual @'Partition'
@DESCRIPTION:	<p>Partition which implements the partitioning based on NL characters.</p>
				<p>As of Led 3.0, it is the only concrete partition supported, and its the overwhelmingly
			most likely one you would want to use. But - in some applications (notably LogoVista Translator Assistant) -
			it can be quite handy being able to override how this partitioning takes place.</p>
*/
class	LineBasedPartition : public Partition {
	private:
		typedef	Partition	inherited;
	public:
		LineBasedPartition (TextStore& textStore);
	protected:
		enum SpecialHackToDisableInit { eSpecialHackToDisableInit };
		LineBasedPartition (TextStore& textStore, SpecialHackToDisableInit hack);
		virtual		void	FinalConstruct () override;

	public:
		virtual		void	UpdatePartitions (PartitionMarker* pm, const UpdateInfo& updateInfo) throw () override;

	protected:
		virtual	void	CheckForSplits (PartitionMarker* pm, const UpdateInfo& updateInfo, size_t i) throw () override;
		virtual	bool	NeedToCoalesce (PartitionMarker* pm) throw () override;

#if		qDebug
	protected:
		virtual		void	Invariant_ () const override;
#endif
};





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



		}
	}
}


#endif	/*__LineBasedPartition_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
