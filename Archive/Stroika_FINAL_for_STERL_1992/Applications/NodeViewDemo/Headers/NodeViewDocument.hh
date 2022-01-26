/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__NodeViewDocument__
#define	__NodeViewDocument__
/*
 * $Header: /fuji/lewis/RCS/NodeViewDocument.hh,v 1.3 1992/09/01 17:30:27 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: NodeViewDocument.hh,v $
 *		Revision 1.3  1992/09/01  17:30:27  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  92/05/01  23:23:56  23:23:56  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 *
 */

#include "Document.hh"


class NodeViewWindow;
class DemoBranchNode;
class DemoLeafNode;
class	NodeViewDocument : public FileBasedDocument {
	public:
		NodeViewDocument ();
		~NodeViewDocument ();

		override	off_t	EstimatedFileSize ();
		
		nonvirtual	void		DoNewState ();
				
		/* Parameters for Nodes (uniform within our document) */
		nonvirtual	Point	GetSpacing () const;
		nonvirtual	void	SetSpacing (const Point& spacing);

		nonvirtual	Boolean	GetExpandable () const;
		nonvirtual	void	SetExpandable (Boolean expandable); 

		nonvirtual	Point	GetArcLength () const;
		nonvirtual	void	SetArcLength (const Point& arcLength);
	
		nonvirtual	Point::Direction	GetDirection () const;
		nonvirtual	void				SetDirection (Point::Direction direction);
		
		nonvirtual	NodeViewWindow&	GetNodeWindow () const;

	protected:
		override	void		DoRead_ (class istream& from);
		override	void		DoWrite_ (class ostream& to, int tabCount) const;

	private:
		nonvirtual	void		ReadInNodes (class istream& from, DemoBranchNode* parent, Int16 tabCount);
		nonvirtual	Int16		ReadOneNode (class istream& from, Int16 tabCount, String& nodeTitle);
		nonvirtual	void		WriteOutNodes (class ostream& to);
		nonvirtual	void		WriteOneNode (class ostream& to, const DemoLeafNode& node);
		nonvirtual	DemoLeafNode*	BuildNode (const String& nodeString);
		nonvirtual	DemoBranchNode*	BuildGroupNode (DemoBranchNode& parent);
		
		NodeViewWindow*		fNodeWindow;
		Point				fSpacing;
		Point				fArcLength;
		Boolean				fExpandable;
		Point::Direction 	fDirection;
};




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__NodeViewDocument__*/

