/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Trace_h__
#define	__Trace_h__	1

#include	"StroikaPreComp.h"

#ifdef	_WIN32
#include	<windows.h>
#include	<tchar.h>
#endif

#include	"ModuleInit.h"
#include	"StringUtils.h"
#include	"Support.h"



namespace	Stroika {	
	namespace	Foundation {

/*
 *	So we can distribute near-release versions of HF to users, and get feedback - a trace file
 *	to help debug problems we cannot reproduce.
 */
#ifndef	qTraceToFile
	#define	qTraceToFile	0
#endif



/*
@CONFIGVAR:		qDefaultTracingOn
@DESCRIPTION:	<p>Led contains a limited amount of pre-built tracing code. This could be expanded someday,
			depending on how useful people find it. This defaults to being on only for Windows and if @'qDebug' is
			on (windows only cuz thats the only place I've implemented the trace message emitter so far).</p>
				<p>See also @'qDebug' and @'DebugTrace'</p>
 */
#ifndef	qDefaultTracingOn
	#if		defined(_DEBUG)
		#define	qDefaultTracingOn	1
	#else
		#define	qDefaultTracingOn	0
	#endif
#endif



namespace	Trace {


	namespace	Private {
		struct	MODULE_INIT;
	};

	class	Emitter {
		private:
			Emitter ();
		public:
			static	Emitter&	Get ();

	#if		qTraceToFile
		public:
			nonvirtual	tstring	GetTraceFileName () const;
	#endif

		public:
			nonvirtual	void	EmitTraceMessage (const char* format, ...);
			nonvirtual	void	EmitTraceMessage (const wchar_t* format, ...);

			// The 'TraceLastBufferedWriteTokenType' overload of EmitTraceMessage allows you to specify a set of (trailing) characters to
			// be temporarily buffered. These characters are not immediately emitted, and can be cleared via UnputBufferedCharactersForMatchingToken ().
			// They will eventually be flushed out on the next call to EmitTraceMessage ().
			typedef	int	TraceLastBufferedWriteTokenType;
			nonvirtual	TraceLastBufferedWriteTokenType	EmitTraceMessage (size_t bufferLastNChars, const char* format, ...);
			nonvirtual	TraceLastBufferedWriteTokenType	EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...);

		public:
			// if the last write matches the given token (no writes since then) and the timestamp is unchanged, abandon
			// the buffered characters and return true. Else flush(write) them, and return false.
			nonvirtual	bool	UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token);
		
			template	<typename	CHARTYPE>
				nonvirtual	void	EmitUnadornedText (const CHARTYPE* p);

		private:
			// This is the same as EmitTraceMessage_ - but it takes a plain string - and assumes the caller does any 'sprintf' stuff...
			template	<typename	CHARTYPE>
				nonvirtual	TraceLastBufferedWriteTokenType	DoEmitMessage_ (size_t bufferLastNChars, CHARTYPE* p);
		private:
			size_t							fLastNCharBufCharCount;
			char							fLastNCharBuf_CHAR[10]; 
			wchar_t							fLastNCharBuf_WCHAR[10];
			bool							fLastNCharBuf_WCHARFlag;
			TraceLastBufferedWriteTokenType	fLastNCharBuf_Token;
			float							fLastNCharBuf_WriteTickcount;

			nonvirtual	void	BufferNChars_ (size_t nChars, const char* p);
			nonvirtual	void	BufferNChars_ (size_t nChars, const wchar_t* p);

		private:
			nonvirtual	void	FlushBufferedCharacters_ ();

		private:
			nonvirtual	void	DoEmit_ (const char* p);
			nonvirtual	void	DoEmit_ (const wchar_t* p);

		private:
			friend	struct	Private::MODULE_INIT;
	};



	class	TraceContextBumper {
		public:
			TraceContextBumper ();
			TraceContextBumper (const TCHAR* contextName);
			~TraceContextBumper ();

	#if		qDefaultTracingOn
		public:
			bool	fDoEndMarker;
		private:
			TCHAR										fSavedContextName[50];
			Emitter::TraceLastBufferedWriteTokenType	fLastWriteToken;			// used to COMBINE items into a single line if they happen quickly enuf

		public:
			static	unsigned int	GetCount ();
		private:
			static	void	IncCount ();
			static	void	DecrCount ();
	#endif
	};


}



/*
@METHOD:		DbgTrace
@DESCRIPTION:	<p>This function either does NOTHING (trying to not even evaluate its arguments)
			or does a printf style PRINT function by delegating to @'EmitTraceMessage'. Which of
			these two behaviors you see is conditioned on @'qDefaultTracingOn'</p>
*/
#ifndef	DbgTrace
	#if		qDefaultTracingOn
		#define	DbgTrace	Trace::Emitter::Get ().EmitTraceMessage
	#else
		#define	DbgTrace	__noop
	#endif
#endif



	}
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */



namespace	Stroika {	
	namespace	Foundation {

namespace	Trace {

//	class	Trace::Emitter
	template	<typename	CHARTYPE>
		inline	void	Trace::Emitter::EmitUnadornedText (const CHARTYPE* p)
			{
				DoEmit_ (p);
			}

//	class	TraceContextBumper
	inline	TraceContextBumper::TraceContextBumper ()
		#if		qDefaultTracingOn
			: fDoEndMarker (false)
			//,fSavedContextName ()
		#endif
		{
			#if		qDefaultTracingOn
				IncCount ();
			#endif
		}
	inline	TraceContextBumper::TraceContextBumper (const TCHAR* contextName)
		#if		qDefaultTracingOn
			: fDoEndMarker (true)
			//,fSavedContextName ()
		#endif
		{
			#if		qDefaultTracingOn
				fLastWriteToken = Trace::Emitter::Get ().EmitTraceMessage (5, _T ("<%s> {\r\n"), contextName);
				_tcsncpy_s (fSavedContextName, contextName, NEltsOf (fSavedContextName)-1);
				fSavedContextName[NEltsOf(fSavedContextName)-1] = '\0';
				IncCount ();
			#endif
		}
	inline	TraceContextBumper::~TraceContextBumper ()
		{
			#if		qDefaultTracingOn
				DecrCount ();
				if (fDoEndMarker) {
					if (Trace::Emitter::Get ().UnputBufferedCharactersForMatchingToken (fLastWriteToken)) {
						Trace::Emitter::Get ().EmitUnadornedText ("/>\r\n");
					}
					else {
						Trace::Emitter::Get ().EmitTraceMessage (_T ("} </%s>"), fSavedContextName);
					}
				}
			#endif
		}
}



namespace	Trace {
	namespace	Private {
		struct	MODULE_INIT {
			MODULE_INIT ();
			~MODULE_INIT ();

			Emitter	fEmitter;
		};
	}
}
	}
}

namespace	{
	Stroika::Foundation::ModuleInit::ModuleInitializer<Stroika::Foundation::Trace::Private::MODULE_INIT>	_TracePrivateINIT_;	// this object constructed for the CTOR/DTOR per-module side-effects
}

inline	Stroika::Foundation::Trace::Emitter&	Stroika::Foundation::Trace::Emitter::Get ()
	{
		return _TracePrivateINIT_.Actual ().fEmitter;
	}


#endif	/*__Trace_h__*/
