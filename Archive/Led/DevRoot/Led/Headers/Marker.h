/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Marker_h__
#define	__Marker_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Marker.h,v 2.64 2003/11/26 19:48:26 lewis Exp $
 */


/*
@MODULE:	Marker
@DESCRIPTION:
		<p>Marker positions are measured in units of @'Led_tChar's - which don't
	necessarily correspond exactly to bytes or characters.</p>
		<p>A TextStore contains an array of character (Led_tChar) cells numbered from 0..n-1,
	where n is the length of the buffer. But we will rarely refer to these,
	and more often refer to Marker positions.</p>
		<p>A Marker is an abstract entity which is used to keep track of portions
	of the text. It maintains "pointers" just before, and just after character (Led_tChar)
	cells.</p>
		<p>It is IMPORTANT not to confuse Marker positions with character (Led_tChar) positions.
	They are closely related, but not identical. Marker positions are
	numbered from 0..n, where n is the length of the buffer. The marker position ALPHA,
	comes just before the character (Led_tChar) position ALPHA. In other words, the character ALPHA
	lies between marker positions ALPHA, and ALPHA+1.
<pre><code>
		  (Marker m)
		 /      |
		0 1 2 3 4 5 6 7....         (marker positions)
		|H|E|L|L|O| |W|O|R|L|D|
		 0 1 2 3 4 5 6 7....        (character positions)
</code></pre>
	</p>
		<p>In this example, Marker m extends from marker positions 0..4, and has a
	length of 4-0=4, and bounds the characters "HELL" (which were at character (Led_tChar)
	positions 0..3).</p>
		<p>Since there is the potential for confusion between charater positions and
	marker positions, we will opt to ALWAYS refer to Marker positions - and NEVER
	refer to character positions. This will make things much clearer (I hope).</p>
		<p>So our insert routines will, for example insert not AT a particular location, but
	just AFTER a particular marker. Similarly for deletes, etc. When you ask for
	text to be copied out of the editor into a 'C' array of characters, you will specify
	the marker position just before the first character (Led_tChar) you want copied out - and so on.</p>
		<p>The main point of markers is to keep track of particular bits of text, and to
	attatch logical attributes to them. Some of these attributes might be visible (e.g.
	Bolding) and some may not (say a hyperlink, or dictionary source marker). But these
	markers are intended to be ATTACHED to particular words (or sequences of bytes) in
	the text, and NOT to positions in the text buffer. Therefore, it is one of the principle
	functions of a marker to adjust its position values so that it can keep track of a
	particular bit of text.</p>
		<p>Here is how we define the behavior of markers and their tracking of text in the
	presence of edit operations. If text is simply changed within a marker, the marker
	doesn't move (though notification methods are called). If text is inserted to
	the left (before) a given marker, its left and right sides are adjusted by the same
	amount to the right. If text is removed from the left (before) the marker, then
	the left and right sides are adjusted the same amount to the left. If the modification -
	addition or removal - takes place to the right (after) the right side of the marker, it
	has no effect. If an insertion or removal takes place INSIDE the bounds of the left
	and right sides of the marker, the left side remains unchanged, and the right side
	is incremented by the size of the insertion or decremented by the size of the removal.
	Note: this specification counts heavily on our definition that the text changes must
	happen before or after marker positions - and cannot happen AT those positions.
	A markers size can never collapse below zero.</p>
		<p>One implication of this definition of marker updating is that once a
	markers size becomes zero, it will never be automatically increased.
	Once they contain no text, the marker can only increase in size through programatic
	intervention. For this reason (at least partly) it is likely that many classes of
	markers will destroy themselves when their size decreses to zero.</p>
		<p>Markers can also be used to do things other than mark particular bits of text. They
	can be used to insert other sorts of graphix into the flow of the text. One particular
	marker subclass might display a picture. Since this marker really has nothing todo with
	any text, it might be a zero-length marker. Even zero length markers CAN end
	up being displayed. They participate in the TextImager layout, and display.
	But, more typically, it would be a marker of length 1 (with a sentinal character in the
	character position marked by that marker), so that cursoring in the editor the image
	is treated as a single item (like a character of text).</p>
		<p>It is for this reason (and others) that we care about the ordering of markers with
	zero length.</p>
 		<p>Markers have a natural ordering. They are ordered by their left hand sides start
	positions. This ordering could be further refined to take into account the
	markers right-hand sides - but that ordering would still not be a well-ordering (we
	can easily built two markers with the same left and right sides).</p>
		<p>Since we care for display purposes about the relative ordering of markers - even
	those with zero length - we require a further constraint on the ordering of markers to
	make them well-ordered. We call this the "marker sub-position order". Each marker within
	the buffer which starts at a particular location has a sub-position index from 0..m-1,
	where m is the number of other markers which start at the same position. This sub-position
	order defines precedence at display time when markers overlap, and compete in the
	TextLayout and imaging process described in the TextImager header file.</p>
 */



