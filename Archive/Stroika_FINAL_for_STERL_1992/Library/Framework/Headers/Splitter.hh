/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Splitter__
#define	__Splitter__

/*
 * $Header: /fuji/lewis/RCS/Splitter.hh,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *		Support for grouping a bunch of views together, with splitters seperating them, and
 * allowing the user to resize and alot the space available among the given views. Views which
 * are invisible do not participate in layout, and when the become visible, they are automnatically
 * layed out.
 *
 * 		There is support for refining the behavior of splitting by subclassing either the Splitter View,
 * or more likely, the AbstractSplitView. There is also, convieniecne support to just add any old
 * view to a SplitterView, and have default behavior when splitting, and closing of panes occurrs.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Splitter.hh,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *		Revision 1.3  1992/07/03  00:13:32  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  07:01:13  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_ (+more?)/
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/04/30  14:46:28  sterling
 *		Initial revision
 *		
 */
 
#include	"Sequence.hh"

#include	"View.hh"




 
class	AbstractSplitViewInfo {
	public:
		AbstractSplitViewInfo ();
		virtual	~AbstractSplitViewInfo ();
		
		nonvirtual	Point	GetMinSize () const;
		nonvirtual	void	SetMinSize (const Point& size);
		
		nonvirtual	Point	GetMaxSize () const;
		nonvirtual	void	SetMaxSize (const Point& size);

		nonvirtual	View&	GetView () const;
	
		nonvirtual	void	Invariant () const
		{
#if 	qDebug		
			Invariant_ ();
#endif
		}
		
	protected:
		virtual	Point	GetMinSize_ () const				 			= Nil;
		virtual	void	SetMinSize_ (const Point& size)		 			= Nil;
		
		virtual	Point	GetMaxSize_ () const	 			 			= Nil;
		virtual	void	SetMaxSize_ (const Point& size)	 	 			= Nil;
		
		virtual	View&	GetView_ () const								= Nil;

#if 	qDebug		
		virtual		void	Invariant_ () const;
#endif
};




class	SplitViewInfo : public AbstractSplitViewInfo {
	public:
		SplitViewInfo (View& view);
	
	protected:	
		override	Point	GetMinSize_ () const;
		override	void	SetMinSize_ (const Point& size);
		
		override	Point	GetMaxSize_ () const;
		override	void	SetMaxSize_ (const Point& size);
		
		override	View&	GetView_ () const;
		
	private:
		View*	fView;
		Point	fMinSize;
		Point	fMaxSize;
};


#if		!qRealTemplatesAvailable
	#if 	qMPW_MacroOverflowProblem
	#define	PrivateSplitViewInfoPtr	PSptVInfoPtr
	#endif
	
	typedef	class	PrivateSplitViewInfo*	PrivateSplitViewInfoPtr;
	Declare (Iterator, PrivateSplitViewInfoPtr);
	Declare (Collection, PrivateSplitViewInfoPtr);
	Declare (AbSequence, PrivateSplitViewInfoPtr);
	Declare (Array, PrivateSplitViewInfoPtr);
	Declare (Sequence_Array, PrivateSplitViewInfoPtr);
	Declare (Sequence, PrivateSplitViewInfoPtr);
#endif


class	AbstractSplitter : public View {
	public:
		AbstractSplitter (Point::Direction orientation);
		
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState);
		
		nonvirtual	Point::Direction	GetOrientation () const;
		nonvirtual	void				SetOrientation (Point::Direction orientation, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	void	AddSplitView (const SplitViewInfo& info, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	AddSplitView (View& view, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	RemoveSplitView (const SplitViewInfo& info, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	RemoveSplitView (View& view, UpdateMode update = eDelayedUpdate);
	
		nonvirtual	Coordinate	GetGapHeight () const;
		nonvirtual	void		SetGapHeight (Coordinate gapHeight, UpdateMode update = eDelayedUpdate);
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		override	void	Draw (const Region& update);
		
		nonvirtual	Real	CalcProportion (Coordinate size);
		
		virtual		void	DrawGap (const Rect& where) = Nil;
		
		virtual		void	ResizeSplitter (const MousePressInfo& mouseInfo, CollectionSize index);
		nonvirtual	Rect	CalcGapRect (const View& view) const;
		
	private:
		Point::Direction									fOrientation;
		Coordinate											fGapHeight;
		Sequence(PrivateSplitViewInfoPtr)	fSplitViews;
};



class	Splitter_MacUI : public AbstractSplitter {
	public:
		Splitter_MacUI (Point::Direction orientation);
	protected:
		override	void	DrawGap (const Rect& where);
};



class	Splitter_MotifUI : public AbstractSplitter {
	public:
		Splitter_MotifUI (Point::Direction orientation);
		
		nonvirtual	Coordinate	GetSashLength () const;
		nonvirtual	void		SetSashLength (Coordinate sashLength, UpdateMode update = eDelayedUpdate);
		
	protected:
		override	void	DrawGap (const Rect& where);
	
	private:
		Coordinate	fSashLength;
};


class	Splitter_WinUI : public AbstractSplitter {
	public:
		Splitter_WinUI (Point::Direction orientation);
		
	protected:
		override	void	DrawGap (const Rect& where);
};


class	Splitter :
#if qMacUI
	public Splitter_MacUI
#elif qMotifUI
	public	Splitter_MotifUI
#elif qWinUI
	public	Splitter_WinUI
#endif
{
	public:
		Splitter (Point::Direction orientation);
};




#endif /* __Splitter__ */
