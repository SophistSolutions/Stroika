/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Options__
#define	__Options__

/*
 * $Header: /fuji/lewis/RCS/Options.hh,v 1.9 1992/11/15 04:15:03 lewis Exp $
 *
 * Description:
 *		Class to parse command line options in a system independent fasion.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *		This software is just in the prototyping phase. Don't count on it.
 *		I'm just playing around with this to see what can be done to help...
 *
 *
 *
 * Changes:
 *	$Log: Options.hh,v $
 *		Revision 1.9  1992/11/15  04:15:03  lewis
 *		Fix qRealTemplatesAvailable problem.
 *
 *		Revision 1.8  1992/10/29  22:45:37  lewis
 *		Fix so will work with templates. Make Bag<OptionDefinition> arg to CTOR
 *		instead of C-style array. Add SnagNextIntArg () utility function.
 *
 *		Revision 1.7  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.6  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.5  1992/10/07  22:54:48  sterling
 *		Conditionally compile/use new GenClass stuff.
 *
 *		Revision 1.4  1992/09/30  03:20:52  lewis
 *		(LGP checkin for STERL)Broke up declare macro so would compile
 *		on mac.
 *
 *		Revision 1.3  1992/09/29  14:23:01  lewis
 *		Mostly rewritten. Now dont use static fuction procs, use IDs instead.
 *
 *		Revision 1.2  1992/09/28  04:14:23  lewis
 *		Add MissingParam function, and SnagNextArg() utility function.
 *
 *
 *
 */

#include	"Bag.hh"
#include	"File.hh"
#include	"Sequence.hh"
#include	"String.hh"


#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SequenceOfString_hh
#endif

	class OptionParser;
	// qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	OptionParser;
	typedef	int	OptionID;		//qCFront_NestedTypesHalfAssed
	struct	OptionDefinition {
		OptionDefinition (const String& optionName, const String& optionAbbreviation, OptionID id, Boolean caseSensative);
#if		qGCC_BadDefaultCopyConstructorGeneration
		OptionDefinition (const OptionDefinition& from) :
			fOptionName (from.fOptionName),
			fOptionAbbreviation (from.fOptionAbbreviation),
			fID (from.fID),
			fCaseSensative (from.fCaseSensative)
		{
		}
#endif

		String		fOptionName;
		String		fOptionAbbreviation;
		OptionID	fID;
		Boolean		fCaseSensative;
	};
	inline	Boolean	operator== (const OptionDefinition& lhs, const OptionDefinition& rhs) { return Boolean (rhs.fOptionName == lhs.fOptionName); }




#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include BagOfOptionDefinition_hh
#endif


class	OptionParser {
	public:
		class	OptionParseException : public Exception {
			public:
				OptionParseException ();
				OptionParseException (const Exception& exception);
				OptionParseException (const OptionParseException& optionParseException);

				nonvirtual	void	Raise (const String& message);
				override	void	Raise ();

				nonvirtual	String			GetMessage () const;
				nonvirtual	void			SetMessage (const String& message);

			private:
				char	fMessage[200];	// use fixed size buf since must be CTORd before main!
										// NB: this constant size is not counted on, anywhere, so it
										// can be freely changed.
		};
		static	OptionParseException	sBadArgs;


	/*
	 * This object is designed to be subclassed, and the subclass will pass in the optionDefinitions
	 * in its CTOR, and provide access to answers after Parse is called.
	 */
	protected:
#if		qRealTemplatesAvailable
		OptionParser (const Bag<OptionDefinition>& optionDefinitions);
#else
		OptionParser (const Bag(OptionDefinition)& optionDefinitions);
#endif


	/*
	 * This is what you call to have the given arguments parsed. Typically as a side
	 * effect, this wills set public fields of some subclass of this object or at least allow
	 * public access to the results.
	 */
	public:
#if		qRealTemplatesAvailable
		nonvirtual	void	Parse (const Sequence<String>& args);
		nonvirtual	void	Parse (const String& programName, const Sequence<String>& args);
#else
		nonvirtual	void	Parse (const Sequence(String)& args);
		nonvirtual	void	Parse (const String& programName, const Sequence(String)& args);
#endif
		nonvirtual	void	Parse (unsigned argc, const char* argv[]);