/*
 * Changes:
 *	$Log: Marker.h,v $
 *	Revision 2.64  2003/11/26 19:48:26  lewis
 *	rename arg from byte to charcount
 *	
 *	Revision 2.63  2003/11/26 16:18:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.62  2003/05/20 21:26:04  lewis
 *	SPR#1492: fixed the semantics of TempMarker to match its documentation - 3rd arg is end - not length
 *	
 *	Revision 2.61  2003/03/27 15:43:53  lewis
 *	SPR#1384- UpdateInfo now has an extra field fRealContentUpdate and realContentUpdate CTOR arg to keep track of whether or not the update is a REAL data change requiring a DIRTYFLAG setting
 *	
 *	Revision 2.60  2003/03/20 15:01:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.59  2003/01/29 19:33:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2003/01/29 19:15:08  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.57  2003/01/11 19:28:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.56  2003/01/08 15:36:54  lewis
 *	moved TextImager::CopyOut etc to MarkerOwner::CopyOut (same for GetLength/GetEnd/FindNextCharacter/FindPrevCharacter.
 *	Done so they can be somewhat more easily used. No longer inline - but thats not a big deal since not used in
 *	performance critical places (and if they are - you can manually use/cache the TextStore and call its version).
 *	Also - got rid of obsolete TextImager::PeekAtTextStore_ (some old compile BWA)
 *	
 *	Revision 2.55  2002/12/03 16:09:40  lewis
 *	Same as before - private X(X&)/op= - but now for Marker class. Also - had forgotten to do op= on last checkin
 *	
 *	Revision 2.54  2002/12/02 23:46:51  lewis
 *	Add docs to note that MarkerOwner objects cannot be copied by value and added private X(X&)
 *	and op=(X&) to enforce. Avoid confusion one customer had
 *	
 *	Revision 2.53  2002/09/20 00:26:46  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.52  2002/05/06 21:33:31  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.51  2001/11/27 00:29:43  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.50  2001/10/20 13:38:55  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.49  2001/10/17 20:42:51  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.48  2001/09/03 18:54:59  lewis
 *	SPR#1009- Added MarkerOwner::EarlyDidUpdateText () and calls to such from TextStore::DoDidUpdateCalls ()
 *	
 *	Revision 2.47  2001/08/29 23:00:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.46  2001/08/28 18:43:28  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.45  2001/04/26 16:39:28  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.44  2000/09/05 21:24:37  lewis
 *	make DTOR public
 *	
 *	Revision 2.43  2000/08/13 04:04:58  lewis
 *	SPR#0822- call PreRemoveMarker from MarkerMorturary<>
 *	
 *	Revision 2.42  2000/07/26 05:46:35  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg
 *	to CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.41  2000/04/26 13:14:28  lewis
 *	Added TemporaryMarkerSlideDown<MARKER> helper template
 *	
 *	Revision 2.40  2000/04/26 13:09:21  lewis
 *	Revise LessThan<> template to accomodate markers whose start is the same but end-point varies
 *	
 *	Revision 2.39  2000/04/24 21:10:45  lewis
 *	Added TempMarker utility class
 *	
 *	Revision 2.38  2000/04/15 14:32:34  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.37  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.36  2000/03/31 17:01:12  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2000/03/17 22:31:31  lewis
 *	use sort () directly on LessThan<MARKER> - rather than trivial 'SortMarker' method.
 *	And moved LessThan<MARKER> to Marker.h from MarkerCover<>
 *	
 *	Revision 2.34  1999/12/15 01:04:35  lewis
 *	Add MarkerOwner::UpdateInfo::GetResultingRHS () helper
 *	
 *	Revision 2.33  1999/11/13 16:32:18  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.32  1999/07/13 22:39:11  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds
 *	that were only needed for these old compilers
 *	
 *	Revision 2.31  1999/06/15 01:57:44  lewis
 *	cosmetic cleanups
 *	
 *	Revision 2.30  1999/05/03 22:04:55  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.29  1999/05/03 21:41:22  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.28  1998/10/30 14:13:00  lewis
 *	Use vector<> instead of Led_Array
 *	
 *	Revision 2.27  1998/06/03  01:29:58  lewis
 *	Add assert in MarkerMortuary that they call come from the same TextStore.
 *
 *	Revision 2.26  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.25  1998/04/25  01:21:44  lewis
 *	*** empty log message ***
 *
 *	Revision 2.24  1998/04/09  01:25:55  lewis
 *	New MarkerMortuary<> template.
 *
 *	Revision 2.23  1998/04/08  01:12:06  lewis
 *	Added GetTextStore() wrapper which asserts and calls PeekAtTextStore ().
 *
 *	Revision 2.22  1998/03/04  20:18:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.21  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/09/29  14:33:00  lewis
 *	Lose qSupportLed21CompatAPI.
 *
 *	Revision 2.19  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.18  1997/07/23  23:03:51  lewis
 *	Doc changes
 *
 *	Revision 2.17  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.16  1997/07/12  20:01:11  lewis
 *	AutoDoc support.
 *	TextStore_ renamed TextStore_
 *
 *	Revision 2.15  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1997/06/18  02:48:05  lewis
 *	MAJOR cleanup of Contains/Overlap code.
 *	Overlap moved to static method of TextStore_.
 *	Only a few overlaps of Contains () remain.
 *	Most of the rest ifdefed qSupportLed21CompatAPI.
 *
 *	Revision 2.13  1997/03/04  20:05:00  lewis
 *	Added more Contains() overloads
 *
 *	Revision 2.12  1997/01/10  02:54:05  lewis
 *	DidUpdateText() takes throw specifier, and AboutToUpdateText () now returns VOID.
 *	Instead of returing bool, do a THROW out of the update code.
 *
 *	Revision 2.11  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.10  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.8  1996/06/01  02:04:15  lewis
 *	Lose qNoRTTISupportBug
 *
 *	Revision 2.7  1996/05/23  19:22:14  lewis
 *	Use GetRange() instead of GetStart/GetEnd() in a few places
 *	 to reduce virtual function calls.
 *
 *	Revision 2.6  1996/02/26  18:38:41  lewis
 *	Use Led_Min/Led_Max instead of Min/Max.
 *
 *	Revision 2.5  1995/12/06  01:28:36  lewis
 *	Add MarkerOwner_::PeekAtTextStore() method, so in DoUpdate calls, we
 *	have access to our owning text store (needed - for example - to remove
 *	ourselves).
 *
 *	Revision 2.4  1995/11/25  00:18:54  lewis
 *	Added IsA_EmbeddedObjectStyleMarker() method for compilers with no rtti
 *
 *	Revision 2.3  1995/10/19  22:05:27  lewis
 *	UpdateInfo now takes boolean flag about if text is really being modified,
 *	or if update consists of setting non-text associated data (eg. markers,
 *	like font markers).
 *
 *	Revision 2.2  1995/10/09  22:18:42  lewis
 *	Moved UpdateInfo into MarkerOwner class.
 *	Allow Marker::GetOwner() call with NULL textstorehook - just means no owner
 *	yet.
 *	added AboutToUpdateText/didupdate to MarkerOwner before and after
 *	all calls for markers themselves.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.8  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.7  1995/05/29  23:38:38  lewis
 *	Marker now concrete classs - SPR 0295.
 *
 *	Revision 1.6  1995/05/18  08:06:40  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *
 *	Revision 1.5  1995/05/06  19:24:35  lewis
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *
 *	Revision 1.4  1995/04/20  06:41:30  lewis
 *	Cleanups
 *
 *	Revision 1.3  1995/03/17  02:59:19  lewis
 *	Got rid of IsA_HackMarker() method - now taken care of completly
 *	within the source of ChunkedArray.cpp.
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *
 */

