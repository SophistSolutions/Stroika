
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef	_Stroika_Foundation_Debug_Trace_inl_
#define	_Stroika_Foundation_Debug_Trace_inl_	1

namespace	Stroika {	
	namespace	Foundation {
		namespace	Debug {


		//	class	Trace::Emitter
			template	<typename	CHARTYPE>
				inline	void	Emitter::EmitUnadornedText (const CHARTYPE* p)
					{
						DoEmit_ (p);
					}

		//	class	TraceContextBumper
			inline	TraceContextBumper::TraceContextBumper ()
				#if		qDefaultTracingOn
					: fDoEndMarker (false)
					//,fSavedContextName_ ()
				#endif
				{
					#if		qDefaultTracingOn
						IncCount ();
					#endif
				}
			inline	TraceContextBumper::TraceContextBumper (const TChar* contextName)
				#if		qDefaultTracingOn
					: fDoEndMarker (true)
					//,fSavedContextName_ ()
				#endif
				{
					#if		qDefaultTracingOn
						fLastWriteToken_ = Emitter::Get ().EmitTraceMessage (5, TSTR ("<%s> {"), contextName);
						size_t	len	=	min (NEltsOf (fSavedContextName_), char_traits<TChar>::length (contextName));
						char_traits<TChar>::copy (fSavedContextName_, contextName, len);
						*(EndOfArray (fSavedContextName_)-1) = '\0';
						fSavedContextName_[len] = '\0';
						IncCount ();
					#endif
				}
			#if		!qDefaultTracingOn
			inline	TraceContextBumper::~TraceContextBumper ()
				{
				}
			#endif



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
	Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Debug::Private::MODULE_INIT>	_TracePrivateINIT_;	// this object constructed for the CTOR/DTOR per-module side-effects
}

inline	Stroika::Foundation::Debug::Emitter&	Stroika::Foundation::Debug::Emitter::Get ()
	{
		return _TracePrivateINIT_.Actual ().fEmitter;
	}
#endif	/*_Stroika_Foundation_Debug_Trace_inl_*/