	/*
	 * This function is rarely overriden by may be to modify the behaviour of parsing
	 * command line arguments in case your needs are unusual.
	 */
	protected:
#if		qRealTemplatesAvailable
		virtual	void	Parse_ (const String& programName, const Sequence<String>& args);
#else
		virtual	void	Parse_ (const String& programName, const Sequence(String)& args);
#endif


	/*
	 * This function is called internally by Parse_() for each arg list item,
	 * and can be overriden to provide unusual behavior for the given option.
	 */
	protected:
		nonvirtual	void	ParseArg (const String& param);
		virtual		void	ParseArg_ (const String& param);


	/*
	 */
	protected:
		nonvirtual	void	ParseOption (OptionID optionID);
		virtual		void	ParseOption_ (OptionID optionID);



	protected:
		nonvirtual	void	AddIthParam (const String& param, unsigned i);
		virtual		void	AddIthParam_ (const String& param, unsigned i);


	/*
	 * Utility functions to aid in arg parsing...
	 */
	protected:
		nonvirtual	String		SnagNextArg ();
		nonvirtual	PathName	SnagNextPathNameArg ();
		nonvirtual	int			SnagNextIntArg ();

	private:
		nonvirtual	void	UsageError ();


	protected:
		nonvirtual	String				GetProgramName () const;
#if		qRealTemplatesAvailable
		nonvirtual	Sequence<String>	GetArgs () const;
#else
		nonvirtual	Sequence(String)	GetArgs () const;
#endif
		nonvirtual	String				GetCurrentArg () const;
		nonvirtual	size_t				GetCurrentIndex () const;

	private:
#if		qDebug
		Boolean					fParsing;				// We avoid passing extra args by keeping
														// instance variables - this prevents recursive
														// calls, but that should not be a problem - keep
														// this bool just for asserts...
#endif
		size_t					fCurrentIndex;			// index of current item - goes from
															// 1..fCurrentArgs.GetLength () - after that were DONE
		String					fCurrentProgramName;	// only valid in the context of a parse
#if		qRealTemplatesAvailable
		Sequence<String>		fCurrentArgs;			// only valid in the context of a parse
		Bag<OptionDefinition>	fOptionDefinitions;
#else
		Sequence(String)		fCurrentArgs;			// only valid in the context of a parse
		Bag(OptionDefinition)	fOptionDefinitions;
#endif
		unsigned				fIthParam;				//	Ith NON-DASH parameter - usually file name...
};




#if		!qCFront_NestedTypesHalfAssed
	// We need this typedef for several reasons. One, it would be nightmarishly
	// complex to use two different syntaxes in the code that uses this stuff - one
	// for real nested types, and one for CFront 2.1 -half-assed. Further, gcc 2.2.1
	// has a bug which this works around. When all the various compilers come up to
	// speed here, we can get rid of this, and use the nested types stuff directly
	// in the source too...
	typedef	OptionParser::OptionParseException	OptionParseException;
	
#endif



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
	inline	String				OptionParser::GetProgramName () const
	{
		Require (fParsing);
		return (fCurrentProgramName);
	}

#if		qRealTemplatesAvailable
	inline	Sequence<String>	OptionParser::GetArgs () const
#else
	inline	Sequence(String)	OptionParser::GetArgs () const
#endif
	{
		Require (fParsing);
		return (fCurrentArgs);
	}

	inline	String		OptionParser::GetCurrentArg () const
	{
		Require (fParsing);
		return (fCurrentArgs [fCurrentIndex]);
	}

	inline	size_t		OptionParser::GetCurrentIndex () const
	{
		Require (fParsing);
		return (fCurrentIndex);
	}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Options__*/