#include	"LedSupport.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif



class	Marker;
class	TextStore;


/*
@CLASS:			MarkerOwner
@DESCRIPTION:
	 <p>Object which owns Markers. You register this with a @'TextStore'. And then when those markers are changed
	 (contents within the marker), you are updated via the  @'MarkerOwner::AboutToUpdateText' and @'MarkerOwner::DidUpdateText'.
	 methods.</p>
		<p>Note that these objects should not be copied with X(X&) CTORs etc and are generally handled by pointer - because pointers
	to particular instances are stored in TextStore objects</p>
*/
class	MarkerOwner {
	protected:
		MarkerOwner ();
	public:
		virtual ~MarkerOwner ();

	// Methods private and not actually implemented. Just declared to prevent users from accidentally copying instances of this class. Not intended to be
	// used that way.
	private:
		MarkerOwner (const MarkerOwner&);
		const MarkerOwner& operator= (const MarkerOwner&);

	public:
		class	UpdateInfo;

	public:
		virtual	void	AboutToUpdateText (const UpdateInfo& updateInfo);
		virtual	void	EarlyDidUpdateText (const UpdateInfo& updateInfo) throw ();
		virtual	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	public:
		/*
		@METHOD:		MarkerOwner::PeekAtTextStore
		@DESCRIPTION:	<p>Returns the currently associated TextStore for this @'MarkerOwner'.
			This method can return NULL only if owns no markers!</p>
		*/
		virtual	TextStore*	PeekAtTextStore () const	=	0;

	public:
		nonvirtual	TextStore&	GetTextStore () const;


	/*
	 *	Trivial wrappers on owned TextStore
	 */
	public:
		nonvirtual	size_t	FindNextCharacter (size_t afterPos) const;
		nonvirtual	size_t	FindPreviousCharacter (size_t beforePos) const;
		nonvirtual	size_t	GetLength () const;
		nonvirtual	size_t	GetEnd () const;
		nonvirtual	void	CopyOut (size_t from, size_t count, Led_tChar* buffer) const;
	#if		qMultiByteCharacters
	public:
		nonvirtual	void	Assert_CharPosDoesNotSplitCharacter (size_t charPos) const;
	#endif


