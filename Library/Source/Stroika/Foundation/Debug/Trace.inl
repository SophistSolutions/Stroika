
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	Debug {

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
}

namespace	{
	Stroika::Foundation::ModuleInit::ModuleInitializer<Stroika::Foundation::Debug::Trace::Private::MODULE_INIT>	_TracePrivateINIT_;	// this object constructed for the CTOR/DTOR per-module side-effects
}

inline	Stroika::Foundation::Debug::Trace::Emitter&	Stroika::Foundation::Debug::Trace::Emitter::Get ()
	{
		return _TracePrivateINIT_.Actual ().fEmitter;
	}