	// this field is managed by the TextStore subclass which
	// keeps track of these markers. NO-ONE ELSE SHOULD TOUCH!!!
	// The only reason this is public is cuz it can be used by
	// ANY subclass of TextStore which implements the marker store.
	public:
		class	HookData;
		HookData*	fTextStoreHook;
};




/*
@CLASS:			MarkerOwner::HookData
@DESCRIPTION:	<p>An implementation detail of implementing a @'TextStore'. This class is used
	as an abstract interface for the data that must be kept track of by a @'TextStore' about
	a @'Marker' when it is added to that @'TextStore'.</p>
		<p>This class should only be of interest to those implementing a new @'TextStore' subclass.</p>
*/
class	MarkerOwner::HookData {
	protected:
		HookData ();
	public:
		virtual ~HookData ();
};





/*
@CLASS:			MarkerOwner::UpdateInfo
@DESCRIPTION:	<p>A packaging up of information about an update, for @'MarkerOwner::AboutToUpdateText' or
	@'MarkerOwner::DidUpdateText' methods.</p>
*/
class	MarkerOwner::UpdateInfo {
    public:
    	UpdateInfo (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, bool textModified, bool realContentUpdate);

		size_t				fReplaceFrom;
		size_t				fReplaceTo;
		const Led_tChar*	fTextInserted;				// text that was/is to be inserted
		size_t				fTextLength;
		bool				fTextModified;				// if true, fTextInserted contains any inserted text.
														// if false, may have just been a font/style or some such change
		bool				fRealContentUpdate;			// true if change is an action which needs to be 'saved'
														//		and should be interpretted as 'dirtying' the document.

	public:
		nonvirtual	size_t	GetResultingRHS () const;
};






/*
@CLASS:			Marker
@DESCRIPTION:	<p>A basic building-block of Led. This class marks a region of text, and sticks to that text, even as
			text before or after is updated. Also, Markers can be notified when any attempted update happens
			within their bounds.</p>
				<p>Note that these objects should not be copied with X(X&) CTORs etc and are generally handled by pointer - because pointers
			to particular instances are stored in TextStore objects</p>
*/
class    Marker {
    public:
		Marker ();
		virtual ~Marker ();

	// Methods private and not actually implemented. Just declared to prevent users from accidentally copying instances of this class. Not intended to be
	// used that way.
	private:
		Marker (const Marker&);
		const Marker& operator= (const Marker&);


	// These methods are only legal to call when the marker has been added to a marker owner...
	public:
		nonvirtual	size_t			GetStart () const;
		nonvirtual	size_t			GetEnd () const;
		nonvirtual	size_t			GetLength () const;
		nonvirtual	MarkerOwner*	GetOwner () const;
		nonvirtual	void			GetRange (size_t* start, size_t* end) const;


	/*
	 *	Considered having one notification method since this would be more efficient - but
	 *	using TWO like this is necessary to allow for cooperative transation processing.
	 *	Need to be able to say NO to this transation - and then need to be notified and
	 *	update our internals if it goes through.
	 *
	 *	So AboutToUpdateText () may or may not be balanced with a call to DidUpdateText (),
	 *	but DidUpdateText () is ALWAYS proceeded by a call to AboutToUpdateText () (for this
	 *	update).
	 *
	 *	Another interesting note on updates. The question is - what sorts of modifications
	 *	produce update calls. I had considered having markers notified when text was changed
	 *	INSIDE of them. This is clearly needed. But it is also - very often - quite convenient
	 *	to be notified when text is changed one character to the left or right of our boundaries.
	 *	For example - when we use markers to define boundaries of words - and someone types
	 *	a non-space character just before or just after the word - we might want to re-consider
	 *	whether we still have a valid word marker.
	 */
    public:
    	typedef	MarkerOwner::UpdateInfo	UpdateInfo;
		virtual	void	AboutToUpdateText (const UpdateInfo& updateInfo);	// throw to avoid actual update
		virtual	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();


	// this field is managed by the TextStore subclass which
	// keeps track of these markers. NO-ONE ELSE SHOULD TOUCH!!!
	// The only reason this is public is cuz it can be used by
	// ANY subclass of TextStore which implements the marker store.
	public:
		class	HookData;
		HookData*	fTextStoreHook;
};


/*
@CLASS:			Marker::HookData
@DESCRIPTION:	<p>An implementation detail of implementing a @'TextStore'. This class is used
	as an abstract interface for the data that must be kept track of by a @'TextStore' about
	a @'Marker' when it is added to that @'TextStore'.</p>
		<p>This class should only be of interest to those implementing a new @'TextStore' subclass.</p>
*/
class	Marker::HookData {
	protected:
		HookData ();
	public:
		virtual ~HookData ();

	public:
		virtual	MarkerOwner*	GetOwner () const								=	0;
		virtual	size_t			GetStart () const								=	0;
		virtual	size_t			GetEnd () const									=	0;
		virtual	size_t			GetLength () const								=	0;
		virtual	void			GetStartEnd (size_t* start, size_t* end) const	=	0;
};





/*
@CLASS:			MarkerMortuary<MARKER>
@BASES:			
@DESCRIPTION:	<p>MarkerMortuary is a template used to help delete markers from a TextStore, in a situation where
	the markers might still be refered to someplace.</p>
		<p>A typical use would be to accomulate them for deletion - marking them as uninteresting. Then calling
	FinalizeMarkerDeletions () when there are certain to be no more outstanding pointers.</p>
		<p>NB: We require that the markers added to a MarkerMortuary all share a common TextStore.</p>
*/
template	<typename	MARKER>	class	MarkerMortuary {
	public:
		MarkerMortuary ();
		~MarkerMortuary ();
	public:
		nonvirtual	void	AccumulateMarkerForDeletion (MARKER* m);
		nonvirtual	void	SafeAccumulateMarkerForDeletion (MARKER* m);
		nonvirtual	void	FinalizeMarkerDeletions () throw ();
		nonvirtual	bool	IsEmpty () const throw ();
	private:
		vector<MARKER*>	fMarkersToBeDeleted;
};


bool	Contains (const Marker& containedMarker, const Marker& containerMarker);
bool	Contains (size_t containedMarkerStart, size_t containedMarkerEnd, const Marker& containerMarker);
bool	Contains (const Marker& marker, size_t charPos);
bool	Contains (size_t containedMarkerStart, size_t containedMarkerEnd, size_t charPos);








/*
@CLASS:			LessThan<MARKER>
@BASES:			
@DESCRIPTION:	
	<p>Use this class when you have a vector of some marker subclass and want to sort it.</p>
	<p>As in:
	</p>
	<code><pre>
	vector&lt;MyMarker*&gt; markers = get_em ();
	sort (markers.begin (), markers.end (), LessThan&lt;MyMarker&gt; ())
	</pre></code>
		<p>Note that as of Led 3.0d6, this allows for where two markers have
	the same start, and then measures less
	according to where they end.</p>
*/
template	<typename	MARKER>
	struct	LessThan : public binary_function <MARKER*, MARKER*,bool> {
		bool	operator () (const MARKER* lhs, const MARKER* rhs)
			{
				Led_RequireNotNil (lhs);
				Led_RequireNotNil (rhs);
				int	diff	=	int (lhs->GetStart ()) - int (rhs->GetStart ());
				if (diff == 0) {
					return (lhs->GetEnd () < rhs->GetEnd ());
				}
				else {
					return (diff < 0);
				}
			}
	};






/*
@CLASS:			TempMarker
@BASES:			@'MarkerOwner'
@DESCRIPTION:	<p>A trivial helper class which can be used in a 'stack based' fashion to
			keep temporary track of a region of text, without all the bookkeeping of
			having to add/remove the marker and marker owner, etc.</p>
*/
class	TempMarker : public MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;
	public:
		TempMarker (TextStore& ts, size_t start, size_t end);
		~TempMarker ();

	public:
		nonvirtual	size_t	GetStart () const;
		nonvirtual	size_t	GetEnd () const;
		nonvirtual	size_t	GetLength () const;
		nonvirtual	void	GetLocation (size_t* from, size_t* to) const;

	public:
		override	TextStore*	PeekAtTextStore () const;

	private:
		TextStore&	fTextStore;
		Marker		fMarker;
};






/*
@CLASS:			TemporaryMarkerSlideDown<MARKER>
@DESCRIPTION:	<p>A simple helper class to take a vector of markers, and slide them one way or the other, and then restore them.
			This can occasionally be helpful when you are about to do some operation which you don't want to affect these markers - you can slide
			them out of the way - so they don't get updated - and then slide them back.</p>
*/
template	<typename	MARKER>
	class	TemporaryMarkerSlideDown {
		public:
			TemporaryMarkerSlideDown (TextStore& ts, const vector<MARKER*>& m, ptrdiff_t slideBy = 1);
			~TemporaryMarkerSlideDown ();

		private:
			TextStore&		fTextStore;
			vector<MARKER*>	fMarkers;
			ptrdiff_t		fSlideBy;
	};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	MarkerOwner;
	inline	MarkerOwner::MarkerOwner ():
		fTextStoreHook (NULL)
		{
		}
	inline	MarkerOwner::~MarkerOwner ()
		{
		}
	/*
	@METHOD:		MarkerOwner::GetTextStore
	@DESCRIPTION:	<p>This is a trivial wrapper on @'MarkerOwner::PeekAtTextStore' which asserts that
		PeekAtTextStore () didn't return NULL, and then returns its result dereferences. Use this for
		clarity sake when you are sure the markerowner must have an associated TextStore.</p>
	*/
	inline	TextStore&	MarkerOwner::GetTextStore () const
		{
			TextStore*	ts	=	PeekAtTextStore ();
			Led_EnsureNotNil (ts);
			return *ts;
		}
	/*
	@METHOD:		MarkerOwner::AboutToUpdateText
	@DESCRIPTION:	<p>This method is called by a TextStore when text is being updated for all registered MarkerOwners
		(see @'TextStore::AddMarkerOwner'). You can throw an exception to prevent text from being updated.</p>
	*/
	inline	void	MarkerOwner::AboutToUpdateText (const UpdateInfo& /*updateInfo*/)
		{
		}
	/*
	@METHOD:		MarkerOwner::EarlyDidUpdateText
	@DESCRIPTION:	<p>This method is called by a @'TextStore' <em>just</em>after a text update has taken place (and before the <em>DidUpdateText</em> calls take place).
		You can use this call just to set private data in your objects - but make no calls out (til the DidUpdateText). This is just so that
		in some rare cases - you can tell if your getting a callback between your AboutToUpdate() call and your DidUpdateText () call - so you can
		tell if the text is truely updated yet.</p>
	*/
	inline	void	MarkerOwner::EarlyDidUpdateText (const UpdateInfo& /*updateInfo*/) throw ()
		{
		}
	/*
	@METHOD:		MarkerOwner::DidUpdateText
	@DESCRIPTION:	<p>This method is called by a @'TextStore' after a text update has taken place. You can use this to update
		some appropriate data structures based on the change. NB: an exception <em>cannot</em> be raised by this method, or
		any overrides of it.</p>
	*/
	inline	void	MarkerOwner::DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ()
		{
		}





//	class	Marker;
	inline	Marker::Marker ():
		fTextStoreHook (NULL)
		{
		}
    inline	Marker::~Marker ()
    	{
    	}
	/*
	@METHOD:		Marker::AboutToUpdateText
	@DESCRIPTION:	<p>Notification callback which is called by a @'TextStore' when the text is updated. You override this
		if you want todo something special when the text marked by this particular marker is changed. You can throw
		from this method to prevent the update from actually taking place.</p>
	*/
	inline	void	Marker::AboutToUpdateText (const UpdateInfo& /*updateInfo*/)
		{
		}
	/*
	@METHOD:		Marker::DidUpdateText
	@DESCRIPTION:	<p>Notification callback which is called by a @'TextStore' after the text is updated. You override this
		if you want todo something special when the text marked by this particular marker is changed. You cannot throw
		from this routine. Use @'Marker::AboutToUpdateText' to preflight, and assure any DidUpdateText method calls will
		work without a hitch.</p>
	*/
	inline	void	Marker::DidUpdateText (const UpdateInfo& /*updateInfo*/)	throw ()
		{
		}
	/*
	@METHOD:		Marker::GetStart
	@DESCRIPTION:	<p>Returns the marker position of the start (lhs) of the marker.</p>
			<p>It is illegal to call this
		if the marker is not currently added to a @'TextStore'.</p>
	*/
	inline	size_t	Marker::GetStart () const
		{
			Led_AssertNotNil (fTextStoreHook);
			return (fTextStoreHook->GetStart ());
		}
	/*
	@METHOD:		Marker::GetEnd
	@DESCRIPTION:	<p>Returns the marker position of the end (rhs) of the marker.</p>
			<p>It is illegal to call this
		if the marker is not currently added to a @'TextStore'.</p>
	*/
	inline	size_t	Marker::GetEnd () const
		{
			Led_AssertNotNil (fTextStoreHook);
			return (fTextStoreHook->GetEnd ());
		}
	/*
	@METHOD:		Marker::GetLength
	@DESCRIPTION:	<p>Return the length - in @'Led_tChar's - of the marker span. This value can be zero. But if it
		ever goes to zero, Led will never re-inflate the marker. You must reset its bounds manually via @'Marker::SetRange' ().</p>
	*/
	inline	size_t	Marker::GetLength () const
		{
			Led_AssertNotNil (fTextStoreHook);
			return (fTextStoreHook->GetLength ());
		}
	/*
	@METHOD:		Marker::GetOwner
	@DESCRIPTION:	<p>Return the current marker owner. Unlike most Marker methods, this <em>can</em> be called when the
		marker hasn't yet been added to a @'TextStore'. It just returns NULL in that case. Note, it can return NULL anyhow,
		as that is a valid value to specify in @'TextStore::AddMarker' ().</p>
	*/
	inline	MarkerOwner*	Marker::GetOwner () const
		{
			// fTextStoreHook CAN be NULL here if we don't yet have a marker owner!
			return (fTextStoreHook == NULL? NULL: fTextStoreHook->GetOwner ());
		}
	/*
	@METHOD:		Marker::GetRange
	@DESCRIPTION:	<p>Return the start and end position of the marker. You must set its bounds
		via @'Marker::SetRange' ().</p>
	*/
	inline	void	Marker::GetRange (size_t* start, size_t* end) const
		{
			Led_RequireNotNil (start);
			Led_RequireNotNil (end);
			Led_RequireNotNil (fTextStoreHook);
			fTextStoreHook->GetStartEnd (start, end);
		}




//	class	MarkerOwner::HookData;
	inline	MarkerOwner::HookData::HookData ()
		{
		}
	inline	MarkerOwner::HookData::~HookData ()
		{
		}




//	class	MarkerOwner::UpdateInfo;
	inline	MarkerOwner::UpdateInfo::UpdateInfo (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, bool textModified, bool realContentUpdate):
		fReplaceFrom (from),
		fReplaceTo (to),
		fTextInserted (withWhat),
		fTextLength (withWhatCharCount),
		fTextModified (textModified),
		fRealContentUpdate (realContentUpdate)
		{
		}
	inline	size_t	MarkerOwner::UpdateInfo::GetResultingRHS () const
		{
			return fTextModified? (fReplaceFrom + fTextLength): fReplaceTo;
		}





//	class	Marker::HookData;
	inline	Marker::HookData::HookData ()
		{
		}
	inline	Marker::HookData::~HookData ()
		{
		}








//	class	MarkerMortuary<MARKER>;
	template	<typename	MARKER>
		inline	MarkerMortuary<MARKER>::MarkerMortuary ():
			fMarkersToBeDeleted ()
			{
			}
	template	<typename	MARKER>
		inline	MarkerMortuary<MARKER>::~MarkerMortuary ()
			{
				Led_Assert (fMarkersToBeDeleted.size () == 0);		// these better be deleted by now!
			}
	template	<typename	MARKER>
		/*
		@METHOD:		MarkerMortuary<MARKER>::AccumulateMarkerForDeletion
		@DESCRIPTION:	<p>Since Led can remove large numbers of markers at a time much faster than it can
			remove a single one, we have this special interface to accumulate markers to be
			deleted, and delete them all at once. This doesn't ususally <em>NEED</em> to be used. But it sometimes
			does <em>Need</em> to be used to avoid the case where a marker is kept in a list (say back on the stack in
			some caller for example), and we decide it must be deleted, but we don't know for sure if it
			will be accessed from that stack-based list again.</p>
		 		<p>After a call to this routine, the marker is effectively dead and removed
			from OUR marker list, but it is not 'delete' d. And the call to RemoveMarker</p>
			is postponed til '@'MarkerMortuary<MARKER>::FinalizeMarkerDeletions ()'.
				<p>NB:	It is illegal to accumulate a marker for deletion twice (detected error). And greatly
			discouraged using it afterwards.</p>
		*/
		inline	void	MarkerMortuary<MARKER>::AccumulateMarkerForDeletion (MARKER* m)
			{
				Led_RequireNotNil (m);
				Led_Require (IndexOf (fMarkersToBeDeleted, m) == kBadIndex);
				#if		qDebug
					if (fMarkersToBeDeleted.size () != 0) {
						Led_RequireNotNil (static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ());
						Led_RequireNotNil (static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ()->PeekAtTextStore ());
						Led_RequireNotNil (static_cast<Marker*> (m)->GetOwner ());
						Led_RequireNotNil (static_cast<Marker*> (m)->GetOwner ()->PeekAtTextStore ());
						Led_Require (static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ()->PeekAtTextStore () == static_cast<Marker*> (m)->GetOwner ()->PeekAtTextStore ());
					}
				#endif

				// Added for SPR#0822 - see for details
				static_cast<Marker*> (m)->GetOwner ()->GetTextStore ().PreRemoveMarker (m);

				// NB: fMarkersToBeDeleted SB a linked list, so we don't need todo any mem allocations
				// and don't need to worry about failing to allocate memory here!!!
				fMarkersToBeDeleted.push_back (m);
			}
	template	<typename	MARKER>
		/*
		@METHOD:		MarkerMortuary<MARKER>::SafeAccumulateMarkerForDeletion
		@DESCRIPTION:	<p>Like @'MarkerMortuary<MARKER>::AccumulateMarkerForDeletion', but its OK to add a marker more than once.</p>
		*/
		void	MarkerMortuary<MARKER>::SafeAccumulateMarkerForDeletion (MARKER* m)
			{
				Led_RequireNotNil (m);
				if (IndexOf (fMarkersToBeDeleted, m) == kBadIndex) {
					AccumulateMarkerForDeletion (m);
				}
			}
	template	<typename	MARKER>
		/*
		@METHOD:		MarkerMortuary<MARKER>::FinalizeMarkerDeletions
		@DESCRIPTION:	<p>Call anytime, but preferably after we've finished doing a bunch of marker deletions.
			See @'MarkerMortuary<MARKER>::AccumulateMarkerForDeletion' for more information.</p>
		*/
		void	MarkerMortuary<MARKER>::FinalizeMarkerDeletions () throw ()
			{
				if (fMarkersToBeDeleted.size () != 0) {
					MarkerOwner*	owner		=	static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ();
					Led_AssertNotNil (owner);
					TextStore&		textStore	=	owner->GetTextStore ();

					// NB: No exceptions can happen in any of this - all these deletes allocate no memory (LGP 950415)
					MARKER*const*	markersToBeDeleted_	=	&fMarkersToBeDeleted.front ();
					Marker*const*	markersToBeDeleted	=	(Marker*const*)markersToBeDeleted_;	// need cast - but safe - cuz array of MARKER* and looking for Marker* - safe cuz const array!

					textStore.RemoveMarkers (markersToBeDeleted, fMarkersToBeDeleted.size ());
					for (size_t i = 0; i < fMarkersToBeDeleted.size (); i++) {
						delete fMarkersToBeDeleted[i];
					}
					fMarkersToBeDeleted.clear ();
				}
			}
	template	<typename	MARKER>
		/*
		@METHOD:		MarkerMortuary<MARKER>::IsEmpty
		@DESCRIPTION:	<p>Mostly used for assertions. Checks all accumulated markers have been finalized.</p>
		*/
		inline	bool	MarkerMortuary<MARKER>::IsEmpty () const throw ()
			{
				return fMarkersToBeDeleted.size () == 0;
			}




//	::Contains;
	/*
	@METHOD:		Contains
	@DESCRIPTION:	<p>Four overloads. Basically this tests if the first marker is contained in the second.
				</p>
					<p>Note that Led doesn't treat the beginning and ending of a marker symetrically. It
				is considered legitimate containment if the marker start positions are equal, but <em>not</em>
				if the marker end positions are equal</p>
	*/
	inline	bool	Contains (const Marker& containedMarker, const Marker& containerMarker)
		{
			size_t	containerStart;
			size_t	containerEnd;
			containerMarker.GetRange (&containerStart, &containerEnd);

			size_t	containedStart;
			size_t	containedEnd;
			containedMarker.GetRange (&containedStart, &containedEnd);
			return ( (containedStart >= containerStart) and (containerEnd >= containedEnd) );
		}
	inline	bool	Contains (size_t containedMarkerStart, size_t containedMarkerEnd, const Marker& containerMarker)
		{
			Led_Assert (containedMarkerStart <= containedMarkerEnd);
			size_t	containerStart;
			size_t	containerEnd;
			containerMarker.GetRange (&containerStart, &containerEnd);
			return ( (containedMarkerStart >= containerStart) and (containerEnd >= containedMarkerEnd) );
		}
	inline	bool	Contains (const Marker& marker, size_t charPos)
		{
			size_t	start;
			size_t	end;
			marker.GetRange (&start, &end);
			return (charPos >= start and charPos < end);
		}
	inline	bool	Contains (size_t containedMarkerStart, size_t containedMarkerEnd, size_t charPos)
		{
			return (charPos >= containedMarkerStart and charPos < containedMarkerEnd);
		}





//	class	TempMarker
	inline	size_t	TempMarker::GetStart () const
		{
			return fMarker.GetStart ();
		}
	inline	size_t	TempMarker::GetEnd () const
		{
			return fMarker.GetEnd ();
		}
	inline	size_t	TempMarker::GetLength () const
		{
			return fMarker.GetLength ();
		}
	inline	void	TempMarker::GetLocation (size_t* start, size_t* end) const
		{
			fMarker.GetRange (start, end);
		}






//	class	TemporaryMarkerSlideDown<MARKER>
	template	<typename	MARKER>
		TemporaryMarkerSlideDown<MARKER>::TemporaryMarkerSlideDown (TextStore& ts, const vector<MARKER*>& m, ptrdiff_t slideBy):
			fTextStore (ts),
			fMarkers (m),
			fSlideBy (slideBy)
			{
				for (typename vector<MARKER*>::iterator i = fMarkers.begin (); i != fMarkers.end (); ++i) {
					size_t	start	=	0;
					size_t	end		=	0;
					(*i)->GetRange (&start, &end);
					fTextStore.SetMarkerRange ((*i), start+fSlideBy, end+fSlideBy);
				}
			}
	template	<typename	MARKER>
		TemporaryMarkerSlideDown<MARKER>::~TemporaryMarkerSlideDown ()
			{
				for (typename vector<MARKER*>::iterator i = fMarkers.begin (); i != fMarkers.end (); ++i) {
					size_t	start	=	0;
					size_t	end		=	0;
					(*i)->GetRange (&start, &end);
					fTextStore.SetMarkerRange ((*i), start-fSlideBy, end-fSlideBy);
				}
			}



#if		qLedUsesNamespaces
}
#endif


#endif	/*__Marker_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
